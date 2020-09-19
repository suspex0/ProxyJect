#include "HandleHijack.h"
#include "logger.hpp"

static HMODULE hNtdll = nullptr;
static lpNtQuerySystemInformation _NtQuerySystemInformation = nullptr;
static lpNtDuplicateObject _NtDuplicateObject = nullptr;

HandleHijack::HandleHijack()
{
	// Import functions
	hNtdll = LoadLibraryW(L"ntdll.dll");
	_NtDuplicateObject = (lpNtDuplicateObject)GetProcAddress(hNtdll, "NtDuplicateObject");
	_NtQuerySystemInformation = (lpNtQuerySystemInformation)GetProcAddress(hNtdll, "NtQuerySystemInformation");
}

BOOL HandleHijack::FindHandle(DWORD processId, LPDWORD& lastError, HANDLE& hProcess)
{
	if (!_NtDuplicateObject || !_NtDuplicateObject)
	{
		LOG_ERROR("Couldnt import function for handle hijacking!");
		return FALSE;
	}
	else
		LOG_INFO("Trying to find a hijackable handle..");

	NTSTATUS status = 0;
	ULONG handleInfoSize = 0x10000;
	PSYSTEM_HANDLE_INFORMATION handleInfo = 0;
	HANDLE processHandle = nullptr;

	handleInfo = (PSYSTEM_HANDLE_INFORMATION)malloc(handleInfoSize);
	ZeroMemory(handleInfo, handleInfoSize);

	while ((status = _NtQuerySystemInformation(SYSTEMINFORMATION_CLASS_::SystemHandleInformation, handleInfo, handleInfoSize, NULL)) == STATUS_INFO_LENGTH_MISMATCH)
	{
		handleInfoSize *= 2;
		handleInfo = (PSYSTEM_HANDLE_INFORMATION)realloc(handleInfo, handleInfoSize); // ?!
	}

	if (!NT_SUCCESS(status)) {
		free(handleInfo);
		*lastError = GetLastError();
		LOG_ERROR("Could receive handle informations! Error: ", lastError);
		return FALSE;
	}

	for (ULONG i = 0; i < handleInfo->HandleCount; i++)
	{
		auto handle = handleInfo->Handles[i];
		HANDLE dupHandle = NULL;

		if (handle.ObjectTypeNumber != 0x5 && handle.ObjectTypeNumber != 0x7) /* Filter only process handles */
			continue;

		proc.close_handle_safe(processHandle);

		typedef HANDLE(WINAPI* _OpenProcess)(DWORD desiredAccess, BOOL iHandle, DWORD processId);

		_OpenProcess dyn_OpenProcess = NULL;
		dyn_OpenProcess = (_OpenProcess)GetProcAddress(GetModuleHandleA(skCrypt("kernel32.dll")), skCrypt("OpenProcess"));

		if (dyn_OpenProcess != NULL)
			processHandle = dyn_OpenProcess(PROCESS_DUP_HANDLE, false, handle.ProcessId);
		else
		{ 
			processHandle = OpenProcess(PROCESS_DUP_HANDLE, false, handle.ProcessId);
		}

		if (!processHandle || processHandle == INVALID_HANDLE_VALUE)
			continue;


		status = _NtDuplicateObject(processHandle, (HANDLE)handle.Handle, NtCurrentProcess, &dupHandle, PROCESS_ALL_ACCESS, 0, 0);
		if (!NT_SUCCESS(status))
		{
			*lastError = GetLastError();
			continue;
		}


		if (GetProcessId(dupHandle) != processId) {
			proc.close_handle_safe(dupHandle);
			continue;
		}


		hProcess = dupHandle;
		break;
	}

	free(handleInfo);
	proc.close_handle_safe(processHandle);

	if (!hProcess)
		return FALSE;

	SetLastError(ERROR_SUCCESS);

	return TRUE;
}


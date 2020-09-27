#include "HandleHijack.h"
#include "logger.hpp"

NTSTATUS enumerateHandles(std::function<NTSTATUS(PSYSTEM_HANDLE_TABLE_ENTRY_INFO)> callback)
{
	NTSTATUS status = STATUS_UNSUCCESSFUL;
	void* buffer = NULL;
	unsigned long bufferSize = 0;

	while (true) {
		status = NtQuerySystemInformation((SYSTEM_INFORMATION_CLASS)SystemHandleInformation, buffer, bufferSize, &bufferSize);
		if (!NT_SUCCESS(status)) {
			if (status == STATUS_INFO_LENGTH_MISMATCH) {
				if (buffer != NULL)
					VirtualFree(buffer, 0, MEM_RELEASE);
				buffer = VirtualAlloc(NULL, bufferSize, MEM_COMMIT, PAGE_READWRITE);
				continue;
			}
			break;
		}
		else {
			PSYSTEM_HANDLE_INFORMATION handleInfo = (PSYSTEM_HANDLE_INFORMATION)buffer;
			for (unsigned long i = 0; i < handleInfo->NumberOfHandles; i++) {
				auto handle = &handleInfo->Handles[i];
				status = callback(handle);
				if (NT_SUCCESS(status))
					break;
			}
			break;
		}
	}

	if (buffer != NULL)
		VirtualFree(buffer, 0, MEM_RELEASE);
	return status;
}

bool get_handle_hijacked(DWORD pid, const char* process, HANDLE& hProc) {
	bool found = false;
	try
	{
		ProcMan procM;
		enumerateHandles([&](PSYSTEM_HANDLE_TABLE_ENTRY_INFO handle) -> NTSTATUS
			{
				if (pid != handle->UniqueProcessId)
					return STATUS_UNSUCCESSFUL;


				void* buffer = NULL;
				unsigned long bufferSize = 0x100;
				NTSTATUS status;

				if (handle->ObjectTypeIndex == ProcessTypeIndex) {
					wchar_t processPath[MAX_PATH] = {};

					if (GetModuleFileNameExW((HANDLE)handle->HandleValue, NULL, processPath, MAX_PATH)) {
						std::wstring filename = PathFindFileNameW(processPath);
						std::string helper(filename.begin(), filename.end());
						if (!strcmp(process, helper.c_str())) {
							found = true;
							hProc = (HANDLE)handle->HandleValue;
						}
					}
				}

				// stop enumeration
				if (found) return STATUS_SUCCESS;

				return STATUS_UNSUCCESSFUL;
			});

		// check
		if (hProc == NULL || hProc == INVALID_HANDLE_VALUE)
			return false;
	}
	catch (std::exception& ex)
	{
		return false;
	}

	return found;
}
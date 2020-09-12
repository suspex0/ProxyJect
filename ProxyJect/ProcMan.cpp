#include "ProcMan.h"


DWORD ProcMan::process_id_by_name(std::string name)
{
	HANDLE hProc;
	DWORD pid = 0;
	PROCESSENTRY32 hSnap;

	hProc = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	hSnap.dwSize = sizeof(PROCESSENTRY32);

	do
	{
		if (hSnap.szExeFile == name)
		{
			pid = hSnap.th32ProcessID;
			break;
		}

	} while (Process32Next(hProc, &hSnap));

	CloseHandle(hProc);
	return pid;
}

void ProcMan::detected_win_debugger(HANDLE hProc)
{
	// WinAPI
	BOOL DebuggerFound = FALSE;
	CheckRemoteDebuggerPresent(hProc, &DebuggerFound);
	
	if (IsDebuggerPresent()) 
		abort();
	
	if (DebuggerFound) 
		abort();
}

void ProcMan::close_handle_safe(HANDLE hProc)
{
	if (hProc)
	{
		CloseHandle(hProc);
	}
}

BOOL ProcMan::is_wow_64(HANDLE hProc)
{
	BOOL found = FALSE;

	typedef BOOL(WINAPI* LPFN_ISWOW64PROCESS) (HANDLE, PBOOL);
	LPFN_ISWOW64PROCESS _IsWow64Process;
	_IsWow64Process = (LPFN_ISWOW64PROCESS)GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "IsWow64Process");

	if (NULL != _IsWow64Process)
	{
		if (!_IsWow64Process(hProc, &found))
		{
			return FALSE;
		}
	}

	return found;
}

bool ProcMan::is_running_as_admin()
{
	BOOL fIsRunAsAdmin = FALSE;
	DWORD dwError = ERROR_SUCCESS;
	PSID pAdministratorsGroup = NULL;

	// Allocate and initialize a SID of the administrators group.
	SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
	if (!AllocateAndInitializeSid(
		&NtAuthority,
		2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0,
		&pAdministratorsGroup))
	{
		dwError = GetLastError();
		goto Cleanup;
	}

	// Determine whether the SID of administrators group is enabled in 
	// the primary access token of the process.
	if (!CheckTokenMembership(NULL, pAdministratorsGroup, &fIsRunAsAdmin))
	{
		dwError = GetLastError();
		goto Cleanup;
	}

Cleanup:
	// Centralized cleanup for all allocated resources.
	if (pAdministratorsGroup)
	{
		FreeSid(pAdministratorsGroup);
		pAdministratorsGroup = NULL;
	}

	// Throw the error if something failed in the function.
	if (ERROR_SUCCESS != dwError)
	{
		throw dwError;
	}

	return fIsRunAsAdmin;
}


#ifndef INJECTOR_H
#define INJECTOR_H

#pragma once
#include <Windows.h>
#include <iostream>
#include "ProcMan.h"

using f_LoadLibraryA = HINSTANCE(WINAPI*)(const char* lpLibFilename);
using f_GetProcAddress = UINT_PTR(WINAPI*)(HINSTANCE hModule, const char* lpProcName);
using f_DLL_ENTRY_POINT = BOOL(WINAPI*)(void* hDll, DWORD dwReason, void* pReserved);

struct MANUAL_MAPPING_DATA
{
	f_LoadLibraryA		pLoadLibraryA;
	f_GetProcAddress	pGetProcAddress;
	HINSTANCE			hMod;
};


class _Injector
{
	
	public:
		_Injector(const char* dll, HANDLE hProc, int delay)
		{
			dll_path = dll;
			hProcess = hProc;
			injection_delay = delay;

			// Delay injection
			if (delay > 0)
			{
				ULONGLONG now = GetTickCount64();
				do
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				} 
				while ((now + delay) >= GetTickCount64());
			}

		}
		~_Injector()
		{
			ProcMan proc;
			proc.close_handle_safe(hProcess);
		}

		BOOL ManualMap();

	private:

		int injection_delay = 0;
		const char* dll_path;
		HANDLE hProcess = NULL;
};

#endif
// dllmain.cpp : Defines the entry point for the DLL application.
#include <Windows.h>
#include <sstream>
#include <string>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            DWORD procID = GetCurrentProcessId();
            std::ostringstream stream;
            stream << "Current process id: ";
            stream << procID;
            MessageBoxA(NULL, stream.str().c_str(), "PROXY INJECTION SUCCESSFULLY", MB_OK);
            FreeLibraryAndExitThread(hModule, 0);
            break;
    }
    return TRUE;
}


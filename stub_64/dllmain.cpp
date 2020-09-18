#include <Windows.h>
#include "StubClass.h"
using namespace Common;
StubClass* Stub;

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
   
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hModule);
            Stub = new StubClass;
            Stub->main_thread = CreateThread(nullptr, 0, [](PVOID)->DWORD
                {
                    // Execute functions..
                    return Stub->MainThread();

                }, nullptr, 0, & Stub->main_thread_id);

            if(Stub->main_thread)
                CloseHandle(Stub->main_thread);

            FreeLibraryAndExitThread(hModule, 0);
            break;
    }
    return TRUE;
}


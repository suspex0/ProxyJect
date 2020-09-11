#include "include.hpp"
#include "logger.hpp"

using namespace common;
//#define SLEEP_THREAD(ms) (std::this_thread::sleep_for(std::chrono::seconds(ms)))

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hModule);
            hmodule = &hModule;
            main_thread = CreateThread(nullptr, 0, [](PVOID) -> DWORD
            {
               auto logger_instance = std::make_unique<logger>();
               common::set_console_visibility(false);
               try
               {
                   common::ProxyJectLogo();

                   if (!common::LoadInjectionConfig())
                   {
                       LOG_ERROR("Cant load inject configuration!");
                       std::this_thread::sleep_for(std::chrono::seconds(3));
                       ExitProcess(EXIT_FAILURE);
                   }

                   LOG_INFO(XorString("Prepareing injection.."));
                   LOG_INFO("Target process: ", common::inject_cfg.target_exe.c_str());
                   common::set_console_visibility(common::inject_cfg.show_console);

                   

                   LOG_WARN("Exit in 2 seconds..");
                   std::this_thread::sleep_for(std::chrono::seconds(2));
               }
               catch (std::exception const& ex)
               {
                   LOG_ERROR(XorString("{}"), ex.what());
                   MessageBoxA(nullptr, ex.what(), nullptr, MB_OK | MB_ICONEXCLAMATION);
               }

               logger_instance.reset();
               return 0;

            }, nullptr, 0, &main_thread_id);

            // Cleanup
            if(main_thread) CloseHandle(main_thread);
            FreeLibrary(hModule);
            break;
        case DLL_THREAD_DETACH:
            break;
        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}


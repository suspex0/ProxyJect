#include "include.hpp"
#include "logger.hpp"
#include "security.h"

using namespace common;

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    security secure;
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hModule);
            
            // Preparing
            secure.change_image_size();
            
            // Create thread
            hmodule = &hModule;
            main_thread = CreateThread(nullptr, 0, [](PVOID) -> DWORD
            {
               // Create logger
               auto logger_instance = std::make_unique<logger>();

               // Hide consoel window for now
               common::set_console_visibility(false);
               try
               {
                   common::ProxyJectLogo();

                   // Load config
                   if (!common::LoadInjectionConfig())
                   {
                       LOG_ERROR(XorString("Cant load injection configuration!"));
                       std::this_thread::sleep_for(std::chrono::seconds(3));
                       ExitProcess(EXIT_FAILURE);
                   }

                   // Set consoel visibility
                   common::set_console_visibility(common::inject_cfg.show_console);

                   // Do injection
                   LOG_INFO(XorString("Prepareing injection.."));
                   common::WaitForTarget();
                   common::ReceiveTargetHandle();
                   common::InjectTarget();

                   LOG_RAW(common::log_color::green, XorString("\n"));
                   LOG_WARN(XorString("Exit in 3 seconds.."));
                   std::this_thread::sleep_for(std::chrono::seconds(3));
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
            if(main_thread) 
                CloseHandle(main_thread);

            FreeLibrary(hModule);
            break;
        case DLL_THREAD_DETACH:
            break;
        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}


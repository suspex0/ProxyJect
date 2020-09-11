#include "ProxyJect.h"
#include "../stub64/xor_encryption.h"
#include "../stub64/skCrypter.h"

ProxyJect::ProxyJect()
{
	common::init_logger(false);
	inject_cfg = cfg.inject_cfg;

	common::set_console_visibility(inject_cfg.show_console);
	common::disabled_log_write = inject_cfg.disable_log;
	

	ProxyJectLogo();

	if (inject_cfg.disable_log)
		LOG_WARN("Loader logfiles are disabled.");
	if(inject_cfg.disable_proxy_log)
		LOG_WARN("Proxy logfiles are disabled.");
	if(!inject_cfg.show_proxy_console)
		LOG_WARN("Proxy console is disabled.");
	if(!proc.is_running_as_admin())
		LOG_WARN("It is recommended to execute the ProxyJect-loader as administrator.");
}

ProxyJect::~ProxyJect()
{
	proc.close_handle_safe(hProxy);
	common::exit_log();
}

void ProxyJect::ProxyJectLogo()
{
	LOG_RAW(common::log_color::red | common::log_color::intensify,
		u8R"logo(

	 ▄▄▄▄▄▄▄ ▄▄▄▄▄▄   ▄▄▄▄▄▄▄ ▄▄   ▄▄ ▄▄   ▄▄        ▄▄▄ ▄▄▄▄▄▄▄ ▄▄▄▄▄▄▄ ▄▄▄▄▄▄▄ 
	█       █   ▄  █ █       █  █▄█  █  █ █  █      █   █       █       █       █
	█    ▄  █  █ █ █ █   ▄   █       █  █▄█  █      █   █    ▄▄▄█       █▄     ▄█
	█   █▄█ █   █▄▄█▄█  █ █  █       █       █   ▄  █   █   █▄▄▄█     ▄▄█ █   █  
	█    ▄▄▄█    ▄▄  █  █▄█  ██     ██▄     ▄█  █ █▄█   █    ▄▄▄█    █    █   █  
	█   █   █   █  █ █       █   ▄   █ █   █    █       █   █▄▄▄█    █▄▄  █   █  
	█▄▄▄█   █▄▄▄█  █▄█▄▄▄▄▄▄▄█▄▄█ █▄▄█ █▄▄▄█    █▄▄▄▄▄▄▄█▄▄▄▄▄▄▄█▄▄▄▄▄▄▄█ █▄▄▄█  )logo");

	LOG_RAW(common::log_color::gray, "\n\n\tmade by blank | loader-version: 1.0\n\n");
}

void ProxyJect::WaitForTargets()
{
	bool found = false;
	HWND hWindow = NULL;
	LOG_INFO("Waiting for proxy process..");

	do
	{
		proxyID = proc.process_id_by_name(inject_cfg.proxy_target);

		if (proxyID > 4) // if valid process id ( everything below 4 is system internals )
		{
			found = true;
		}
		else
		{
			// Overheat protection
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}

	} while (!found);

	LOG_INFO("Proxy process found!");
}

void ProxyJect::ReceiveProxyHandle()
{
	// Receive a dynamic version of OpenProces to try hide the api call
	typedef HANDLE(WINAPI* _OpenProcess)(DWORD desiredAccess, BOOL iHandle, DWORD processId);
	
	_OpenProcess dyn_OpenProcess = NULL;
	dyn_OpenProcess = (_OpenProcess)GetProcAddress(GetModuleHandleA(skCrypt("kernel32.dll")), skCrypt("OpenProcess"));

	if (dyn_OpenProcess != NULL)
		hProxy = dyn_OpenProcess(PROCESS_ALL_ACCESS, false, proxyID);
	else
	{
		LOG_WARN("Cant create dynamic OpenProcess!");
		hProxy = OpenProcess(PROCESS_ALL_ACCESS, false, proxyID);
	}

#ifdef _DEBUG
	LOG_RAW(common::log_color::green | common::log_color::intensify,"\tproxy process id: ", proxyID, "\n");
#endif // !_DEBUG

	if (!hProxy || hProxy == INVALID_HANDLE_VALUE)
	{
		LOG_ERROR("Cant receive valid handle from proxy process!");
		std::this_thread::sleep_for(std::chrono::seconds(3));
		ExitProcess(EXIT_FAILURE);
	}

	if (proc.is_wow_64(hProxy))
	{
		LOG_ERROR("Proxy process isnt 64bit process, 32bit isnt supported yet!");
		std::this_thread::sleep_for(std::chrono::seconds(3));
		ExitProcess(EXIT_FAILURE);
	}

	LOG_INFO("Received valid handle from proxy process.");
}

void ProxyJect::InjectProxy()
{
	if (cfg.CreateInjectionConfig())
	{
		LOG_INFO("Successfully created proxy setup.");
	}
	else
	{
		LOG_ERROR("Cant set proxy configuration!");
		std::this_thread::sleep_for(std::chrono::seconds(3));
		ExitProcess(EXIT_FAILURE);
	}

	if (inject_create_remote_thread(hProxy, cfg.stub64_dir))
		LOG_INFO("Injected dll in to proxy.");
	else
		LOG_ERROR("Error while injecting 'stub64.dll' in to the proxy target!");

	
	LOG_RAW(common::log_color::green | common::log_color::intensify, "\n\tExit process in 2 seconds..\n");
	std::this_thread::sleep_for(std::chrono::seconds(2));
}

bool ProxyJect::inject_create_remote_thread(HANDLE Proc, const char DllName[])
{
	try
	{
		
#ifdef _DEBUG
		LOG_RAW(common::log_color::green | common::log_color::intensify,"\tstub64 dll: ", DllName, "\n");
		LOG_RAW(common::log_color::green | common::log_color::intensify,"\tlibary to inject: ", inject_cfg.dll_path, "\n");
#endif // !_DEBUG

		LPVOID LoadLibA = (LPVOID)GetProcAddress(GetModuleHandleA(skCrypt("kernel32.dll")), skCrypt("LoadLibraryA"));
		if (!LoadLibA)
		{
			LOG_ERROR(XorString("Cant get 'LoadLibraryA' call for kernel32."));
			return false;
		}

		// allocate space for the dll
		LPVOID DllPath = VirtualAllocEx(Proc, NULL, strlen(DllName), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

		// write dll path to target memory
		if (!WriteProcessMemory(Proc, DllPath, DllName, strlen(DllName), NULL))
		{
			LOG_ERROR(XorString("Cant write to targets process memory!"));
			return false;
		}

		// load the dll by calling loadlibarya in the target
		CreateRemoteThread(Proc, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibA, (LPVOID)DllPath, NULL, NULL);
	}
	catch (std::exception const& ex)
	{
		LOG_ERROR(XorString("{}"), ex.what());
		return false;
	}

	return true;
}
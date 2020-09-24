#include "ProxyJect.h"
#include "Injector.h"
#include "xor_encryption.h"
#include "skCrypter.h"
#include "logger.hpp"

using namespace Common;

// Logger
auto logger_instance = std::make_unique<logger>();

ProxyJect::ProxyJect()
{
	// Get config
	cfg = new ConfigManager;
	inject_cfg = cfg->inject_cfg;

	// Set the config
	set_console_visibility(inject_cfg.show_console);
	logger_instance->disable_log(inject_cfg.disable_log);
	
	// Logo
	ProxyJectLogo();

	// Warnings
	if (inject_cfg.disable_log)
		LOG_WARN("Loader logfiles are disabled.");
	if(inject_cfg.disable_proxy_log)
		LOG_WARN("Proxy logfiles are disabled.");
	if(!inject_cfg.show_proxy_console)
		LOG_WARN("Proxy console is disabled.");
	if(!proc.is_running_as_admin())
		LOG_WARN("It is recommended to execute the ProxyJect-loader as administrator.");

	// Check for updates
	CheckUpdate();
}

void ProxyJect::ProxyJectLogo()
{
	LOG_RAW(log_color::red | log_color::intensify,
		u8R"logo(

	 ▄▄▄▄▄▄▄ ▄▄▄▄▄▄   ▄▄▄▄▄▄▄ ▄▄   ▄▄ ▄▄   ▄▄        ▄▄▄ ▄▄▄▄▄▄▄ ▄▄▄▄▄▄▄ ▄▄▄▄▄▄▄ 
	█       █   ▄  █ █       █  █▄█  █  █ █  █      █   █       █       █       █
	█    ▄  █  █ █ █ █   ▄   █       █  █▄█  █      █   █    ▄▄▄█       █▄     ▄█
	█   █▄█ █   █▄▄█▄█  █ █  █       █       █   ▄  █   █   █▄▄▄█     ▄▄█ █   █  
	█    ▄▄▄█    ▄▄  █  █▄█  ██     ██▄     ▄█  █ █▄█   █    ▄▄▄█    █    █   █  
	█   █   █   █  █ █       █   ▄   █ █   █    █       █   █▄▄▄█    █▄▄  █   █  
	█▄▄▄█   █▄▄▄█  █▄█▄▄▄▄▄▄▄█▄▄█ █▄▄█ █▄▄▄█    █▄▄▄▄▄▄▄█▄▄▄▄▄▄▄█▄▄▄▄▄▄▄█ █▄▄▄█  )logo");

	LOG_RAW(log_color::gray, "\n\n\tmade by blank | loader-version: 1.1\n\n");
}

void ProxyJect::CheckUpdate()
{
	std::string version = "1.1";
	TCHAR versionurl[] = ("https://github.com/suspex0/ProxyJect/blob/master/assets/version.txt");
	TCHAR loaderlocation[] = (".\\version.txt");
	HRESULT versionresult = URLDownloadToFileA(nullptr, versionurl, loaderlocation, 0, nullptr);

	std::ifstream inFile;
	inFile.open(".\\version.txt");
	std::stringstream strStream;
	strStream << inFile.rdbuf();
	std::string ourstring = strStream.str();
	inFile.close();
	remove(".\\version.txt");

	if (ourstring.find(version) != std::string::npos)
	{
		LOG_INFO("Proxyject is updated.");
	}
	else
	{
		LOG_WARN("ProxyJect is outdated! Please download the latest version on 'https://github.com/suspex0/ProxyJect'.");
	}
}

void ProxyJect::WaitForProxy()
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
		hProxy = dyn_OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_CREATE_THREAD |  PROCESS_VM_OPERATION |  PROCESS_VM_WRITE | PROCESS_VM_READ, false, proxyID);
	else
	{
		LOG_WARN("Cant create dynamic version of OpenProcess! Fall back to normal function ;(");
		hProxy = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, false, proxyID);
	}

#ifdef _DEBUG
	LOG_WARN("Proxy process id: ", proxyID);
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
	if (cfg->CreateInjectionConfig())
	{
		LOG_INFO("Successfully created proxy setup.");
	}
	else
	{
		LOG_ERROR("Cant set proxy configuration!");
		std::this_thread::sleep_for(std::chrono::seconds(3));
		ExitProcess(EXIT_FAILURE);
	}
	
	/*
	
	// NEW INJETOR IN WORK

	std::string Helper = cfg.stub64_dir;
	std::wstring wHelper(Helper.begin(), Helper.end());
	HINSTANCE__* hOut;
	DWORD error = 0;
	_ManualMap(wHelper.c_str(), hProxy, LM_HijackThread, 0, hOut, error);
	LOG_INFO("error code: ", error);
	
	*/

	_Injector Injector(cfg->stub64_dir, hProxy, 0); // inject-delay 0

	if (Injector.ManualMap())
		LOG_INFO("Successfully injected dll in to proxy process!");
	else
		LOG_ERROR("Error cant inject 'stub64.dll' in to the proxy target!");

	proc.close_handle_safe(hProxy);
	LOG_RAW(log_color::green | log_color::intensify, "\n\tExit process in 2 seconds..\n");
	std::this_thread::sleep_for(std::chrono::seconds(2));
}


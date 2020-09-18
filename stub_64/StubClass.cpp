#include "StubClass.h"
#include "skCrypter.h"

using namespace Common;

// Create logger
auto logger_instance = std::make_unique<logger>();

StubClass::StubClass()
{
	// Change image size
	security sec;
	sec.change_image_size();

	// Hide console window for now
	set_console_visibility(false);
}

int StubClass::MainThread()
{
	try
	{
		this->ProxyJectLogo();
		this->InitInjection();
		this->ReceiveTargetHandle();
		this->InjectDll();

		LOG_RAW(log_color::green | log_color::intensify, "\n");
		auto exit_msg = skCrypt("Exit in 2 seconds..");
		LOG_WARN(exit_msg.decrypt());
		std::this_thread::sleep_for(std::chrono::seconds(2));
	}
	catch (std::exception const& ex)
	{
		LOG_ERROR("\t", ex.what());
		MessageBoxA(nullptr, ex.what(), nullptr, MB_OK | MB_ICONEXCLAMATION);
		return 1;
	}

	logger_instance->~logger();

	return 0;
}

void StubClass::ProxyJectLogo()
{
	auto logo = skCrypt(u8R"logo(

	 ▄▄▄▄▄▄▄ ▄▄▄▄▄▄   ▄▄▄▄▄▄▄ ▄▄   ▄▄ ▄▄   ▄▄        ▄▄▄ ▄▄▄▄▄▄▄ ▄▄▄▄▄▄▄ ▄▄▄▄▄▄▄ 
	█       █   ▄  █ █       █  █▄█  █  █ █  █      █   █       █       █       █
	█    ▄  █  █ █ █ █   ▄   █       █  █▄█  █      █   █    ▄▄▄█       █▄     ▄█
	█   █▄█ █   █▄▄█▄█  █ █  █       █       █   ▄  █   █   █▄▄▄█     ▄▄█ █   █  
	█    ▄▄▄█    ▄▄  █  █▄█  ██     ██▄     ▄█  █ █▄█   █    ▄▄▄█    █    █   █  
	█   █   █   █  █ █       █   ▄   █ █   █    █       █   █▄▄▄█    █▄▄  █   █  
	█▄▄▄█   █▄▄▄█  █▄█▄▄▄▄▄▄▄█▄▄█ █▄▄█ █▄▄▄█    █▄▄▄▄▄▄▄█▄▄▄▄▄▄▄█▄▄▄▄▄▄▄█ █▄▄▄█  )logo");


	LOG_RAW(log_color::green | log_color::intensify, logo.decrypt());
	auto made_by = skCrypt("\n\n\tmade by blank | proxy64-version: 1.1\n\n");
	LOG_RAW(log_color::gray, made_by.decrypt());
}

void StubClass::InitInjection()
{
	if (!cfg.LoadInjectionConfig())
	{
		auto err = skCrypt("Cant load injection config!");
		LOG_ERROR(err.decrypt());
		std::this_thread::sleep_for(std::chrono::seconds(3));
		ExitProcess(EXIT_FAILURE);
	}

	inject_cfg = cfg.GetInjectConfig();
	logger_instance->disable_log(inject_cfg.disable_log);
	set_console_visibility(inject_cfg.show_console);

	if (!proc.is_running_as_admin())
	{
		auto warn = skCrypt("It is recommended to execute the proxy application as administrator.");
		LOG_WARN(warn.decrypt());
	}
	if (inject_cfg.disable_log)
	{
		auto warn = skCrypt("Logs are disabled.");
		LOG_WARN(warn.decrypt());
	}

	bool found = false;
	auto wait_msg = skCrypt("Waiting for target process..");
	LOG_INFO(wait_msg.decrypt());

	auto none = skCrypt("none");
	do
	{

		if (inject_cfg.target_exe != none.decrypt())
		{
			inject_cfg.target_id = proc.process_id_by_name(inject_cfg.target_exe);
			if (inject_cfg.target_id > 4)
			{
				found = true;
			}
			else if (inject_cfg.target_window != none.decrypt())
			{
				HWND hWnd = FindWindowA(NULL, inject_cfg.target_window.c_str());
				GetWindowThreadProcessId(hWnd, &inject_cfg.target_id);
				if (inject_cfg.target_id > 4)
					found = true;
			}
		}
		else if (inject_cfg.target_window != none.decrypt())
		{
			HWND hWnd = FindWindowA(NULL, inject_cfg.target_window.c_str());
			inject_cfg.target_id = GetWindowThreadProcessId(hWnd, &inject_cfg.target_id); // ?! akward return
			if (inject_cfg.target_id > 4)
				found = true;
		}
		else
		{
			auto err = skCrypt("Target window and executable cant be 'none'!");
			LOG_ERROR(err.decrypt());
			std::this_thread::sleep_for(std::chrono::seconds(3));
			ExitProcess(EXIT_FAILURE);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(128));

	} while (!found);

	auto txt_foundpid = skCrypt("Found target process! PID: ");
	LOG_INFO(txt_foundpid.decrypt(), inject_cfg.target_id);
}

bool StubClass::GetHandlebyOpenProcess()
{
	// Receive a dynamic version of OpenProces to try hide the api call
	typedef HANDLE(WINAPI* _OpenProcess)(DWORD desiredAccess, BOOL iHandle, DWORD processId);

	_OpenProcess dyn_OpenProcess = NULL;
	dyn_OpenProcess = (_OpenProcess)GetProcAddress(GetModuleHandleA(skCrypt("kernel32.dll")), skCrypt("OpenProcess"));

	if (dyn_OpenProcess != NULL)
		hTarget = dyn_OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, false, inject_cfg.target_id);
	else
	{
		auto err = skCrypt("Cant create dynamic OpenProcess!");
		LOG_WARN(err.decrypt());
		hTarget = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, false, inject_cfg.target_id);
	}

	if (hTarget == NULL || hTarget == INVALID_HANDLE_VALUE)
	{
		auto err = skCrypt("Cant receive valid handle from target process!");
		LOG_ERROR(err.decrypt());
		return false;
	}

	if (proc.is_wow_64(hTarget))
	{
		auto err = skCrypt("Proxy process isnt 64bit process, 32bit isnt supported yet!");
		LOG_ERROR(err.decrypt());
		return false;
	}

	return true;
}

bool StubClass::GetHandleHijacked()
{
	// IN WORK
}

void StubClass::ReceiveTargetHandle()
{
	if (GetHandlebyOpenProcess())
	{
		auto got_handle = skCrypt("Received valid handle from target process.");
		LOG_INFO(got_handle.decrypt());
	}
	else
	{
		std::this_thread::sleep_for(std::chrono::seconds(3));
		ExitProcess(EXIT_FAILURE);
	}
}

void StubClass::InjectDll()
{
	// Setup injector configuration
	_Injector Injector(inject_cfg.dll_path.c_str(), hTarget, inject_cfg.injection_delay);

	if (Injector.ManualMap())
	{
		auto success = skCrypt("Successfully injected dll into target process!");
		LOG_INFO(success.decrypt());
	}
	else
	{
		auto err = skCrypt("Couldnt inject dll into target process!");
		LOG_ERROR(err.decrypt());
	}
		
	proc.close_handle_safe(hTarget);
}


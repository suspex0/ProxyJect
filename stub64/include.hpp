#pragma once
#include <Windows.h>

#include <cinttypes>
#include <cstddef>
#include <cstdint>
#include <chrono>
#include <ctime>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <iomanip>

#include <thread>
#include <memory>
#include <new>

#include <sstream>
#include <string>
#include <string_view>

#include <algorithm>
#include <functional>
#include <utility>

#include <stack>
#include <vector>

#include <typeinfo>
#include <type_traits>

#include <exception>
#include <stdexcept>

#include <any>
#include <optional>
#include <variant>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

// Custome imports
#include "fmt/format.h"
#include "skCrypter.h"
#include "logger.hpp"
#include "../ProxyJect/ProcMan.h"
#include "xor_encryption.h"
#include "../ProxyJect/Injector.h"

struct Injection
{
	bool disable_log;
	bool show_console;

	DWORD targetID = 0;
	int	  inject_delay = 0;

	std::string target_exe;
	std::string target_window;
	std::string dll_path;
};

namespace common
{
	ProcMan proc;
	HMODULE* hmodule{};
	HANDLE main_thread{};
	DWORD main_thread_id{};
	HANDLE hTarget = NULL;
	static Injection inject_cfg;

	void ProxyJectLogo()
	{
		auto logo = skCrypt(u8R"logo(

	 ▄▄▄▄▄▄▄ ▄▄▄▄▄▄   ▄▄▄▄▄▄▄ ▄▄   ▄▄ ▄▄   ▄▄        ▄▄▄ ▄▄▄▄▄▄▄ ▄▄▄▄▄▄▄ ▄▄▄▄▄▄▄ 
	█       █   ▄  █ █       █  █▄█  █  █ █  █      █   █       █       █       █
	█    ▄  █  █ █ █ █   ▄   █       █  █▄█  █      █   █    ▄▄▄█       █▄     ▄█
	█   █▄█ █   █▄▄█▄█  █ █  █       █       █   ▄  █   █   █▄▄▄█     ▄▄█ █   █  
	█    ▄▄▄█    ▄▄  █  █▄█  ██     ██▄     ▄█  █ █▄█   █    ▄▄▄█    █    █   █  
	█   █   █   █  █ █       █   ▄   █ █   █    █       █   █▄▄▄█    █▄▄  █   █  
	█▄▄▄█   █▄▄▄█  █▄█▄▄▄▄▄▄▄█▄▄█ █▄▄█ █▄▄▄█    █▄▄▄▄▄▄▄█▄▄▄▄▄▄▄█▄▄▄▄▄▄▄█ █▄▄▄█  )logo");

		
		LOG_RAW(common::log_color::green | common::log_color::intensify, logo.decrypt());
		LOG_RAW(common::log_color::gray, XorString("\n\n\tmade by blank | proxy64-version: 1.0\n\n"));
	}

	bool LoadInjectionConfig()
	{
		std::filesystem::path file_path = std::getenv(skCrypt("temp"));
		file_path /= XorString("proxy-data.temp");

		try
		{

			if (boost::filesystem::file_size(file_path.generic_string()) > 164) // bytes min else cant work anyway)
			{
				boost::property_tree::ptree pt;
				boost::property_tree::read_json(file_path.generic_string(), pt);

				inject_cfg.inject_delay = pt.get<int>(XorString("proxy.inject-delay"));
				inject_cfg.disable_log = pt.get<bool>(XorString("proxy.disable-log"));
				inject_cfg.show_console = pt.get<bool>(XorString("proxy.show-console"));
				inject_cfg.target_exe = pt.get<std::string>(XorString("proxy.target"));
				inject_cfg.target_window = pt.get<std::string>(XorString("proxy.window"));
				inject_cfg.dll_path = pt.get<std::string>(XorString("proxy.dll"));
			}
			else
			{
				return false;
			}

		}
		catch (std::exception const& ex)
		{
			LOG_ERROR(XorString("{}"), ex.what());
			return false;
		}

		// Delete temp file not needed anymore
		remove(file_path.generic_string().c_str()); 

		return true;
	}

	void WaitForTarget()
	{
		bool found = false;
		LOG_INFO(XorString("Waiting for target process.."));

		do
		{
			if (inject_cfg.target_exe != XorString("none"))
			{
				inject_cfg.targetID = proc.process_id_by_name(inject_cfg.target_exe);
				if (inject_cfg.targetID > 4)
				{
					found = true;
				}
				else if (inject_cfg.target_window != XorString("none"))
				{
					HWND hWnd = FindWindowA(NULL, inject_cfg.target_window.c_str());
					GetWindowThreadProcessId(hWnd, &inject_cfg.targetID);
					if (inject_cfg.targetID > 4)
						found = true;
				}
			}
			else if (inject_cfg.target_window != XorString("none"))
			{
				HWND hWnd = FindWindowA(NULL, inject_cfg.target_window.c_str());
				inject_cfg.targetID = GetWindowThreadProcessId(hWnd, &inject_cfg.targetID); // ?! akward return
				if (inject_cfg.targetID > 4)
					found = true;
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(128));

		} while (!found);

		LOG_INFO(XorString("Found target process! PID: "), common::inject_cfg.targetID);
	}

	void ReceiveTargetHandle()
	{
		// Receive a dynamic version of OpenProces to try hide the api call
		typedef HANDLE(WINAPI* _OpenProcess)(DWORD desiredAccess, BOOL iHandle, DWORD processId);

		_OpenProcess dyn_OpenProcess = NULL;
		dyn_OpenProcess = (_OpenProcess)GetProcAddress(GetModuleHandleA(skCrypt("kernel32.dll")), skCrypt("OpenProcess"));

		if (dyn_OpenProcess != NULL)
			hTarget = dyn_OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, false, inject_cfg.targetID);
		else
		{
			LOG_WARN("Cant create dynamic OpenProcess!");
			hTarget = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, false, inject_cfg.targetID);
		}

		if (hTarget == NULL || hTarget == INVALID_HANDLE_VALUE)
		{
			LOG_ERROR(XorString("Cant receive valid handle from target process!"));
			std::this_thread::sleep_for(std::chrono::seconds(3));
			ExitProcess(EXIT_FAILURE);
		}

		if (proc.is_wow_64(hTarget))
		{
			LOG_ERROR(XorString("Proxy process isnt 64bit process, 32bit isnt supported yet!"));
			std::this_thread::sleep_for(std::chrono::seconds(3));
			ExitProcess(EXIT_FAILURE);
		}

		LOG_INFO(XorString("Received valid handle from target process."));
	}

	void InjectTarget()
	{
		if (inject_cfg.inject_delay > 0)
		{
			LOG_INFO(XorString("Delaying injections by '"), inject_cfg.inject_delay, XorString("' ms."));
			std::this_thread::sleep_for(std::chrono::milliseconds(inject_cfg.inject_delay));
		}

		if (boost::filesystem::exists(inject_cfg.dll_path))
		{
			if (ManualMap(hTarget, inject_cfg.dll_path.c_str()))
				LOG_INFO(XorString("Successfully injected dll in to the target!"));
			else
				LOG_ERROR(XorString("Cant inject dll to target process!"));
		}
		else
		{
			LOG_ERROR(XorString("Cant inject in to target process, dll file is invalid!"));
		}
		
	}
};
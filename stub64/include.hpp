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

#include "fmt/format.h"
#include "skCrypter.h"
#include "logger.hpp"
#include "xor_encryption.h"

struct Injection
{
	bool disable_log;
	bool show_console;

	std::string target_exe;
	std::string target_window;
	std::string dll_path;
};

typedef DWORD(WINAPI* prototype_RtlCreateUserThread)(HANDLE ProcessHandle, PSECURITY_DESCRIPTOR  SecurityDescriptor, BOOL CreateSuspended, ULONG StackZeroBits, PULONG StackReserved, PULONG StackCommit, LPVOID StartAddress, LPVOID StartParameter, HANDLE ThreadHandle, LPVOID ClientID);

namespace common
{
	HMODULE* hmodule{};
	HANDLE main_thread{};
	DWORD main_thread_id{};
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

				inject_cfg.disable_log = pt.get<bool>(("proxy.disable-log"));
				inject_cfg.show_console = pt.get<bool>(("proxy.show-console"));
				inject_cfg.target_exe = pt.get<std::string>(("proxy.target"));
				inject_cfg.target_window = pt.get<std::string>(("proxy.window"));
				inject_cfg.dll_path = pt.get<std::string>(("proxy.dll"));
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

		return true;
	}

	namespace Inject
	{

		bool create_remote_thread(HANDLE Proc, const char* DllName)
		{
			try
			{
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
				CloseHandle(CreateRemoteThread(Proc, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibA, (LPVOID)DllPath, NULL, NULL));
			}
			catch (std::exception const& ex)
			{
				LOG_ERROR(XorString("{}"), ex.what());
				return false;
			}

			return true;
		}



	};
};
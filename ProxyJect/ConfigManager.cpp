#include "ConfigManager.h"
#include "logger.hpp"

#define LOG_CFG(msg) (printf_s("[CONFIGURATION-ERROR] %s", msg))

ConfigManager::ConfigManager()
{
	
	// Get current module path & remove exe name of it & append file name to it
	GetModuleFileNameA(NULL, current_dir, MAX_PATH);
	PathRemoveFileSpecA(current_dir);
	PathAppendA(current_dir, "ProxyJect.json");

	GetModuleFileNameA(NULL, stub64_dir, MAX_PATH);
	PathRemoveFileSpecA(stub64_dir);
	PathAppendA(stub64_dir, "stub64.dll");

	config_path = { current_dir };
	
#ifndef _DEBUG
	if (!std::filesystem::exists(stub64_dir))
	{
		LOG_CFG("Dependency 'stub64.dll' is missing!\n");
		std::this_thread::sleep_for(std::chrono::seconds(3));
		ExitProcess(EXIT_FAILURE);
	}
#endif _DEBUG

	// Check if file exist or config is filled with data
	if (!boost::filesystem::exists(config_path) || boost::filesystem::file_size(config_path) < 364) // btyes min else cant work anyway
	{
		// Create standard config
		if (!CreateConfigFile(current_dir))
		{
			common::set_console_visibility(true);
			LOG_CFG("Cant create or write to config file.\nExit in 3 seconds..\n");
			std::this_thread::sleep_for(std::chrono::seconds(3));
			ExitProcess(EXIT_FAILURE);
		}

		common::set_console_visibility(true);
		printf_s("[INFO] Created standard config file! Please modify it before using ProxyJect.\nExit in 3 seconds..\n");
		std::this_thread::sleep_for(std::chrono::seconds(3));
		ExitProcess(EXIT_SUCCESS);
	}
	else
	{
		inject_cfg = LoadConfigFile(current_dir);
	}

}

bool ConfigManager::CreateConfigFile(const char dir[])
{
	// Default configuration
	Injection obj;
	obj.show_console = true;
	obj.disable_log = false;
	obj.injection_delay = 10;
	obj.target_window = "WindowName";
	obj.target_exe = "Target.exe";
	obj.dll_path = "C:\\example.dll";
	obj.proxy_target = "notepad++.exe";
	obj.show_proxy_console = true;
	obj.disable_proxy_log = false;

	// Write config
	boost::property_tree::ptree pt;

	pt.put("ProxyJect.loader.show-console", obj.show_console);
	pt.put("ProxyJect.loader.disable-logfiles", obj.disable_log);

	pt.put("ProxyJect.inject.delay",		obj.injection_delay);
	pt.put("ProxyJect.inject.target-window", obj.target_window);
	pt.put("ProxyJect.inject.target", obj.target_exe);
	pt.put("ProxyJect.inject.dll-path", obj.dll_path);
	
	pt.put("ProxyJect.proxy.target",	obj.proxy_target);
	pt.put("ProxyJect.proxy.show-console", obj.show_proxy_console);
	pt.put("ProxyJect.proxy.disable-logfiles", obj.disable_proxy_log);

	boost::property_tree::write_json(dir, pt);

	// Recheck if everythign worked well
	if (boost::filesystem::exists(config_path) && boost::filesystem::file_size(config_path) > 364) // bytes min else cant work anyway
		return true;
	else
		return false;
}

Injection ConfigManager::LoadConfigFile(const char dir[])
{
	boost::property_tree::ptree pt;
	boost::property_tree::read_json(dir, pt);

	Injection obj;

	obj.show_console = pt.get<bool>("ProxyJect.loader.show-console");
	obj.disable_log = pt.get<bool>("ProxyJect.loader.disable-logfiles");

	obj.injection_delay = pt.get<int>("ProxyJect.inject.delay");
	obj.target_window = pt.get<std::string>("ProxyJect.inject.target-window");
	obj.target_exe = pt.get<std::string>("ProxyJect.inject.target");
	obj.dll_path = pt.get<std::string>("ProxyJect.inject.dll-path");

	obj.proxy_target = pt.get<std::string>("ProxyJect.proxy.target");
	obj.show_proxy_console = pt.get<bool>("ProxyJect.proxy.show-console");
	obj.disable_proxy_log = pt.get<bool>("ProxyJect.proxy.disable-logfiles");

	bool error_found = false;

	// Basic/Ghetto error check for configuration
	if (obj.target_exe == "none" && obj.target_window == "none")
	{
		common::set_console_visibility(true);
		LOG_CFG("Please enter atleast one of parameters for the target executable(target.exe) or window name!\n");
		error_found = true;
	}
	
	if (strlen(obj.target_exe.c_str()) <= 3 || strlen(obj.target_window.c_str()) < 1 || strlen(obj.proxy_target.c_str()) <= 3 || strlen(obj.dll_path.c_str()) <= 3)
	{
		common::set_console_visibility(true);
		LOG_CFG("One of config parameters is missing or is invalid!\n");
		error_found = true;
	}

	if (obj.dll_path == "none")
	{
		common::set_console_visibility(true);
		LOG_CFG("The dll path cant be 'none'!\n");
		error_found = true;
	}

	if (obj.proxy_target == "none")
	{
		common::set_console_visibility(true);
		LOG_CFG("The proxy executable cant be 'none'!\n");
		error_found = true;
	}

	if (error_found)
	{
		LOG_CFG("Exit in 3 seconds..\n");
		std::this_thread::sleep_for(std::chrono::seconds(3));
		ExitProcess(EXIT_FAILURE);
	}

	return obj;
}

bool ConfigManager::CreateInjectionConfig()
{
	std::filesystem::path file_path = std::getenv("temp");
	file_path /= "proxy-data.temp";

	boost::property_tree::ptree pt;

	pt.put("proxy.disable-log", inject_cfg.disable_proxy_log);
	pt.put("proxy.show-console", inject_cfg.show_proxy_console);
	pt.put("proxy.target", inject_cfg.target_exe);
	pt.put("proxy.window", inject_cfg.target_window);
	pt.put("proxy.dll", inject_cfg.dll_path);

	boost::property_tree::write_json(file_path.generic_string(), pt);

	// Recheck if everythign worked well
	if (boost::filesystem::exists(file_path.generic_string()) && boost::filesystem::file_size(file_path.generic_string()) > 10) // bytes min else cant work anyway
		return true;
	else
		return false;
}


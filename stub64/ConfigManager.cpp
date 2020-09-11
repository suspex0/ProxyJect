#include "ConfigManager.h"
//#include "logger.hpp"

ConfigManager::ConfigManager()
{
	char current_dir[MAX_PATH] = { 0 };

	// Get current module path & remove exe name of it
	GetModuleFileNameA(NULL, current_dir, MAX_PATH);
	PathRemoveFileSpecA(current_dir);

	// Append config name
	PathAppendA(current_dir, "ProxyJect.json");
	config_path = { current_dir };

	// Check if file exist or config is filled with data
	if (!boost::filesystem::exists(config_path) || boost::filesystem::file_size(config_path) < 230) // 230 bytes are min else cant work anyway
	{
		// Create standard config
		if (!CreateConfigFile(current_dir))
		{
			common::set_console_visibility(true);
			LOG_RAW(common::log_color::red, "[CONFIG ERROR] Cant create or write to config file.");
			std::this_thread::sleep_for(std::chrono::seconds(3));
			ExitProcess(EXIT_FAILURE);
		}

		common::set_console_visibility(true);
		LOG_RAW(common::log_color::blue, "\t[FIRST USAGE] Created the standard config file! Please modify it before using ProxyJect.");
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
	// Create the file
	boost::filesystem::ofstream file(dir);
	file.close();

	// Default configuration
	Injection obj;
	obj.injection_delay = 10;
	obj.show_console = true;
	obj.proxy_target = "proxy.exe";
	obj.target_exe = "target.exe";
	obj.target_window = "none";
	obj.dll_path = "C:\\example.dll";

	// Write config
	boost::property_tree::ptree pt;
	pt.put("ProxyJect.inject-delay",		obj.injection_delay);
	pt.put("ProxyJect.show-console",		obj.show_console);
	pt.put("ProxyJect.proxy-executable",	obj.proxy_target);
	pt.put("ProxyJect.target-executable",	obj.target_exe);
	pt.put("ProxyJect.target-window",		obj.target_window);
	pt.put("ProxyJect.dll-path",			obj.dll_path);

	boost::property_tree::write_json(dir, pt);

	// Recheck if everythign worked well
	if (boost::filesystem::exists(config_path) && boost::filesystem::file_size(config_path) > 230) // 230 bytes are min else cant work anyway
		return true;
	else
		return false;
}

Injection ConfigManager::LoadConfigFile(const char dir[])
{
	boost::property_tree::ptree pt;
	boost::property_tree::read_json(dir, pt);

	Injection obj;
	obj.injection_delay			= pt.get<int>("ProxyJect.inject-delay");
	obj.show_console			= pt.get<bool>("ProxyJect.show-console");

	// Convert string for better compatibility with boost libary
	obj.proxy_target			= pt.get<std::string>("ProxyJect.proxy-executable");
	obj.target_exe				= pt.get<std::string>("ProxyJect.target-executable");
	obj.target_window			= pt.get<std::string>("ProxyJect.target-window");
	obj.dll_path				= pt.get<std::string>("ProxyJect.dll-path");

	// "Ghetto" error check for configuration
	if (obj.target_exe == "none" && obj.target_window == "none")
	{
		common::set_console_visibility(true);
		LOG_RAW(common::log_color::red, "[CONFIG ERROR] Please enter atleast one of parameters for the target executable(target.exe) or window name!");
		std::this_thread::sleep_for(std::chrono::seconds(3));
		ExitProcess(EXIT_FAILURE);
	}
	else if (strlen(obj.target_exe.c_str()) <= 3 || strlen(obj.target_window.c_str()) < 1 || strlen(obj.proxy_target.c_str()) <= 3 || strlen(obj.dll_path.c_str()) <= 3)
	{
		common::set_console_visibility(true);
		LOG_RAW(common::log_color::red, "[CONFIG ERROR] One of config parameters is missing or is invalid!");
		std::this_thread::sleep_for(std::chrono::seconds(3));
		ExitProcess(EXIT_FAILURE);
	}
	else if (obj.dll_path == "none")
	{
		common::set_console_visibility(true);
		LOG_RAW(common::log_color::red, "[CONFIG ERROR] The dll path cant be 'none'!");
		std::this_thread::sleep_for(std::chrono::seconds(3));
		ExitProcess(EXIT_FAILURE);
	}
	else if (obj.proxy_target == "none")
	{
		common::set_console_visibility(true);
		LOG_RAW(common::log_color::red, "[CONFIG ERROR] The proxy executable cant be 'none'!");
		std::this_thread::sleep_for(std::chrono::seconds(3));
		ExitProcess(EXIT_FAILURE);
	}

	return obj;
}


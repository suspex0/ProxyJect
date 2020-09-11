#pragma once
#include "main.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")


struct Injection
{
	int					injection_delay = 0;
	bool				show_console;
	bool				disable_log;
	bool				show_proxy_console;
	bool				disable_proxy_log;

	std::string			proxy_target;
	std::string			target_window;
	std::string			target_exe;
	std::string			dll_path;
};


class ConfigManager
{

	public:


		ConfigManager();

		bool		CreateConfigFile(const char dir[]);
		Injection	LoadConfigFile(const char dir[]);
		bool		CreateInjectionConfig();
		bool	    LoadInjectionConfig();

		Injection inject_cfg;
		char current_dir[MAX_PATH] = { 0 };
		char stub64_dir[MAX_PATH] = { 0 };

	private:
		
		boost::filesystem::path config_path;
};
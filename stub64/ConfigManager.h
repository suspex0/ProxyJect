#pragma once
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>
#include <shlwapi.h>

#pragma comment(lib, "Shlwapi.lib")


struct Injection
{
	int					injection_delay;
	bool				show_console;

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

		Injection inject_cfg;

	private:
		
		boost::filesystem::path config_path;
};
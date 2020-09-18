#pragma once
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include <boost/filesystem.hpp>

#include <shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")


struct Injection
{
	DWORD				target_id = 0;
	int					injection_delay = 0;
	bool				show_console;
	bool				disable_log;

	std::string			target_window;
	std::string			target_exe;
	std::string			dll_path;
};


class ConfigManager
{
	public:
		bool LoadInjectionConfig();

		Injection GetInjectConfig() { return inject_cfg; }

	private:
		Injection inject_cfg;
};
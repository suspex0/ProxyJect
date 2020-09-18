#include "ConfigManager.h"
#include "logger.hpp"
#include "xor_encryption.h"


using namespace Common;

bool ConfigManager::LoadInjectionConfig()
{
	std::filesystem::path file_path = std::getenv(XorString("temp"));
	file_path /= XorString("proxy-data.temp");

	try
	{
		if (!boost::filesystem::exists(file_path.generic_string()))
			return false;

		if (boost::filesystem::file_size(file_path.generic_string()) > 164) // bytes min else cant work anyway)
		{
			boost::property_tree::ptree pt;
			boost::property_tree::read_json(file_path.generic_string(), pt);

			inject_cfg.injection_delay = pt.get<int>(XorString("proxy.inject-delay"));
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
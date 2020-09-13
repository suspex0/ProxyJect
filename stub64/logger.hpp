#pragma once
#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "include.hpp"

#define LOGGER_CONSOLE_FORMAT	XorString("%H:%M:%S")
#define LOGGER_FILE_FORMAT		XorString("%Y-%m-%d %H:%M:%S")

namespace common
{
	static void set_console_visibility(bool show_console)
	{
		if (show_console)
			ShowWindow(GetConsoleWindow(), SW_SHOW);
		else
			ShowWindow(GetConsoleWindow(), SW_HIDE);
	}

	enum class log_color : std::uint16_t
	{
		red = FOREGROUND_RED,
		green = FOREGROUND_GREEN,
		blue = FOREGROUND_BLUE,
		gray = 0x00 | FOREGROUND_INTENSITY,
		intensify = FOREGROUND_INTENSITY
	};

	inline log_color operator|(log_color a, log_color b)
	{
		return static_cast<log_color>(static_cast<std::underlying_type_t<log_color>>(a) | static_cast<std::underlying_type_t<log_color>>(b));
	}


	class logger;
	logger *g_logger{};

	class logger
	{
	public:
		explicit logger() :
			m_file_path(std::getenv(XorString("appdata")))
			{
			m_file_path /= XorString("ProxyJect");
			try
			{
				if (!std::filesystem::exists(m_file_path))
				{
					std::filesystem::create_directory(m_file_path);
				}
				else if (!std::filesystem::is_directory(m_file_path))
				{
					std::filesystem::remove(m_file_path);
					std::filesystem::create_directory(m_file_path);
				}

				m_file_path /= XorString("ProxyJect-proxy.log");
				m_file_out.open(m_file_path, std::ios_base::out | std::ios_base::trunc);
			}
			catch (std::filesystem::filesystem_error const&)
			{
			}
			
			FreeConsole(); // Freeconsole if it already exist
			if ((m_did_console_exist = AttachConsole(GetCurrentProcessId())) == false)
				AllocConsole();

			if ((m_console_handle = GetStdHandle(STD_OUTPUT_HANDLE)) != nullptr)
			{
				SetConsoleOutputCP(CP_UTF8);
				if(!common::inject_cfg.disable_log) 
					m_console_out.open(XorString("CONOUT$"), std::ios_base::out | std::ios_base::app);
			}

			g_logger = this;
		}

		~logger()
		{
			if (!m_did_console_exist)
				FreeConsole();

			g_logger = nullptr;
		}

		template <typename T>
		static int convert_ms(const std::chrono::time_point<T>& tp)
		{
			using namespace std::chrono;

			auto dur = tp.time_since_epoch();
			return static_cast<int>(duration_cast<milliseconds>(dur).count());
		}

		static std::string get_time_stamp(const char time_format[])
		{
			auto now = std::chrono::system_clock::now();
			std::time_t current_time = std::chrono::system_clock::to_time_t(now);

			std::tm* time = std::localtime(&current_time);

			char buffer[128];

			size_t string_size = strftime(buffer, sizeof(buffer), time_format, time);

			int ms = convert_ms(now) % 1000;

			string_size += std::snprintf(buffer + string_size, sizeof(buffer) - string_size, XorString(".%03d"), ms);

			return std::string(buffer, buffer + (int)(string_size));
		}

		template <typename ...Args>
		void raw(log_color color, Args const &...args)
		{
			raw_to_console(color, args...);
			//raw_to_file(args...);
		}

		template <typename ...Args>
		void log(log_color color, std::string_view prefix, std::string_view format, Args const &...args)
		{
			auto console_timestamp = get_time_stamp(LOGGER_CONSOLE_FORMAT);	// REMOVED fmt functions for now made sometimes issues in the client.. will redo it in feature.
			auto file_timestamp = get_time_stamp(LOGGER_FILE_FORMAT);		// 

			raw_to_console(color, XorString("\t"), console_timestamp, XorString(" - "), prefix, XorString(" - "), format, args..., XorString("\n"));
			raw_to_file(file_timestamp, XorString(" - "), prefix, XorString(" - "), format, args..., XorString("\n"));
		}
	private:
		template <typename ...Args>
		void raw_to_console(log_color color, Args const &...args)
		{
			if (m_console_handle)
			{
				SetConsoleTextAttribute(m_console_handle, static_cast<std::uint16_t>(color));
			}

			if (m_console_out)
			{
				((m_console_out << args), ...);
				m_console_out << std::flush;
			}
		}

		template <typename ...Args>
		void raw_to_file(Args const &...args)
		{
			if (m_file_out)
			{
				((m_file_out << args), ...);
				m_file_out << std::flush;
			}
		}
	private:
		bool m_did_console_exist{};
		HANDLE m_console_handle{};
		std::ofstream m_console_out;

		std::filesystem::path m_file_path;
		std::ofstream m_file_out;
	};

	template <typename ...Args>
	inline void log_info(std::string_view format, Args const &...args)
	{
		if (g_logger)
		{
			g_logger->log(log_color::blue | log_color::intensify, XorString("Info "), format, args...);
		}
	}

	template <typename ...Args>
	inline void log_error(std::string_view format, Args const &...args)
	{
		if (g_logger)
		{
			g_logger->log(log_color::red | log_color::intensify, XorString("Error"), format, args...);
		}
	}

	template <typename ...Args>
	inline void log_warn(std::string_view format, Args const &...args)
	{
		if (g_logger)
		{
			g_logger->log(log_color::green | log_color::intensify, XorString("Warn "), format, args...);
		}
	}

	template <typename ...Args>
	inline void log_raw(log_color color, Args const &...args)
	{
		if (g_logger)
		{
			g_logger->raw(color, args...);
		}
	}

#define LOG_INFO_IMPL(format, ...) (::common::log_info(format, __VA_ARGS__))
#define LOG_INFO(format, ...) LOG_INFO_IMPL(format, __VA_ARGS__)

#define LOG_ERROR_IMPL(format, ...) (::common::log_error(format, __VA_ARGS__))
#define LOG_ERROR(format, ...) LOG_ERROR_IMPL(format, __VA_ARGS__)

#define LOG_WARN_IMPL(format, ...) (::common::log_warn(format, __VA_ARGS__))
#define LOG_WARN(format, ...) LOG_WARN_IMPL(format, __VA_ARGS__)

#define LOG_RAW_IMPL(color, ...) (::common::log_raw(color, __VA_ARGS__))
#define LOG_RAW(color, ...) LOG_RAW_IMPL(color, __VA_ARGS__)
}

#endif

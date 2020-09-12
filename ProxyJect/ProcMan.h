#pragma once
#include "main.h"

class ProcMan
{
	public:
		DWORD process_id_by_name(std::string name);
		void  close_handle_safe(HANDLE hProc);
		BOOL  is_wow_64(HANDLE hProc);
		void  detected_win_debugger(HANDLE hProc);
		bool  is_running_as_admin();
	private:
};


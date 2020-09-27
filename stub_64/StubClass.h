#pragma once
#include <Windows.h>
#include <thread>
#include <chrono>

//#include "HandleHijack.h"
#include "security.h"
#include "Injector.h"
#include "HandleHijack.h"
#include "ConfigManager.h"
#include "ProcMan.h"
#include "logger.hpp"
#include "skCrypter.h"

class StubClass
{
	public:
		HANDLE main_thread = NULL;
		DWORD  main_thread_id{};

		StubClass();

		int  MainThread();
		void ProxyJectLogo();
		void CheckUpdate();
		void InitInjection();
		bool GetHandleHijacked();
		bool GetHandlebyOpenProcess();
		void ReceiveTargetHandle();
		void InjectDll();

	private:
		ProcMan			proc;
		ConfigManager	cfg;
		Injection		inject_cfg;
		HANDLE			hTarget = NULL;

};


#pragma once
#include "main.h"

// Custome header
#include "logger.hpp"
#include "ProcMan.h"
#include "ConfigManager.h"

class ProxyJect
{

	public:
		ProxyJect();
		~ProxyJect();

		void ProxyJectLogo();
		void WaitForTargets();
		void ReceiveProxyHandle();
		void InjectProxy();
	
	private:
		ConfigManager	cfg;
		ProcMan			proc;
		Injection		inject_cfg;
		DWORD			proxyID = 0;
		DWORD			targetID = 0;
		HANDLE			hProxy = NULL;

		bool inject_create_remote_thread(HANDLE Proc, const char* DllName);
};


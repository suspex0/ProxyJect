#pragma once
#include "main.h"

// Custome header
#include "logger.hpp"
#include "ProcMan.h"
#include "ConfigManager.h"
#include "Injector.h"

class ProxyJect
{

	public:
		ProxyJect();
		~ProxyJect();

		void ProxyJectLogo();
		void WaitForProxy();
		void ReceiveProxyHandle();
		void InjectProxy();
	
	private:
		ConfigManager	cfg;
		ProcMan			proc;
		Injection		inject_cfg;
		DWORD			proxyID = 0;
		DWORD			targetID = 0;
		HANDLE			hProxy = NULL;
		
};

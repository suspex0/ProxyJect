#pragma once
#include "main.h"
#include <Urlmon.h>
#include <WinInet.h>

#pragma comment (lib, "UrlMon.lib")

// Custome header
#include "logger.hpp"
#include "ProcMan.h"
#include "ConfigManager.h"

class ProxyJect
{

	public:
		ProxyJect();

		void ProxyJectLogo();
		void CheckUpdate();
		void WaitForProxy();
		void ReceiveProxyHandle();
		void InjectProxy();
	
	private:
		ConfigManager*	cfg;
		ProcMan			proc;
		Injection		inject_cfg;
		DWORD			proxyID = 0;
		DWORD			targetID = 0;
		HANDLE			hProxy = NULL;
		
};

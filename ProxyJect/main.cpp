#include "main.h"
#include "ProxyJect.h"



int main()
{
	try
	{
		ProxyJect ProxyJect;
		ProxyJect.WaitForProxy();
		ProxyJect.ReceiveProxyHandle();
		ProxyJect.InjectProxy();
	}
	catch (std::exception const& ex)
	{
		MessageBoxA(nullptr, ex.what(), nullptr, MB_OK | MB_ICONEXCLAMATION);
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
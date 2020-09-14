#ifndef SECURITY_H
#define SECURITY_H
#pragma once

#include <Windows.h>
#include <winternl.h>
#include <TlHelp32.h>
#include <iostream>

class security
{
	public:

		security()
		{
			local_seed = GetTickCount64() + GetCurrentProcessId();
		}

		// Generate a random int
		int range_random_int(int min, int max);
		// Changes image size in PE header to confuse memory dumpers
		VOID change_image_size();

	private:
		int local_seed = 0;

};

#endif
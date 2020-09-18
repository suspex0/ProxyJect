#include "security.h"


// Generate a random int
int security::range_random_int(int min, int max) {
	srand(GetTickCount64() + local_seed);
	int n = max - min + 1;
	int remain = RAND_MAX % n;
	int x;
	do
	{
		x = rand();
	} while (x >= RAND_MAX - remain);
	return min + x % n;
}

// Changes image size in PE header to confuse memory dumpers
VOID security::change_image_size()
{
	// Get 64bit PPEB
	PPEB pPeb = (PPEB)__readgsqword(0x60);

	// The following pointer hackery is because winternl.h defines incomplete PEB types
	PLIST_ENTRY InLoadOrderModuleList = (PLIST_ENTRY)pPeb->Ldr->Reserved2[1]; // pPeb->Ldr->InLoadOrderModuleList
	PLDR_DATA_TABLE_ENTRY tableEntry = CONTAINING_RECORD(InLoadOrderModuleList, LDR_DATA_TABLE_ENTRY, Reserved1[0] /*InLoadOrderLinks*/);
	PULONG pEntrySizeOfImage = (PULONG)&tableEntry->Reserved3[1]; // &tableEntry->SizeOfImage
	*pEntrySizeOfImage = (ULONG)((INT_PTR)tableEntry->DllBase + (1048750 + (range_random_int(0, 500)))); // Randomize it
}

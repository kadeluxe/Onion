#include <Windows.h>
#include "Utils.h"

HMODULE g_Module;


void Start(HMODULE h)
{
	g_Module = h;
	Utils::InitConsole("Onion");
}

BOOL WINAPI DllMain(HMODULE hDllHandle, DWORD dwReason, LPVOID lpreserved)
{
	if (dwReason == DLL_PROCESS_ATTACH) {
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Start, hDllHandle, 0, 0);
	}
	return 1;
}

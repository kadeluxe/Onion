#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h>
#include "Utils.h"
#include "Log.h"
#include "InterfaceManager.h"
#include "NetVar.h"
#include "hooks\CreateMove.h"
#include "interfaces\InterfaceClientMode.h"
#include <ctime> //time(0)


HMODULE g_Module;



void Start(HMODULE h)
{
	srand(time(0));
	g_Module = h;
	//Utils::InitConsole("Onion");
	InterfaceManager::GetInterfaces();
	g_NetVars.Init();

	VMT* clientmodehook = new VMT(g_pClientMode);
	clientmodehook->init();
	clientmodehook->setTableHook();
	clientmodehook->hookFunction(24, hkCreateMove);
	Log::Write(DEBUG, "CreateMove hooked!");

	Log::Write("Onion v1 loaded!");
}

BOOL WINAPI DllMain(HMODULE hDllHandle, DWORD dwReason, LPVOID lpreserved)
{
	if (dwReason == DLL_PROCESS_ATTACH) {
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Start, hDllHandle, 0, 0);
	}
	return 1;
}

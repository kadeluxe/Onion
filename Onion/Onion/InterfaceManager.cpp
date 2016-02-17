#include "InterfaceManager.h"
#include <iomanip>
#include <sstream>
#include "Log.h"
#include <Windows.h>
#include "Defines.h"
#include "interfaces\InterfaceClient.h"
#include "interfaces\InterfaceEngine.h"
#include "interfaces\InterfaceEntityList.h"
#include "interfaces\InterfaceTrace.h"
#include "interfaces\InterfaceClientMode.h"
#include "interfaces\InterfaceModelInfo.h"
#include "interfaces\InterfaceGlobals.h"
#include "Address.h"
#include "Sigscan.h"

void InterfaceManager::GetInterfaces()
{
	ASSERT(g_pClient = GetInterface<CClient*>("client.dll", "VClient"));
	ASSERT(g_pEngine = GetInterface<CEngine*>("engine.dll", "VEngineClient"));
	ASSERT(g_pEntityList = GetInterface<CEntityList*>("client.dll", "VClientEntityList"));
	ASSERT(g_pTrace = GetInterface<CTrace*>("engine.dll","EngineTraceClient"));
	ASSERT(g_pModelInfo = GetInterface<CModelInfo*>("engine.dll","VModelInfoClient"));


	//Credits to lilneo/exept/null for the address class and this cool code. It looks neater and stops pointer-arithmetic from being a problem.
	Address CDLLTable = ((Address)g_pClient).To<DWORD*>();
	Address pShutdown = (CDLLTable.As<DWORD*>())[4];
	ASSERT(g_pClientMode = *pShutdown.GetOffset(0xF2).To<CClientMode**>());
	Log::Write(DEBUG, "g_pClientMode found 0x%.8X", g_pClientMode);

	SigScan clientinit((DWORD*)(CDLLTable.As<DWORD*>())[0], 0x100);
	ASSERT(g_pGlobals = **(CGlobals***)((DWORD)clientinit.Scan("\xA3", 1)));
	Log::Write(DEBUG, "g_pGlobals found 0x%.8X", g_pGlobals);

}

template<typename fn> fn InterfaceManager::GetInterface(std::string modulename, std::string interfacename)
{
	CreateInterfaceFn CreateInterface;
	ASSERT(CreateInterface =(CreateInterfaceFn)GetProcAddress(GetModuleHandle(modulename), "CreateInterface"));
	
	fn pInterface = 0;
	for (int i = 100; i > 0; i--)
	{
		std::stringstream intf;
		intf << interfacename << std::setfill('0') << std::setw(3) << i;
		pInterface = (fn)(CreateInterface(intf.str().c_str(), 0));
		if (pInterface) { 
			Log::Write(DEBUG,"%s found 0x%.8X", intf.str().c_str(), pInterface); 
			break;
		}
	}

	return pInterface;
}
#include "InterfaceManager.h"
#include <iomanip>
#include <string>

void InterfaceManager::GetInterfaces()
{

}

template<typename fn> fn InterfaceManager::GetInterface(char* modulename, char* interfacename)
{
	CreateInterfaceFn CreateInterface = (CreateInterfaceFn)GetProcAddress(GetModuleHandle(modulename), "CreateInterface");
	fn pInterface = 0;
	for (int i = 100; i > 0; i--)
	{
		std::stringstream intf;
		intf << interfacename << std::setfill('0') << std::setw(3) << i;
		pInterface = (fn)(CreateInterface(intf.str().c_str(), 0));
		if (pInterface) { printf("%s found 0x%.8X\n", intf.str().c_str(), pInterface); }
	}

	return pInterface;
}
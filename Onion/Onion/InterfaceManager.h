#pragma once


typedef void* (*CreateInterfaceFn)(const char* name, int* returncode);
class InterfaceManager
{
public:
	static void GetInterfaces();
	template<typename fn> static fn GetInterface(char* modulename, char* interfacename);
};

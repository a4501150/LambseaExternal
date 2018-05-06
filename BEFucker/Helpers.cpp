#include "stdafx.h"
#include "Helpers.h"
#include <Detours.h>
#include <fstream>

using namespace std;

ofstream myfile;

//HOOKING
void Helpers::HookFunction(PVOID *oFunction, PVOID pDetour)
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(oFunction, pDetour);
	DetourTransactionCommit();
}
void Helpers::UnhookFunction(PVOID *oFunction, PVOID pDetour)
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach(oFunction, pDetour);
	DetourTransactionCommit();
}





//MISCELLANEOUS

void Helpers::Log(char* szMessage)
{
	std::cout << "[+] " << szMessage << std::endl;
}

void Helpers::LogToFile(char * szMessage)
{
	myfile.open("c:\\intel\\GUID", std::ios_base::app);
	myfile << szMessage << endl;
	myfile.close();
}

void Helpers::LogToFileEx(const char * szMessage)
{
	myfile.open("c:\\intel\\GUID", std::ios_base::app);
	myfile << szMessage << endl;
	myfile.close();
}

void Helpers::LogAddressToFile(char * szMessage, uintptr_t iAddress)
{
	myfile.open("c:\\intel\\GUID", std::ios_base::app);
	myfile << szMessage << ": 0x" << hex << iAddress << endl;
	myfile.close();
}

void Helpers::LogValueToFile(char * szMessage, DWORD_PTR value)
{
	myfile.open("c:\\intel\\GUID", std::ios_base::app);
	myfile << szMessage << value << endl;
	myfile.close();
}

void Helpers::LogAddress(char* szName, uintptr_t iAddress)
{
	std::cout << "[+] " << szName << ": 0x" << std::hex << iAddress << std::endl;
}
void Helpers::LogError(char* szMessage)
{
	std::cout << "[Error] " << szMessage << std::endl;
}

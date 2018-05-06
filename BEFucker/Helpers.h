#pragma once
#include "stdafx.h"

namespace Helpers
{
	void LogAddress(char* szName, uintptr_t iAddress);
	void LogError(char* szMessage);
	void Log(char* szMessage);
	void LogToFile(char* szMessage);
	void LogToFileEx(const char * szMessage);
	void LogAddressToFile(char * szMessage, uintptr_t iAddress);

	void LogValueToFile(char * szMessage, DWORD_PTR value);

	void HookFunction(PVOID *oFunction, PVOID pDetour);
	void UnhookFunction(PVOID *oFunction, PVOID pDetour);


}

namespace Iat_hook
{

	inline void** find(const char* function, HMODULE module)
	{
		if (!module)
			module = GetModuleHandle(0);

		PIMAGE_DOS_HEADER img_dos_headers = (PIMAGE_DOS_HEADER)module;
		PIMAGE_NT_HEADERS img_nt_headers = (PIMAGE_NT_HEADERS)((BYTE*)img_dos_headers + img_dos_headers->e_lfanew);
		PIMAGE_IMPORT_DESCRIPTOR img_import_desc = (PIMAGE_IMPORT_DESCRIPTOR)((BYTE*)img_dos_headers + img_nt_headers->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);
		if (img_dos_headers->e_magic != IMAGE_DOS_SIGNATURE)
			printf("ERROR: e_magic is no valid DOS signature\n");

		for (IMAGE_IMPORT_DESCRIPTOR *iid = img_import_desc; iid->Name != 0; iid++) {
			for (int func_idx = 0; *(func_idx + (void**)(iid->FirstThunk + (size_t)module)) != nullptr; func_idx++) {
				char* mod_func_name = (char*)(*(func_idx + (size_t*)(iid->OriginalFirstThunk + (size_t)module)) + (size_t)module + 2);
				const intptr_t nmod_func_name = (intptr_t)mod_func_name;
				if (nmod_func_name >= 0) {
					if (!::strcmp(function, mod_func_name))
						return func_idx + (void**)(iid->FirstThunk + (size_t)module);
				}
			}
		}

		return 0;

	}

	inline uintptr_t detour_iat_ptr(const char* function, void* newfunction, HMODULE module = 0)
	{
		auto&& func_ptr = find(function, module);
		if (*func_ptr == newfunction || *func_ptr == nullptr)
			return 0;

		uintptr_t ret = (uintptr_t)*func_ptr;
		*func_ptr = newfunction;

		return ret;
	}
};
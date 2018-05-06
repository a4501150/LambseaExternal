#include "HandleGetter.h"
NTSTATUS EnumerateHandles(ENUM_HANDLE_CALLBACK callback)
{
	NTSTATUS                    status = STATUS_UNSUCCESSFUL;
	PVOID                       buffer = NULL;
	ULONG                       bufferSize = 0;

	do {
		status = NtQuerySystemInformation(SystemHandleInformation, buffer, bufferSize, &bufferSize);
		if (!NT_SUCCESS(status)) {
			if (status == STATUS_INFO_LENGTH_MISMATCH) {
				if (buffer != NULL)
					VirtualFree(buffer, 0, MEM_RELEASE);
				buffer = VirtualAlloc(NULL, bufferSize, MEM_COMMIT, PAGE_READWRITE);
				continue;
			}
			break;
		}
		else {
			PSYSTEM_HANDLE_INFORMATION handleInfo = (PSYSTEM_HANDLE_INFORMATION)buffer;
			for (ULONG i = 0; i < handleInfo->NumberOfHandles; i++) {
				auto handle = &handleInfo->Handles[i];
				status = callback(handle);
				if (NT_SUCCESS(status))
					break;
			}
			break;
		}
	} while (true);

	if (buffer != NULL)
		VirtualFree(buffer, 0, MEM_RELEASE);
	return status;
}

HANDLE GetHandleToProcess(LPWSTR process)
{
	HANDLE hProcess = NULL;

	EnumerateHandles([&](PSYSTEM_HANDLE_TABLE_ENTRY_INFO handle) -> NTSTATUS {

		if (GetCurrentProcessId() != handle->UniqueProcessId) return STATUS_UNSUCCESSFUL;

		BOOL        found = FALSE;
		PVOID       buffer = NULL;
		ULONG       bufferSize = 0x100;
		NTSTATUS    status;


		do {
			buffer = VirtualAlloc(NULL, bufferSize, MEM_COMMIT, PAGE_READWRITE);
			status = NtQueryObject((HANDLE)handle->HandleValue, ObjectTypeInformation, buffer, bufferSize, &bufferSize);
			if (NT_SUCCESS(status)) break;
			if (buffer != NULL)
				VirtualFree(buffer, 0, MEM_RELEASE);
		} while (status == STATUS_INFO_LENGTH_MISMATCH);

		BOOL isProcess = FALSE;
		
		try
		{
			isProcess = std::wstring(((POBJECT_TYPE_INFORMATION)buffer)->TypeName.Buffer) == std::wstring(L"Process");
		}
		catch (...)
		{
			isProcess = FALSE;
		}

		if (isProcess) {
			WCHAR processPath[MAX_PATH] = {};

			if (GetModuleFileNameExW((HANDLE)handle->HandleValue, NULL, processPath, MAX_PATH)) {
				LPWSTR filename = PathFindFileNameW(processPath);
				if (!wcscmp(filename, process)) {
					found = TRUE;
					hProcess = (HANDLE)handle->HandleValue;
				}
			}
		}

		if (buffer != NULL)
			VirtualFree(buffer, 0, MEM_RELEASE);

		if (found)
			return STATUS_SUCCESS;

		return STATUS_UNSUCCESSFUL;

	});

	return hProcess;
}

void PrintSelfToOthersHandles()
{
	printf("---------------------Begin------------------------\n");

	EnumerateHandles([](PSYSTEM_HANDLE_TABLE_ENTRY_INFO handle) {

		if (GetCurrentProcessId() != handle->UniqueProcessId) return STATUS_UNSUCCESSFUL;

		//
		// Print name information
		// 
		PVOID       buffer = NULL;
		ULONG       bufferSize = 0x100;
		NTSTATUS    status;

		//do {
		//	buffer = VirtualAlloc(NULL, bufferSize, MEM_COMMIT, PAGE_READWRITE);
		//	status = NtQueryObject((HANDLE)handle->HandleValue, (OBJECT_INFORMATION_CLASS)1/*ObjectNameInformation*/, buffer, bufferSize, &bufferSize);
		//	if (NT_SUCCESS(status)) break;
		//	if (buffer != NULL)
		//		VirtualFree(buffer, 0, MEM_RELEASE);
		//} while (status == STATUS_INFO_LENGTH_MISMATCH);

		//if (!NT_SUCCESS(status))
		//	goto EXIT;

		//printf("Handle Object:  0x%p\n", handle->Object);
		//printf("Handle Value:   0x%04X\n", handle->HandleValue);
		//printf("Granted Access: 0x%08X\n", handle->GrantedAccess);
		//printf("Name: %ws\n", ((POBJECT_NAME_INFORMATION)buffer)->Name.Buffer);
		//VirtualFree(buffer, 0, MEM_RELEASE);

		do {
			buffer = VirtualAlloc(NULL, bufferSize, MEM_COMMIT, PAGE_READWRITE);
			status = NtQueryObject((HANDLE)handle->HandleValue, ObjectTypeInformation, buffer, bufferSize, &bufferSize);
			if (NT_SUCCESS(status)) break;
			if (buffer != NULL)
				VirtualFree(buffer, 0, MEM_RELEASE);
		} while (status == STATUS_INFO_LENGTH_MISMATCH);

		if (!NT_SUCCESS(status))
			goto EXIT;


		{
			std::wstring typeName = ((POBJECT_TYPE_INFORMATION)buffer)->TypeName.Buffer;
			if (typeName.find(L"rocess") == std::wstring::npos && typeName.find(L"hread") == std::wstring::npos)
				goto EXIT;

			printf("Handle Object:  0x%p\n", handle->Object);
			printf("Handle Value:   0x%04X\n", handle->HandleValue);
			printf("Granted Access: 0x%08X\n", handle->GrantedAccess);
			printf("Type:           %ws\n", typeName.c_str());
		}
		//
		// 7 is the process type index on my system. It's possible that this is different value on your system.
		// You can compare the TypeName to "Process" instead of doing it like this
		//

		if (std::wstring(((POBJECT_TYPE_INFORMATION)buffer)->TypeName.Buffer) == std::wstring(L"Process")) {
			WCHAR processPath[MAX_PATH];
			//
			// The handle needs to have PROCESS_QUERY_INFORMATION and PROCESS_VM_READ
			// access, otherwise this call fails
			// 
			if (GetModuleFileNameExW((HANDLE)handle->HandleValue, NULL, processPath, MAX_PATH)) {
				printf("Process:        %ws\n", PathFindFileNameW(processPath));
			}
		}

		if (std::wstring(((POBJECT_TYPE_INFORMATION)buffer)->TypeName.Buffer) == std::wstring(L"Thread")) {
			printf("HostProcess ID: %d\n", GetProcessIdOfThread((HANDLE)handle->HandleValue));
		}

		printf("-------------------------------------------------\n");

	EXIT:
		if (buffer != NULL)
			VirtualFree(buffer, 0, MEM_RELEASE);

		return STATUS_UNSUCCESSFUL;
	});
}

bool EnablePrivilege(
	LPCWSTR lpPrivilegeName
)
{
	TOKEN_PRIVILEGES Privilege;
	HANDLE hToken;
	DWORD dwErrorCode;

	Privilege.PrivilegeCount = 1;
	Privilege.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if (!LookupPrivilegeValueW(NULL, lpPrivilegeName,
		&Privilege.Privileges[0].Luid))
		return GetLastError();

	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES, &hToken))
		return GetLastError();

	if (!AdjustTokenPrivileges(hToken, FALSE, &Privilege, sizeof(Privilege),
		NULL, NULL)) {
		dwErrorCode = GetLastError();
		CloseHandle(hToken);
		return dwErrorCode;
	}

	CloseHandle(hToken);
	return TRUE;
}

bool DisablePrivilege(
	LPCWSTR lpPrivilegeName
)
{
	TOKEN_PRIVILEGES Privilege;
	HANDLE hToken;
	DWORD dwErrorCode;

	Privilege.PrivilegeCount = 1;
	Privilege.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED_BY_DEFAULT;
	if (!LookupPrivilegeValueW(NULL, lpPrivilegeName,
		&Privilege.Privileges[0].Luid))
		return GetLastError();

	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES, &hToken))
		return GetLastError();

	if (!AdjustTokenPrivileges(hToken, FALSE, &Privilege, sizeof(Privilege),
		NULL, NULL)) {
		dwErrorCode = GetLastError();
		CloseHandle(hToken);
		return dwErrorCode;
	}

	CloseHandle(hToken);
	return TRUE;
}
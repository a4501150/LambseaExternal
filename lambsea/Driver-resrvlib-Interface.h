#pragma once
#include "stdafx.h"

typedef BOOL(WINAPI *reserve_init)();

typedef BOOL(WINAPI *reserve_module)
(
	ULONG uProc, 
	LPCSTR sModule, 
	ULONGLONG *phmModule, 
	ULONG *puModSize
);

typedef BOOL(WINAPI *reserve_alloc)
(
	ULONG uProc, 
	ULONGLONG &pbAddr, 
	ULONG nSize,
	ULONG uProtect
);

typedef BOOL(WINAPI *reserve_free)
(
	ULONG uProc, 
	ULONGLONG pbAddr, 
	ULONG nSize
);

typedef BOOL(WINAPI *reserve_read)
(
	ULONG uProc, 
	ULONGLONG pbAddr, 
	PVOID pbData,
	ULONG uSize
);

typedef BOOL(WINAPI *reserve_write)
(
	ULONG uProc, 
	ULONGLONG pbAddr, 
	PVOID pbData,
	ULONG uSize
);
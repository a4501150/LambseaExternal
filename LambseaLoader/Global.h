#pragma once
#if !defined UNICODE
#error ANSI build is not supported
#endif

#if (_MSC_VER >= 1900) 
#ifdef _DEBUG
#pragma comment(lib, "vcruntimed.lib")
#pragma comment(lib, "ucrtd.lib")
#else
//#pragma comment(lib, "libvcruntime.lib")
//#pragma comment(lib, "vcruntime.lib")
//#pragma comment(lib, "ucrt.lib")
#endif
#endif


//disable nonmeaningful warnings.
#pragma warning(disable: 4005) // macro redefinition
#pragma warning(disable: 4054) // %s : from function pointer %s to data pointer %s
#pragma warning(disable: 4055) // %s : from data pointer %s to function pointer %s
#pragma warning(disable: 4152) // nonstandard extension, function/data pointer conversion in expression
#pragma warning(disable: 4201) // nonstandard extension used : nameless struct/union
#pragma warning(disable: 6102) // Using %s from failed function call at line %u
#pragma warning(disable: 6320) // exception-filter expression is the constant EXCEPTION_EXECUTE_HANDLER

#define _CRT_SECURE_NO_WARNINGS

#include <Windows.h>
#include <ntstatus.h>
#include "ntos.h"
#include "hde\hde64.h"
#include <stdio.h>
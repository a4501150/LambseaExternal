#pragma once
#include "stdafx.h"
#include <iostream>
#include "DriverInterface.h"
#define INRANGE(x,a,b)		(x >= a && x <= b) 
#define getBits( x )		(INRANGE(x,'0','9') ? (x - '0') : ((x&(~0x20)) - 'A' + 0xa))
#define getByte( x )		(getBits(x[0]) << 4 | getBits(x[1]))
#define DriverInterface LambseaDriver
//Specific for PUBG
class Memory {

public:
	
	Memory() {
		DriverInterface::Init("TslGame.exe",L"TslGame.exe");
		//DriverInterface::LoadDriver();
		baseaddr = DriverInterface::DriverGetModulebase(moduleSize);
		wprintf_s(L"内存组件加载完毕\n");
		wprintf_s(L"Found TslGame.exe -> Gamebase Address At 0x%I64X, Size is %I64d\n", baseaddr, moduleSize);
		moduleBuffer = { 0 };
	}

	~Memory() {
		DriverInterface::UnLoadDriver();
		wprintf_s(L"内存组件释放完毕\n");
	}

	template<typename T> 
	inline T read(DWORD_PTR Address)
	{
		T buffer;

		auto ret = DriverInterface::DriverReadMemory(Address, &buffer, sizeof(T));

		if (ret)
			return buffer;
		else
			return T();

	}

	inline bool readSTR(DWORD_PTR Address, char* buff, DWORD buffSize)
	{
		auto ret = DriverInterface::DriverReadMemory(Address, buff, buffSize);
		return ret;
	}

	inline bool readWSTR(DWORD_PTR Address, wchar_t* buff, DWORD buffSize)
	{
		auto ret = DriverInterface::DriverReadMemory(Address, buff, buffSize);
		return ret;
	}

	template<typename T>
	bool write(DWORD_PTR address, T buff)
	{
		auto ret = DriverInterface::DriverWriteMemory(address, &buff, sizeof(T));
		return ret;
	}

	inline DWORD_PTR GetProcBase()
	{
		return this->baseaddr;
	}

	/* Subroutine For Comparing Bytes From The Find Pattern Routine */
	/* Compares Bytes To Pattern For Pattern Length */
	bool CompareBytes(const BYTE* pData, const char* pattern)
	{
		for (; *pattern; *pattern != ' ' ? ++pData : pData, ++pattern)
		{
			if (*pattern == ' ' || *pattern == '?')
				continue;
			if (*pData != getByte(pattern))
				return false;
			++pattern;
		}
		return true;
	}

	/* Find Desired Pattern And Return The Address of The Start Of The Pattern */
	uint64_t FindPattern(const char* pattern)
	{
		size_t size = 65535000;

		BYTE* data = (BYTE*)malloc(size);

		DriverInterface::DriverReadMemory(GetProcBase(), data, size);

		for (DWORD i = 0; i < size; i++)
		{
			if (CompareBytes((const BYTE*)(data + i), pattern))
			{
				free(data);
				uint64_t add = GetProcBase() + i;
				return add;
			}
		}
		free(data);
		return 0;
	}


	void addVEH(PVECTORED_EXCEPTION_HANDLER func) {
		AddVectoredExceptionHandler(0, func);
	}

	void rmVEH(PVECTORED_EXCEPTION_HANDLER func) {
		RemoveVectoredExceptionHandler(func);
	}


private:
	BYTE* moduleBuffer;
	SIZE_T moduleSize;
	DWORD_PTR baseaddr;
};

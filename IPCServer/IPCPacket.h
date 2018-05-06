#pragma once
#include "stdafx.h"

#define MAXPIPEFILESIZE 0x500
#define MAXDATASIZE 0x200

#define PIPENAME L"\\\\.\\pipe\\lambsea"

enum OrderType 
{ 
	IPCPing,
	IPCGetBaseOrder,
	IPCReadOrder,
	IPCWriteOrder
};

typedef struct IPCPacket IPCPacket;
struct IPCPacket {
	int Order = 0;
	int size = 0;
	DWORD_PTR Address = 0;
	unsigned char data[MAXDATASIZE] = {0};
};
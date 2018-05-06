#pragma once
#include "IPCPacket.h"

class ServerGateway
{
public:
	ServerGateway();
	~ServerGateway();

	void Init();
	BOOL AnswerPing(IPCPacket incomingPacket);
	BOOL AnswerGetBaseaddress();
	BOOL AnswerReadProcessMemory(IPCPacket incomingPacket);
	BOOL AnswerWriteProcessMemory(IPCPacket incomingPacket);
	
private:
	HANDLE hNamedPipe;
	HANDLE hGame;
	LPWSTR PipeName;
};
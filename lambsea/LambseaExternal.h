#pragma once
#include "stdafx.h"
//-------------定义初始化函数-------------//
void WINAPI Initialize();
void WINAPI Release();


//--------开关----------------
void WINAPI Switch();
BOOL WINAPI ConsoleHandler(DWORD CtrlType);

////------自动获取Offset
//BOOL WINAPI SetupOffsets();

//------主循环
void WINAPI MainLoop();

//----多线程读取camera
void WINAPI CameraLoop();
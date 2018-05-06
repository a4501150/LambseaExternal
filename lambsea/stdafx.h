// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once
#define _CRT_SECURE_NO_WARNINGS 1

//windows SDK
#include "targetver.h"
#include <Windows.h>

//windows common lib API
#include <Psapi.h>
#pragma comment(lib, "Psapi.lib")
#include <TlHelp32.h>

//d3d9 - related
#include <d3d9.h>
#include <d3dx9.h>
#include <Dwmapi.h> 

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"Dwmapi.lib")


#include <stdio.h>
#include <tchar.h>
// TODO:  在此处引用程序需要的其他头文件
#include <atlconv.h> //A2W W2A

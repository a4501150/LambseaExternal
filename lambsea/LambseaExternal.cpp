#include "stdafx.h"
#include "LambseaExternal.h"
#include <iostream>
#include "GameBase.h"
#include "Injection.h"
#include "UndocumentNt.h"

#include "detours.h"
#pragma comment(lib, "detours.lib")

HANDLE hThreadMainLoop;

DWORD GetProcessIDbyName(wchar_t * proc)
{
	auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	auto pe = PROCESSENTRY32W{ sizeof(PROCESSENTRY32W) };

	if (Process32First(snapshot, &pe)) {
		do {
			if (!_wcsicmp(proc, pe.szExeFile)) {
				CloseHandle(snapshot);
				return pe.th32ProcessID;
			}
		} while (Process32Next(snapshot, &pe));
	}
	CloseHandle(snapshot);
	return 0;
}
/*
void Hook(void * oFunction, PVOID pDetour)
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(reinterpret_cast<PVOID*>(oFunction), pDetour);
	DetourTransactionCommit();
}

void UnHook(void * oFunction, PVOID pDetour)
{
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach(reinterpret_cast<PVOID*>(oFunction), pDetour);
	DetourTransactionCommit();
}


typedef HRESULT(__stdcall *fD3D11Present)
(
	IDXGISwapChain* pSwapChain,
	UINT SyncInterval,
	UINT Flags
	);

fD3D11Present D3d11PresentOrig = NULL;


HRESULT D3D11PresentHook(IDXGISwapChain * pSwapChain, UINT SyncInterval, UINT Flags)
{
#ifndef D3D9RENDER
	if (GameBase::render == NULL)
	{
		GameBase::render = new D3D11Renderer(pSwapChain);
		GameBase::render->Initialize();
		GameBase::screen_width = GameBase::render->GetScreenWidth();
		GameBase::screen_height = GameBase::render->GetScreenHeight();
	}

	GameBase::render->BeginScene();

	//this is actully not nessary since I setup check ptr in Memory Class. BTW, this can prevent any crash in entityloop
	try
	{
		GameBase::EntityLoop();
	}
	catch (...)
	{
		wprintf_s(L"Error In EntityLoop\n");
	}

	GameBase::render->EndScene();
#endif // !D3D9RENDER
	return D3d11PresentOrig(pSwapChain, SyncInterval, Flags);
}


void SetupD3D11Virables() {

	//HWND hWnd = FindWindow(L"UnrealWindow", NULL);
	HWND hWnd = GetForegroundWindow();

#pragma region Initialise DXGI_SWAP_CHAIN

	static ID3D11Device *pDevice = NULL;
	static ID3D11DeviceContext *pContext = NULL;
	static IDXGISwapChain* pSwapChain = NULL;

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(scd));

	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	scd.OutputWindow = hWnd;
	scd.SampleDesc.Count = 1; // Set to 1 to disable multisampling
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Windowed = ((GetWindowLongPtr(hWnd, GWL_STYLE) & WS_POPUP) != 0) ? false : true;
	scd.BufferDesc.Width = 1;
	scd.BufferDesc.Height = 1;
	scd.BufferDesc.RefreshRate.Numerator = 0;
	scd.BufferDesc.RefreshRate.Denominator = 1;

	if (FAILED(D3D11CreateDeviceAndSwapChain
	(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		&featureLevel,
		1,
		D3D11_SDK_VERSION,
		&scd,
		&pSwapChain,
		&pDevice,
		NULL,
		&pContext
	)))
	{
		wprintf_s(L"D3D11CreateDevice Failed\n");
		return;
	}

#pragma endregion

	//resolve retarded vtable
	DWORD_PTR*  pSwapChainVT = reinterpret_cast<DWORD_PTR*>(pSwapChain);
	pSwapChainVT = reinterpret_cast<DWORD_PTR*>(pSwapChainVT[0]);
	D3d11PresentOrig = reinterpret_cast<fD3D11Present>(pSwapChainVT[8]);
	Hook(&D3d11PresentOrig, D3D11PresentHook);

	wprintf_s(L"D3D11Render Init! original %I64X,Hook at %I64X\n", D3d11PresentOrig, D3D11PresentHook);
}
*/

bool WipeUSNJournalData(std::string szVolume)
{
	if (szVolume.empty())
		return false;

	HANDLE hVolume = CreateFileA(szVolume.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	if (hVolume == INVALID_HANDLE_VALUE)
		return false;

	DWORD dwBuffer = 0;
	USN_JOURNAL_DATA USNJournalData;
	if (DeviceIoControl(hVolume, FSCTL_QUERY_USN_JOURNAL, NULL, 0, &USNJournalData, sizeof(USN_JOURNAL_DATA), &dwBuffer, NULL))
	{
		CloseHandle(hVolume);
		return false;
	}

	DELETE_USN_JOURNAL_DATA USNJournalDataDelete;
	USNJournalDataDelete.DeleteFlags = USN_DELETE_FLAG_DELETE;
	USNJournalDataDelete.UsnJournalID = USNJournalData.UsnJournalID;

	bool bWipedData = (DeviceIoControl(hVolume, FSCTL_DELETE_USN_JOURNAL, &USNJournalDataDelete, sizeof(DELETE_USN_JOURNAL_DATA), NULL, 0, &dwBuffer, NULL) == 0) ? true : false;
	CloseHandle(hVolume);

	return bWipedData;
}

void Initialize()
{
	if(!GetConsoleWindow())
		AllocConsole();

	ShowWindow(GetConsoleWindow(), SW_SHOWNORMAL);

	SetConsoleCtrlHandler((PHANDLER_ROUTINE)ConsoleHandler, TRUE);

	freopen("CON", "w", stdout);
	//SetConsoleTitle(L"");
	setlocale(LC_ALL, "");
	wprintf_s(L"正在初始化...\n"); //intiializing

	while (GetProcessIDbyName(L"BEService.exe") || GetProcessIDbyName(L"TslGame.exe")) {
		wprintf_s(L"请关闭游戏...\n");
		wprintf_s(L"Please Close Game Completely...\n");
		Sleep(1000 * 2);
	}

	wprintf_s(L"正在加载驱动...\n"); //intiializing
	auto retDriver = DriverInterface::LoadDriver();
	if (!retDriver)
	{
		wprintf_s(L"Cannot Load Driver...Report Back!\n"); //intiializing
		system("pause");
		ExitProcess(0);
	}

	int BEPID = 0;
	while (! (BEPID = GetProcessIDbyName(L"BEService.exe")) || ! (DriverInterface::ProcID = GetProcessIDbyName(L"TslGame.exe")) ) {
		wprintf_s(L"请打开游戏...\n");
		wprintf_s(L"Please Open Game Now...\n");
		Sleep(1000 * 2);
	}

	//SetupD3D11Virables();

	wprintf_s(L"正在加载内存组件...\n");
	GameBase::mem = new Memory();

	wprintf_s(L"正在读取游戏初始数据...\n");

	//while(!GameBase::SetupOffsets())
	//	Sleep(1000);

	GameBase::WorldOffset = 0x3d87f10;
	GameBase::GNameOffset = 0x3DF1F38;

	GameBase::DumpGNAME();

	wprintf_s(L"正在加载开关...\n");
	CreateThread(NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(Switch), NULL, NULL, NULL);
	//Release();

	VirtualProtect(GetModuleHandle(NULL), 0x1000, PAGE_NOACCESS, NULL);
	

	while (true) {
		Sleep(1000 * 3);
	}


}

void Release()
{
	wprintf_s(L"释放资源成功.\n");
	ShowWindow(GetConsoleWindow(), SW_HIDE);
	FreeConsole();
}

void ReAllocCheat() {

	if(GameBase::render)
		DestroyWindow(GameBase::render->GetSpawnedWindow());

	TerminateThread(hThreadMainLoop, 0);

	delete GameBase::mem;
	DriverInterface::LoadDriver();
	GameBase::mem = new Memory();

	delete GameBase::render;
	GameBase::render = nullptr;

	GameBase::render = new D3D9Render();
	GameBase::render->SetupWindow();

	hThreadMainLoop = CreateThread(NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(MainLoop), NULL, NULL, NULL);
}


typedef UNICODE_STRING *PUNICODE_STRING;
NTSTATUS(NTAPI* NtUnloadDriver)(_In_ PUNICODE_STRING DriverServiceName);
VOID(NTAPI* RtlInitUnicodeString)( PUNICODE_STRING DestinationString, PCWSTR SourceString);


void UnLoadMyDriver() {

	delete GameBase::mem;
	NtUnloadDriver = (decltype(NtUnloadDriver))GetProcAddress(GetModuleHandle(L"ntdll.dll"), "NtUnloadDriver");
	RtlInitUnicodeString = (decltype(RtlInitUnicodeString))GetProcAddress(GetModuleHandle(L"ntdll.dll"), "RtlInitUnicodeString");
	std::string ServiceRegKey = "\\Registry\\Machine\\System\\CurrentControlSet\\Services\\Lambsea";
	UNICODE_STRING usKey{ 0 };
	std::wstring ServiceRegKeyW(ServiceRegKey.begin(), ServiceRegKey.end());
	RtlInitUnicodeString(&usKey, ServiceRegKeyW.c_str());
	NtUnloadDriver(&usKey);
	system("fltmc unload Lambsea");
	DeleteFile(L"Lambsea.sys");

}

void Switch()
{
	MSG msg;

	ReAllocCheat();

	while (true)
	{

		if (GetAsyncKeyState(VK_F4))
		{
			GameBase::bPlayer = !GameBase::bPlayer;
			if (GameBase::bPlayer)
			{
				//Beep(500, 800);
				wprintf_s(L"人物透视开\n");
			}
			else
			{
				//Beep(500, 800);
				wprintf_s(L"人物透视关\n");
			}
		}

		if (GetAsyncKeyState(VK_F5))
		{
			GameBase::bLoot = !GameBase::bLoot;
			if (GameBase::bLoot)
			{
				//Beep(500, 800);
				wprintf_s(L"物品透视开\n");
			}
			else
			{
				//Beep(500, 800);
				wprintf_s(L"物品透视关\n");
			}
		}

		if (GetAsyncKeyState(VK_F6))
		{
			GameBase::bVehicle = !GameBase::bVehicle;
			if (GameBase::bVehicle)
			{
				//Beep(500, 800);
				wprintf_s(L"交通工具透视开\n");
			}
			else
			{
				//Beep(500, 800);
				wprintf_s(L"交通工具透视关\n");
			}


		}

		if (GetAsyncKeyState(VK_F7))
		{
			GameBase::bBot = !GameBase::bBot;
			if (GameBase::bBot)
			{
				//Beep(500, 800);
				wprintf_s(L"自瞄关\n");
			}
			else
			{
				//Beep(500, 800);
				wprintf_s(L"自瞄关\n");
			}
		}

		if (GetAsyncKeyState(VK_F8))
		{
			//Beep(500, 800);
			if (GameBase::streamerMode)
				GameBase::modifyWeaponStreamer();
			else
				GameBase::modifyWeapon();
			wprintf_s(L"无后座刷新\n");
		}

		if (GetAsyncKeyState(VK_F9))
		{
			//Beep(500, 800);
			GameBase::bMenu = !GameBase::bMenu;
			wprintf_s(L"Menu 开关\n");
		}

		if (GetAsyncKeyState(VK_F10))
		{
			//Beep(500, 800);
			GameBase::streamerMode = !GameBase::streamerMode;
			wprintf_s(L"主播模式 开关\n");
		}

		if (GetAsyncKeyState(VK_F3))
		{
			//Beep(500, 800);
			wprintf_s(L"重置辅助!\n");
			ReAllocCheat();
		}

		if (GetAsyncKeyState(VK_NUMPAD1))
		{
			if(GameBase::recoilReduceCoe<1.0f)
				GameBase::recoilReduceCoe += 0.1f;
		}

		if (GetAsyncKeyState(VK_NUMPAD2))
		{
			if (GameBase::recoilReduceCoe>=0.0f)
				GameBase::recoilReduceCoe -= 0.1f;
		}

		if (GetAsyncKeyState(VK_NUMPAD3))
		{
			GameBase::aimHead = !GameBase::aimHead;
		}

		if ( NtQueryProcessStatus((HANDLE)DriverInterface::ProcID) == (HANDLE)0xdeadbeef)
		{
			char buffer[64] = { 0 };
			if (!DriverInterface::DriverReadMemory(GameBase::mem->GetProcBase(), buffer, sizeof(buffer)))
			{
				wprintf_s(L"游戏已关闭.\n");
				UnLoadMyDriver();
				ExitProcess(0);
			}
		}

		ZeroMemory(&msg, sizeof(MSG));
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		Sleep(500);

	}
}

BOOL WINAPI ConsoleHandler(DWORD CtrlType) {

	if (CtrlType == CTRL_CLOSE_EVENT || CtrlType == CTRL_BREAK_EVENT || CtrlType == CTRL_C_EVENT)
	{
		UnLoadMyDriver();
		return TRUE;
	}
	return FALSE;
}


void MainLoop()
{
	//while (true) {

	//	if(auto hwnd = FindWindow(L"UnrealWindow", 0))
	//		SetWindowPos(hwnd, GameBase::render->GetSpawnedWindow(), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	//	GameBase::render->Render(GameBase::EntityLoop);
	//	Sleep(6);

	//}
}

void CameraLoop()
{
	while (true) 
	{
		GameBase::UpdateAddresses();
		Sleep(5);
	}
}

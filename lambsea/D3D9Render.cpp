#pragma once
#include "D3D9Render.h"
#include "GameBase.h"
#include "XorStrings.h"

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message)
	{
	case WM_PAINT:
		//render();
		break;

	case WM_CREATE:
		//DwmExtendFrameIntoClientArea(hWnd, &margin);
		break;

	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}



D3D9Render::D3D9Render()
{
	wprintf_s(L"图像绘制组件成功加载\n");
}


D3D9Render::~D3D9Render()
{
	wprintf_s(L"图像绘制组件成功释放\n");
}

void D3D9Render::Initialize()
{
	if (GameBase::screen_width == 0 || GameBase::screen_height == 0) {
		GameBase::screen_width = GetScreenWidth();
		GameBase::screen_height = GetScreenHeight();
	}

	this->margin = { 0,0, GameBase::screen_width , GameBase::screen_height };

	d3d = Direct3DCreate9(D3D_SDK_VERSION);    // create the Direct3D interface

	D3DPRESENT_PARAMETERS d3dpp;    // create a struct to hold various device information
	ZeroMemory(&d3dpp, sizeof(d3dpp));    // clear out the struct for use
	d3dpp.Windowed = TRUE;    // program windowed, not fullscreen
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;    // discard old frames
	d3dpp.hDeviceWindow = hSpawnedWindow;    // set the window to be used by Direct3D
	d3dpp.BackBufferFormat = D3DFMT_A8R8G8B8;     // set the back buffer format to 32-bit
	d3dpp.BackBufferWidth = GameBase::screen_width;    // set the width of the buffer s_width
	d3dpp.BackBufferHeight = GameBase::screen_height;    // set the height of the buffer  s_height

	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;

	// create a device class using this information and the info from the d3dpp stuct
	d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hSpawnedWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3ddev);
	D3DXCreateFontA(d3ddev, 18, 0, FW_SEMIBOLD, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Fang Song", &pFont);
	D3DXCreateLine(d3ddev, &d3dLine);
}

int D3D9Render::GetScreenWidth()
{
	return GetSystemMetrics(SM_CXSCREEN);
}

int D3D9Render::GetScreenHeight()
{
	return GetSystemMetrics(SM_CYSCREEN);
}

void D3D9Render::Render(void(*entityloop)(void))
{
	// clear the window alpha
	auto ret = d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	if (ret != D3D_OK)
		return;

	d3ddev->BeginScene();    // begins the 3D scene
	entityloop();
	d3ddev->EndScene();    // ends the 3D scene
	d3ddev->Present(NULL, NULL, NULL, NULL);   // displays the created frame on the screen
}

void D3D9Render::SetupWindow()
{
	auto s_width = GetSystemMetrics(SM_CXSCREEN);
	auto s_height = GetSystemMetrics(SM_CYSCREEN);

	/* Registering Window Class for our overlay window
	More help on MSDN (RegisterClassEx function): https://msdn.microsoft.com/en-us/library/windows/desktop/ms633587(v=vs.85).aspx */

	WNDCLASSEX overlayWindowClass;
	ZeroMemory(&overlayWindowClass, sizeof(WNDCLASSEX)); // Initialises, sets all bits to 0 to remove garbage data
	overlayWindowClass.cbClsExtra = NULL;
	overlayWindowClass.cbWndExtra = NULL;
	overlayWindowClass.cbSize = sizeof(WNDCLASSEX);
	overlayWindowClass.style = CS_HREDRAW | CS_VREDRAW;
	overlayWindowClass.lpfnWndProc = WindowProc; // Function that will be executed when the window receives a "message" (input). Required! (crashes if set to NULL)
	overlayWindowClass.hInstance = NULL;
	overlayWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	overlayWindowClass.hIcon = LoadIcon(0, IDI_APPLICATION);
	overlayWindowClass.hIconSm = LoadIcon(0, IDI_APPLICATION);
	overlayWindowClass.hbrBackground = (HBRUSH)RGB(0, 0, 0);
	overlayWindowClass.lpszClassName = L"USurface_7155"; // Class name to use with the Windows API function CreateWindow(Ex) to create the window
	overlayWindowClass.lpszMenuName = L"";
	RegisterClassEx(&overlayWindowClass);
	hSpawnedWindow = CreateWindowEx(NULL, overlayWindowClass.lpszClassName, L"", WS_POPUP | WS_VISIBLE, 0, 0, s_width-1, s_height-1, NULL, NULL, NULL, NULL);

	/* This makes the window transparent (somehow, I figured this out by trying many solutions, it doesn't seem designed for this purpose)
	More help on MSDN (DwmExtendFrameIntoClientArea function): https://msdn.microsoft.com/en-us/library/windows/desktop/aa969512(v=vs.85).aspx */
	//MARGINS margin = { 0,0,s_width,s_height };

	MARGINS margin = { -1,-1,-1,-1 };
	DwmExtendFrameIntoClientArea(hSpawnedWindow, &margin);

	// No need to use WS_EX_TOPMOST since I replace the window on top periodically
	SetWindowLong(hSpawnedWindow, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT);

	this->Initialize();
}


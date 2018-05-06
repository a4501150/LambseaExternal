#pragma once
#include "stdafx.h"

//d3d9 - related
#include <d3d9.h>
#include <d3dx9.h>
#include <Dwmapi.h> 

#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"Dwmapi.lib")

#define D3D9RENDER

class D3D9Render
{
public:
	D3D9Render();
	~D3D9Render();

	void Initialize();
	int GetScreenWidth();
	int GetScreenHeight();

	void Render(void(*entityloop)(void));
	void SetupWindow();

	MARGINS GetMargines() { return this->margin; };
	LPD3DXFONT GetFont() { return this->pFont; };
	HWND GetSpawnedWindow() { return this->hSpawnedWindow; };

	void DrawString(int x, int y, DWORD color, LPD3DXFONT g_pFont, const  wchar_t *fmt, ...);
	void DrawLine(float x, float y, float xx, float yy, D3DCOLOR color);
	void DrawBox(float x, float y, float width, float height, D3DCOLOR color);
	void DrawFilledRect(float x, float y, float w, float h, D3DCOLOR color);
	void FillRect(float x, float y, float w, float h, D3DCOLOR color);
	void DrawHealthBar(float x, float y, float w, float h, float health, float max);
	void DrawHealthBar(float x, float y, float w, float health, float max);




private:

	struct vertex
	{
		FLOAT x, y, z, rhw;
		DWORD color;
	};

	LPDIRECT3DVERTEXBUFFER9 g_pVB = 0;    // Buffer to hold vertices
	LPDIRECT3DINDEXBUFFER9  g_pIB = 0;    // Buffer to hold indices
	LPD3DXFONT pFont;
	LPDIRECT3D9 d3d;    // the pointer to our Direct3D interface
	LPDIRECT3DDEVICE9 d3ddev;
	MARGINS  margin;
	ID3DXLine* d3dLine;

	HWND hSpawnedWindow;
};


inline void D3D9Render::DrawString(int x, int y, DWORD color, LPD3DXFONT g_pFont, const wchar_t * fmt, ...)
{
	wchar_t buf[1024] = { 0 };
	va_list va_alist;
	RECT FontPos = { x, y, x + 150, y + 20 };
	va_start(va_alist, fmt);
	vswprintf_s(buf, fmt, va_alist);
	va_end(va_alist);
	g_pFont->DrawTextW(NULL, buf, -1, &FontPos, DT_NOCLIP, color);
}

inline void D3D9Render::DrawLine(float x, float y, float xx, float yy, D3DCOLOR color)
{
	D3DXVECTOR2 dLine[2];

	d3dLine->SetWidth(1.f);

	dLine[0].x = x;
	dLine[0].y = y;
	dLine[1].x = xx;
	dLine[1].y = yy;

	d3dLine->Draw(dLine, 2, color);
}

inline void D3D9Render::DrawBox(float x, float y, float width, float height, D3DCOLOR color)
{
	D3DXVECTOR2 points[5];
	points[0] = D3DXVECTOR2(x, y);
	points[1] = D3DXVECTOR2(x + width, y);
	points[2] = D3DXVECTOR2(x + width, y + height);
	points[3] = D3DXVECTOR2(x, y + height);
	points[4] = D3DXVECTOR2(x, y);
	d3dLine->SetWidth(1);
	d3dLine->Draw(points, 5, color);
}

inline void D3D9Render::DrawFilledRect(float x, float y, float w, float h, D3DCOLOR color)
{

	D3DXVECTOR2 dLine[2];

	y = y + h / 2;

	d3dLine->SetWidth(h);

	dLine[0].x = x;
	dLine[0].y = y;

	dLine[1].x = x + w;
	dLine[1].y = y;

	d3dLine->Draw(dLine, 2, color);

}

inline void D3D9Render::FillRect(float x, float y, float w, float h, D3DCOLOR color)
{
	return DrawFilledRect(x, y, w, h, color);
}

inline void D3D9Render::DrawHealthBar(float x, float y, float w, float h, float health, float max)
{
	if (!max)
		return;

	if (w < 5)
		return;

	if (health < 0)
		health = 0;

	float ratio = health / max;

	D3DCOLOR col = D3DCOLOR_ARGB(255, (int)(255 - 255 * ratio), (int)(255 * ratio), 0);

	float step = (w / max);
	float draw = (step * health);

	DrawFilledRect(x, y, w, h, D3DCOLOR_ARGB(255, 0, 0, 0));
	DrawFilledRect(x, y, draw, h, col);
}

inline void D3D9Render::DrawHealthBar(float x, float y, float w, float health, float max)
{
	DrawHealthBar(x, y, w, 4.0f, health, max);
}



#pragma once

#pragma warning (push)
#pragma warning (disable: 4005)
#include <d3d9.h>
#include <d3d11.h>
#include <d3dx9.h>
#include <d3dx10.h>
#include <d3dx11.h>
#include <D3DX11tex.h>
#include <DXGI.h>
#pragma warning (pop)
#include <stdio.h>
#include <Windows.h>

#include "Helper.h"
#include "D3D11StateSaver.h"
#include "D3D11Shader.h"
#pragma comment(lib, "d3d11.lib") 

#include "../FW1FontWrapper/FW1FontWrapper.h"

typedef unsigned char uchar;

class Color
{
public:
	uchar A, R, G, B;

	Color()
	{
		A = R = G = B = 0;
	}

	Color(uchar A, uchar R, uchar G, uchar B)
	{
		this->A = A;
		this->R = R;
		this->G = G;
		this->B = B;
	}
};

class D3D11Renderer
{
private:
	struct COLOR_VERTEX
	{
		D3DXVECTOR3	Position;
		D3DXCOLOR Color;
	};

	ID3D11RasterizerState * rwState;
	ID3D11RasterizerState * rsState;

	enum eDepthState
	{
		ENABLED,
		DISABLED,
		READ_NO_WRITE,
		NO_READ_NO_WRITE,
		_DEPTH_COUNT
	};

	ID3D11DepthStencilState* myDepthStencilStates[static_cast<int>(eDepthState::_DEPTH_COUNT)];

	IDXGISwapChain *swapChain;
	ID3D11Device *device;
	ID3D11DeviceContext *deviceContext;
	ID3D11InputLayout *mInputLayout;
	ID3D11Buffer *mVertexBuffer;
	ID3D11VertexShader *mVS;
	ID3D11PixelShader *mPS;
	ID3D11BlendState *transparency;

	ID3D11Texture2D* RenderTargetTexture;
	ID3D11RenderTargetView* RenderTargetView;

	D3D11StateSaver *stateSaver;
	bool restoreState = false;
	IFW1FontWrapper *pFontWrapper;

public:
	D3D11Renderer(IDXGISwapChain *SwapChain);
	~D3D11Renderer();

	bool Initialize();
	bool InitializePresent();

	void FillRect(float x, float y, float w, float h, Color color);
	void DrawLine(float x1, float y1, float x2, float y2, Color color);
	void DrawHealthBar(float x, float y, float w, float health, float max);
	void DrawHealthBar(float x, float y, float w, float h, float health, float max);

	void DrawRectangle(float width, float height, float centerx, float centery, Color color);
	void DrawCircle(int x0, int y0, int radius, float thickness, Color color);
	void DrawCircleEx(float r, float centerx, float centery, Color color);
	void DrawString(int x, int y, DWORD color, void* g_pFont, const wchar_t *fmt, ...);
	void DrawBox(float x, float y, float width, float height, Color color);

	void* GetFont();

	ID3D11Device* GetDevice() { return this->device; };
	ID3D11DeviceContext* GetContext() { return this->deviceContext; };
	ID3D11RenderTargetView* GetRTV() { return this->RenderTargetView; };

	float GetScreenWidth();
	float GetScreenHeight();
	void BeginScene();
	void EndScene();
};
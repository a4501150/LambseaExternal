#include "D3D11Renderer.h"

D3D11Renderer::D3D11Renderer(IDXGISwapChain *SwapChain)
{
	this->device = NULL;
	this->deviceContext = NULL;
	this->mVS = NULL;
	this->mPS = NULL;
	this->transparency = NULL;
	this->mInputLayout = NULL;
	this->mVertexBuffer = NULL;
	this->swapChain = SwapChain;
	this->stateSaver = new D3D11StateSaver();
}

D3D11Renderer::~D3D11Renderer()
{
	SAFE_DELETE(this->stateSaver);
	SAFE_RELEASE(this->mVS);
	SAFE_RELEASE(this->mPS);
	SAFE_RELEASE(this->transparency);
	SAFE_RELEASE(this->mInputLayout);
	SAFE_RELEASE(this->mVertexBuffer);
	SAFE_RELEASE(this->swapChain);
	SAFE_RELEASE(this->device);
	SAFE_RELEASE(this->deviceContext);
}

bool D3D11Renderer::Initialize()
{
	HRESULT hr;

	if (!this->swapChain)
		return false;

	this->swapChain->GetDevice(__uuidof(this->device), (void**)&this->device);
	if (!this->device)
		return false;

	this->device->GetImmediateContext(&this->deviceContext);
	if (!this->deviceContext)
		return false;
	
	typedef HRESULT(__stdcall* D3DCompile_t)(LPCVOID pSrcData, SIZE_T SrcDataSize, LPCSTR pSourceName, const D3D_SHADER_MACRO *pDefines, ID3DInclude *pInclude, LPCSTR pEntrypoint, LPCSTR pTarget, UINT Flags1, UINT Flags2, ID3DBlob **ppCode, ID3DBlob *ppErrorMsgs);
	D3DCompile_t myD3DCompile = (D3DCompile_t)GetProcAddress(GetD3DCompiler(), "D3DCompile");
	if (!myD3DCompile)
		return false;

	ID3D10Blob *VS, *PS;
	hr = myD3DCompile(D3D11FillShader, sizeof(D3D11FillShader), NULL, NULL, NULL, "VS", "vs_4_0", 0, 0, &VS, NULL);
	if (FAILED(hr))
		return false;

	hr = this->device->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &this->mVS);
	if (FAILED(hr))
	{
		SAFE_RELEASE(VS);
		return false;
	}
				
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = this->device->CreateInputLayout(layout, ARRAYSIZE(layout), VS->GetBufferPointer(), VS->GetBufferSize(), &this->mInputLayout);
	SAFE_RELEASE(VS);
	if (FAILED(hr))
		return false;
		
	myD3DCompile(D3D11FillShader, sizeof(D3D11FillShader), NULL, NULL, NULL, "PS", "ps_4_0", 0, 0, &PS, NULL);
	if (FAILED(hr))
		return false;

	hr = this->device->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &this->mPS);
	if (FAILED(hr))
	{
		SAFE_RELEASE(PS);
		return false;
	}

	D3D11_BUFFER_DESC bufferDesc;

	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = 4 * sizeof(COLOR_VERTEX);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.MiscFlags = 0;

	hr = this->device->CreateBuffer(&bufferDesc, NULL, &this->mVertexBuffer);
	if(FAILED(hr))
		return false;

	D3D11_BLEND_DESC blendStateDescription;
	ZeroMemory(&blendStateDescription, sizeof(blendStateDescription));

	blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
	blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	hr = this->device->CreateBlendState(&blendStateDescription, &this->transparency);
	if (FAILED(hr))
		return false;


	if (SUCCEEDED(swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&RenderTargetTexture)))
	{
		device->CreateRenderTargetView(RenderTargetTexture, NULL, &RenderTargetView);
		RenderTargetTexture->Release();
	}

	//Create Font
	IFW1Factory* pFW1Factory;
	HRESULT hResult = FW1CreateFactory(FW1_VERSION, &pFW1Factory);
	hResult = pFW1Factory->CreateFontWrapper(this->device, L"Fang Song", &pFontWrapper);
	pFW1Factory->Release();

	return true;
}

bool D3D11Renderer::InitializePresent()
{
	//D3D11_DEPTH_STENCIL_DESC  stencilDesc;
	//stencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	//stencilDesc.StencilEnable = true;
	//stencilDesc.StencilReadMask = 0xFF;
	//stencilDesc.StencilWriteMask = 0xFF;
	//stencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	//stencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	//stencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//stencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	//stencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	//stencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	//stencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//stencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//stencilDesc.DepthEnable = true;
	//stencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	//device->CreateDepthStencilState(&stencilDesc, &myDepthStencilStates[static_cast<int>(eDepthState::ENABLED)]);

	//stencilDesc.DepthEnable = false;
	//stencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	//device->CreateDepthStencilState(&stencilDesc, &myDepthStencilStates[static_cast<int>(eDepthState::DISABLED)]);

	//stencilDesc.DepthEnable = false;
	//stencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	//stencilDesc.StencilEnable = false;
	//stencilDesc.StencilReadMask = UINT8(0xFF);
	//stencilDesc.StencilWriteMask = 0x0;
	//device->CreateDepthStencilState(&stencilDesc, &myDepthStencilStates[static_cast<int>(eDepthState::NO_READ_NO_WRITE)]);

	//stencilDesc.DepthEnable = true;
	//stencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	//stencilDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
	//stencilDesc.StencilEnable = false;
	//stencilDesc.StencilReadMask = UINT8(0xFF);
	//stencilDesc.StencilWriteMask = 0x0;

	//stencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;
	//stencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;
	//stencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	//stencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

	//stencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_ZERO;
	//stencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_ZERO;
	//stencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_ZERO;
	//stencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
	//device->CreateDepthStencilState(&stencilDesc, &myDepthStencilStates[static_cast<int>(eDepthState::READ_NO_WRITE)]);

	//D3D11_RASTERIZER_DESC rwDesc;
	//deviceContext->RSGetState(&rwState); //wireframe
	//rwState->GetDesc(&rwDesc);
	//rwDesc.FillMode = D3D11_FILL_WIREFRAME;
	//rwDesc.CullMode = D3D11_CULL_NONE;
	//device->CreateRasterizerState(&rwDesc, &rwState);

	//D3D11_RASTERIZER_DESC rsDesc;
	//deviceContext->RSGetState(&rsState);  //solid
	//rsState->GetDesc(&rsDesc);
	//rsDesc.FillMode = D3D11_FILL_SOLID;
	//rsDesc.CullMode = D3D11_CULL_BACK; //rsDesc.CullMode = D3D11_CULL_NONE;
	//device->CreateRasterizerState(&rsDesc, &rsState);
}

void D3D11Renderer::FillRect(float x, float y, float w, float h, Color color)
{
	if (this->deviceContext == NULL)
		return;

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	this->deviceContext->OMSetBlendState(this->transparency, blendFactor, 0xffffffff);

	int a = color.A & 0xff;
	int r = color.R & 0xff;
	int g = color.G & 0xff;
	int b = color.B & 0xff;

	UINT viewportNumber = 1;

	D3D11_VIEWPORT vp;

	this->deviceContext->RSGetViewports(&viewportNumber, &vp);

	float x0 = x;
	float y0 = y;
	float x1 = x + w;
	float y1 = y + h;

	float xx0 = 2.0f * (x0 - 0.5f) / vp.Width - 1.0f;
	float yy0 = 1.0f - 2.0f * (y0 - 0.5f) / vp.Height;
	float xx1 = 2.0f * (x1 - 0.5f) / vp.Width - 1.0f;
	float yy1 = 1.0f - 2.0f * (y1 - 0.5f) / vp.Height;

	COLOR_VERTEX* v = NULL;
	D3D11_MAPPED_SUBRESOURCE mapData;

	if (FAILED(this->deviceContext->Map(this->mVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapData)))
		return;

	v = (COLOR_VERTEX*)mapData.pData;

	v[0].Position.x = (float)x0;
	v[0].Position.y = (float)y0;
	v[0].Position.z = 0;
	v[0].Color = D3DXCOLOR(
		((float)r / 255.0f),
		((float)g / 255.0f),
		((float)b / 255.0f),
		((float)a / 255.0f));

	v[1].Position.x = (float)x1;
	v[1].Position.y = (float)y1;
	v[1].Position.z = 0;
	v[1].Color = D3DXCOLOR(
		((float)r / 255.0f),
		((float)g / 255.0f),
		((float)b / 255.0f),
		((float)a / 255.0f));

	v[0].Position.x = xx0;
	v[0].Position.y = yy0;
	v[0].Position.z = 0;
	v[0].Color = D3DXCOLOR(
		((float)r / 255.0f),
		((float)g / 255.0f),
		((float)b / 255.0f),
		((float)a / 255.0f));

	v[1].Position.x = xx1;
	v[1].Position.y = yy0;
	v[1].Position.z = 0;
	v[1].Color = D3DXCOLOR(
		((float)r / 255.0f),
		((float)g / 255.0f),
		((float)b / 255.0f),
		((float)a / 255.0f));

	v[2].Position.x = xx0;
	v[2].Position.y = yy1;
	v[2].Position.z = 0;
	v[2].Color = D3DXCOLOR(
		((float)r / 255.0f),
		((float)g / 255.0f),
		((float)b / 255.0f),
		((float)a / 255.0f));

	v[3].Position.x = xx1;
	v[3].Position.y = yy1;
	v[3].Position.z = 0;
	v[3].Color = D3DXCOLOR(
		((float)r / 255.0f),
		((float)g / 255.0f),
		((float)b / 255.0f),
		((float)a / 255.0f));


	this->deviceContext->Unmap(this->mVertexBuffer, NULL);

	UINT Stride = sizeof(COLOR_VERTEX);
	UINT Offset = 0;

	this->deviceContext->IASetVertexBuffers(0, 1, &this->mVertexBuffer, &Stride, &Offset);
	this->deviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	this->deviceContext->IASetInputLayout(this->mInputLayout);

	this->deviceContext->VSSetShader(this->mVS, 0, 0);
	this->deviceContext->PSSetShader(this->mPS, 0, 0);
	this->deviceContext->GSSetShader(NULL, 0, 0);
	this->deviceContext->Draw(4, 0);
}

void D3D11Renderer::DrawLine(float x1, float y1, float x2, float y2, Color color)
{
	if (this->deviceContext == NULL)
		return;

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	this->deviceContext->OMSetBlendState(this->transparency, blendFactor, 0xffffffff);

	int a = color.A & 0xff;
	int r = color.R & 0xff;
	int g = color.G & 0xff;
	int b = color.B & 0xff;

	UINT viewportNumber = 1;

	D3D11_VIEWPORT vp;

	this->deviceContext->RSGetViewports(&viewportNumber, &vp);
		
	float xx0 = 2.0f * (x1 - 0.5f) / vp.Width - 1.0f;
	float yy0 = 1.0f - 2.0f * (y1 - 0.5f) / vp.Height;
	float xx1 = 2.0f * (x2 - 0.5f) / vp.Width - 1.0f;
	float yy1 = 1.0f - 2.0f * (y2 - 0.5f) / vp.Height;

	COLOR_VERTEX* v = NULL;

	D3D11_MAPPED_SUBRESOURCE mapData;

	if (FAILED(this->deviceContext->Map(this->mVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapData)))
		return;

	v = (COLOR_VERTEX*)mapData.pData;

	v[0].Position.x = xx0;
	v[0].Position.y = yy0;

	v[0].Position.z = 0;
	v[0].Color = D3DXCOLOR(
		((float)r / 255.0f),
		((float)g / 255.0f),
		((float)b / 255.0f),
		((float)a / 255.0f));

	v[1].Position.x = xx1;
	v[1].Position.y = yy1;
	v[1].Position.z = 0;
	v[1].Color = D3DXCOLOR(
		((float)r / 255.0f),
		((float)g / 255.0f),
		((float)b / 255.0f),
		((float)a / 255.0f));

	this->deviceContext->Unmap(this->mVertexBuffer, NULL);

	UINT Stride = sizeof(COLOR_VERTEX);
	UINT Offset = 0;

	this->deviceContext->IASetVertexBuffers(0, 1, &this->mVertexBuffer, &Stride, &Offset);
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	this->deviceContext->IASetInputLayout(this->mInputLayout);

	this->deviceContext->VSSetShader(this->mVS, 0, 0);
	this->deviceContext->PSSetShader(this->mPS, 0, 0);
	this->deviceContext->GSSetShader(NULL, 0, 0);
	this->deviceContext->Draw(2, 0);
}

void D3D11Renderer::DrawHealthBar(float x, float y, float w, float health, float max)
{
	this->DrawHealthBar(x, y, w, 2, health, max);
}

void D3D11Renderer::DrawHealthBar(float x, float y, float w, float h, float health, float max)
{
	if (!max)
		return;

	if (w < 5)
		return;

	if (health < 0)
		health = 0;

	float ratio = health / max;

	Color col = Color(255, (uchar)(255 - 255 * ratio), (uchar)(255 * ratio), 0);

	float step = (w / max);
	float draw = (step * health);

	FillRect(x, y, w, h, Color(255, 0, 0, 0));
	FillRect(x, y, draw, h, col);
}

void D3D11Renderer::DrawRectangle(float width, float height, float centerx, float centery, Color color)
{
	float x1, y1, x2, y2;
	
	//top line
	x1 = centerx - width / 2;
	y1 = centery - height / 2;
	x2 = centerx + width / 2;
	y2 = centery - height / 2;
	DrawLine(x1, y1, x2, y2, color);

	//bottom line
	x1 = centerx - width / 2;
	y1 = centery + height / 2;
	x2 = centerx + width / 2;
	y2 = centery + height / 2;
	DrawLine(x1, y1, x2, y2, color);

	//left line
	x1 = centerx - width / 2;
	y1 = centery - height / 2;
	x2 = centerx - width / 2;
	y2 = centery + height / 2;
	DrawLine(x1, y1, x2, y2, color);

	//right line
	x1 = centerx + width / 2;
	y1 = centery - height / 2;
	x2 = centerx + width / 2;
	y2 = centery + height / 2;
	DrawLine(x1, y1, x2, y2, color);

}

void D3D11Renderer::DrawCircleEx(float r, float centerx, float centery, Color color)
{


	if (this->deviceContext == NULL)
		return;

	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	this->deviceContext->OMSetBlendState(this->transparency, blendFactor, 0xffffffff);

	const int NUMPOINTS = 30;
	UINT viewportNumber = 1;

	D3D11_VIEWPORT vp;

	this->deviceContext->RSGetViewports(&viewportNumber, &vp);

	COLOR_VERTEX* v = NULL;

	D3D11_MAPPED_SUBRESOURCE mapData;

	if (FAILED(this->deviceContext->Map(mVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mapData)))
		return;

	v = (COLOR_VERTEX*)mapData.pData;

	float WedgeAngle = (float)((2 * D3DX_PI) / NUMPOINTS);

	for (int i = 0; i <= NUMPOINTS; i++)
	{
		float Theta = (float)(i * WedgeAngle);
		float x = (float)(centerx + r * cos(Theta));
		float y = (float)(centery - r * sin(Theta));

		v[i].Position.x = 2.0f * (x - 0.5f) / vp.Width - 1.0f;
		v[i].Position.y = 1.0f - 2.0f * (y - 0.5f) / vp.Height;
		v[i].Position.z = 0.0f;
		v[i].Color.r = ((FLOAT)color.R / 255.0f);
		v[i].Color.g = ((FLOAT)color.G / 255.0f);
		v[i].Color.b = ((FLOAT)color.B / 255.0f);
		v[i].Color.a = ((FLOAT)color.A / 255.0f);
	}


	this->deviceContext->Unmap(this->mVertexBuffer, NULL);

	UINT Stride = sizeof(COLOR_VERTEX);
	UINT Offset = 0;

	this->deviceContext->IASetVertexBuffers(0, 1, &this->mVertexBuffer, &Stride, &Offset);
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	this->deviceContext->IASetInputLayout(this->mInputLayout);

	this->deviceContext->VSSetShader(this->mVS, 0, 0);
	this->deviceContext->PSSetShader(this->mPS, 0, 0);
	this->deviceContext->GSSetShader(NULL, 0, 0);
	this->deviceContext->Draw(NUMPOINTS + 1, 0);
	


}

void D3D11Renderer::DrawString(int x, int y, DWORD color, void * g_pFont, const wchar_t * fmt, ...)
{
	wchar_t buf[1024] = { 0 };
	va_list va_alist;
	va_start(va_alist, fmt);
	vswprintf_s(buf, fmt, va_alist);
	va_end(va_alist);

	DWORD r = (color >> 16) & 0xFF;
	DWORD b = color & 0xFF;
	auto oColor = (color & 0xFF00FF00) | (b << 16) | r;

	pFontWrapper->DrawString(this->deviceContext, buf, 16.0f, x, y, oColor, FW1_RESTORESTATE);
}

void D3D11Renderer::DrawBox(float x, float y, float width, float height, Color color)
{
	//top
	DrawLine(x, y, x + width, y, color);
	//bottom
	DrawLine(x, y+height, x + width, y+height, color);
	//left
	DrawLine(x, y, x, y + height, color);
	//right
	DrawLine(x + width, y, x + width, y + height, color);
}

void* D3D11Renderer::GetFont()
{
	return 0;
}

void D3D11Renderer::DrawCircle(int x0, int y0, int radius, float thickness, Color color)
{
	int x = radius, y = 0;
	int radiusError = 1 - x;

	while (x >= y)
	{
		this->FillRect(x + x0, y + y0, thickness, thickness, color);
		this->FillRect(y + x0, x + y0, thickness, thickness, color);
		this->FillRect(-x + x0, y + y0, thickness, thickness, color);
		this->FillRect(-y + x0, x + y0, thickness, thickness, color);
		this->FillRect(-x + x0, -y + y0, thickness, thickness, color);
		this->FillRect(-y + x0, -x + y0, thickness, thickness, color);
		this->FillRect(x + x0, -y + y0, thickness, thickness, color);
		this->FillRect(y + x0, -x + y0, thickness, thickness, color);
		y++;
		if (radiusError<0)
		{
			radiusError += 2 * y + 1;
		}
		else {
			x--;
			radiusError += 2 * (y - x + 1);
		}
	}
}

float D3D11Renderer::GetScreenWidth()
{
	D3D11_VIEWPORT vp;
	UINT nvp = 1;
	this->deviceContext->RSGetViewports(&nvp, &vp);
	return vp.Width;
}

float D3D11Renderer::GetScreenHeight()
{
	D3D11_VIEWPORT vp;
	UINT nvp = 1;
	this->deviceContext->RSGetViewports(&nvp, &vp);
	return vp.Height;
}

void D3D11Renderer::BeginScene()
{
	this->deviceContext->OMSetRenderTargets(1, &RenderTargetView, NULL);

	this->restoreState = false;
	if (SUCCEEDED(this->stateSaver->saveCurrentState(this->deviceContext)))
		this->restoreState = true;
	this->deviceContext->IASetInputLayout(this->mInputLayout);
}

void D3D11Renderer::EndScene()
{
	if (this->restoreState)
		this->stateSaver->restoreSavedState();
}
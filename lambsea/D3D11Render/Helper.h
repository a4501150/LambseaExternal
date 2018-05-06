#pragma once

#include "D3D11Renderer.h"

static HMODULE GetD3DCompiler()
{
	wchar_t buf[32];
	for (int i = 50; i >= 30; i--)
	{
		wsprintf(buf, L"D3DCompiler_%d.dll", i);
		HMODULE mod = LoadLibrary(buf);
		if (mod)
			return mod;
	}

	return NULL;
}

template<class T> inline void SAFE_DELETE(T *&p)
{
	if (p)
	{
		delete p;
		p = NULL;
	}
}

template<class T> inline void SAFE_DELETE_ARRAY(T *&p)
{
	if (p)
	{
		delete[] p;
		p = NULL;
	}
}

template<class T> inline void SAFE_RELEASE(T *&p)
{
	if (p)
	{
		p->Release();
		p = NULL;
	}
}
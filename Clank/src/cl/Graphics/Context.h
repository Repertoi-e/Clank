#pragma once

#include "cl/stdafx.h"

#include <d3d11.h>

namespace cl {

	class Context
	{
	private:
		IDXGISwapChain* m_pSwapChain;
		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;

		ID3D11RenderTargetView* m_pBackbuffer;
	public:
		Context();
		~Context();

		void Create(HWND hWnd);

		IDXGISwapChain* GetSwapChain() { return m_pSwapChain; }
		ID3D11Device* GetDevice() { return m_pDevice; }
		ID3D11DeviceContext* GetDeviceContext() { return m_pDeviceContext; }
		ID3D11RenderTargetView* GetBackbuffer() { return m_pBackbuffer; }
	};
}
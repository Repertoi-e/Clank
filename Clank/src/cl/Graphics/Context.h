#pragma once

#include "cl/stdafx.h"

#include "cl/Utils/Singleton.h"

#include <d3d11.h>

namespace cl {

	class Context : public Singleton<Context>
	{
	private:
		IDXGISwapChain* m_pSwapChain;
		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;

		ID3D11RenderTargetView* m_pBackbuffer;
	public:
		Context(void);
		~Context(void);

		void Create(HWND hWnd);

		inline IDXGISwapChain* GetSwapChain(void) { return m_pSwapChain; }
		inline ID3D11Device* GetDevice(void) { return m_pDevice; }
		inline ID3D11DeviceContext* GetDeviceContext(void) { return m_pDeviceContext; }
		inline ID3D11RenderTargetView* GetBackbuffer(void) { return m_pBackbuffer; }

		inline void Present(void) { m_pSwapChain->Present(0, 0); }
	};
}
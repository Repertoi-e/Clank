#include "cl/stdafx.h"
#include "Context.h"

namespace cl {

	Context::Context()
	{
	}

	Context::~Context()
	{
		m_pSwapChain->SetFullscreenState(FALSE, NULL);

		m_pSwapChain->Release();
		m_pDevice->Release();
		m_pDeviceContext->Release();
		m_pBackbuffer->Release();
	}

	void Context::Create(HWND hWnd)
	{
		RECT rect;
		if (!GetClientRect(hWnd, &rect))
		{
			MessageBox(NULL, L"Error when trying to retrieve window's width and height.", NULL, NULL);
			return;
		}

		DXGI_SWAP_CHAIN_DESC scdesc;
		ZeroMemory(&scdesc, sizeof(DXGI_SWAP_CHAIN_DESC));
		scdesc.BufferCount = 1;
		scdesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		scdesc.BufferDesc.Width = rect.right - rect.left;
		scdesc.BufferDesc.Height = rect.bottom - rect.top;
		scdesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scdesc.OutputWindow = hWnd;
		scdesc.SampleDesc.Count = 4;
		scdesc.Windowed = TRUE;
		scdesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		
		D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL, 
			D3D11_SDK_VERSION, &scdesc, &m_pSwapChain, &m_pDevice, NULL, &m_pDeviceContext);

		ID3D11Texture2D *pBackBuffer;
		m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

		m_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &m_pBackbuffer);
		pBackBuffer->Release();

		m_pDeviceContext->OMSetRenderTargets(1, &m_pBackbuffer, NULL);

		D3D11_VIEWPORT viewport;
		ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = rect.right - rect.left;
		viewport.Height = rect.bottom - rect.top;

		m_pDeviceContext->RSSetViewports(1, &viewport);
	}
}
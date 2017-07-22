#include "cl/stdafx.h"
#include "Context.h"

#include "cl/Utils/Log.h"

namespace cl {

#define SafeRelease(x)	\
	if (x)				\
	{					\
		x->Release();	\
		x = nullptr;	\
	}

	Context::Context(void)
		: m_pSwapChain(NULLPTR), m_pDevice(NULLPTR), m_pDeviceContext(NULLPTR), m_pRenderTargetView(NULLPTR), m_pSettings(NULLPTR),
		m_pDepthStencilBuffer(NULLPTR), m_pDepthStencilState(NULLPTR), m_pDepthStencilView(NULLPTR), m_pRasterState(NULLPTR) 
	{
	}

	Context::~Context(void)
	{
		if (m_pSwapChain)
			m_pSwapChain->SetFullscreenState(FALSE, NULL);
	
		SafeRelease(m_pSwapChain);
		SafeRelease(m_pDevice);
		SafeRelease(m_pDeviceContext);
		SafeRelease(m_pRenderTargetView);
		SafeRelease(m_pDepthStencilBuffer);
		SafeRelease(m_pDepthStencilState);
		SafeRelease(m_pDepthStencilView);
		SafeRelease(m_pRasterState);
	}

	void Context::Create(HWND hWnd, ApplicationSettings& settings)
	{
		m_FeatureLevel = D3D_FEATURE_LEVEL_11_0;
		
		m_pSettings = &settings;

		u32 Width = m_pSettings->Width;
		u32 Height = m_pSettings->Height;

		IDXGIFactory* factory;
		HR(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory));

		IDXGIAdapter* adapter;
		HR(factory->EnumAdapters(0, &adapter));

		IDXGIOutput* adapterOutput;
		HR(adapter->EnumOutputs(0, &adapterOutput));

		u32 numModes;
		HR(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL));
		
		DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[numModes];
		HR(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList));
		
		u32 numerator, denominator;
		for (u32 i = 0; i < numModes; i++)
		{
			if (displayModeList[i].Width == Width)
			{
				if (displayModeList[i].Height == Height)
				{
					numerator = displayModeList[i].RefreshRate.Numerator;
					denominator = displayModeList[i].RefreshRate.Denominator;
				}
			}
		}
		DXGI_ADAPTER_DESC adapterDesc;
		HR(adapter->GetDesc(&adapterDesc));

		LOG_WARN("----------------------------------\n");
		LOG_WARN(" Direct3D ", D3DFeatureLevelToString(m_FeatureLevel),":\n");
		LOG_WARN("    ", adapterDesc.Description, "\n");
		LOG_WARN("    ", "VRAM: ", adapterDesc.DedicatedVideoMemory / 1024 / 1024, " MB\n");
		LOG_WARN("----------------------------------\n\n");

		SafeRelease(adapterOutput);
		SafeRelease(adapter);
		SafeRelease(factory);

		delete[] displayModeList;

		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		{
			ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
			
			swapChainDesc.BufferCount = 1;
			swapChainDesc.BufferDesc.Width = Width;
			swapChainDesc.BufferDesc.Height = Height;
			swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swapChainDesc.BufferDesc.RefreshRate.Numerator = m_pSettings->VSync ? numerator : 0;
			swapChainDesc.BufferDesc.RefreshRate.Denominator = m_pSettings->VSync ? denominator : 1;
			swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.SampleDesc.Count = 1;
			swapChainDesc.SampleDesc.Quality = 0;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
			swapChainDesc.OutputWindow = hWnd;
			swapChainDesc.Windowed = !m_pSettings->Fullscreen;
			swapChainDesc.Flags = 0;
		}
		
		HR(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &m_FeatureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc,
			&m_pSwapChain, &m_pDevice, NULL, &m_pDeviceContext));

		ID3D11Texture2D* backBufferPtr;
		HR(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr));
		HR(m_pDevice->CreateRenderTargetView(backBufferPtr, NULL, &m_pRenderTargetView));

		SafeRelease(backBufferPtr);
		
		D3D11_TEXTURE2D_DESC depthBufferDesc;
		{
			ZeroMemory(&depthBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));

			depthBufferDesc.Width = Width;
			depthBufferDesc.Height = Height;
			depthBufferDesc.MipLevels = 1;
			depthBufferDesc.ArraySize = 1;
			depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			depthBufferDesc.SampleDesc.Count = 1;
			depthBufferDesc.SampleDesc.Quality = 0;
			depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			depthBufferDesc.CPUAccessFlags = 0;
			depthBufferDesc.MiscFlags = 0;
		}
		HR(m_pDevice->CreateTexture2D(&depthBufferDesc, NULL, &m_pDepthStencilBuffer));

		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		{
			ZeroMemory(&depthStencilDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
			
			depthStencilDesc.DepthEnable = true;
			depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

			depthStencilDesc.StencilEnable = true;
			depthStencilDesc.StencilReadMask = 0xFF;
			depthStencilDesc.StencilWriteMask = 0xFF;

			depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
			depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
			depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		}
		HR(m_pDevice->CreateDepthStencilState(&depthStencilDesc, &m_pDepthStencilState));
		m_pDeviceContext->OMSetDepthStencilState(m_pDepthStencilState, 1);

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		{
			ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

			depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			depthStencilViewDesc.Texture2D.MipSlice = 0;
		}
		HR(m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, &depthStencilViewDesc, &m_pDepthStencilView));
		m_pDeviceContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
	
		D3D11_RASTERIZER_DESC rasterDesc;
		{
			ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
			
			rasterDesc.AntialiasedLineEnable = false;
			rasterDesc.CullMode = D3D11_CULL_BACK;
			rasterDesc.DepthBias = 0;
			rasterDesc.DepthBiasClamp = 0.0f;
			rasterDesc.DepthClipEnable = true;
			rasterDesc.FillMode = D3D11_FILL_SOLID;
			rasterDesc.FrontCounterClockwise = false;
			rasterDesc.MultisampleEnable = false;
			rasterDesc.ScissorEnable = false;
			rasterDesc.SlopeScaledDepthBias = 0.0f;
		}
		HR(m_pDevice->CreateRasterizerState(&rasterDesc, &m_pRasterState));
		m_pDeviceContext->RSSetState(m_pRasterState);

		D3D11_VIEWPORT viewport;
		{
			ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

			viewport.Width = cast(float32) Width;
			viewport.Height = cast(float32) Height;
			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;
			viewport.TopLeftX = 0.0f;
			viewport.TopLeftY = 0.0f;
		}
		m_pDeviceContext->RSSetViewports(1, &viewport);

		float fieldOfView, screenAspect;
	}

	void Context::Clear(const vec4& color)
	{
		float32 c[] = { color.r, color.g, color.b, color.a };

		m_pDeviceContext->ClearRenderTargetView(m_pRenderTargetView, c);
		m_pDeviceContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	void Context::Present(void)
	{
		m_pSwapChain->Present(m_pSettings->VSync ? 1 : 0, 0);
	}

	String Context::D3DFeatureLevelToString(D3D_FEATURE_LEVEL level)
	{
		switch (level)
		{
		case D3D_FEATURE_LEVEL_10_0:
			return L"10";
		case D3D_FEATURE_LEVEL_10_1:
			return L"10.1";
		case D3D_FEATURE_LEVEL_11_0:
			return L"11";
		case D3D_FEATURE_LEVEL_11_1:
			return L"11.1";
		}
		return L"Unknown D3D feature level";
	}
}
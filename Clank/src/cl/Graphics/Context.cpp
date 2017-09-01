#include "cl/stdafx.h"
#include "Context.h"

#include "cl/Utils/Log.h"

namespace cl {

	Context::Context(void)
	{
	}

	Context::~Context(void)
	{
		if (m_SwapChain)
			m_SwapChain->SetFullscreenState(FALSE, NULL);

		SafeRelease(m_SwapChain);
		SafeRelease(m_Device);
		SafeRelease(m_DeviceContext);
		SafeRelease(m_RenderTargetView);
		SafeRelease(m_DepthStencilBuffer);
		SafeRelease(m_DepthStencilStates[0]);
		SafeRelease(m_DepthStencilStates[1]);
		SafeRelease(m_BlendStates[0]);
		SafeRelease(m_BlendStates[1]);
		SafeRelease(m_DepthStencilView);
		SafeRelease(m_RasterState);
	}

	void Context::Create(HWND hWnd, ApplicationDesc& appDesc)
	{
		m_FeatureLevel = D3D_FEATURE_LEVEL_11_0;

		m_Settings = &appDesc;

		u32 Width = m_Settings->Width;
		u32 Height = m_Settings->Height;

		IDXGIFactory* factory;
		HR(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory));

		IDXGIAdapter* adapter;
		HR(factory->EnumAdapters(0, &adapter));

		IDXGIOutput* adapterOutput;
		HR(adapter->EnumOutputs(0, &adapterOutput));

		u32 numModes;
		HR(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL));

		DXGI_MODE_DESC* displayModeList = anew DXGI_MODE_DESC[numModes];
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
		LOG_WARN(" Direct3D ", D3DFeatureLevelToString(m_FeatureLevel), ":\n");
		LOG_WARN("    ", adapterDesc.Description, "\n");
		LOG_WARN("    ", "VRAM: ", adapterDesc.DedicatedVideoMemory / 1024 / 1024, " MB\n");
		LOG_WARN("----------------------------------\n\n");

		SafeRelease(adapterOutput);
		SafeRelease(adapter);
		SafeRelease(factory);

		del[] displayModeList;

		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		{
			ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

			swapChainDesc.BufferCount = 1;
			swapChainDesc.BufferDesc.Width = Width;
			swapChainDesc.BufferDesc.Height = Height;
			swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swapChainDesc.BufferDesc.RefreshRate.Numerator = m_Settings->VSync ? numerator : 0;
			swapChainDesc.BufferDesc.RefreshRate.Denominator = m_Settings->VSync ? denominator : 1;
			swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
			swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.SampleDesc.Count = 1;
			swapChainDesc.SampleDesc.Quality = 0;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
			swapChainDesc.OutputWindow = hWnd;
			swapChainDesc.Windowed = !m_Settings->Fullscreen;
			swapChainDesc.Flags = 0;
		}
		HR(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &m_FeatureLevel, 1, D3D11_SDK_VERSION, &swapChainDesc,
			&m_SwapChain, &m_Device, NULL, &m_DeviceContext));

		CreateBlendStates();
		CreateDepthStencilStates();
	}

	void Context::ChangeSize(u32 width, u32 height)
	{
		SafeRelease(m_RenderTargetView);
		SafeRelease(m_DepthStencilView);
		SafeRelease(m_DepthStencilBuffer);

		HR(m_SwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0));

		ID3D11Texture2D* backBufferPtr;
		HR(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr));
		HR(m_Device->CreateRenderTargetView(backBufferPtr, NULL, &m_RenderTargetView));
		SafeRelease(backBufferPtr);

		D3D11_TEXTURE2D_DESC depthBufferDesc;
		{
			ZeroMemory(&depthBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));

			depthBufferDesc.Width = width;
			depthBufferDesc.Height = height;
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
		HR(m_Device->CreateTexture2D(&depthBufferDesc, NULL, &m_DepthStencilBuffer));
		m_Device->CreateDepthStencilView(m_DepthStencilBuffer, 0, &m_DepthStencilView);
		
		m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);

		D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
		{
			ZeroMemory(&depthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

			depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			depthStencilViewDesc.Texture2D.MipSlice = 0;
		}
		HR(m_Device->CreateDepthStencilView(m_DepthStencilBuffer, &depthStencilViewDesc, &m_DepthStencilView));
		m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView);

		D3D11_VIEWPORT viewport;
		{
			ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

			viewport.Width = cast(float32) width;
			viewport.Height = cast(float32) height;
			viewport.MinDepth = 0.0f;
			viewport.MaxDepth = 1.0f;
			viewport.TopLeftX = 0.0f;
			viewport.TopLeftY = 0.0f;
		}
		m_DeviceContext->RSSetViewports(1, &viewport);

		D3D11_RASTERIZER_DESC rasterDesc;
		{
			ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));

			rasterDesc.AntialiasedLineEnable = false;
			rasterDesc.CullMode = D3D11_CULL_BACK;
			rasterDesc.DepthBias = 0;
			rasterDesc.DepthBiasClamp = 0.0f;
			rasterDesc.DepthClipEnable = TRUE;
			rasterDesc.FillMode = D3D11_FILL_SOLID;
			rasterDesc.FrontCounterClockwise = FALSE;
			rasterDesc.MultisampleEnable = FALSE;
			rasterDesc.ScissorEnable = FALSE;
			rasterDesc.SlopeScaledDepthBias = 0.0f;
			rasterDesc.FrontCounterClockwise = TRUE;
		}
		HR(m_Device->CreateRasterizerState(&rasterDesc, &m_RasterState));
		m_DeviceContext->RSSetState(m_RasterState);
	}

	void Context::CreateBlendStates(void)
	{
		D3D11_BLEND_DESC desc;
		{
			ZeroMemory(&desc, sizeof(D3D11_BLEND_DESC));

			desc.AlphaToCoverageEnable = FALSE;
			desc.IndependentBlendEnable = FALSE;

			desc.RenderTarget[0].BlendEnable = TRUE;
			desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
			desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		}
		HR(m_Device->CreateBlendState(&desc, &m_BlendStates[0]));
		{
			ZeroMemory(&desc, sizeof(D3D11_BLEND_DESC));

			desc.RenderTarget[0].BlendEnable = FALSE;
			desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		}
		HR(m_Device->CreateBlendState(&desc, &m_BlendStates[1]));
	}

	void Context::CreateDepthStencilStates(void)
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		{
			ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_DESC));

			desc.DepthEnable = TRUE;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			desc.StencilEnable = TRUE;
			desc.StencilReadMask = 0xff;
			desc.StencilWriteMask = 0xff;

			desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR_SAT;
			desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
			desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			desc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
		}
		HR(m_Device->CreateDepthStencilState(&desc, &m_DepthStencilStates[0]));
		{
			ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_DESC));
			
			desc.DepthEnable = FALSE;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
			desc.StencilEnable = TRUE;
			desc.StencilReadMask = 0xff;
			desc.StencilWriteMask = 0xff;

			desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
			desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR_SAT;
			desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
			desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			desc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
		}
		HR(m_Device->CreateDepthStencilState(&desc, &m_DepthStencilStates[1]));
	}

	void Context::Clear(const vec4& color)
	{
		float32 c[] = { color.r, color.g, color.b, color.a };

		m_DeviceContext->ClearRenderTargetView(m_RenderTargetView, c);
		m_DeviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	void Context::Present(void)
	{
		m_SwapChain->Present(m_Settings->VSync ? 1 : 0, 0);
	}

	void Context::SetBlend(bool enabled)
	{
		m_DeviceContext->OMSetBlendState(enabled ? m_BlendStates[0] : m_BlendStates[1], NULLPTR, 0xffffffff);
	}

	void Context::SetDepthTesting(bool enabled)
	{
		m_DeviceContext->OMSetDepthStencilState(enabled ? m_DepthStencilStates[0] : m_DepthStencilStates[1], 0);
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
#pragma once

#include "cl/stdafx.h"

#include "cl/Utils/Singleton.h"
#include "cl/Maths/vec4.h"

#include <d3d11.h>

#include "cl/Application/Application.h"

namespace cl {

	class API Context : public Singleton<Context>
	{
	private:
		static std::vector<ID3D11BlendState*> s_BlendStates;
		static std::vector<ID3D11DepthStencilState*> s_DepthStencilStates;
	private:
		ApplicationDesc* m_Settings;

		D3D_FEATURE_LEVEL m_FeatureLevel;

		IDXGISwapChain* m_SwapChain;
		ID3D11Device* m_Device;
		ID3D11DeviceContext* m_DeviceContext;

		ID3D11RenderTargetView* m_RenderTargetView;
		
		ID3D11Texture2D* m_DepthStencilBuffer;
		ID3D11BlendState* m_BlendState;
		ID3D11DepthStencilState* m_DepthStencilState;
		ID3D11DepthStencilView* m_DepthStencilView;
		ID3D11RasterizerState* m_RasterState;
	public:
		Context(void);
		~Context(void);

		void Create(HWND hWnd, ApplicationDesc& settings);

		void Clear(const vec4& color);
		void Present(void);

		inline IDXGISwapChain* GetSwapChain(void) { return m_SwapChain; }
		inline ID3D11Device* GetDevice(void) { return m_Device; }
		inline ID3D11DeviceContext* GetDeviceContext(void) { return m_DeviceContext; }
		inline ID3D11RenderTargetView* GetRenderTargetView(void) { return m_RenderTargetView; }

		static String D3DFeatureLevelToString(D3D_FEATURE_LEVEL level);
	};
}
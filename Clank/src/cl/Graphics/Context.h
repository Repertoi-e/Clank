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
		ApplicationSettings* m_pSettings;

		D3D_FEATURE_LEVEL m_FeatureLevel;

		IDXGISwapChain* m_pSwapChain;
		ID3D11Device* m_pDevice;
		ID3D11DeviceContext* m_pDeviceContext;

		ID3D11RenderTargetView* m_pRenderTargetView;
		
		ID3D11Texture2D* m_pDepthStencilBuffer;
		ID3D11DepthStencilState* m_pDepthStencilState;
		ID3D11DepthStencilView* m_pDepthStencilView;
		ID3D11RasterizerState* m_pRasterState;
	public:
		Context(void);
		~Context(void);

		void Create(HWND hWnd, ApplicationSettings& settings);

		void Clear(const vec4& color);
		void Present(void);

		inline IDXGISwapChain* GetSwapChain(void) { return m_pSwapChain; }
		inline ID3D11Device* GetDevice(void) { return m_pDevice; }
		inline ID3D11DeviceContext* GetDeviceContext(void) { return m_pDeviceContext; }
		inline ID3D11RenderTargetView* GetRenderTargetView(void) { return m_pRenderTargetView; }

		static String D3DFeatureLevelToString(D3D_FEATURE_LEVEL level);
	};
}
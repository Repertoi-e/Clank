#pragma once

#include "cl/stdafx.h"

#include <d3d11.h>

namespace cl {

	// @Incomplete: I want to load a texture flipped x/y and
	// I cannot do that. Make it so I can.
	class API Texture
	{
	private:
		ID3D11ShaderResourceView* m_Handle;
		ID3D11SamplerState* m_SamplerState;
		D3D11_SAMPLER_DESC m_SamplerDesc;
	public:
		Texture();
		~Texture();

		// @Incomplete: Currently there is no way to set the data of the texture
		// without loading it from a file.
		void CreateFromFile(String file);
		
		void Bind(u32 slot = 0);
		void Unbind(u32 slot = 0);

		inline ID3D11ShaderResourceView* GetHandle() { return m_Handle; }
	};
}
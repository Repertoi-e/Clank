#pragma once

#include "cl/stdafx.h"

#include <d3d11.h>

namespace cl {

	class Shader
	{
	private:
		ID3D11VertexShader* m_pVS;
		ID3D11PixelShader* m_pPS;
	public:
		Shader();
		~Shader();

		void Create(LPCWSTR source, ID3D11Device* device);

		void Bind(ID3D11DeviceContext* devcon);
	};
}
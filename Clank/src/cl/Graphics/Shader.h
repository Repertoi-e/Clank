#pragma once

#include "cl/stdafx.h"

#include <d3d11.h>

namespace cl {

	class Shader
	{
	public:
		struct Data
		{
			ID3D10Blob* vs;
			ID3D10Blob* ps;
		};
	private:
		Data m_Data;
		ID3D11VertexShader* m_VS;
		ID3D11PixelShader* m_PS;
	public:
		Shader(void);
		~Shader(void);

		void Create(LPCWSTR vertSrc, LPCWSTR pixelSrc);

		inline Data& GetData(void) { return m_Data; }

		inline ID3D11VertexShader* GetVS(void) { return m_VS; }
		inline ID3D11PixelShader* GetPS(void) { return m_PS; }

		void Bind();
	};
}
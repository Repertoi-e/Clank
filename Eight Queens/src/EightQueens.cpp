#include <Clank.h>
#include <ShellAPI.h>

#include "../Resource.h"

#define ASPECT_RATIO    (WIDTH / HEIGHT)

using namespace cl;
using namespace std::literals;

class EightQueens : public Scene2D
{
private:
	Renderable2D* m_Background = NULLPTR;

	u32 N = 8;
	// Assumes WIDTH == HEIGHT
	float32 m_SquareSize = WIDTH / 8;

	const u32 m_Color1 = 0xff255aaa;
	const u32 m_Color2 = 0xffa3c6e0;
public:
	EightQueens()
		: Scene2D(mat4::Orthographic(-cast(float32) WIDTH / 2, cast(float32) WIDTH / 2, -cast(float32) HEIGHT / 2, cast(float32) HEIGHT / 2, -1.0f, 1.0f))
	{
	}

	void OnPush(void) override
	{
		const String& path = g_ApplicationDesc.Path;

		Renderer2DDesc desc;
		{
			ZeroMemory(&desc, sizeof(Renderer2DDesc));

			desc.MaxQuads = 50000u;
			desc.MaxVertices = desc.MaxQuads * 4;
			desc.BufferSize = desc.MaxVertices * sizeof(Vertex);
			desc.MaxIndices = desc.MaxQuads * 6;
			desc.VertexShaderFile = L"VS_R2D.cso";
			desc.PixelShaderFile = L"PS_R2D.cso";
		}
		Renderer2D::Create(m_Renderer, desc);
		m_Renderer->SetCamera(m_Camera);

		// Texture* a = NULLPTR;
		// m_Background = Add(anew Renderable2D(vec2(0.0f, 0.0f), vec2(0.0f, 0.0f), a));
	}

	void OnPop(void) override
	{
		// Remove(m_Background);
		// del m_Background;
	}

	void PreRender(Renderer2D* renderer, Context* context) override
	{
		context->Clear(vec4(224 / 255.0f, 249 / 255.0f, 240 / 255.0f, 255 / 255.0f));
	}

	void LateRender(Renderer2D* renderer, Context* context) override
	{
		bool color = true;
		for (u32 i = 0; i < N; i++)
		{
			float32 x = i * m_SquareSize;
			for (u32 j = 0; j < N; j++)
			{
				float32 y = j * m_SquareSize;
				renderer->FillRect(vec2(x, y) - vec2(WIDTH) / 2, vec2(m_SquareSize), color ? m_Color1 : m_Color2);
				color = !color;
			}
			if (N % 2 == 0)
				color = !color;
		}
	}

	void OnUpdate(const DeltaTime& dt) override
	{
		// m_Camera->SetProjectionMatrix(mat4::Orthographic(-m_Width / 2 * m_CameraZoom, m_Width / 2 * m_CameraZoom, -m_Height / 2 * m_CameraZoom, m_Height / 2 * m_CameraZoom, -1.0f, 1.0f));
	}

	void UpdateConfig(String path)
	{
		if (GetFilenameFromPath(path) != L"8queens_config.txt")
			return;

		std::wifstream wif(path);

		String line;
		while (std::getline(wif, line))
		{
			if (line[0] == L'#')
				continue;

			line.erase(std::remove(line.begin(), line.end(), L' '), line.end());

			static constexpr wchar n[] = L"N:";

			if (line.find(n) != String::npos)
			{
				line.erase(0, wcslen(n));

				N = std::stoi(line);
				N = clamp(N, 4, 15);
				// Again assumes window is square
				m_SquareSize = WIDTH / N;
			}
		}
	}

	void OnEvent(Event& event) override
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<WindowFileDropEvent>([&](WindowFileDropEvent& e) -> bool
		{
			UpdateConfig(e.GetFilePath());
			return true;
		});
	}
};

void AppMain(const String& path, wchar** args, s32 argsCount)
{
	g_Application.PushScene(anew EightQueens());
	g_Application.ShowWindow();
	g_Application.Start();
}
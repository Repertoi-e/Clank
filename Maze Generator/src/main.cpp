#include <Clank.h>

constexpr u32	  WIDTH = 800u;
constexpr u32	  HEIGHT = 600u;
constexpr float32 ASPECT_RATIO = WIDTH / HEIGHT;

using namespace cl;

class Game : public Layer2D
{
private:
	Texture* m_Background; 
	OrthographicCamera* m_Camera;
public:
	Game()
		: Layer2D(mat4::Orthographic(0.0f, cast(float32) WIDTH, 0.0f, cast(float32) HEIGHT, -1.0f, 1.0f)), m_Background(anew Texture),
		m_Camera(anew OrthographicCamera(m_ProjectionMatrix))	
	{
	}

	~Game()
	{
		del m_Camera;
	}

	void Init(Context* context, Renderer2D* renderer) override
	{
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
		Renderer2D::Create(renderer, desc);

		renderer->SetCamera(m_Camera);

		TextureDesc textureDesc;
		TextureLoadProperties textureLoadProperties;
		{
			ZeroMemory(&textureDesc, sizeof(TextureDesc));
			ZeroMemory(&textureLoadProperties, sizeof(TextureLoadProperties));

			textureDesc.Filter = TextureFilter::NEAREST;

			textureLoadProperties.FlipHorizontal = false;
			textureLoadProperties.FlipVertical = false;
		}
		
		// Texture::CreateFromFile(background, L"xxx", textureDesc, textureLoadProperties);
	}

	void OnUpdate(const DeltaTime& dt)
	{
	}

	void OnTick() override
	{
	}

	void PreRender(Context* context, Renderer2D* renderer) override
	{
	}

	void OnRender(Context* context, Renderer2D* renderer) override
	{
		renderer->DrawLine(vec2(20, 20), vec2(500, 500), 0xffff00ff, 10.0f);
	}
};

int main()
{
	SetLocale(LC_ALL);

	ApplicationDesc desc;
	{
		ZeroMemory(&desc, sizeof(ApplicationDesc));

		desc.Name = L"Maze Generator";
		desc.ClassName = L"Window class master race";
		desc.Width = WIDTH;
		desc.Height = HEIGHT;
		desc.WindowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_SIZEBOX;

		desc.Cycle.UpdateTick = 1000.0f / 60.0f;
		desc.Cycle.Timer = anew Timer;
		desc.Cycle.UpdateTimer = 0.0f;
		desc.Cycle.UpdateDeltaTime = anew DeltaTime(0.0f);
	}

	g_Application.Create(desc);
	
	Game* game = cast(Game*)
	g_Application.PushLayer(anew Game());
	g_Application.ShowWindow();
	g_Application.Start();
	g_Application.PopLayer(game);
	
	return EXIT_SUCCESS;
}

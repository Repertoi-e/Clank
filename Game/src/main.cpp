#include <Clank.h>

#include "game/Level.h"
#include "game/Player.h"

#define WIDTH		  800
#define HEIGHT		  600

using namespace cl;

class Game : public Layer2D
{
	Level* level;
	Player* player;
public:
	Game(void)
		: Layer2D(mat4::Orthographic(0, WIDTH, 0, HEIGHT, -1.0f, 1.0f))
	{
	}

	~Game(void)
	{
	}

	void Init(Context* context, Renderer2D* renderer) override
	{
		Renderer2DDesc desc;
		{
			ZeroMemory(&desc, sizeof(Renderer2DDesc));

			desc.MaxQuads = 60000u;
			desc.MaxVertices = desc.MaxQuads * 4;
			desc.BufferSize = desc.MaxVertices * sizeof(Vertex);
			desc.MaxIndices = desc.MaxQuads * 6;
			
			desc.VertexShaderFile = L"VS_R2D.cso";
			desc.PixelShaderFile = L"PS_R2D.cso";
		}
		Renderer2D::Create(renderer, desc);
		
		OrthographicCamera* camera = anew OrthographicCamera(m_ProjectionMatrix);
		renderer->SetCamera(camera);

		player = cast(Player*) Add(anew Player);
		level = anew Level(player);
	}

	void OnUpdate(const DeltaTime& dt) override
	{
		player->Update(dt, level);
	}

	void PreRender(Context* context, Renderer2D* renderer) override
	{
	}

	void EarlyRender(Context* context, Renderer2D* renderer) override
	{
		level->Render(renderer);
	}

	void OnEvent(Event& event) override
	{
		player->OnEvent(event);
	}
};

// Release mode builds without the console
#ifdef _DEBUG
int main(void)
#else
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif
{
	SetLocale(LC_ALL);

	Timer init;
	
	ApplicationDesc desc;
	{
		ZeroMemory(&desc, sizeof(ApplicationDesc));
	
		desc.Name = L"Game";
		desc.ClassName = L"Game Window Class";
		desc.Width = WIDTH;
		desc.Height = HEIGHT;
		desc.VSync = FALSE;
		desc.Fullscreen = FALSE;
		desc.WindowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_SIZEBOX;
		
		desc.Cycle.UpdateTick = 1000.0f / 60.0f;
		desc.Cycle.Timer = anew Timer;
		desc.Cycle.UpdateTimer = 0.0f;
		desc.Cycle.UpdateDeltaTime = anew DeltaTime(0.0f);
	}
	g_Application.Create(desc);
	
	g_Application.PushLayer(anew Game);
	{
		float32 time = init.Elapsed().Seconds();
		print("\nInit took /% seconds\n\n", time);

		String name = g_Application.GetDescription().Name + L" | " + std::to_wstring(time);
		g_Application.SetWindowTitle(name.c_str());
	}
	
	g_Application.ShowWindow();

	g_Application.Start(); 

	return 0;
}

#include <Clank.h>

#include "game/Level.h"
#include "game/Player.h"

#define WIDTH		  800
#define HEIGHT		  600

using namespace cl;

class TestLayer : public Layer2D
{
private:
	Texture* texture, *texture2;
	Renderable2D* m_Renderable, *m_Renderable2;
	Folder* folder;
public:
	TestLayer(void)
		: Layer2D(mat4::Orthographic(0, WIDTH, 0, HEIGHT, -1.0f, 1.0f))
	{
	}

	~TestLayer(void)
	{
		delete m_Renderable;
		delete m_Renderable2;
		delete texture;
		delete texture2;
	}

	void Init(Context* context, Renderer2D* renderer) override
	{
		Renderer2DSettings rendererSettings;
		{
			ZeroMemory(&rendererSettings, sizeof(Renderer2DSettings));

			rendererSettings.MaxQuads = 60000;
			rendererSettings.MaxVertices = rendererSettings.MaxQuads * 4;
			rendererSettings.BufferSize = rendererSettings.MaxVertices * sizeof(Vertex);
			rendererSettings.MaxIndices = rendererSettings.MaxQuads * 6;
		}
		renderer->SetSettings(rendererSettings);
		renderer->Create();

		OrthographicCamera* camera = new OrthographicCamera(this->m_ProjectionMatrix);
		renderer->SetCamera(camera);

		TextureSettings textureSettings;
		textureSettings.Filter = TextureFilter::NEAREST;

		TextureLoadSettings textureLoadSettings;
		textureLoadSettings.FlipHorizontal = false;
		textureLoadSettings.FlipVertical = false;

		texture = new Texture(textureSettings, textureLoadSettings);
		texture2 = new Texture(textureSettings, textureLoadSettings);

		texture->CreateFromFile(L"data/textures/test.dds");
		texture2->CreateFromFile(L"data/textures/test2.dds");

		m_Renderable = new Renderable2D({ 0, 0 }, { 50,  50 }, texture);
		folder = cast(Folder*) Add(new Folder(mat4::Identity()));
		folder->Add(m_Renderable);
		m_Renderable2 = Add(new Renderable2D({ 300, 300 }, { 50, 200 }, texture2));
	}

	float32 elapsed = 0, colort, tim = 0;
	void OnUpdate(const DeltaTime& dt) override
	{
		tim += 1.0f;
		elapsed++;
		colort = (cl::sin(elapsed / 10) + 1) / 2;
		folder->SetTransformationMatrix(mat4::Translate(vec3(400, 300, 0)) * mat4::Rotate(tim, vec3(0, 0, 1)));
	}
	
	vec4 blue = { 0.0f, 0.2f, 0.4f, 1.0f };
	void PreRender(Context* context, Renderer2D* renderer) override
	{
		float32 color[4] = { blue.r * colort, blue.g * colort, blue.b * colort, blue.a };
		context->GetDeviceContext()->ClearRenderTargetView(context->GetRenderTargetView(), color);
	}

	void OnRender(Context* context, Renderer2D* renderer) override
	{
	}
};

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
		Renderer2DSettings settings;
		settings.MaxQuads = 60000;
		settings.MaxVertices = settings.MaxQuads * 4;
		settings.BufferSize = settings.MaxVertices * sizeof(Vertex);
		settings.MaxIndices = settings.MaxQuads * 6;
		renderer->SetSettings(settings);

		renderer->Create();

		OrthographicCamera* camera = new OrthographicCamera(m_ProjectionMatrix);
		renderer->SetCamera(camera);

		player = cast(Player*) Add(new Player);
		level = new Level(player);
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
	{
		ApplicationDesc settings;
		{
			ZeroMemory(&settings, sizeof(ApplicationDesc));

			settings.Name = L"Game";
			settings.ClassName = L"Game&&";
			settings.Width = WIDTH;
			settings.Height = HEIGHT;
			settings.VSync = FALSE;
			settings.Fullscreen = FALSE;
			settings.WindowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_SIZEBOX | WS_VISIBLE;
		
			settings.Cycle.UpdateTick = 1000.0f / 60.0f;
			settings.Cycle.Timer = new Timer;
			settings.Cycle.UpdateTimer = 0.0f;
			settings.Cycle.UpdateDeltaTime = new DeltaTime(0.0f);
		}
		g_Application.SetSettings(settings);
		g_Application.DoWindow();

		g_Application.PushLayer(new Game);
	}
	LOG_WARN("Init took ", init.Elapsed().Millis(), " ms\n\n");

	g_Application.Start(); 

	return 0;
}

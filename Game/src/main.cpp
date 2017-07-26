#include <Clank.h>

#define WIDTH		  800
#define HEIGHT		  600

using namespace cl;

class TestLayer : public Layer2D
{
private:
	float32 elapsed = 0, colort;
	vec4 blue = { 0.0f, 0.2f, 0.4f, 1.0f };

	Renderable2D* m_Renderable, *m_Renderable2;
public:
	TestLayer(void)
		: Layer2D(mat4::Orthographic(0, WIDTH, 0, HEIGHT, -1.0f, 1.0f))
	{
	}

	~TestLayer(void)
	{
		delete m_Renderable;
		delete m_Renderable2;
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

		Texture* texture = new Texture;
		texture->CreateFromFile(L"data/textures/test.dds");

		Texture* texture2 = new Texture;
		texture2->CreateFromFile(L"data/textures/test2.dds");

		m_Renderable = new Renderable2D({ 0, 0 }, { 50, 50 }, texture /*0xff00ffff*/);
		m_Renderable2 = new Renderable2D({ -100, 0 }, { 50, 200 }, texture2 /*0xffff00ff*/);
	}

	void OnUpdate(const DeltaTime& dt) override
	{
		elapsed++;
		colort = (cl::sin(elapsed / 10) + 1) / 2;
	}
	
	void OnEvent(Event& event) override
	{
	}

	void OnRender(Context* context, Renderer2D* renderer) override
	{
		float32 color[4] = { blue.r * colort, blue.g * colort, blue.b * colort, blue.a };
		context->GetDeviceContext()->ClearRenderTargetView(context->GetRenderTargetView(), color);

		renderer->Begin();
		renderer->Submit(m_Renderable);
		renderer->Submit(m_Renderable2);
		renderer->End();
		renderer->Present();
	}

	void OnTick(void) override
	{
	}
};

#define FULLSCREEN	  FALSE
#define VSYNC		  FALSE
#define WINDOW_STYLE  WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_SIZEBOX | WS_VISIBLE

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
		g_Application.SetName(L"义勇军进行曲");
		g_Application.SetSettings({ WIDTH, HEIGHT, VSYNC, FULLSCREEN, WINDOW_STYLE });

		g_Application.DoWindow();

		g_Application.PushLayer(new TestLayer());

		CycleInfo info;
		ZeroMemory(&info, sizeof(info));
		info.UpdateTick = 1000.0f / 60.0f;
		g_Application.SetCycleInfo(info);
	}
	LOG_WARN("Init took ", init.Elapsed().Millis(), " ms\n\n");

	g_Application.DoCycle(); 

	return 0;
}

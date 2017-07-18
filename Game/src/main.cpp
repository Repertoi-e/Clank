#include <Clank.h>

using namespace cl;

class TestLayer : public Layer
{
private:
	float32 elapsed = 0, colort;
	vec4 blue = { 0.0f, 0.2f, 0.4f, 1.0f };

	Renderer2D* m_pRenderer;
	Renderable2D* m_pRenderable;
	Context* m_pContext;
public:
	TestLayer(void)
		: m_pRenderer(new Renderer2D())
	{
	}

	~TestLayer(void)
	{
	}

	void Init(Context* context) override
	{
		m_pContext = context;

		Renderer2DSettings settings;
		settings.MaxQuads = 60000;
		settings.MaxVertices = settings.MaxQuads * 4;
		settings.BufferSize = settings.MaxVertices * sizeof(Vertex);
		settings.MaxIndices = settings.MaxQuads * 6;
		m_pRenderer->SetSettings(settings);

		m_pRenderer->Create();

		m_pRenderable = new Renderable2D({ 0.5f, 0.5f }, { 0.5f, 0.5f }, 0xff00ffff);
	}

	void Update(const DeltaTime& dt)
	{
		elapsed++;
		colort = (cl::sin(elapsed / 10) + 1) / 2;
	}

	void Render(void)
	{
		float32 color[4] = { blue.r * colort, blue.g * colort, blue.b * colort, blue.a };
		m_pContext->GetDeviceContext()->ClearRenderTargetView(m_pContext->GetBackbuffer(), color);

		m_pRenderer->Begin();
		m_pRenderer->Submit(m_pRenderable);
		m_pRenderer->End();
		m_pRenderer->Present();
	}

	void Tick(void)
	{
	}
};

#define WIDTH		  800
#define HEIGHT		  600
#define FULLSCREEN	  FALSE
#define VSYNC		  FALSE
#define WINDOW_STYLE  WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX

int main(void)
{
	// Output: Hello this is 10 and this is a per cent: %, and this is /%
	print("Hello this is /% and this is a per cent: %, and this is /% \n", 10, 5);
	Timer init;
	{
		g_Application.SetName(L"义勇军进行曲");
		g_Application.SetSettings({ WIDTH, HEIGHT, VSYNC, FULLSCREEN, WINDOW_STYLE | WS_SIZEBOX });

		g_Application.DoWindow();

		g_Application.PushLayer(new TestLayer());

		CycleInfo info;
		ZeroMemory(&info, sizeof(info));
		info.UpdateTick = 1000.0f / 60.0f;
		g_Application.SetCycleInfo(info);
	}
	LOG_WARN("Init took ", init.Elapsed().Millis(), " ms\n");

	g_Application.DoCycle();

	return 0;
}

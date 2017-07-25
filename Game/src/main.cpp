#include <Clank.h>

using namespace cl;

class TestLayer : public Layer
{
private:
	float32 elapsed = 0, colort;
	vec4 blue = { 0.0f, 0.2f, 0.4f, 1.0f };

	Renderer2D* m_Renderer;
	Renderable2D* m_Renderable, *m_Renderable2;
	Context* m_Context;
public:
	TestLayer(void)
		: m_Renderer(new Renderer2D())
	{
	}

	~TestLayer(void)
	{
		delete m_Renderer;
		delete m_Renderable;
		delete m_Renderable2;
	}

	void Init(Context* context) override
	{
		m_Context = context;

		Renderer2DSettings settings;
		settings.MaxQuads = 60000;
		settings.MaxVertices = settings.MaxQuads * 4;
		settings.BufferSize = settings.MaxVertices * sizeof(Vertex);
		settings.MaxIndices = settings.MaxQuads * 6;
		m_Renderer->SetSettings(settings);

		m_Renderer->Create();

		Texture* texture = new Texture;
		texture->CreateFromFile(L"data/textures/test.dds");

		Texture* texture2 = new Texture;
		texture2->CreateFromFile(L"data/textures/test2.dds");

		m_Renderable = new Renderable2D({ 0, 0 }, { 50, 50 }, texture /*0xff00ffff*/);
		m_Renderable2 = new Renderable2D({ -100, 0 }, { 50, 200 }, texture2 /*0xffff00ff*/);
	}

	void Update(const DeltaTime& dt)
	{
		elapsed++;
		colort = (cl::sin(elapsed / 10) + 1) / 2;
	}

	void Render(void)
	{
		float32 color[4] = { blue.r * colort, blue.g * colort, blue.b * colort, blue.a };
		m_Context->GetDeviceContext()->ClearRenderTargetView(m_Context->GetRenderTargetView(), color);

		m_Renderer->Begin();
		m_Renderer->Submit(m_Renderable);
		m_Renderer->Submit(m_Renderable2);
		m_Renderer->End();
		m_Renderer->Present();
	}

	void Tick(void)
	{
	}
};

#define WIDTH		  800
#define HEIGHT		  600
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

	print(L"Le pré est vénéneux mais joli en automne\nLes vaches y paissant\nLentement s’empoisonnent\nLe colchique couleur de cerne et de lilas\nY fleurit tes yeux sont comme cette fleur - la\nViolâtres comme leur cerne et comme cet automne\nEt ma vie pour tes yeux lentement s’empoisonne \n\n");

	print("Hello this is /% and this is a per cent: %, and this is /% \n\n", 10, 5);
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

	String s = L"Hello this is a String!";
	LOG_WARN(s, "\n\n");

	print(L"This is chinese! 义勇军进行曲 (If you have the codepage right...)\n\n");

	g_Application.DoCycle(); 

	return 0;
}

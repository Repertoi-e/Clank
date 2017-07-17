#include <Clank.h>

#include <fstream>

struct VERTEX
{
	float32 x, y, z;
	float32 r, g, b, a;
};

class TestLayer : public cl::Layer
{
private:
	float32 elapsed = 0, colort;
	cl::vec4 blue = { 0.0f, 0.2f, 0.4f, 1.0f };
	
	cl::Context* m_Context;
	cl::Shader* m_Shader;
	cl::VertexBuffer* m_Buffer;
public:
	TestLayer()
		: m_Shader(NULLPTR), m_Buffer(NULLPTR)
	{
	}
	
	~TestLayer() 
	{
	}

	void Init(cl::Context* context) override
	{
		m_Context = context;

		m_Shader = new cl::Shader();
		m_Shader->Create(L"res/Vertex.hlsl", L"res/Pixel.hlsl");
		m_Shader->Bind();

		VERTEX vertices[] =
		{
			{ 0.0f,   0.5f, 0.0f,    1.0f, 0.0f, 0.0f, 1.0f },
			{ 0.45f, -0.5,  0.0f,    0.0f, 1.0f, 0.0f, 1.0f },
			{ -0.45f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f, 1.0f }
		};

		m_Buffer = new cl::VertexBuffer();
		m_Buffer->Create(cl::BufferUsage::DYNAMIC, 3 * sizeof(VERTEX), cl::BufferCPUA::WRITE);

		void* data = m_Buffer->Map(cl::BufferMapCPUA::WRITE_DISCARD);
		memcpy(data, vertices, sizeof(vertices));
		m_Buffer->Unmap();

		cl::InputLayout ila;
		ila.Push<cl::vec3>("POSITION");
		ila.Push<cl::vec4>("COLOR");

		ID3D10Blob* vsData = m_Shader->GetData().vs;
		m_Buffer->SetInputLayout(ila , vsData->GetBufferPointer(), vsData->GetBufferSize());
	}

	void Update(const cl::DeltaTime& dt)
	{
		elapsed++;
		colort = (sin(elapsed / 10) + 1) / 2;
	}

	void Render()
	{
		float32 fcolor[4] = { blue.r * colort, blue.g * colort, blue.b * colort, blue.a };
		m_Context->GetDeviceContext()->ClearRenderTargetView(m_Context->GetBackbuffer(), fcolor);

		m_Buffer->Bind(sizeof(VERTEX), 0, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_Context->GetDeviceContext()->Draw(3, 0);
	}

	void Tick()
	{
	
	}
};

#define WIDTH		  800
#define HEIGHT		  600
#define FULLSCREEN	  FALSE
#define VSYNC		  FALSE
#define WINDOW_STYLE  WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX

int main()
{
	cl::print("Hello this is /% and this is a per cent: %, and this is /% \n", 10, 5);
	cl::Timer init;
	{
		cl::g_Application.SetName(L"义勇军进行曲");
		cl::g_Application.SetSettings({ WIDTH, HEIGHT, VSYNC, FULLSCREEN, WINDOW_STYLE });

		cl::g_Application.DoWindow();

		cl::g_Application.PushLayer(new TestLayer());

		cl::CycleInfo info;
		ZeroMemory(&info, sizeof(info));
		info.UpdateTick = 1000.0f / 60.0f;
		cl::g_Application.SetCycleInfo(info);
	}
	LOG_INFO(init.Elapsed().Millis(), " ms\n");

	cl::g_Application.DoCycle();
	
	return 0;
}

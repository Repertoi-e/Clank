#include <Clank.h>

#include "Universe.h"

#include <thread>
#include <fstream>

constexpr u32 WIDTH = 800u;
constexpr u32 HEIGHT = 600u;

using namespace cl;

class Game : public Layer2D
{
private:
	Universe* m_Universe;
	
	Renderable2D* m_Background;
	Renderer2D* m_BackgroundRenderer;

	OrthographicCamera* m_Camera;
	OrthographicCamera* m_BackgroundCamera;
	
	vec2 m_MouseDelta;
	vec2 m_CameraOffset;

	float32 m_CameraZoom = 0.0f;

	u32 m_Speed;

	String m_HotloaderPath;
	std::thread m_HotloaderThread;
	bool m_HotloaderShouldClose;
public:
	Game(const String& path)
		: Layer2D(mat4::Orthographic(0.0f, cast(float32) WIDTH, 0.0f, cast(float32) HEIGHT, -1.0f, 1.0f)), m_Universe(new Universe), 
		m_HotloaderPath(path), m_Camera(cl_new OrthographicCamera(m_ProjectionMatrix)), m_BackgroundCamera(cl_new OrthographicCamera(m_ProjectionMatrix)),
		m_BackgroundRenderer(cl_new Renderer2D)
	{
		m_HotloaderShouldClose = false;

		UpdateConfigFile(m_HotloaderPath + L"config.txt");

		m_HotloaderThread = std::thread([&]() 
		{
			HANDLE hDir = CreateFileW(m_HotloaderPath.c_str(), FILE_LIST_DIRECTORY, FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);

			FILE_NOTIFY_INFORMATION strFileNotifyInfo[1024];
			DWORD dwBytesReturned = 0;
			BOOL hack = FALSE;
			while(!m_HotloaderShouldClose)
			{
				if (ReadDirectoryChangesW(hDir, (LPVOID)&strFileNotifyInfo, sizeof(strFileNotifyInfo), FALSE, FILE_NOTIFY_CHANGE_LAST_WRITE, &dwBytesReturned, NULL, NULL) != 0)
				{
					hack = !hack;
					if (!hack)
					{
						if (String(strFileNotifyInfo[0].FileName) == L"config.txt")
							UpdateConfigFile(m_HotloaderPath + L"config.txt");
					}
				}
			}
		});
	}

	~Game()
	{
		m_HotloaderShouldClose = true;
		m_HotloaderThread.join();
	}

	void Init(Context* context, Renderer2D* renderer) override
	{
		Renderer2DDesc desc;
		{
			ZeroMemory(&desc, sizeof(Renderer2DDesc));

			desc.MaxQuads = 250200u;
			desc.MaxVertices = desc.MaxQuads * 4;
			desc.BufferSize = desc.MaxVertices * sizeof(Vertex);
			desc.MaxIndices = desc.MaxQuads * 6;
			desc.VertexShaderFile = L"VS_R2D.cso";
			desc.PixelShaderFile = L"PS_R2D.cso";
		}
		Renderer2D::Create(renderer, desc);
		Renderer2D::Create(m_BackgroundRenderer, desc);

		renderer->SetCamera(m_Camera);
		m_BackgroundRenderer->SetCamera(m_BackgroundCamera);

		Texture* background = new Texture;

		TextureDesc textureDesc;
		TextureLoadProperties textureLoadProperties;
		{
			ZeroMemory(&textureDesc, sizeof(TextureDesc));
			ZeroMemory(&textureLoadProperties, sizeof(TextureLoadProperties));

			textureDesc.Filter = TextureFilter::NEAREST;
			
			textureLoadProperties.FlipHorizontal = false;
			textureLoadProperties.FlipVertical = false;
		}
		Texture::CreateFromFile(background, L"cgl_data/bg.jpg", textureDesc, textureLoadProperties);
		
		m_Background = cl_new Renderable2D({ WIDTH / 2.0f, HEIGHT / 2.0f }, { 1920 / 3.6f, 1080 / 3.6f }, background, 0xffffffff);
	}

	void OnEvent(Event& event) override
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<MouseMovedEvent>([&](MouseMovedEvent& e) -> bool
		{
			if (e.IsDragged())
			{
				vec2 amount = m_MouseDelta - vec2(e.GetX(), e.GetY());
				amount *= 0.2f;

				m_CameraOffset += vec2(amount.x, -amount.y);
			}

			m_MouseDelta = vec2(e.GetX(), e.GetY());

			return false;
		});

		dispatcher.Dispatch<MouseScrollEvent>([&](MouseScrollEvent& e) -> bool
		{
			m_CameraZoom += e.GetDistance() * -1;

			if (m_CameraZoom < 20)
				m_CameraZoom = 20;
			if (m_CameraZoom > 840)
				m_CameraZoom = 840;

			return false;
		});
	}

	bool ChangePreset(String preset)
	{
		std::transform(preset.begin(), preset.end(), preset.begin(), towlower);
		
		static String lastPreset = L"";
		if (lastPreset == preset)
			return false;

		if (preset == L"-clear")
			m_Universe->Clear();
		if (preset == L"-random")
			m_Universe->Randomize();

		m_Universe->SetPreset(preset);

		lastPreset = preset;
		return true;
	}

	void UpdateConfigFile(String path)
	{
		std::wifstream wif(path);
		wif.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar>));
		
		String wline;
		while (std::getline(wif, wline))
		{
			wchar first = wline[0];
			if (first == L'#')
				continue;

			u32 speed = wline.find(L"speed:");
			if (speed < 20000000u)
			{
				wline.erase(0, 6);
				wline.erase(std::remove_if(wline.begin(), wline.end(), std::bind(std::isspace<wchar>, std::placeholders::_1, std::locale::classic())), wline.end());

				s32 newspeed = std::stoi(wline);
				if (m_Speed != newspeed)
				{
					m_Speed = newspeed;
					
					print("[Hotloader] Setting speed to /%\n", m_Speed);
				}
			}

			u32 preset = wline.find(L"preset:");
			if (preset < 20000000u)
			{
				wline.erase(0, 7);
				wline.erase(std::remove_if(wline.begin(), wline.end(), std::bind(std::isspace<wchar>, std::placeholders::_1, std::locale::classic())), wline.end());

				if (ChangePreset(wline))
					print("[Hotloader] Setting preset to /%\n", wline);
			}
		}
	}

	s32 g_t;
	void OnUpdate(const DeltaTime& dt)
	{
		g_t++;
		if (m_Speed != 0)
		{
			if (g_t % m_Speed == 0)
				m_Universe->DoGeneration();
		}
	
		float32 aspect = WIDTH / HEIGHT;
		m_Camera->SetProjectionMatrix(mat4::Orthographic(-m_CameraZoom, cast(float32) WIDTH + m_CameraZoom, -m_CameraZoom * aspect, cast(float32) HEIGHT + m_CameraZoom * aspect, -1.0f, 1.0f));
	}

	void OnTick() override
	{		
	}

	void PreRender(Context* context, Renderer2D* renderer) override
	{
		m_BackgroundRenderer->Begin();
		m_BackgroundRenderer->Submit(m_Background);
		m_BackgroundRenderer->End();
		m_BackgroundRenderer->Present();
	}

	void OnRender(Context* context, Renderer2D* renderer) override
	{
		m_Universe->Draw(renderer, m_CameraOffset);
	}
};

int main(int argc, char* args[])
{
	wbyte file[1024] = {};
	GetModuleFileNameW(NULL, file, 1024);

	String path = String(file);
	path = path.substr(0, path.find_last_of(L'\\') + 1);

	SetLocale(LC_ALL);

	ApplicationDesc desc;
	{
		ZeroMemory(&desc, sizeof(ApplicationDesc));

		desc.Name = L"Conway's Game of Life";
		desc.ClassName = L"Edgy window class name! :^)";
		desc.Width = WIDTH;
		desc.Height = HEIGHT;
		desc.WindowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_SIZEBOX;

		desc.Cycle.UpdateTick = 1000.0f / 60.0f;
		desc.Cycle.Timer = cl_new Timer;
		desc.Cycle.UpdateTimer = 0.0f;
		desc.Cycle.UpdateDeltaTime = cl_new DeltaTime(0.0f); 
	}

	Game* game = cl_new Game(path);

	g_Application.Create(desc);
	g_Application.PushLayer(game);
	g_Application.ShowWindow();

	g_Application.Start();

	cl_delete game;

	return EXIT_SUCCESS;
}

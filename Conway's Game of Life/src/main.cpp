#include <Clank.h>

#include "Universe.h"
#include "Hotloader.h"

constexpr u32	  WIDTH		   = 800u;
constexpr u32	  HEIGHT	   = 600u;
constexpr float32 ASPECT_RATIO = WIDTH / HEIGHT;

using namespace cl;

class Game : public Layer2D
{
private:
	Universe* m_Universe;
	Hotloader* m_Hotloader;

	Renderable2D* m_Background;
	Renderer2D* m_BackgroundRenderer;

	OrthographicCamera* m_Camera;
	OrthographicCamera* m_BackgroundCamera;

	vec2 m_MouseDelta;
	vec2 m_CameraOffset;
	
	vec2 m_TargetCameraOffset;

	float32 m_CameraZoom = 0.0f;
	float32 m_TargetCameraZoom;
	
	u32 m_Speed;
public:
	Game()
		: Layer2D(mat4::Orthographic(0.0f, cast(float32) WIDTH, 0.0f, cast(float32) HEIGHT, -1.0f, 1.0f)), m_Universe(anew Universe),
		m_Camera(anew OrthographicCamera(m_ProjectionMatrix)), m_BackgroundCamera(anew OrthographicCamera(m_ProjectionMatrix)),
		m_BackgroundRenderer(anew Renderer2D), m_Hotloader(anew Hotloader)
	{
		const String& path = Application::Instance().GetDescription().Path;

		UpdateConfigFile(path + L"config.txt");

		Hotloader::Create(m_Hotloader, path, std::bind(&Game::UpdateConfigFile, this, std::placeholders::_1));
	}

	~Game()
	{
		m_Hotloader->Join();
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

		m_Background = anew Renderable2D({ WIDTH / 2.0f, HEIGHT / 2.0f }, { 1920 / 3.6f + 100, 1080 / 3.6f + 100 }, background, 0xffffffff);
	}

	void OnEvent(Event& event) override
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<MouseMovedEvent>([&](MouseMovedEvent& e) -> bool
		{
			if (e.IsDragged())
			{
				vec2 amount = m_MouseDelta - vec2(cast(float32) e.GetX(), cast(float32) e.GetY());
				amount *= 0.4f;

				m_TargetCameraOffset += vec2(amount.x, -amount.y);
			}

			m_MouseDelta = vec2(e.GetX(), e.GetY());

			return false;
		});

		dispatcher.Dispatch<MouseScrollEvent>([&](MouseScrollEvent& e) -> bool
		{
			m_TargetCameraZoom += e.GetDistance() * -1;

			if (m_TargetCameraZoom < 20)
				m_TargetCameraZoom = 20;
			if (m_TargetCameraZoom > 840)
				m_TargetCameraZoom = 840;

			return false;
		});
	}

	s32 t = 0;
	
	void OnUpdate(const DeltaTime& dt)
	{
		t++;
		
		if (m_Speed != 0)
		{
			if (t % m_Speed == 0)
				m_Universe->DoGeneration();
		}

		m_CameraOffset.Lerp(m_TargetCameraOffset, 0.8f);
		m_CameraZoom += (m_TargetCameraZoom - m_CameraZoom) * 0.3f;

		m_Background->bounds.position = vec2(WIDTH / 2.0f, HEIGHT / 2.0f) + m_CameraOffset * 0.01f;

		m_Camera->SetProjectionMatrix(mat4::Orthographic(-m_CameraZoom, cast(float32) WIDTH + m_CameraZoom, -m_CameraZoom * ASPECT_RATIO, cast(float32) HEIGHT + m_CameraZoom * ASPECT_RATIO, -1.0f, 1.0f));
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

	bool ChangePreset(String preset)
	{
		std::transform(preset.begin(), preset.end(), preset.begin(), towlower);

		static String lastPreset = L"";
		if (lastPreset == preset)
			return false;

		if (preset == L"-clear")
			m_Universe->Clear();
		if (preset == L"-random")
		{
			m_Universe->Randomize();
			return true;
		}

		m_Universe->SetPreset(preset);

		lastPreset = preset;
		return true;
	}

	void UpdateConfigFile(String path)
	{
		String file = path;
		file = file.substr(file.find_last_of(L'\\') + 1, file.length());
		if (file != L"config.txt")
			return;

		std::wifstream wif(path);
		wif.imbue(std::locale(std::locale::empty(), anew std::codecvt_utf8<wchar>));

		String wline;
		while (std::getline(wif, wline))
		{
			if (wline[0] == L'#')
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

					print("[Config] Setting speed to /%\n", m_Speed);
				}
			}

			u32 preset = wline.find(L"preset:");
			if (preset < 20000000u)
			{
				wline.erase(0, 7);
				wline.erase(std::remove_if(wline.begin(), wline.end(), std::bind(std::isspace<wchar>, std::placeholders::_1, std::locale::classic())), wline.end());

				if (ChangePreset(wline))
					print("[Config] Setting preset to /%\n", wline);
			}
		}
	}
};

int main()
{
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

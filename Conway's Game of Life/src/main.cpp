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
	UniversePreset m_Preset;

	std::unordered_map<String, UniversePreset> m_Presets;
public:
	Game()
		: Layer2D(mat4::Orthographic(0.0f, cast(float32) WIDTH, 0.0f, cast(float32) HEIGHT, -1.0f, 1.0f)), 
		m_Universe(anew Universe), m_Hotloader(anew Hotloader), m_BackgroundRenderer(anew Renderer2D), 
		m_Camera(anew OrthographicCamera(m_ProjectionMatrix)), m_BackgroundCamera(anew OrthographicCamera(m_ProjectionMatrix)), 
		m_CameraOffset(10000.0f), m_TargetCameraOffset(10000.0f), m_Preset(DEFAULT)
	{
		m_Presets[L"-clear"] = CLEAR;
		m_Presets[L"-glider"] = GLIDER;
		m_Presets[L"-spaceship"] = SPACESHIP;
		m_Presets[L"-exploder"] = EXPLODER;
		m_Presets[L"-exploder_small"] = EXPLODER_SMALL;
		m_Presets[L"-tumbler"] = TUMBLER;
		m_Presets[L"-gosper_glider_gun"] = GOSPER_GLIDER_GUN;
		m_Presets[L"-ten_cell_row"] = TEN_CELL_ROW;
		m_Presets[L"-random"] = RANDOM;
	}

	~Game()
	{
		m_Hotloader->Join();
		
		del m_Universe;
		del m_Hotloader;
		del m_Background;
		del m_BackgroundRenderer;
		del m_Camera;
		del m_BackgroundCamera;
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

		const String& path = Application::Instance().GetDescription().Path;

		UpdateConfigFile(path + L"config.txt");
		Hotloader::Create(m_Hotloader, path, std::bind(&Game::UpdateConfigFile, this, std::placeholders::_1));
	}

	void OnEvent(Event& event) override
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<MouseMovedEvent>([&](MouseMovedEvent& e) -> bool
		{
			if (e.IsDragged())
			{
				vec2 amount = m_MouseDelta - vec2(cast(float32) e.GetX(), cast(float32) e.GetY());
			
				m_TargetCameraOffset += vec2(amount.x * 0.4f, -amount.y * 0.4f);
			}

			m_MouseDelta = vec2(cast(float32) e.GetX(), cast(float32) e.GetY());

			return false;
		});

		dispatcher.Dispatch<MouseScrollEvent>([&](MouseScrollEvent& e) -> bool
		{
			m_TargetCameraZoom += e.GetDistance() * -1;
			m_TargetCameraZoom = clamp(m_TargetCameraZoom, 200, 840);

			return false;
		});
	}

	void OnUpdate(const DeltaTime& dt)
	{
		m_Universe->Update();

		m_CameraOffset.Lerp(m_TargetCameraOffset, 0.4f);
		m_CameraZoom += (m_TargetCameraZoom - m_CameraZoom) * 0.3f;

		m_Camera->SetProjectionMatrix(mat4::Orthographic(-m_CameraZoom, cast(float32) WIDTH + m_CameraZoom, -m_CameraZoom, cast(float32) HEIGHT + m_CameraZoom, -1.0f, 1.0f));
	
		wchar buffer[1024];
		sprint(buffer, NULL, "/% | FPS: /%", g_Application.GetDescription().Name, g_Application.GetDescription().Cycle.FPS);

		g_Application.SetWindowTitle(String(buffer));
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

	bool ChangeSpeed(u32 speed)
	{
		if (m_Speed == speed)
			return false;

		m_Speed = speed;
		m_Universe->SetSpeed(m_Speed);

		return true;
	}

	bool ChangePreset(UniversePreset preset)
	{
		if (m_Preset == preset)
			return false;

		m_Preset = preset;
		m_Universe->ApplyPreset(m_Preset);

		return true;
	}

	void UpdateConfigFile(String path)
	{
		String file = path;
		file = file.substr(file.find_last_of(L'\\') + 1, file.length());
		if (file != L"config.txt")
			return;

		std::wifstream wif(path);

		String line;
		while (std::getline(wif, line))
		{
			if (line[0] == L'#')
				continue;

			line.erase(std::remove(line.begin(), line.end(), L' '), line.end());

			static constexpr wchar speedField[] = L"speed:";
			static constexpr wchar presetField[] = L"preset:";
			
			if (line.find(speedField) != String::npos)
			{
				line.erase(0, wcslen(speedField));

				if (ChangeSpeed(std::stoi(line)))
					print("[Config] Setting speed to /%\n", m_Speed);
			}

			if (line.find(presetField) != String::npos)
			{
				line.erase(0, wcslen(presetField));

				std::transform(line.begin(), line.end(), line.begin(), towlower);

				if (ChangePreset(m_Presets[line]))
					print("[Config] Setting preset to /%\n", line);
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

	print("/%\n", g_Application.GetDescription().Path);

	Game* game = cast(Game*)
	g_Application.PushLayer(anew Game());
	g_Application.ShowWindow();
	g_Application.Start();
	g_Application.PopLayer(game);

	return EXIT_SUCCESS;
}

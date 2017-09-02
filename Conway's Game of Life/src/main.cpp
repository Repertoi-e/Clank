#include "stdafx.h"

#include "Universe.h"
#include "Hotloader.h"

constexpr u32	  WIDTH = 800u;
constexpr u32	  HEIGHT = 600u;
constexpr float32 ASPECT_RATIO = WIDTH / HEIGHT;

constexpr float32 MOVE_SENSITIVITY = 0.4f;

using namespace cl;

class Game : public Scene2D
{
private:
	Universe* m_Universe = anew Universe;
	Hotloader* m_Hotloader = anew Hotloader;

	Renderable2D* m_Background;
	Renderer2D* m_BackgroundRenderer = anew Renderer2D;

	OrthographicCamera* m_BackgroundCamera;

	vec2 m_MouseDelta;
	vec2 m_CameraOffset = vec2(10000.0f);
	vec2 m_TargetCameraOffset = vec2(10000.0f);

	float32 m_CameraZoom = 0.0f;
	float32 m_TargetCameraZoom;

	u32 m_Speed;
	UniversePreset m_Preset = DEFAULT;

	std::unordered_map<String, UniversePreset> m_Presets;
public:
	Game(void)
		: Scene2D(mat4::Orthographic(0.0f, cast(float32) WIDTH, 0.0f, cast(float32) HEIGHT, -1.0f, 1.0f)),
		m_BackgroundCamera(anew OrthographicCamera(m_Camera->GetProjectionMatrix()))
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

	~Game(void)
	{
		m_Hotloader->Join();

		del m_Universe;
		del m_Hotloader;
		del m_Background;
		del m_BackgroundRenderer;
		del m_Camera;
		del m_BackgroundCamera;
	}

	void OnPush(void) override
	{
		const String& path = g_ApplicationDesc.Path;

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
		Renderer2D::Create(m_Renderer, desc);
		Renderer2D::Create(m_BackgroundRenderer, desc);

		m_Renderer->SetCamera(m_Camera);
		m_BackgroundRenderer->SetCamera(m_BackgroundCamera);

		TextureDesc textureDesc;
		textureDesc.Name = L"Background Space";
		textureDesc.Filter = TextureFilter::NEAREST;

		Texture* background = anew Texture;
		Texture::CreateFromFile(background, FileLoadProperties(L"cgl_data/bg.jpg"), textureDesc);
		g_Textures.Add(background);

		m_Background = anew Renderable2D({ WIDTH * 0.5f, HEIGHT * 0.5f }, { 633.3f, 400.0f }, g_Textures.Get(L"Background Space"));

		FontDesc fontDesc;
		{
			fontDesc.Name = L"Roboto";
			fontDesc.FileName = L"cgl_data/Roboto-Regular.ttf";
			fontDesc.Size = 60;
			fontDesc.Scale = vec2(1.0f, 1.0f);

			g_Fonts.Add(Font::CreateFromFile(anew Font, fontDesc));
		}
		{
			fontDesc.Name = L"Alex Brush";
			fontDesc.FileName = L"cgl_data/AlexBrush-Regular.ttf";
			fontDesc.Size = 90;
			fontDesc.Scale = vec2(1.0f, 1.0f);

			g_Fonts.Add(Font::CreateFromFile(anew Font, fontDesc));
		}


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

				m_TargetCameraOffset += vec2(amount.x * MOVE_SENSITIVITY, -amount.y * MOVE_SENSITIVITY);
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

		m_CameraOffset.Lerp(m_TargetCameraOffset, 0.2f);
		m_CameraZoom += (m_TargetCameraZoom - m_CameraZoom) * 0.3f;

		m_Camera->SetProjectionMatrix(mat4::Orthographic(-m_CameraZoom, cast(float32) WIDTH + m_CameraZoom, -m_CameraZoom, cast(float32) HEIGHT + m_CameraZoom, -1.0f, 1.0f));

		wchar buffer[1024];
		sprint(buffer, NULL, "/% | FPS: /%", g_ApplicationDesc.Name, g_ApplicationDesc.Cycle.FPS);

		g_Application.SetWindowTitle(String(buffer));
	}

	void PreRender(Renderer2D* renderer, Context* context) override
	{
		m_BackgroundRenderer->Begin();
		m_BackgroundRenderer->Submit(m_Background);
		m_BackgroundRenderer->End();
		m_BackgroundRenderer->Present();
	}

	void LateRender(Renderer2D* renderer, Context* context) override
	{
		// m_Universe->Draw(renderer, m_CameraOffset);

		for (u32 i = 0; i < 5; i++)
		{
			renderer->DrawString(L"Don't lie, idiot", vec2(rand() % 300, rand() % 300), *g_Fonts.Get(L"Roboto"), 0xffffffff);
			renderer->DrawString(L"I think this is some kind of a text? :^)", vec2(rand() % 300, rand() % 300), *g_Fonts.Get(L"Roboto"), 0xffffffff);
			renderer->DrawString(L"This is a strange font! (Alex Brush)", vec2(rand() % 300, rand() % 300), *g_Fonts.Get(L"Alex Brush"), 0xff6b20f7);
		}
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

	void UpdateConfigFile(const String& path)
	{
		if (GetFilenameFromPath(path) != L"config.txt")
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

				auto it = m_Presets.find(line);
				if (it == m_Presets.end())
					print("[Config] Invalid preset: /%\n", line);
				else if (ChangePreset(it->second))
					print("[Config] Setting preset to /%\n", line);
			}
		}
	}
};

void AppMain(const String& path, wchar** args, s32 argsCount)
{
	Game* game = cast(Game*)
		g_Application.PushScene(anew Game());
	g_Application.ShowWindow();
	g_Application.Start();
	g_Application.PopScene(game);
}

#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")

s32 main()
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
		desc.EntryPoint = AppMain;

		desc.Cycle.UpdateTick = 1000.0f / 60.0f;
		desc.Cycle.Timer = anew Timer;
		desc.Cycle.UpdateTimer = 0.0f;
		desc.Cycle.UpdateDeltaTime = anew DeltaTime(0.0f);
	}
	g_Application.Create(desc);

	return EXIT_SUCCESS;
}

#include <Clank.h>

#include <Shellapi.h>

#include "../Resource.h"

using namespace cl;

#define WIDTH			800
#define HEIGHT			600
#define ASPECT_RATIO    (WIDTH / HEIGHT)

#pragma comment(linker, "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")

class Viewer : public Layer2D
{
private:
	String m_Image;

	s32 m_Width = WIDTH, m_Height = HEIGHT;
	float32 m_AspectRatio = ASPECT_RATIO;

	OrthographicCamera* m_Camera;

	Texture* m_ViewerTexture = NULLPTR;
	Renderable2D* m_Viewer = NULLPTR;

	float32 m_CameraZoom = 1.0f;
	vec2 m_MouseDelta;
public:
	Viewer(const String& image)
		: Layer2D(mat4::Orthographic(-cast(float32) WIDTH / 2, cast(float32) WIDTH / 2, -cast(float32) HEIGHT / 2, cast(float32) HEIGHT / 2, -1.0f, 1.0f)),
		m_Image(image), m_Camera(anew OrthographicCamera(m_ProjectionMatrix))
	{
		m_ViewerTexture = anew Texture;
	}

	void UpdateImage()
	{
		TextureDesc textureDesc;
		{
			textureDesc.Name = L"Image";
			textureDesc.Filter = TextureFilter::LINEAR;
		}
		Texture::CreateFromFile(m_ViewerTexture, FileLoadProperties(m_Image, false, true), textureDesc);

		const TextureDesc& imageDesc = m_ViewerTexture->GetDesc();

		float32 width, height;
		float32 imgAspectRatio = cast(float32) imageDesc.Width / cast(float32) imageDesc.Height;

		if (imgAspectRatio > m_AspectRatio)
		{
			width = m_Width < imageDesc.Width ? m_Width : imageDesc.Width;
			height = width / imgAspectRatio;
		}
		else
		{
			height = m_Height < imageDesc.Height ? m_Height : imageDesc.Height;
			width = height * imgAspectRatio;
		}
		if (!m_Viewer)
			m_Viewer = Add(anew Renderable2D(vec2(0.0f, 0.0f), vec2(width / 2, height / 2), m_ViewerTexture));
		else
		{
			m_Viewer->bounds = cl::Rectangle(0, 0, width / 2, height / 2);
		}
		if (m_Image != L"NULL")
			g_Application.SetWindowTitle(L"Photo Viewer - " + m_Image);
		else
			g_Application.SetWindowTitle(L"Photo Viewer");
	}

	void Init(Context* context, Renderer2D* renderer) override
	{
		const String& path = Application::Instance().GetDescription().Path;

		Renderer2DDesc desc;
		{
			ZeroMemory(&desc, sizeof(Renderer2DDesc));

			desc.MaxQuads = 50000u;
			desc.MaxVertices = desc.MaxQuads * 4;
			desc.BufferSize = desc.MaxVertices * sizeof(Vertex);
			desc.MaxIndices = desc.MaxQuads * 6;
			desc.VertexShaderFile = L"VS_R2D.cso";
			desc.PixelShaderFile = L"PS_R2D.cso";
		}
		Renderer2D::Create(renderer, desc);
		renderer->SetCamera(m_Camera);

		UpdateImage();
	}

	void PreRender(Context* context, Renderer2D* renderer) override
	{
		context->Clear(vec4(224 / 255.0f, 249 / 255.0f, 240 / 255.0f, 255 / 255.0f));
	}  

	void OnUpdate(const DeltaTime& dt) override
	{
		m_Camera->SetProjectionMatrix(mat4::Orthographic(-m_Width / 2 * m_CameraZoom, m_Width / 2 * m_CameraZoom, -m_Height / 2 * m_CameraZoom, m_Height / 2 * m_CameraZoom, -1.0f, 1.0f));
	}

	void OnEvent(Event& event) override
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<MouseScrollEvent>([&](MouseScrollEvent& e) -> bool
		{
			m_CameraZoom += e.GetDistance() * -0.001f;
			m_CameraZoom = clamp(m_CameraZoom, 0.1f, 2);

			return false;
		});
		dispatcher.Dispatch<MouseMovedEvent>([&](MouseMovedEvent& e) -> bool
		{
			if (e.IsDragged())
			{
				vec2 amount = m_MouseDelta - vec2(cast(float32) e.GetX(), cast(float32) e.GetY());

				cl::Rectangle& bounds = m_Viewer->bounds;
				bounds.position += vec2(-amount.x * m_CameraZoom, amount.y * m_CameraZoom);
				bounds.x = clamp(bounds.x, -bounds.width * 3, bounds.width * 3);
				bounds.y = clamp(bounds.y, -bounds.height * 3, bounds.height * 3);
			}

			m_MouseDelta = vec2(cast(float32) e.GetX(), cast(float32) e.GetY());

			return false;
		});
		dispatcher.Dispatch<WindowResizeEvent>([&](WindowResizeEvent& e) -> bool
		{
			m_Width = e.GetWidth();
			m_Height = e.GetHeight();
			m_AspectRatio = m_Width / m_Height;
			UpdateImage();

			return true;
		});
		dispatcher.Dispatch<WindowFileDropEvent>([&](WindowFileDropEvent& e) -> bool
		{
			m_Image = e.GetFilePath();
			UpdateImage();

			return true;
		});
	}
};

void AppMain(const String& exePath, wchar** args, s32 argsCount)
{
	String fileName = L"NULL";

	if (argsCount > 1)
		fileName = args[1];

	g_Application.PushLayer(anew Viewer(fileName));
}

s32 main()
{
	SetLocale(LC_ALL);

	ApplicationDesc desc;
	{
		ZeroMemory(&desc, sizeof(ApplicationDesc));

		desc.Name = L"Photo Viewer";
		desc.ClassName = L"Clank Application";
		desc.Width = WIDTH;
		desc.Height = HEIGHT;
		desc.WindowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SIZEBOX;
		desc.EntryPoint = AppMain;
		desc.SleepInCycle = true;
		desc.VSync = true;
		desc.AcceptDroppedFiles = true;
		desc.Icon = IDI_ICON1;
		desc.SmallIcon = IDI_ICON1_SMALL;

		desc.Cycle.UpdateTick = 1000.0f / 60.0f;
		desc.Cycle.Timer = anew Timer;
		desc.Cycle.UpdateTimer = 0.0f;
		desc.Cycle.UpdateDeltaTime = anew DeltaTime(0.0f);
	}
	g_Application.Create(desc);

	g_Application.ShowWindow();
	g_Application.Start();

	return EXIT_SUCCESS;
}
#include <Clank.h>
#include <Shellapi.h>
#include "../Resource.h"

#define ASPECT_RATIO    (WIDTH / HEIGHT)

using namespace cl;
using namespace std::literals;

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
		m_Image(image), m_Camera(anew OrthographicCamera(m_ProjectionMatrix)), m_ViewerTexture(anew Texture)
	{
	}

	void Init(Context* context, Renderer2D* renderer) override
	{
		const String& path = g_ApplicationDesc.Path;

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

		m_Viewer = Add(anew Renderable2D(vec2(0.0f, 0.0f), vec2(0.0f, 0.0f), m_ViewerTexture));
		UpdateImage();
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
			width = cast(float32) (m_Width < cast(s32) imageDesc.Width ? m_Width : imageDesc.Width);
			height = width / imgAspectRatio;
		}
		else
		{
			height = cast(float32) (m_Height < cast(s32) imageDesc.Height ? m_Height : imageDesc.Height);
			width = height * imgAspectRatio;
		}
		m_Viewer->bounds = cl::Rectangle(0, 0, width / 2, height / 2);

		g_Application.SetWindowTitle(g_ApplicationDesc.Name + ((m_Image != L"NULL") ? L" - "s + m_Image : L""));
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
			m_Width = cast(s32) e.GetWidth();
			m_Height = cast(s32) e.GetHeight();
			m_AspectRatio = cast(float32) (m_Width / m_Height);
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

void AppMain(const String& path, wchar** args, s32 argsCount)
{
	String fileName = L"NULL";

	if (argsCount > 1)
		fileName = args[1];

	g_Application.PushLayer(anew Viewer(fileName));

	g_Application.ShowWindow();
	g_Application.Start();
}
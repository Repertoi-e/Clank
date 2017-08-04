#pragma once

#include <Clank.h>

using namespace cl;

class Level;

class Player : public cl::Renderable2D
{
public:
	Texture* tex;

	vec2 velocity;
	vec2 acceleration;
	bool flipped;
	bool moving = false;

	const float32 speed = 350;

	bool pressedW = false , pressedA = false, pressedS = false, pressedD = false;

	Player()
		: Renderable2D({ 40, 30 }, {20, 32}, NULLPTR, 0xffffffff)
	{
		TextureSettings textureSettings;
		textureSettings.Filter = TextureFilter::NEAREST;

		TextureLoadSettings textureLoadSettings;
		textureLoadSettings.FlipHorizontal = false;
		textureLoadSettings.FlipVertical = false;

		tex = new Texture(textureSettings, textureLoadSettings);
		tex->CreateFromFile(L"data/textures/player.dds");

		texture = tex;
	}

	void OnEvent(cl::Event& event)
	{
		cl::EventDispatcher disp(event);
		disp.Dispatch<cl::KeyPressedEvent>([&](cl::KeyPressedEvent& e) -> bool {
			if (e.GetButton() == KEY_W)
				pressedW = true;
			if (e.GetButton() == KEY_A)
				pressedA = true;
			if (e.GetButton() == KEY_S)
				pressedS = true;
			if (e.GetButton() == KEY_D)
				pressedD = true;
			return true;
		});

		disp.Dispatch<cl::KeyReleasedEvent>([&](cl::KeyReleasedEvent& e) -> bool {
			if (e.GetButton() == KEY_W)
				pressedW = false;
			if (e.GetButton() == KEY_A)
				pressedA = false;
			if (e.GetButton() == KEY_S)
				pressedS = false;
			if (e.GetButton() == KEY_D)
				pressedD = false;
			return true;
		});
	}

	void Collision(Level* level, float32 dt);

	void Update(const DeltaTime& dt, Level* level);
};
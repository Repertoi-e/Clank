#include "Player.h"

#include "Level.h"

u32 time;

void Player::Update(const DeltaTime& dt, Level* level)
{
	time++;

	s32 xa = 0, ya = 0;
	if (pressedW)
		ya++;
	if (pressedA)
		xa--;
	if (pressedS)
		ya--;
	if (pressedD)
		xa++;

	velocity.x = xa;
	velocity.y = ya;
	if (xa != 0 && ya != 0)
		velocity.Normalise();

	Time t = dt.Elapsed();
	Collision(level, t.Seconds());
}

void Player::Collision(Level* level, float32 dt)
{
	vec2 dest = bounds.position + velocity * speed * dt;

	bool collidingX = false, collidingY = false; 
	for (s32 xx = -1; xx <= 1; xx++)
		for (s32 yy = -1; yy <= 1; yy++)
		{
			u32 tileX = dest.x / 32 + xx;
			u32 tileY = dest.y / 32 + yy;

			if (tileX < 0 || tileY < 0 || tileX >= level->width || tileY >= level->height)
				continue;

			if (TileDatabase::Instance().tiles[level->tiles[tileX][tileY].id]->collidable)
			{
				cl::Rectangle r;
				r.position = vec2(tileX * 32 + 16, tileY * 32 + 16);
				r.size = vec2(16, 16);

				//bounds.position.x = round(bounds.position.x);
				//bounds.position.y = round(bounds.position.y);

				if (dest.x + bounds.width > r.x - r.width
					&& dest.x - bounds.width < r.x + r.width
					&& bounds.y + bounds.height > r.y - r.height
					&& bounds.y - bounds.height < r.y + r.height)
					collidingX = true;

				if (bounds.x + bounds.width > r.x - r.width
					&& bounds.x - bounds.width < r.x + r.width
					&& dest.y + bounds.height > r.y - r.height
					&& dest.y - bounds.height < r.y + r.height)
					collidingY = true;
			}
		}

	if (!collidingX)
		bounds.position.x += velocity.x * speed * dt;
	if (!collidingY)
		bounds.position.y += velocity.y * speed * dt;

	velocity.x = 0;
	velocity.y = 0;

	if (bounds.position.x < 0)
		bounds.position.x = 0;
}
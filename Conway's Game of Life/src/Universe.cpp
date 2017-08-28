#include "stdafx.h"
#include "Universe.h"

using namespace cl;

Universe::Universe()
{
	m_Cells = anew CellState[UNIVERSE_SIZE * UNIVERSE_SIZE];
	m_PrevCells = anew CellState[UNIVERSE_SIZE * UNIVERSE_SIZE];
}

void Universe::SetAlive(u32 x, u32 y)
{
	m_Cells[x + y * UNIVERSE_SIZE] = ALIVE;
}

void Universe::ApplyPreset(UniversePreset preset)
{
	Clear();

	// This is so horrible
	// Please fix

	if (preset == CLEAR) Clear();
	if (preset == RANDOM) Randomize();
	if (preset == GLIDER) { SetAlive(250, 250); SetAlive(251, 251); SetAlive(251, 252); SetAlive(250, 252);
							SetAlive(249, 252); }
	if (preset == SPACESHIP) { SetAlive(250, 250); SetAlive(250, 252); SetAlive(251, 253); SetAlive(252, 253); 
							   SetAlive(253, 253); SetAlive(254, 253); SetAlive(254, 252); SetAlive(254, 251); 
							   SetAlive(253, 250); }
	if (preset == EXPLODER) { SetAlive(250, 250); SetAlive(250, 251); SetAlive(250, 252); SetAlive(250, 253); 
							  SetAlive(250, 254); SetAlive(254, 250); SetAlive(254, 251); SetAlive(254, 252); 
							  SetAlive(254, 253); SetAlive(254, 254); SetAlive(252, 254); SetAlive(252, 250); }
	if (preset == EXPLODER_SMALL) { SetAlive(250, 250); SetAlive(249, 251); SetAlive(249, 252); SetAlive(250, 252);
									SetAlive(250, 253); SetAlive(251, 251); SetAlive(251, 252); }
	if (preset == TUMBLER) { SetAlive(250, 250); SetAlive(251, 250); SetAlive(255, 250); SetAlive(256, 250); 
							 SetAlive(250, 251); SetAlive(252, 251); SetAlive(254, 251); SetAlive(256, 251); 
							 SetAlive(250, 252); SetAlive(252, 252); SetAlive(254, 252); SetAlive(256, 252); 
							 SetAlive(252, 253); SetAlive(254, 253); SetAlive(252, 254); SetAlive(254, 254); 
							 SetAlive(251, 254); SetAlive(255, 254); SetAlive(252, 255); SetAlive(254, 255); 
							 SetAlive(251, 255); SetAlive(255, 255); }
	if (preset == GOSPER_GLIDER_GUN) { SetAlive(250, 250); SetAlive(250, 251); SetAlive(250, 252); SetAlive(251, 252); 
									   SetAlive(252, 251); SetAlive(242, 252); SetAlive(242, 253); SetAlive(243, 252); 
									   SetAlive(244, 254); SetAlive(243, 254); SetAlive(244, 253); SetAlive(256, 254); 
									   SetAlive(256, 255); SetAlive(257, 254); SetAlive(258, 256); SetAlive(257, 256); 
									   SetAlive(258, 255); SetAlive(234, 253); SetAlive(235, 253); SetAlive(234, 254); 
									   SetAlive(235, 254); SetAlive(268, 255); SetAlive(269, 255); SetAlive(268, 256); 
									   SetAlive(269, 256); SetAlive(269, 247); SetAlive(269, 248); SetAlive(269, 249); 
									   SetAlive(270, 249); SetAlive(271, 248); SetAlive(258, 243); SetAlive(259, 242); 
									   SetAlive(258, 244); SetAlive(259, 244); SetAlive(260, 244); }
	if (preset == TEN_CELL_ROW) { SetAlive(247, 250); SetAlive(248, 250); SetAlive(249, 250); SetAlive(250, 250); 
								  SetAlive(251, 250); SetAlive(252, 250); SetAlive(253, 250); SetAlive(254, 250); 
								  SetAlive(255, 250); SetAlive(256, 250); }
}

void Universe::SetSpeed(u32 speed)
{
	m_Speed = speed;
}

void Universe::Randomize(void)
{
	srand(cast(u32) time(NULL));
	
	for (u32 i = 0; i < UNIVERSE_SIZE * UNIVERSE_SIZE; ++i)
		m_Cells[i] = rand() % 20 > 10 ? DEAD : ALIVE;
}

void Universe::Clear(void)
{
	memset(m_Cells, 0, UNIVERSE_SIZE * UNIVERSE_SIZE * sizeof(CellState));
}

void Universe::DoGeneration(void)
{
	memcpy(m_PrevCells, m_Cells, UNIVERSE_SIZE * UNIVERSE_SIZE * sizeof(CellState));

	for (u32 x = 0; x < UNIVERSE_SIZE; x++)
	{
		for (u32 y = 0; y < UNIVERSE_SIZE; y++)
		{
			s32 count = 0;
			for (s32 xa = -1; xa <= 1; xa++)
				for (s32 ya = -1; ya <= 1; ya++)
				{
					if (xa == 0 && ya == 0)
						continue;

					const u32 xx = x + xa;
					const u32 yy = y + ya;

					if (xx > UNIVERSE_SIZE || yy > UNIVERSE_SIZE)
						continue;

					if (m_PrevCells[xx + yy * UNIVERSE_SIZE] == ALIVE)
						++count;
				}

			CellState& state = m_Cells[x + y * UNIVERSE_SIZE];
			if (count < 2)
				state = DEAD;
			if (count == 3)
				state = ALIVE;
			if (count > 3)
				state = DEAD;
		} // for y..
	} // for x..
}

void Universe::Update(void)
{
	m_Ticks++;

	if (m_Speed != 0)
		if (m_Ticks % m_Speed == 0)
			DoGeneration();
}

void Universe::Draw(Renderer2D* renderer, const cl::vec2& offset)
{
	static Renderable2D* cell = anew Renderable2D({ 0, 0 }, { CELL_RENDER_SIZE / 2, CELL_RENDER_SIZE / 2 }, 0xaa42f48c);

	for (u32 x = 0; x < (UNIVERSE_SIZE); x++)
	{
		for (u32 y = 0; y < (UNIVERSE_SIZE); y++)
		{
			const u32 indeX = (x + cast(u32) offset.x) % (UNIVERSE_SIZE);
			const u32 indeY = (y + cast(u32) offset.y) % (UNIVERSE_SIZE);

			const BOOL& state = m_Cells[indeX + indeY * UNIVERSE_SIZE];
			if (state != ALIVE)
				continue;
			
			const vec2 worldPos = vec2(cast(float32) x, cast(float32) y) * CELL_RENDER_SIZE;

			cell->bounds.position = worldPos + vec2(-830.833374f, -830.833374f);
			renderer->Submit(cell);
		} // for y..
	} // for x..
}

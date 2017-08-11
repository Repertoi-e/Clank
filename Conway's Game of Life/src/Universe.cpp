#include "Universe.h"

using namespace cl;

Universe::Universe()
{
	m_Cells = cl_new CellState[UNIVERSE_SIZE * UNIVERSE_SIZE];
	m_PrevCells = cl_new CellState[UNIVERSE_SIZE * UNIVERSE_SIZE];
}

void Universe::SetAlive(u32 x, u32 y)
{
	m_Cells[x + y * UNIVERSE_SIZE] = ALIVE;
}

void Universe::SetPreset(const String& preset)
{
	Clear();
	
	if(preset==L"-glider"){SetAlive(250,250);SetAlive(251,251);SetAlive(251,252);SetAlive(250,252);SetAlive(249,252);}if(preset==L"-spaceshi\
	p"){SetAlive(250,250);SetAlive(250,252);SetAlive(251,253);SetAlive(252,253);SetAlive(253,253);SetAlive(254,253);SetAlive(254,252);SetAlive
	(254,251);SetAlive(253,250);}if(preset==L"-exploder"){SetAlive(250,250);SetAlive(250,251);SetAlive(250,252);SetAlive(250,253);SetAlive(250
	,254);SetAlive(254,250);SetAlive(254,251);SetAlive(254,252);SetAlive(254,253);SetAlive(254,254);SetAlive(252,254);SetAlive(252,250);}if(preset
	==L"-exploder_small"){SetAlive(250,250);SetAlive(249,251);SetAlive(249,252);SetAlive(250,252);SetAlive(250,253);SetAlive(251,251);SetAlive(251
	,252);}if(preset==L"-tumbler"){SetAlive(250,250);SetAlive(251,250);SetAlive(255,250);SetAlive(256,250);SetAlive(250,251);SetAlive(252,251);
	SetAlive(254,251);SetAlive(256,251);SetAlive(250,252);SetAlive(252,252);SetAlive(254,252);SetAlive(256,252);SetAlive(252,253);SetAlive(254,
	253);SetAlive(252,254);SetAlive(254,254);SetAlive(251,254);SetAlive(255,254);SetAlive(252,255);SetAlive(254,255);SetAlive(251,255);SetAlive
	(255,255);}if(preset==L"-gosper_glider_gun"){SetAlive(250,250);SetAlive(250,251);SetAlive(250,252);SetAlive(251,252);SetAlive(252,251);SetAlive
	(242,252);SetAlive(242,253);SetAlive(243,252);SetAlive(244,254);SetAlive(243,254);SetAlive(244,253);SetAlive(256,254);SetAlive(256,255);SetAlive
	(257,254);SetAlive(258,256);SetAlive(257,256);SetAlive(258,255);SetAlive(234,253);SetAlive(235,253);SetAlive(234,254);SetAlive(235,254);SetAlive
	(268,255);SetAlive(269,255);SetAlive(268,256);SetAlive(269,256);SetAlive(269,247);SetAlive(269,248);SetAlive(269,249);SetAlive(270,249);SetAlive
	(271,248);SetAlive(258,243);SetAlive(259,242);SetAlive(258,244);SetAlive(259,244);SetAlive(260,244);}if(preset==L"-ten_cell_row"){SetAlive(247,
	250);SetAlive(248,250);SetAlive(249,250);SetAlive(250,250);SetAlive(251,250);SetAlive(252,250);SetAlive(253,250);SetAlive(254,250);SetAlive(255,250)
	;SetAlive(256,250);}if(preset==L"-custom"){}
}

void Universe::Randomize()
{
	srand(time(NULL));
	
	CellState* ptr = &m_RNGArray[0];
	for (u32 i = 0; i < UNIVERSE_SIZE * UNIVERSE_SIZE; ++i)
		*ptr++ = rand() % 20 > 10 ? DEAD : ALIVE;

	memcpy(m_Cells, m_RNGArray.data(), UNIVERSE_SIZE * UNIVERSE_SIZE * sizeof(CellState));
}

void Universe::Clear()
{
	memset(m_Cells, 0, UNIVERSE_SIZE * UNIVERSE_SIZE * sizeof(CellState));
}

void Universe::DoGeneration()
{
	memcpy(m_PrevCells, m_Cells, UNIVERSE_SIZE * UNIVERSE_SIZE * sizeof(CellState));

	for (u32 x = 0; x < UNIVERSE_SIZE + 1; x++)
	{
		for (u32 y = 0; y < UNIVERSE_SIZE + 1; y++)
		{
			s32 count = 0;
			for (s32 xa = -1; xa <= 1; xa++)
				for (s32 ya = -1; ya <= 1; ya++)
				{
					if (xa == 0 && ya == 0)
						continue;

					u32 xx = x + xa;
					u32 yy = y + ya;

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

void Universe::Draw(Renderer2D* renderer, const cl::vec2& offset)
{
	static Renderable2D* cell = new Renderable2D({ 0, 0 }, { CELL_RENDER_SIZE / 2, CELL_RENDER_SIZE / 2 }, 0, 0xaa42f48c);

	for (u32 x = 0; x < (UNIVERSE_SIZE); x++)
	{
		for (u32 y = 0; y < (UNIVERSE_SIZE); y++)
		{
			u32 indeX = (x + cast(u32) offset.x) % (UNIVERSE_SIZE);
			u32 indeY = (y + cast(u32) offset.y) % (UNIVERSE_SIZE);

			const CellState& state = m_Cells[indeX + indeY * UNIVERSE_SIZE];
			if (state != ALIVE)
				continue;
			
			cell->bounds.position = vec2(x, y) * CELL_RENDER_SIZE + vec2(CELL_RENDER_SIZE, CELL_RENDER_SIZE) / 2 - (vec2(CELL_RENDER_SIZE, CELL_RENDER_SIZE) * UNIVERSE_SIZE) / 3;
			renderer->Submit(cell);
		} // for y..
	} // for x..
}

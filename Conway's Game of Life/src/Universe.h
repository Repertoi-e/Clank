#pragma once

#include <Clank.h>

#include <time.h>

constexpr u32	  UNIVERSE_SIZE	   = 500u;
constexpr float32 CELL_RENDER_SIZE = 5.0f;

enum UniversePreset : u8
{
	DEFAULT = 0,
	CLEAR,
	GLIDER,
	SPACESHIP,
	EXPLODER,
	EXPLODER_SMALL,
	TUMBLER,
	GOSPER_GLIDER_GUN,
	TEN_CELL_ROW,
	RANDOM,
};

enum CellState : bool
{
	DEAD = false,
	ALIVE = true
};

class Universe
{
private:
	CellState* m_Cells;
	CellState* m_PrevCells;

	s32 m_Ticks;
	u32 m_Speed;
public:
	Universe();

	void SetAlive(u32 x, u32 y);
	void SetSpeed(u32 speed);

	void ApplyPreset(UniversePreset preset);

	void Clear(void);
	void Randomize(void);
	void DoGeneration(void);

	void Update(void);
	void Draw(cl::Renderer2D* renderer, const cl::vec2& offset);
};
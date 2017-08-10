#pragma once

#include <Clank.h>

#include <time.h>

constexpr u32	  UNIVERSE_SIZE	   = 500u;
constexpr float32 CELL_RENDER_SIZE = 5.0f;

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

	std::array<CellState, UNIVERSE_SIZE * UNIVERSE_SIZE> m_RNGArray;
public:
	Universe();

	void SetAlive(u32 x, u32 y);
	void SetPreset(const String& preset);

	void Clear();
	void Randomize();
	void DoGeneration();
	void Draw(cl::Renderer2D* renderer, const cl::vec2& offset);
};
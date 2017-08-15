#pragma once

#include <Clank.h>

#include "ReadBMP.h"

enum Tiles
{
	AIR = 0,
	DIRT = 1,
	GRASS = 2,

	END 
};

struct TileID
{
	byte id;

	TileID(byte id)
		: id(id)
	{}

	bool operator==(const TileID& other)
	{
		return id == other.id;
	}
};

class Tile
{
public:
	TileID id;
	bool collidable = false;

	Tile(TileID id)
		: id(id)
	{
	}

	virtual void Render(cl::Renderer2D* renderer, cl::vec2 position) = 0;
	virtual void Init() {}
};

class AirTile : public Tile
{
public:
	AirTile()
		: Tile(TileID(0))
	{
		collidable = false;
	}
	
	void Render(cl::Renderer2D* renderer, cl::vec2 position) override
	{
	}
};

class ClassicTile : public Tile
{
public:
	cl::Renderable2D* r;

	ClassicTile(TileID id);

	void Render(cl::Renderer2D* renderer, cl::vec2 position) override;

	void Init() override;
};

class TextureAtlas : public cl::Texture
{
public:
	s32 tilesize;

	TextureAtlas(String file, cl::TextureDesc& textureDesc, cl::TextureLoadProperties textureLoadProperties = { 0, 0 })
	{
		Texture::CreateFromFile(this, file, textureDesc, std::move(textureLoadProperties));
	}

	std::vector<cl::vec2> GetUVs(s32 x, s32 y)
	{
		float32 pixelX = x * tilesize;
		float32 pixelY = y * tilesize;

		float32 ax = pixelX / m_Width;
		float32 ay = pixelY / m_Height;

		float32 dx = (pixelX + tilesize) / m_Width;
		float32 dy = (pixelY + tilesize) / m_Height;

		std::vector<cl::vec2> result;
		result.push_back({ ax, ay });
		result.push_back({ dx, ay });
		result.push_back({ ax, dy });
		result.push_back({ dx, dy });

		return result;
	}
};

class TileDatabase : public cl::Singleton<TileDatabase>
{
public:
	TextureAtlas* tatlas;
	std::vector<Tile*> tiles;

	TileDatabase()
		: tiles(Tiles::END)
	{
		cl::TextureDesc textureSettings;
		{
			ZeroMemory(&textureSettings, sizeof(cl::TextureDesc));

			textureSettings.Filter = cl::TextureFilter::NEAREST;
		}
		tatlas = anew TextureAtlas(L"data/textures/atlas.dds", textureSettings);

		tatlas->tilesize = 32;

		tiles[AIR] = anew AirTile();
		tiles[DIRT] = anew ClassicTile(DIRT);
		tiles[GRASS] = anew ClassicTile(GRASS);
	}

	void InitTiles()
	{
		tiles[AIR]->Init();
		tiles[DIRT]->Init();
		tiles[GRASS]->Init();
	}
};

#define FOR_EACH(x) for(u32 x = 0; x < 20; x++) { for (u32 y = 0; y < 20; y++) {  } }

class Player;

class Level
{
public:
	u32 width, height;
	std::vector<std::vector<TileID>> tiles;
	Player* player;
	std::vector<cl::vec2> highlight;
	
	cl::Renderable2D* high = NULLPTR;

	Level(Player* player);

	void Render(cl::Renderer2D* renderer)
	{
		for (u32 x = 0; x < width; x++) { for (u32 y = 0; y < height; y++) {
			TileDatabase::Instance().tiles[tiles[x][y].id]->Render(renderer, { cast(float32) x, cast(float32) y });
		} }

		if (!high)
			high = anew cl::Renderable2D({ 0, 0 }, {16, 16}, 0xff00ffff);
		
		for (auto a : highlight)
		{
			high->bounds.position = { a.x * 32 + 16, a.y * 32 + 16 };
			renderer->Submit(high);
		}
	}
};
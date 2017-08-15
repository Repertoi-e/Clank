#include "Level.h"

#include "Player.h"

Level::Level(Player* player)
	: player(player)
{
	TileDatabase::Instance().InitTiles();

	BMPImage image = BMPImage("data/level1.bmp");
	width = image.info_header->biWidth;
	height = image.info_header->biHeight;

	cl::vec3ui8* pixelColors = cast(cl::vec3ui8*) image.pixels.data();

	for (u32 x = 0; x < width; x++)
	{
		tiles.push_back({});
		for (u32 y = 0; y < height; y++)
			tiles[x].push_back({ 0 });
	}

	for (u32 x = 0; x < width; x++)
		for (u32 y = 0; y < height; y++)
		{
			if (pixelColors[y * width + x] == cl::vec3ui8(0, 51, 127 /*ITS BGR, not RGB*/))
				tiles[x][y].id = DIRT;
			else if (pixelColors[y * width + x] == cl::vec3ui8(0, 255, 21))
				tiles[x][y].id = GRASS;
			else if (pixelColors[y * width + x] == cl::vec3ui8(124, 13, 34))
				player->bounds.position = { cast(float32) x * 32, cast(float32) y * 32 };
		}
}

ClassicTile::ClassicTile(TileID id)
	: Tile(id)
{
	collidable = true;
	r = anew cl::Renderable2D({ 16, 16 }, { 16, 16 }, NULLPTR, 0xffffffff);
}

void ClassicTile::Render(cl::Renderer2D* renderer, cl::vec2 position)
{
	r->bounds.position = position * 32 + cl::vec2(16, 16);
	r->Submit(renderer);
}

void ClassicTile::Init()
{
	r->texture = TileDatabase::Instance().tatlas;
	
	cl::vec2i pos = cl::vec2i();
	if (id == GRASS)
		pos = cl::vec2i(1, 0);

	auto uvs = TileDatabase::Instance().tatlas->GetUVs(pos.x, pos.y);
	auto ptr = r->uvs;
	*ptr = uvs[0];
	ptr++;
	*ptr = uvs[1];
	ptr++;
	*ptr = uvs[2];
	ptr++;
	*ptr = uvs[3];
}

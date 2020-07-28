#define OLC_PGE_APPLICATION
#include <iostream>
#include "olcPixelGameEngine.h"

std::vector<int> findCommonDivisors(int a, int b)
{
	std::vector<int> divisors;
	for (int i = 1; i <= a && i <= b; i++)
	{
		if (a % i == 0 && b % i == 0)
		{
			divisors.push_back(i);
		}
	}
	return divisors;
}

class DnDTool : public olc::PixelGameEngine
{
	struct map
	{
		struct link
		{
			std::string mapIdentifier; 
			/*since a link cant hold a map, it can hold a string, 
			and then this string will be searched inside the vector of maps*/
			olc::vf2d position;
		public:
			link(std::string mapIdentifier_in, olc::vf2d position_in)
			{
				mapIdentifier = mapIdentifier_in;
				position = position_in;
			}
		};
		std::vector<link> links;
		olc::Decal* background;
		int tileScale;
	public:
		std::string mapIdentifier;
		map(std::string mapIdentifier_in, olc::Decal* background_in ,std::vector<link> links_in, int tileScale_in)
		{
			mapIdentifier = mapIdentifier_in;
			links = links_in;
			background = background_in;
			tileScale = tileScale_in;
		}
	};
	struct token
	{

	};
	std::vector<map> maps;
	std::vector<olc::Decal*> backgrounds;
	std::vector<olc::Decal*> icons;
	olc::Decal* gridTile;
	bool selected = false;
	olc::vf2d selectedTile = { 0,0 };
	int currentMap = 0;
	public:
		float width = 1920.0f * 0.4f;
		float height = 1080.0f * 0.4f;
	bool OnUserCreate()override
	{
		loadDecals();
		constructMaps();
		return true;
	}

	bool OnUserUpdate(float fElepsedTime)override
	{
		drawMap();
		if (GetMouse(0).bPressed)
		{
			selected = !selected;
		}
		return true;
	}
	void loadDecals()
	{
		gridTile = new olc::Decal(new olc::Sprite("./Assets/Tile.png"));
		backgrounds.push_back(new olc::Decal(new olc::Sprite("./Assets/1.png")));
	}
	void constructMaps()
	{
		maps =
		{
			map("MapName", backgrounds[0], std::vector<map::link> { map::link("Hello", {5,5}) }, 1)
		};
	}
	void drawMap()
	{
		Clear(olc::WHITE);
		float scale = (width/ maps[currentMap].background->sprite->width);
		DrawDecal({ 0,0 }, maps[currentMap].background, { scale, scale }, olc::WHITE);
		float modifier = findCommonDivisors(width, height)[9];
		float divisor = 256;
		for (int i = 0; i < (width / modifier); i++)
		{
			for (int j = 0; j < (height / modifier); j++)
			{
				bool temp = ((int)((GetMouseX() * divisor) / (modifier * gridTile->sprite->width)) == i) && ((int)((GetMouseY() * divisor) / (modifier * gridTile->sprite->height)) == j);
				olc::Pixel tint = temp ? olc::RED : olc::WHITE;
				DrawDecal({
					(float)i * gridTile->sprite->width * modifier / divisor,
					(float)j * gridTile->sprite->height * modifier / divisor },
					gridTile, { modifier / divisor, modifier / divisor }, tint);
			}
		}
		for (unsigned int i = 0; i < maps[currentMap].links.size(); i++)
		{
			bool temp = ((int)((GetMouseX() * divisor) / (modifier * gridTile->sprite->width)) == maps[currentMap].links[i].position.x) && ((int)((GetMouseY() * divisor) / (modifier * gridTile->sprite->height)) == maps[currentMap].links[i].position.y);
			olc::Pixel tint = temp ? olc::RED : olc::WHITE;
			olc::vf2d position = { maps[currentMap].links[i].position.x * gridTile->sprite->width * modifier / divisor, maps[currentMap].links[i].position.y * gridTile->sprite->height* modifier / divisor + gridTile->sprite->height * modifier / divisor * 0.8f};
			DrawStringDecal(position, maps[currentMap].links[i].mapIdentifier, tint);
		}
		DrawStringDecal({ 60,10 }, "X: " + std::to_string((int)((GetMouseX() * 256) / (modifier * gridTile->sprite->width))) + " Y: " + std::to_string((int)((GetMouseY() * 256) / (modifier * gridTile->sprite->height))), olc::WHITE);
	}
};

int main()
{
	DnDTool dndtool;
	if (dndtool.Construct(dndtool.width, dndtool.height, 2, 2))
	{
		dndtool.Start();
	}
	return 0;
}

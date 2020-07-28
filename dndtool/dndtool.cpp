#define OLC_PGE_APPLICATION
#include <iostream>
#include "olcPixelGameEngine.h"
#include "GabNubUtilities.h"

class DnDTool : public olc::PixelGameEngine
{
	enum InteractionMode 
	{
		MOVE, DRAW, ERASE, MEASURE
	};
	InteractionMode interactionMode = InteractionMode::ERASE;

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
		token(olc::Decal* icon_in, olc::vf2d position_in, olc::Pixel tint_in, std::string name_in)
		{
			icon = icon_in;
			position = position_in;
			tint = tint_in;
			name = name_in;
		}
		olc::Decal* icon;
		olc::Pixel tint;
		olc::vf2d position;
		std::string name;
	};

	std::vector<map> maps;
	std::vector<olc::Decal*> backgrounds;
	olc::Sprite* fogOfWar;
	std::vector<olc::Decal*> icons;

	std::vector<olc::Decal*> cursors;
	std::vector<token> NPCs;
	token* heldToken = nullptr;

	olc::Decal* gridTile;
	int commonDivisorIndex = 9;
	olc::Pixel gridColor = olc::WHITE;

	Gdiplus::Bitmap* iconMask = nullptr;
	Gdiplus::Bitmap* eraserMask = nullptr;

	int BackgroundLayer = 1; 

	olc::vf2d selectedTile = { 0,0 };
	int currentMap = 0;
	public:
		float width = 1920.0f * 0.4f;
		float height = 1080.0f * 0.4f;
	bool OnUserCreate()override
	{
		loadDecals();
		loadPlayers();
		constructMaps();

		return true;
	}
	void loadDecals()
	{
		gridTile = new olc::Decal(new olc::Sprite("./Assets/Tile.png"));
		backgrounds.push_back(new olc::Decal(new olc::Sprite("./Assets/1.png")));

		iconMask = Gdiplus::Bitmap::FromFile(olc::ConvertS2W("./Assets/Mask.png").c_str());
		eraserMask = Gdiplus::Bitmap::FromFile(olc::ConvertS2W("./Assets/Eraser.png").c_str());

		cursors.push_back(new olc::Decal(new olc::Sprite("./Assets/Cursor_Hover.png")));
		cursors.push_back(new olc::Decal(new olc::Sprite("./Assets/Cursor_Grab.png")));
		cursors.push_back(new olc::Decal(new olc::Sprite("./Assets/Cursor_Grabbable.png")));

		cursors.push_back(new olc::Decal(new olc::Sprite("./Assets/Cursor_Draw.png")));
		cursors.push_back(new olc::Decal(new olc::Sprite("./Assets/Cursor_Eraser.png")));
		
		olc::Sprite* sprite = new olc::Sprite("./Assets/Test.png");
		MaskSprite(sprite);
		icons.push_back(new olc::Decal(sprite));
	}
	void loadPlayers()
	{
		NPCs.push_back(token(icons[0], { 0,0 }, olc::WHITE, "Isk"));
		NPCs.push_back(token(icons[0], { 1,0 }, olc::WHITE, "Cinder"));
		NPCs.push_back(token(icons[0], { 2,0 }, olc::WHITE, "Tarrehin"));
		NPCs.push_back(token(icons[0], { 3,0 }, olc::WHITE, "Bob"));
		NPCs.push_back(token(icons[0], { 4,0 }, olc::WHITE, "Nym"));
	}
	void MaskSprite(olc::Sprite* sprite)
	{
		for (int i = 0; i < sprite->width; i++)
		{
			for (int j = 0; j < sprite->height; j++)
			{
				Gdiplus::Color c;
				iconMask->GetPixel(i, j, &c);
				olc::Pixel pixel = olc::Pixel(
					sprite->GetPixel({ i,j }).r,
					sprite->GetPixel({ i,j }).g,
					sprite->GetPixel({ i,j }).b,
					c.GetAlpha());
				sprite->SetPixel(i, j, pixel);
			}
		}
	}
	void constructMaps()
	{
		fogOfWar = new olc::Sprite(width, height);
		for (int x = 0; x < width; x++)
		{
			for (int y = 0; y < height; y++)
			{
				fogOfWar->SetPixel({ x,y }, olc::BLANK);
			}
		}
		maps =
		{
			map("MapName", backgrounds[0], std::vector<map::link> { map::link("Hello", {5,5}) }, 1)
		};
	}
	bool OnUserUpdate(float fElepsedTime)override
	{
		DrawScene();
		if (GetMouse(0).bPressed)
		{
			switch (interactionMode)
			{
				case InteractionMode::MOVE:
				{
					PickUpToken(); break;
				}
			}
		}
		if (GetMouse(0).bHeld)
		{
			switch (interactionMode)
			{
				case InteractionMode::ERASE:
				{
					Erase(); break;
				}
				case InteractionMode::DRAW:
				{
					DrawFog(); break;
				}
			}
		}
		if (GetKey(olc::Q).bPressed)
		{
			gridColor = gridColor == olc::WHITE ? olc::BLACK : olc::WHITE;
		}
		if (GetKey(olc::CTRL).bHeld)
		{
			if (GetKey(olc::Q).bPressed)
			{
				interactionMode = InteractionMode::MOVE;
			}
			if (GetKey(olc::W).bPressed)
			{
				interactionMode = InteractionMode::MEASURE;
			}
			if (GetKey(olc::E).bPressed)
			{
				interactionMode = InteractionMode::ERASE;
			}
			if (GetKey(olc::R).bPressed)
			{
				interactionMode = InteractionMode::DRAW;
			}
			if (GetKey(olc::M).bPressed)
			{
				commonDivisorIndex += 1 * (commonDivisorIndex < 9);
			}
			if (GetKey(olc::N).bPressed)
			{
				commonDivisorIndex-= 1 * (commonDivisorIndex > 1);
			}
			if (GetMouse(0).bPressed && interactionMode == InteractionMode::DRAW)
			{
				for (int x = 0; x < width; x++)
				{
					for (int y = 0; y < height; y++)
					{
						fogOfWar->SetPixel({ x,y }, olc::BLACK);
					}
				}
			}
		}
		return true;
	}
	void Erase()
	{
		for (int i = GetMouseX(); i < GetMouseX()+eraserMask->GetWidth(); i++)
		{
			for (int j = GetMouseY(); j < GetMouseY()+eraserMask->GetHeight(); j++)
			{
				Gdiplus::Color c;
				eraserMask->GetPixel(i - GetMouseX(), j - GetMouseY(), &c);
				olc::Pixel pixel = olc::Pixel(
					0,
					0,
					0,
					c.GetA() & fogOfWar->GetPixel(i,j).a);
				fogOfWar->SetPixel(i, j, pixel);
			}
		}
	}
	void DrawFog()
	{
		for (int i = GetMouseX(); i < GetMouseX() + eraserMask->GetWidth(); i++)
		{
			for (int j = GetMouseY(); j < GetMouseY() + eraserMask->GetHeight(); j++)
			{
				Gdiplus::Color c;
				eraserMask->GetPixel(i - GetMouseX(), j - GetMouseY(), &c);
				olc::Pixel pixel = olc::Pixel(
					0,
					0,
					0,
					~c.GetA() | fogOfWar->GetPixel(i, j).a);
				fogOfWar->SetPixel(i, j, pixel);
			}
		}
	}
	void PickUpToken()
	{
		float modifier = gnu::findCommonDivisors(width, height)[commonDivisorIndex];
		float divisor = 256;
		olc::vf2d MousePositionInXY = { std::floor((GetMouseX() * divisor) / (modifier * gridTile->sprite->width)) , std::floor((GetMouseY() * divisor) / (modifier * gridTile->sprite->height)) };
		for (int i = 0; i < NPCs.size(); i++)
		{
			if (NPCs[i].position.x == MousePositionInXY.x && NPCs[i].position.y == MousePositionInXY.y)
			{
				//if there is something here
				if (heldToken == nullptr)
				{
					heldToken = &NPCs[i];
					NPCs[i].position = { -1,-1 }; //Instead of making the icon invisible, I move it offscreen
				}
				else
				{
					token temp = NPCs[i]; //hold the token in the slot
					temp.position = { -1, -1 };
					NPCs[i] = *heldToken; //put held token in slot
					*heldToken = temp; 
					NPCs[i].position = { MousePositionInXY.x,MousePositionInXY.y }; //Instead of making the icon invisible, I move it offscreen
				}
				return;
			}
		}
		//nothing is on this tile
		if (heldToken != nullptr)
		{
		    heldToken->position = { MousePositionInXY.x,MousePositionInXY.y };
			heldToken = nullptr;
		}
	}
	void DrawScene()
	{
		Clear(olc::BLANK);
		float scale = (width / maps[currentMap].background->sprite->width);
		DrawDecal({ 0,0 }, maps[currentMap].background, { scale,scale });
		float modifier = gnu::findCommonDivisors(width, height)[commonDivisorIndex];
		float divisor = 256;
		olc::vf2d MousePositionInXY = { std::floor((GetMouseX() * divisor) / (modifier * gridTile->sprite->width)) , std::floor((GetMouseY() * divisor) / (modifier * gridTile->sprite->height)) };
		if (interactionMode == InteractionMode::MOVE || interactionMode == InteractionMode::MEASURE)
		{
			DrawGrid(divisor, modifier, MousePositionInXY);
		}
		DrawTokens(divisor, modifier, MousePositionInXY);
		DrawHUD(divisor, modifier, MousePositionInXY);
		DisplayFog();
		DrawCursor(divisor, modifier, MousePositionInXY);
	}
	void DrawGrid(float divisor, float modifier, olc::vf2d mousePosition)
	{
		for (int i = 0; i < (width / modifier); i++)
		{
			for (int j = 0; j < (height / modifier); j++)
			{
				bool temp = ((mousePosition.x == i) && (mousePosition.y == j));
				olc::Pixel tint = temp ? olc::RED : gridColor;
				DrawDecal({
					(float)i * gridTile->sprite->width * modifier / divisor,
					(float)j * gridTile->sprite->height * modifier / divisor },
					gridTile, { modifier / divisor, modifier / divisor }, tint);
			}
		}
	}
	void DrawTokens(float divisor, float modifier, olc::vf2d mousePosition)
	{
		for (unsigned int i = 0; i < NPCs.size(); i++)
		{
			float scale = (float)gridTile->sprite->width/ (float)NPCs[i].icon->sprite->width;
			float scaleShrinker = 0.75f; //to make sure the token isnt the same size as the tile, its a bit smaller
			scale *= scaleShrinker;
			float centeringModifier = ((gridTile->sprite->width * modifier / divisor) - ((gridTile->sprite->width * modifier / divisor)*scaleShrinker))/2;
			olc::vf2d position = { NPCs[i].position.x * gridTile->sprite->width * modifier / divisor + centeringModifier, NPCs[i].position.y * gridTile->sprite->height * modifier / divisor + centeringModifier};
			DrawDecal(position, NPCs[i].icon, {scale * modifier / divisor, scale *modifier / divisor}, NPCs[i].tint);
			if (mousePosition.x == NPCs[i].position.x && mousePosition.y == NPCs[i].position.y)
			{
				//if this NPC is highlighted
				DrawStringDecal({ position.x, position.y + centeringModifier * 6 }, NPCs[i].name, olc::WHITE);
			}
		}
	}
	void DrawHUD(float divisor, float modifier, olc::vf2d mousePosition)
	{
		for (unsigned int i = 0; i < maps[currentMap].links.size(); i++)
		{
			bool selection = ((int)((GetMouseX() * divisor) / (modifier * gridTile->sprite->width)) == maps[currentMap].links[i].position.x) && ((int)((GetMouseY() * divisor) / (modifier * gridTile->sprite->height)) == maps[currentMap].links[i].position.y);
			olc::Pixel tint = selection ? olc::RED : olc::WHITE;
			olc::vf2d position = { maps[currentMap].links[i].position.x * gridTile->sprite->width * modifier / divisor, maps[currentMap].links[i].position.y * gridTile->sprite->height * modifier / divisor + gridTile->sprite->height * modifier / divisor * 0.8f };
			DrawStringDecal(position, maps[currentMap].links[i].mapIdentifier, tint);
		}
		//DrawStringDecal({ 60,10 }, "X: " + std::to_string((int)mousePosition.x) + " Y: " +  std::to_string((int)mousePosition.y));
	}
	void DrawCursor(float divisor, float modifier, olc::vf2d mousePosition)
	{
		bool grabbable = false;
		olc::vf2d scale = { 0.5f,0.5f };
		for (int i = 0; i < NPCs.size(); i++)
		{
			if (NPCs[i].position.x == mousePosition.x && NPCs[i].position.y == mousePosition.y)
			{
				grabbable = true;
			}
		}
		switch (interactionMode)
		{
			case InteractionMode::MOVE:
			{
				if (grabbable && heldToken == nullptr)
				{
					DrawDecal({ (float)GetMouseX(), (float)GetMouseY() }, cursors[2], scale);
				}
				else if (heldToken != nullptr)
				{
					float tokenScale = (float)gridTile->sprite->width / (float)heldToken->icon->sprite->width;
					float scaleShrinker = 0.75f; //to make sure the token isnt the same size as the tile, its a bit smaller
					tokenScale *= scaleShrinker;
					DrawDecal({ (float)GetMouseX() - 10, (float)GetMouseY() -10}, heldToken->icon, { tokenScale * modifier / divisor, tokenScale * modifier / divisor }, heldToken->tint);
					DrawDecal({ (float)GetMouseX(), (float)GetMouseY() }, cursors[1], scale);
				}
				else
				{
					DrawDecal({ (float)GetMouseX(), (float)GetMouseY() }, cursors[0], scale);
				}
				break;
			}
			case InteractionMode::ERASE:
			{
				DrawDecal({ (float)GetMouseX(), (float)GetMouseY() }, cursors[4], scale);
				break;
			}
			case InteractionMode::DRAW:
			{
				DrawDecal({ (float)GetMouseX(), (float)GetMouseY() }, cursors[3], scale);
				break;
			}
		}
	}
	void DisplayFog()
	{
		olc::Decal* temp = new olc::Decal(fogOfWar);
		DrawDecal({ 0,0 }, temp);
		//draw a fog of war
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

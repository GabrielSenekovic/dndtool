#define OLC_PGE_APPLICATION
#include <iostream>
#include "olcPixelGameEngine.h"
#include "GabNubUtilities.h"

class DnDTool : public olc::PixelGameEngine
{
	enum DrawMode
	{
		DRAWMODE_DRAW,DRAWMODE_ERASE,DRAWMODE_FILL
	};
	enum InteractionMode 
	{
		MOVE, DRAW, MEASURE
	};
	InteractionMode interactionMode = InteractionMode::MOVE;

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
	olc::Sprite* fogOfWarSprite = nullptr;
	olc::Decal* fogOfWarDecal = nullptr;
	std::vector<olc::Decal*> icons;

	std::vector<olc::Decal*> cursors;
	std::vector<token> NPCs;
	token* heldToken = nullptr;

	olc::Decal* gridTile = nullptr;
	int commonDivisorIndex = 8;
	olc::Pixel gridColor = olc::WHITE;

	Gdiplus::Bitmap* iconMask = nullptr;
	Gdiplus::Bitmap* eraserMask = nullptr;

	olc::vf2d zoom = { 1,1 };

	olc::Decal* UIBorder = nullptr;
	olc::vf2d UIoffset = { 0,0 }; //how offset the entire image is due to the UI

	olc::vf2d selectedTile = { 0,0 };
	int currentMap = 0;
	public:
		float width = 1920.0f * 0.3f; //4
		float height = 1080.0f * 0.3f;
	bool OnUserCreate()override
	{
		LoadDecals();
		LoadPlayers();
		ConstructMaps();

		return true;
	}
	void LoadDecals()
	{
		gridTile = new olc::Decal(new olc::Sprite("./Assets/Tile.png"));
		backgrounds.push_back(new olc::Decal(new olc::Sprite("./Assets/1.png")));
		UIBorder = new olc::Decal(new olc::Sprite("./Assets/UI_Border.png"));

		iconMask = Gdiplus::Bitmap::FromFile(olc::ConvertS2W("./Assets/Mask.png").c_str());
		eraserMask = Gdiplus::Bitmap::FromFile(olc::ConvertS2W("./Assets/Eraser.png").c_str());

		std::string cursor_paths[5] = { "./Assets/Cursor_Hover.png", "./Assets/Cursor_Grab.png", "./Assets/Cursor_Grabbable.png", "./Assets/Cursor_Draw.png" , "./Assets/Cursor_Eraser.png" };
		for (int i = 0; i < 5; i++)
		{
			cursors.push_back(new olc::Decal(new olc::Sprite(cursor_paths[i])));
		}
		
		olc::Sprite* sprite = new olc::Sprite("./Assets/Test.png");
		MaskSprite(sprite);
		icons.push_back(new olc::Decal(sprite));
	}
	void LoadPlayers()
	{
		NPCs.push_back(token(icons[0], { 0,0 }, olc::WHITE, "Isk"));
		NPCs.push_back(token(icons[0], { 1,0 }, olc::WHITE, "Cinder"));
		NPCs.push_back(token(icons[0], { 2,0 }, olc::WHITE, "Tarrehin"));
		NPCs.push_back(token(icons[0], { 3,0 }, olc::WHITE, "Bob"));
		NPCs.push_back(token(icons[0], { 4,0 }, olc::WHITE, "Nym"));
	}
	void MaskSprite(olc::Sprite* sprite)
	{
		for (size_t i = 0; i < sprite->width * sprite->height; i++)
		{
			Gdiplus::Color c; int x = (int)i % sprite->width; int y = (int)i / sprite->width;
			iconMask->GetPixel(x, y, &c);
			olc::Pixel pixel = olc::Pixel(
				sprite->GetPixel({ x, y }).r,
				sprite->GetPixel({ x, y }).g,
				sprite->GetPixel({ x, y }).b,
				c.GetAlpha());
			sprite->SetPixel(x, y, pixel);
		}
	}
	void ConstructMaps()
	{
		fogOfWarSprite = new olc::Sprite(width, height);
		FillFog(olc::BLANK);
		maps =
		{
			map("MapName", backgrounds[0], std::vector<map::link> { map::link("Hello", {5,5}) }, 1)
		};
	}
	void FillFog(olc::Pixel color)
	{
		for (size_t i = 0; i < width * height; i++)
		{
			fogOfWarSprite->SetPixel({ (int)i % (int)width, (int)i / (int)width }, color);
		}
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
			if (interactionMode == InteractionMode::DRAW)
			{
				EraseDraw(DrawMode::DRAWMODE_DRAW);
			}
		}
		if (GetMouse(1).bHeld)
		{
			if (interactionMode == InteractionMode::DRAW)
			{
				EraseDraw(DrawMode::DRAWMODE_ERASE);
			}
		}
		if (GetKey(olc::Q).bPressed)
		{
			UIoffset = { 32.0f * (UIoffset.x == 0) * width / UIBorder->sprite->width, 16.0f * (UIoffset.y == 0) * height / UIBorder->sprite->height };
		}
		if (GetKey(olc::W).bPressed)
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
				FillFog(olc::BLACK);
			}
		}
		if (GetKey(olc::M).bHeld)
		{
			zoom.x += 0.001f;
			zoom.y += 0.001f;
		}
		if (GetKey(olc::N).bHeld && zoom.x > 1)
		{
			zoom.x -= 0.001f;
			zoom.y -= 0.001f;
		}
		return true;
	}
	void EraseDraw(DrawMode mode)
	{
		for (int i = GetMouseX(); i < GetMouseX() + eraserMask->GetWidth(); i++)
		{
			for (int j = GetMouseY(); j < GetMouseY() + eraserMask->GetHeight(); j++)
			{
				Gdiplus::Color c;
				eraserMask->GetPixel(i - GetMouseX(), j - GetMouseY(), &c);
				olc::Pixel pixel;
				switch (mode)
				{
					case DrawMode::DRAWMODE_ERASE:
					{
						pixel = olc::Pixel(
							0,
							0,
							0,
							c.GetA() & fogOfWarSprite->GetPixel(i, j).a); break;
					}
					case DrawMode::DRAWMODE_DRAW:
					{
						pixel = olc::Pixel(
							0,
							0,
							0,
							~c.GetA() | fogOfWarSprite->GetPixel(i, j).a); break;
					}
				}
				fogOfWarSprite->SetPixel(i, j, pixel);
			}
		}
	}
	void PickUpToken()
	{
		float commonDivisor = gnu::findCommonDivisors(width, height)[commonDivisorIndex];
		float divisor = 256;
		olc::vf2d MousePositionInXY = { std::floor((GetMouseX() * divisor) / (commonDivisor * gridTile->sprite->width)) , std::floor((GetMouseY() * divisor) / (commonDivisor * gridTile->sprite->height)) };
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
					std::swap(NPCs[i], *heldToken);
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
		Clear(olc::BLACK);
		olc::vf2d scaleUIOffset = UIoffset.x == 0 ? olc::vf2d{ 0,0 } : olc::vf2d{ 48 * width / UIBorder->sprite->width, 32 * height / UIBorder->sprite->height };
		
		float scale = maps[currentMap].background->sprite->width > maps[currentMap].background->sprite->height ? (width/ (maps[currentMap].background->sprite->width)) : (height / maps[currentMap].background->sprite->height);
		float offsetScale = maps[currentMap].background->sprite->width > maps[currentMap].background->sprite->height ? ((width - scaleUIOffset.x) / (maps[currentMap].background->sprite->width)) : ((height - scaleUIOffset.y) / maps[currentMap].background->sprite->height);
		
		DrawDecal({ UIoffset.x,UIoffset.y }, maps[currentMap].background, { offsetScale* zoom.x,offsetScale* zoom.y});
		float scaledWidth = maps[currentMap].background->sprite->width * offsetScale * zoom.x;

		float commonDivisor = gnu::findCommonDivisors(maps[currentMap].background->sprite->width * scale, maps[currentMap].background->sprite->height * scale)[commonDivisorIndex]; float divisor = 256;
		olc::vf2d MousePositionInXY = { std::floor((GetMouseX() * divisor) / (commonDivisor * gridTile->sprite->width)), std::floor((GetMouseY() * divisor) / (commonDivisor * gridTile->sprite->height))};
		if (interactionMode == InteractionMode::MOVE || interactionMode == InteractionMode::MEASURE)
		{
			DrawGrid(divisor, commonDivisor, MousePositionInXY, scale, maps[currentMap].background->sprite->width * offsetScale);
		}
		DrawTokens(divisor, commonDivisor, MousePositionInXY);
		DrawLinks(divisor, commonDivisor, MousePositionInXY);
		DisplayFog();
		DrawCursor(divisor, commonDivisor, MousePositionInXY);
		if (UIoffset.x > 0)
		{
			DrawDecal({ 0,0 }, UIBorder, {width/UIBorder->sprite->width, height/UIBorder->sprite->height });
			DisplayHUD();
		}
	}
	void DrawGrid(float divisor, float commonDivisor, olc::vf2d mousePosition, float scale, float mapWidthScaledByUI)
	{
		float tileableSize = commonDivisor / divisor;
		for (size_t i = 0; i < (maps[currentMap].background->sprite->width * scale / commonDivisor) * (maps[currentMap].background->sprite->height * scale / commonDivisor); i++)
		{
			float amountOfColumns = scale / commonDivisor;
			int gridWidth = (maps[currentMap].background->sprite->width * amountOfColumns);
			int x = i % gridWidth; 
			int y = i / gridWidth;
			bool isThisHighlighted = ((mousePosition.x == x && (mousePosition.y == y)));
			float adjustedTileWidth = (mapWidthScaledByUI / gridWidth)/tileableSize;
			float tileWidthRatio = gridTile->sprite->width / adjustedTileWidth;
			olc::Pixel tint = isThisHighlighted ? olc::RED : gridColor;
			DrawDecal({
				(float)x * gridTile->sprite->width * tileableSize * zoom.x / tileWidthRatio + UIoffset.x,
				(float)y * gridTile->sprite->height * tileableSize * zoom.y / tileWidthRatio + UIoffset.y},
				gridTile, { tileableSize * zoom.x / tileWidthRatio, tileableSize * zoom.y / tileWidthRatio}, tint);
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
			olc::vf2d position = {( NPCs[i].position.x * gridTile->sprite->width * modifier / divisor + centeringModifier)*zoom.x + UIoffset.x, (NPCs[i].position.y * gridTile->sprite->height * modifier / divisor + centeringModifier)*zoom.y + UIoffset.y};
			DrawDecal(position, NPCs[i].icon, {scale * modifier / divisor * zoom.x, scale *modifier / divisor * zoom.y}, NPCs[i].tint);
			if (mousePosition.x == NPCs[i].position.x && mousePosition.y == NPCs[i].position.y)
			{
				//if this NPC is highlighted
				DrawStringDecal({ position.x, position.y + centeringModifier * 6 }, NPCs[i].name, olc::WHITE, { zoom.x, zoom.y });
			}
		}
	}
	void DrawLinks(float divisor, float modifier, olc::vf2d mousePosition)
	{
		for (unsigned int i = 0; i < maps[currentMap].links.size(); i++)
		{
			bool selection = ((int)((GetMouseX() * divisor) / (modifier * gridTile->sprite->width)) == maps[currentMap].links[i].position.x) && ((int)((GetMouseY() * divisor) / (modifier * gridTile->sprite->height)) == maps[currentMap].links[i].position.y);
			olc::Pixel tint = selection ? olc::RED : olc::WHITE;
			olc::vf2d position = { maps[currentMap].links[i].position.x * gridTile->sprite->width * modifier / divisor, maps[currentMap].links[i].position.y * gridTile->sprite->height * modifier / divisor + gridTile->sprite->height * modifier / divisor * 0.8f };
			DrawStringDecal(position, maps[currentMap].links[i].mapIdentifier, tint);
		}
	}
	void DrawCursor(float divisor, float modifier, olc::vf2d mousePosition)
	{
		bool grabbable = false;
		olc::vf2d scale = { 0.5f,0.5f };
		olc::vf2d position = { GetMouseX() * zoom.x, GetMouseY() * zoom.y };
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
					DrawDecal(position, cursors[2], scale);
				}
				else if (heldToken != nullptr)
				{
					float tokenScale = (float)gridTile->sprite->width / (float)heldToken->icon->sprite->width;
					float scaleShrinker = 0.75f; //to make sure the token isnt the same size as the tile, its a bit smaller
					tokenScale *= scaleShrinker;
					DrawDecal({ (float)GetMouseX() - 10, (float)GetMouseY() -10}, heldToken->icon, { tokenScale * modifier / divisor, tokenScale * modifier / divisor }, heldToken->tint);
					DrawDecal(position, cursors[1], scale);
				}
				else
				{
					DrawDecal(position, cursors[0], scale);
				}
				break;
			}
			case InteractionMode::DRAW:
			{
				int index = GetMouse(1).bHeld ? 4 : 3;
				DrawDecal(position, cursors[index], scale);
				break;
			}
		}
	}
	void DisplayFog()
	{
		delete fogOfWarDecal; fogOfWarDecal = nullptr;
		fogOfWarDecal = new olc::Decal(fogOfWarSprite);
		DrawDecal({ 0,0 }, fogOfWarDecal);
	}
	void DisplayHUD()
	{

	}
	void Quit()
	{
		for (int i = 0; i < icons.size(); i++)
		{
			delete icons[i];
		}
		for (int i = 0; i < backgrounds.size(); i++)
		{
			delete backgrounds[i];
		}
		for (int i = 0; i < cursors.size(); i++)
		{
			delete cursors[i];
		}
		delete gridTile;
		delete fogOfWarSprite;
	}
};

int main()
{
	DnDTool dndtool;
	if (dndtool.Construct(dndtool.width, dndtool.height, 2, 2))
	{
		dndtool.Start();
	}
	dndtool.Quit();
	return 0;
}
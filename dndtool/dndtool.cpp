#define OLC_PGE_APPLICATION
#include <iostream>
#include "GabNubUtilities.h"
#include "olcPixelGameEngine.h"

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
		olc::Decal* background = nullptr;
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
		token()
		{

		}
		token(olc::Decal* icon_in, olc::vf2d position_in, olc::Pixel tint_in, std::string name_in)
		{
			icon = icon_in;
			position = position_in;
			tint = tint_in;
			name = name_in;
		}
		olc::Decal* icon = nullptr;
		olc::Pixel tint;
		olc::vf2d position;
		std::string name;
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Here follows variables related to different interaction modes:
	//MOVE
	int selectedTile = -1; //if you right click on a tile, it becomes selected. This hold the index
	//Also use this for measuring as the start point
	token* heldToken = nullptr;
	olc::vi2d previousTokenPosition = { 0,0 }; //in order to check if the heldtoken was the one that was selected
	olc::Decal* selection = nullptr; //Decal that will show up behind selected tokens
	float selectionAngle = 0; float selectionRotationSpeed = 0.01f; //Use this to rotate selection image
	token selectedToken;
	//DRAW
	olc::Sprite* fogOfWarSprite = nullptr;
	olc::Decal* fogOfWarDecal = nullptr; //Perhaps I only need to draw the sprite if I draw it on a higher layer
	Gdiplus::Bitmap* eraserMask = nullptr;
	//MEASURE
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::vector<map> maps;
	std::vector<olc::Decal*> backgrounds;
	std::vector<olc::Decal*> icons;
	std::vector<olc::Decal*> cursors;
	std::vector<token> NPCs;

	olc::Decal* gridTile = nullptr;
	int tileDivisor = 256;
	int commonDivisorIndex = 8;
	olc::Pixel gridColor = olc::WHITE;
	float commonDivisor = 0;

	Gdiplus::Bitmap* iconMask = nullptr;

	olc::vf2d zoom = { 1,1 };
	olc::vf2d panOffset = { 0,0 }; //When zoomed in, used to drag the camera around, ie. pan around

	olc::Decal* UIBorder = nullptr;
	olc::vf2d UIoffset = { 0,0 }; //how offset the entire image is due to the UI
	float scaleUnaffectedByUI = 0;
	float scaleAffectedByUI = 0;

	int currentMap = 0;

	public:
		DnDTool(){}
		float width = 1920.0f * 0.3f; //4
		float height = 1080.0f * 0.3f;

	bool OnUserCreate()override
	{
		LoadDecals();
		LoadPlayers();
		ConstructMaps();
		ToggleUI();
		return true;
	}
	void LoadDecals()
	{
		gridTile = new olc::Decal(new olc::Sprite("./Assets/Tile.png"));
		selection = new olc::Decal(new olc::Sprite("./Assets/Selection.png"));
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
		scaleUnaffectedByUI = maps[currentMap].background->sprite->width > maps[currentMap].background->sprite->height ? (width / (maps[currentMap].background->sprite->width)) : (height / maps[currentMap].background->sprite->height);
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
		if (selectedTile > -1)
		{
			selectionAngle += selectionRotationSpeed;
		}
		DrawScene();
		CheckInput();
		return true;
	}
	void CheckInput()
	{
		if (GetMouse(0).bPressed)
		{
			switch (interactionMode)
			{
				case InteractionMode::MOVE:
				{
					PickUpToken(); break;
				}
				case InteractionMode::MEASURE:
				{
					//deselect by right clicking
					int gridWidth = (maps[currentMap].background->sprite->width * (scaleUnaffectedByUI / commonDivisor));
					selectedTile = GetMousePositionInXY().y * gridWidth + GetMousePositionInXY().x;
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
		if (GetMouse(1).bPressed)
		{
			switch (interactionMode)
			{
				case InteractionMode::MOVE:
				{
					//Should only happen if youre pressing a token, otherwise it will deselect
					int gridWidth = (maps[currentMap].background->sprite->width * (scaleUnaffectedByUI / commonDivisor));
					selectionAngle = selectedTile == GetMousePositionInXY().y * gridWidth + GetMousePositionInXY().x ? selectionAngle : 0;
					selectedTile = GetMousePositionInXY().y * gridWidth + GetMousePositionInXY().x;
					for (int i = 0; i < NPCs.size(); i++)
					{
						if (NPCs[i].position.y * gridWidth + NPCs[i].position.x == selectedTile)
						{
							selectedToken = NPCs[i];
						}
					}
					break;
				}
				case InteractionMode::MEASURE:
				{
					//Deselect measurement if you rightclick
					selectedTile = -1;
					break;
				}
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
			ToggleUI();
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
				commonDivisorIndex -= 1 * (commonDivisorIndex > 1);
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
	}
	void ToggleUI()
	{
		UIoffset = { 70.0f * (UIoffset.x == 0) * width / UIBorder->sprite->width, 16.0f * (UIoffset.y == 0) * height / UIBorder->sprite->height };
	}
	void EraseDraw(DrawMode mode)
	{
		for (size_t i = GetMouseX(); i < GetMouseX() + eraserMask->GetWidth(); i++)
		{
			for (size_t j = GetMouseY(); j < GetMouseY() + eraserMask->GetHeight(); j++)
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
	olc::vf2d GetScaleUIOffset()
	{
		return UIoffset.x == 0 ? olc::vf2d{ 0,0 } : olc::vf2d{ 48 * width / UIBorder->sprite->width, 32 * height / UIBorder->sprite->height };
	}
	olc::vi2d GetMousePositionInXY()
	{
		return olc::vi2d({(int) GetMousePositionInXYFloat().x, (int)GetMousePositionInXYFloat().y });
	}
	olc::vf2d GetMousePositionInXYFloat()
	{
		olc::vf2d scaleUIOffset = GetScaleUIOffset();
		float commonDivisor = gnu::findCommonDivisors(maps[currentMap].background->sprite->width * scaleUnaffectedByUI, maps[currentMap].background->sprite->height * scaleUnaffectedByUI)[commonDivisorIndex];
		float mapWidthScaledByUI = maps[currentMap].background->sprite->width * scaleAffectedByUI;
		float tileableSize = commonDivisor / tileDivisor;
		float amountOfColumns = scaleUnaffectedByUI / commonDivisor;
		int gridWidth = (maps[currentMap].background->sprite->width * amountOfColumns);
		float adjustedTileWidth = (mapWidthScaledByUI / gridWidth) / tileableSize;

		olc::vf2d downScaledTileDimensions = { commonDivisor * adjustedTileWidth, commonDivisor * adjustedTileWidth };

		return olc::vf2d({(float)(GetMouseX() - UIoffset.x) * (float)tileDivisor / downScaledTileDimensions.x, (float)(GetMouseY() - UIoffset.y) * (float)tileDivisor / downScaledTileDimensions.y });
	}
	void PickUpToken()
	{
		olc::vf2d MousePositionInXY = GetMousePositionInXY();
		for (int i = 0; i < NPCs.size(); i++)
		{
			if (NPCs[i].position.x == MousePositionInXY.x && NPCs[i].position.y == MousePositionInXY.y)
			{
				//if there is something here
				if (heldToken == nullptr)
				{
					heldToken = &NPCs[i];
					previousTokenPosition = NPCs[i].position;
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
			int gridWidth = (maps[currentMap].background->sprite->width * (scaleUnaffectedByUI / commonDivisor));
			if (selectedTile == previousTokenPosition.y * gridWidth + previousTokenPosition.x)
			{
				selectedTile = GetMousePositionInXY().y * gridWidth + GetMousePositionInXY().x;
			}
		    heldToken->position = { MousePositionInXY.x,MousePositionInXY.y };

			heldToken = nullptr;
		}
	}
	void DrawScene()
	{
		Clear(olc::BLACK);
		olc::vf2d scaleUIOffset = UIoffset.x == 0 ? olc::vf2d{ 0,0 } : olc::vf2d{ 48 * width / UIBorder->sprite->width, 32 * height / UIBorder->sprite->height };
		
		scaleAffectedByUI = maps[currentMap].background->sprite->width > maps[currentMap].background->sprite->height ? ((width - scaleUIOffset.x) / (maps[currentMap].background->sprite->width)) : ((height - scaleUIOffset.y) / maps[currentMap].background->sprite->height);
		
		DrawDecal({ UIoffset.x,UIoffset.y }, maps[currentMap].background, { scaleAffectedByUI* zoom.x,scaleAffectedByUI* zoom.y});
		float scaledWidth = maps[currentMap].background->sprite->width * scaleAffectedByUI * zoom.x;
		commonDivisor = gnu::findCommonDivisors(maps[currentMap].background->sprite->width * scaleUnaffectedByUI, maps[currentMap].background->sprite->height * scaleUnaffectedByUI)[commonDivisorIndex];
		
		float tileableSize = commonDivisor / tileDivisor;
		float amountOfColumns = scaleUnaffectedByUI / commonDivisor;
		int gridWidth = maps[currentMap].background->sprite->width * amountOfColumns;
		float adjustedTileWidth = ((maps[currentMap].background->sprite->width * scaleAffectedByUI) / gridWidth) / tileableSize;
		float tileWidthRatio = gridTile->sprite->width / adjustedTileWidth;
		if (interactionMode == InteractionMode::MOVE || interactionMode == InteractionMode::MEASURE)
		{
			DrawGrid(commonDivisor, scaleUnaffectedByUI, tileWidthRatio, tileableSize, gridWidth);
		}
		DrawTokens(tileWidthRatio);
		DrawLinks(commonDivisor);
		DisplayFog();
		if (UIoffset.x > 0)
		{
			DisplayHUD();
		}
		DrawCursor(commonDivisor);
	}

	void DrawGrid(float commonDivisor, float scale, float tileWidthRatio, float tileableSize, int gridWidth)
	{
		olc::vf2d tileScale = { tileableSize * zoom.x / tileWidthRatio, tileableSize * zoom.y / tileWidthRatio };

		for (size_t i = 0; i < (maps[currentMap].background->sprite->width * scale / commonDivisor) * (maps[currentMap].background->sprite->height * scale / commonDivisor); i++)
		{
			int x = i % gridWidth; 
			int y = i / gridWidth;
			bool isThisHighlighted = ((GetMousePositionInXY().x == x && (GetMousePositionInXY().y == y)));
			olc::Pixel tint = isThisHighlighted ? olc::RED : gridColor;
			olc::vf2d adjustedDimensions = { gridTile->sprite->width * tileableSize * zoom.x / tileWidthRatio , gridTile->sprite->height * tileableSize * zoom.y / tileWidthRatio };
			olc::vf2d position = {
				(float)x * adjustedDimensions.x + UIoffset.x,
				(float)y * adjustedDimensions.y + UIoffset.y };

			if (i == selectedTile)
			{
				float centeringModifier = (gridTile->sprite->width * tileableSize) - ((gridTile->sprite->width * tileableSize)) / 2;
				float iconToTileRatio = (float)gridTile->sprite->width / (float)NPCs[0].icon->sprite->width;
				olc::vf2d scale = { tileableSize * zoom.x * iconToTileRatio / tileWidthRatio, tileableSize * zoom.y * iconToTileRatio / tileWidthRatio };
				olc::vf2d centerOfRotation = { (float)selection->sprite->width/2 , (float)selection->sprite->height/2 };
				olc::vf2d selectionPosition = { (x * gridTile->sprite->width * tileableSize + centeringModifier) * zoom.x / tileWidthRatio + UIoffset.x, (y * gridTile->sprite->height * tileableSize + centeringModifier) * zoom.y / tileWidthRatio + UIoffset.y };

				DrawRotatedDecal(selectionPosition, selection, selectionAngle, centerOfRotation, scale, olc::RED);
			}
			DrawDecal(position,gridTile, tileScale, tint);
		}

		olc::vf2d cursorPosition = { GetMouseX() * zoom.x, GetMouseY() * zoom.y };
		DrawDecal(cursorPosition, gridTile, tileScale, olc::BLUE);
	}
	void DrawTokens(float tileWidthRatio)
	{
		for (unsigned int i = 0; i < NPCs.size(); i++)
		{
			float tileableSize = commonDivisor / tileDivisor;
			float iconToTileRatio = (float)gridTile->sprite->width/ (float)NPCs[i].icon->sprite->width;
			float scaleShrinker = 0.75f; //to make sure the token isnt the same size as the tile, its a bit smaller
			iconToTileRatio *= scaleShrinker;
			float centeringModifier = ((gridTile->sprite->width * tileableSize) - ((gridTile->sprite->width * tileableSize)*scaleShrinker))/2;
			
			olc::vf2d scale = { tileableSize * zoom.x * iconToTileRatio / tileWidthRatio, tileableSize * zoom.y * iconToTileRatio /tileWidthRatio};
			olc::vf2d position = {( NPCs[i].position.x * gridTile->sprite->width * tileableSize + centeringModifier)*zoom.x/tileWidthRatio + UIoffset.x, (NPCs[i].position.y * gridTile->sprite->height * tileableSize + centeringModifier)*zoom.y/tileWidthRatio + UIoffset.y};
			
			DrawDecal(position, NPCs[i].icon, scale, NPCs[i].tint);
			float gridWidth = (maps[currentMap].background->sprite->width * (scaleUnaffectedByUI / commonDivisor));
			bool isSelected = selectedTile == (int)(NPCs[i].position.y * gridWidth + NPCs[i].position.x);

			if (GetMousePositionInXY().x == NPCs[i].position.x && GetMousePositionInXY().y == NPCs[i].position.y || isSelected)
			{
				//if this NPC is highlighted
				float lengthOfName =NPCs[i].name.length() * 8;
				olc::Pixel tint = isSelected ? olc::RED : olc::WHITE;
				float yPositionModifier = isSelected ? 24 : 20;
				float lengthToSubtract = ((lengthOfName - gridTile->sprite->width)/4) + gridTile->sprite->width*tileableSize*2;
				DrawStringDecal({ position.x - lengthToSubtract * zoom.x, position.y + yPositionModifier * zoom.y / tileWidthRatio}, NPCs[i].name, tint, {0.5f * zoom.x, 0.5f * zoom.y });
			}
		}
	}
	void DrawLinks(float modifier)
	{
		for (unsigned int i = 0; i < maps[currentMap].links.size(); i++)
		{
			bool selection = ((int)((GetMouseX() * tileDivisor) / (modifier * gridTile->sprite->width)) == maps[currentMap].links[i].position.x) && ((int)((GetMouseY() * tileDivisor) / (modifier * gridTile->sprite->height)) == maps[currentMap].links[i].position.y);
			olc::Pixel tint = selection ? olc::RED : olc::WHITE;
			olc::vf2d position = { maps[currentMap].links[i].position.x * gridTile->sprite->width * modifier / tileDivisor, maps[currentMap].links[i].position.y * gridTile->sprite->height * modifier / tileDivisor + gridTile->sprite->height * modifier / tileDivisor * 0.8f };
			
			DrawStringDecal(position, maps[currentMap].links[i].mapIdentifier, tint, { 1, 1 });
		}
	}
	void DrawCursor(float modifier)
	{
		bool grabbable = false;
		olc::vf2d scale = { 0.5f,0.5f };
		olc::vf2d position = { (GetMouseX()-10) * zoom.x, (GetMouseY()-10) * zoom.y };
		for (size_t i = 0; i < NPCs.size(); i++)
		{
			if (NPCs[i].position.x == GetMousePositionInXY().x && NPCs[i].position.y == GetMousePositionInXY().y)
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
					DrawDecal({ (float)GetMouseX() - 10, (float)GetMouseY() -10}, heldToken->icon, { tokenScale * modifier / tileDivisor, tokenScale * modifier / tileDivisor }, heldToken->tint);
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
		DrawDecal({ 0,0 }, UIBorder, { width / UIBorder->sprite->width, height / UIBorder->sprite->height });
		if (selectedToken.icon != nullptr)
		{
			float ratio = (62.0f/selectedToken.icon->sprite->width) * width/UIBorder->sprite->width;
			DrawDecal({3,12 }, selectedToken.icon, {ratio,ratio});
			DrawStringDecal({ 3, 64 }, selectedToken.name,olc::WHITE, {0.5f, 0.5f});
		}
		DrawStringDecal({ 2,2 }, 
			"[Grid Coord X: " + std::to_string(GetMousePositionInXY().x) + " Y: " + std::to_string(GetMousePositionInXY().y) + "]"+
			"[F_Grid Coord X: " + std::to_string(GetMousePositionInXYFloat().x) + " Y: " + std::to_string(GetMousePositionInXYFloat().y) + "]" +
			" [Selected tile Index: " + std::to_string(selectedTile) +
			" [Selected tile X: " + std::to_string(selectedTile%(int)(maps[currentMap].background->sprite->width * (scaleUnaffectedByUI / commonDivisor))) + " Y: " + std::to_string((int)(selectedTile / (maps[currentMap].background->sprite->width * (scaleUnaffectedByUI / commonDivisor))))
			,olc::WHITE, { 0.5f, 0.5f });
		DrawStringDecal({ 2,7 }, "[Mouse X: " + std::to_string(GetMouseX()) + " Y: " + std::to_string(GetMouseY()) + "]" +
			" [Zoom:" + std::to_string(zoom.x) + "]"
			, olc::WHITE, { 0.5f, 0.5f });
	}
	void Quit()
	{
		for (size_t i = 0; i < icons.size(); i++)
		{
			delete icons[i];
		}
		for (size_t i = 0; i < backgrounds.size(); i++)
		{
			delete backgrounds[i];
		}
		for (size_t i = 0; i < cursors.size(); i++)
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
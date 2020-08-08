#pragma once
#define OLC_PGE_APPLICATION
#include <iostream>
#include "GabNubUtilities.h"
#include "olcPixelGameEngine.h"
class DnDTool : public olc::PixelGameEngine
{
	enum DrawMode
	{
		DRAWMODE_DRAW, DRAWMODE_ERASE, DRAWMODE_FILL
	};
	enum InteractionMode
	{
		MOVE, //Pick up and place tokens. Kill or down characters
		DRAW, //Edit the fog of war
		MEASURE, //Measure distances between tiles
		BUILD //Place out and delete tokens and tiles before battle. Tokens are selectable from a list on the side
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
		map(std::string mapIdentifier_in, olc::Decal* background_in, std::vector<link> links_in, int tileScale_in)
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
		token(std::pair<olc::Decal*, olc::Decal*> icon_in, olc::vf2d position_in, olc::Pixel tint_in, std::string name_in)
		{
			icon = icon_in.second;
			icon_unmasked = icon_in.first;
			position = position_in;
			tint = tint_in;
			name = name_in;
		}
		olc::Decal* icon = nullptr;
		olc::Decal* icon_unmasked = nullptr;
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
	olc::Decal* measuringLine = nullptr;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::vector<map> maps;
	std::vector<olc::Decal*> backgrounds;
	std::vector<std::pair<olc::Decal*, olc::Decal*>> icons;
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
	olc::Decal* UIButton = nullptr;
	olc::vf2d UIoffset = { 0,0 }; //how offset the entire image is due to the UI
	float scaleUnaffectedByUI = 0;
	float scaleAffectedByUI = 0;

	int currentMap = 0;

public:
	DnDTool() {}
	float width = 1920.0f * 0.3f; //4
	float height = 1080.0f * 0.3f;

	bool OnUserCreate()override;
	void LoadDecals();
	void LoadPlayers();
	void ConstructMaps();

	void MaskSprite(olc::Sprite* sprite);

	bool OnUserUpdate(float fElepsedTime)override;

	//input.cpp
	void CheckInput();
	void EraseDraw(DrawMode mode);
	void ToggleUI();
	void PickUpToken();
	void FillFog(olc::Pixel color);

	//rendering.cpp
	void DrawScene();
	void DrawGrid(float commonDivisor, float scale, float tileWidthRatio, float tileableSize, int gridWidth);
	void DrawTokens(float tileWidthRatio);
	void DrawLinks(float modifier);
	void DrawCursor(float modifier);
	void DisplayFog();
	void DisplayHUD();

	//middlehand.cpp
	olc::vf2d GetScaleUIOffset();
	olc::vi2d GetMousePositionInXY();
	olc::vf2d GetMousePositionInXYFloat();

	void Quit();
};
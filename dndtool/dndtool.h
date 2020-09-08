#ifndef DNDTOOL_H
#define DNDTOOL_H

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include "olcPixelGameEngine.h"
#include <iostream>
#include "GabNubUtilities.h"

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

	struct button
	{
		olc::Decal* icon;
		olc::vf2d position = { 0,0 };
		button(olc::Decal* icon_in, olc::vf2d position_in);
		bool OnPress();
		float Width(); float Height();
	};
	struct UI
	{
		void Render(DnDTool dndTool);
		void RenderButtons(DnDTool dndTool, olc::vf2d scale);
		void RenderText(DnDTool dndTool);
	};
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
		int commonDivisorIndex = 0;

		map(std::string mapIdentifier_in, olc::Decal* background_in, int tileScale_in);
		map(std::string mapIdentifier_in, olc::Decal* background_in, int tileScale_in, int commonDivisor_in);
		map(std::string mapIdentifier_in, olc::Decal* background_in, std::vector<link> links_in, int tileScale_in);
		map(std::string mapIdentifier_in, olc::Decal* background_in, std::vector<link> links_in, int tileScale_in, int commonDivisor_in);
		float Width();
		float Height();
	};
	struct token
	{
		olc::Decal* icon = nullptr;
		olc::Decal* icon_unmasked = nullptr;
		olc::Pixel tint;
		olc::vf2d position;
		std::string name;

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
		void Render(DnDTool* dndTool, float tileWidthRatio, float tileableSize, float gridWidth, olc::vf2d scale, float iconToTileRatio);
		void RenderText(DnDTool* dndTool, float tileWidthRatio, float tileableSize, bool isSelected, olc::vf2d renderPosition);
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
	float iconSizeAdjustment = 0.75f; //Makes sure the icon is smaller than the tile it is on

	olc::Decal* gridTile = nullptr;
	int tileDivisor = 256;
	int commonDivisorIndex = 8; //arbitrary number. This must be changed
	olc::Pixel gridColor = olc::WHITE;
	float commonDivisor = 0;

	Gdiplus::Bitmap* iconMask = nullptr;

	olc::vf2d zoom = { 1,1 };
	olc::vf2d panOffset = { 0,0 }; //When zoomed in, used to drag the camera around, ie. pan around

	olc::Decal* UIBorder = nullptr;
	std::vector<olc::Decal*> buttonIcons;
	olc::vf2d UIoffset = { 0,0 }; //how offset the entire image is due to the UI
	std::vector<button> modeButtons;
	olc::vf2d scaleUIOffset = { 0,0 };
	float scaleUnaffectedByUI = 0;
	float scaleAffectedByUI = 0;

	int currentMap = 0;

public:
	DnDTool() {}
	float width = 1920.0f * 0.3f; //4
	float height = 1080.0f * 0.3f;

	//loading.cpp
	bool OnUserCreate()override;
	void LoadDecals(); void OnLoadDecals(std::vector<olc::Decal*> &list, std::string path);
	void LoadPlayers();
	void ConstructMaps();
	void LoadMap();
	void LoadUI();

	void MaskSprite(olc::Sprite* sprite);

	bool OnUserUpdate(float fElepsedTime)override;

	//input.cpp
	void CheckInput();
	void EraseDraw(DrawMode mode);
	void ToggleUI();
	void PickUpToken();
	void FillFog(olc::Pixel color);

	//rendering.cpp
	void RenderAll(); //Renders everything
	void RenderMap(); //Renders grid, tokens and links
	void RenderGrid(float amountOfColumns, float tileWidthRatio, float tileableSize, int gridWidth, olc::vf2d tileScale, float iconToTileRatio);
	void RenderLinks(float modifier);
	void RenderCursor();
	void RenderFog();

	void RenderImage(olc::Decal* image, olc::vf2d position, olc::vf2d scale, olc::Pixel tint); //Renders one thing on the map
	void RenderImage(olc::Decal* image, olc::vf2d position, olc::vf2d scale, olc::Pixel tint, float angle, olc::vf2d center);
	void RenderText(std::string text, olc::vf2d position, olc::vf2d scale, olc::Pixel tint);

	//middlehand.cpp
	void SetScaleUIOffset();
	void SetScaleAffectedByUI();
	olc::vi2d GetMousePositionInXY();
	olc::vf2d GetMousePositionInXYFloat();

	void Quit();
};
#endif
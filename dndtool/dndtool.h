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
	enum Event
	{
		EVENT_NONE, 
		EVENT_INQUIRY //Will open the inquiry window
	};
	std::pair<Event, bool*> currentEvent = std::pair<Event, bool*>(Event::EVENT_NONE,nullptr);
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
		olc::Decal* icons[3] = {}; int currentIcon = 0;

		olc::vf2d position = { 0,0 };
		olc::vf2d scale = { 1,1 };

		bool active = true;

		std::function<void()> function;

		button(std::array<olc::Decal*, 3> icon_in, olc::vf2d position_in, std::function<void()> function_in); //Normal buttons
		button(std::array<olc::Decal*, 3> icon_in, olc::vf2d position_in, std::function<void()> function_in, bool active_in); //Normal buttons that are hidden
		button(olc::Decal* icon_in, olc::vf2d position_in, olc::vf2d scale_in, std::function<void()> function_in); //Token buttons

		bool OnPress();
		float Width(); float Height();
	};
	struct window
	{
		enum Window_State
		{
			WINDOW_NONE,
			WINDOW_REVEAL,
			WINDOW_DISAPPEAR
		};
		//This struct takes care of each image that carries buttons
		olc::vf2d position; olc::vf2d unrevealedPosition; olc::vf2d revealedPosition;
		std::vector<button> buttons;
		olc::Decal* background;
		std::string text;
		Window_State state;
		float speed = 1;

		window(const std::vector<button> buttons_in, olc::Decal* background_in, olc::vf2d position_in, std::string text_in, olc::vf2d revealedPosition_in);
		window(const std::vector<button> buttons_in, olc::Decal* background_in, olc::vf2d position_in, std::string text_in, olc::vf2d revealedPosition_in, float speed_in);

		void Update(float fElapsedTime);
		void Move(float fElapsedTime, olc::vf2d& a, olc::vf2d b, olc::vf2d source);

		void ToggleReveal();
		button* CheckButtonCollision(olc::vf2d mouse, olc::vf2d UIscale);

		void Render(DnDTool* dndTool);
	};
	struct canvas
	{
		//This struct takes care of each scene
		std::vector<window> windows = {}; // index 0 is always the frame
		olc::vf2d UIoffset = { 0,0 }; //how offset the entire image is due to the UI
		olc::vf2d scaleUIOffset = { 0,0 };

		canvas(std::vector<window> windows_in, olc::vf2d UIoffset_in);

		void Update(float fElapsedTime);

		float FrameWidth();
		float FrameHeight();

		void Render(DnDTool* dndTool);
		void RenderText(DnDTool* dndTool);
	};
	struct token
	{
		olc::Decal* icon = nullptr;
		olc::Decal* icon_unmasked = nullptr;
		olc::Decal* icon_dead = nullptr;
		olc::Decal* icon_deadcrossed = nullptr;

		olc::Pixel tint;
		olc::vf2d position;
		std::string name;
		int icon_index;
		float angle = 0;
		bool dead = false;

		token();
		token(std::vector<olc::Decal*> icon_in, olc::Pixel tint_in, const std::string& name_in, int index_in);
		token(const std::string& name_in, std::vector<olc::Decal*> icon_in, int index_in, float angle_in, olc::vf2d position_in, bool dead_in);

		void Render(DnDTool* dndTool, float tileWidthRatio, float tileableSize, float gridWidth, olc::vf2d scale, float iconToTileRatio);
		void RenderText(DnDTool* dndTool, float tileWidthRatio, float tileableSize, bool isSelected, olc::vf2d renderPosition);

		bool Save(FILE* file, std::vector<uint8_t>& fileData, uint8_t& bytesWritten);
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
		std::vector<token> characters; //All saved characters in the map
		std::vector<link> links;
		olc::Decal* background = nullptr;
		int background_index = 0; //Needed when saving and loading to figure out what image to load

		std::string mapIdentifier;
		int commonDivisorIndex = 0;

		map(std::string mapIdentifier_in, int background_index_in, std::vector<link> links_in);
		map(std::string mapIdentifier_in, int background_index_in, std::vector<token> tokens_in, int commonDivisor_in);
		map(std::string mapIdentifier_in, int background_index_in, std::vector<link> links_in, int commonDivisor_in);

		float Width();
		float Height();

		bool Save(std::string path, const DnDTool::map map);
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//Here follows variables related to different interaction modes:
	//MOVE
	int selectedTile = -1; //if you right click on a tile, it becomes selected. This hold the index
	//Also use this for measuring as the start point
	olc::Sprite* deathCross = nullptr;
	token* heldToken = nullptr;
	olc::vi2d previousTokenPosition = { 0,0 }; //in order to check if the heldtoken was the one that was selected
	olc::Decal* selection = nullptr; //Decal that will show up behind selected tokens
	float selectionAngle = 0; float selectionRotationSpeed = 0.01f; //Use this to rotate selection image
	token* selectedToken;
	//DRAW
	olc::Sprite* fogOfWarSprite = nullptr;
	olc::Decal* fogOfWarDecal = nullptr;
	Gdiplus::Bitmap* eraserMask = nullptr;
	olc::Decal* drawIndicator = nullptr;
	//MEASURE
	olc::Decal* measuringLine = nullptr;
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////

	std::vector<map> maps;
	std::vector<std::vector<olc::Decal*>> icons;
	std::vector<olc::Decal*> cursors;
	std::vector<token> Characters; //A list of all loaded tokens
	float iconSizeAdjustment = 0.75f; //Makes sure the icon is smaller than the tile it is on

	olc::Decal* gridTile = nullptr;
	int tileDivisor = 256;
	olc::Pixel gridColor = olc::WHITE;
	float commonDivisor = 0;

	Gdiplus::Bitmap* iconMask = nullptr;

	olc::vf2d zoom = { 1,1 };
	olc::vf2d panOffset = { 0,0 }; //When zoomed in, used to drag the camera around, ie. pan around

	std::vector<window> windows; //Different windows that may be called from anywhere
	std::vector<canvas> screens; //Different screens, i.e start screen, load screen, map screen
	int currentUI;
	std::vector<std::array<olc::Decal*, 3>> buttonIcons;
	std::vector<std::array<olc::Decal*, 3>> buttonIcons_Special; //Quit, Exit, Save, Load, Yes, No
	float scaleUnaffectedByUI = 0;
	float scaleAffectedByUI = 0;
	button* hoveredButton = nullptr;

	bool debug = false;
	olc::Decal* debugSquare;

	int currentMap = 0;

	bool playing = true;

public:
	DnDTool() {}
	float width = 1920.0f * 0.4f; //4
	float height = 1080.0f * 0.4f;

	//loading.cpp
	bool OnUserCreate()override;
	void LoadDecals(); 
	void OnLoadDecals(std::vector<olc::Decal*>& list, std::string path); 
	void OnLoadDecals(std::vector<std::array<olc::Decal*, 3>>& list, std::string path);
	void OnLoadDecalsMasked(std::vector<std::vector<olc::Decal*>>& list, std::string path);

	void LoadCharacters();
	void ConstructMaps();
	void LoadMap(std::string name);
	void LoadUI();
	std::vector<button> GetTokenButtons();
	
	//saving.cpp
	bool SaveCharacters();
	bool SaveMaps();

	//spriteManipulation.cpp
	void MaskSprite(olc::Sprite* sprite);
	void BrightenSprite(olc::Sprite* sprite);
	void GrayscaleSprite(olc::Sprite* sprite);
	void FuseSprites(olc::Sprite* sprite_1, olc::Sprite* sprite_2);

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
	void RenderWindows();

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
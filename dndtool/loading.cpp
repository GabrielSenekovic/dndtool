#include "dndtool.h"
bool DnDTool::OnUserCreate()
{
	LoadDecals();
	LoadCharacters();
	LoadUI();
	LoadMap("MapName");
	ConstructMaps();
	ToggleUI();
	return true;
}

void DnDTool::OnLoadDecals(std::vector<olc::Decal*> &list, std::string path)
{
	for (size_t i = 1; i > 0; i++)
	{
		std::string temp = path + std::to_string(i) + ".png";
		list.push_back(new olc::Decal(new olc::Sprite(temp.c_str())));
		if (list.back()->sprite->width <= 0)
		{
			list.pop_back();
			return;
		}
	}
}
void DnDTool::OnLoadDecals(std::vector<std::array<olc::Decal*, 3>>& list, std::string path)
{
	//Loading buttons with three states
	for (int i = 1; i > 0; i++)
	{
		std::string temp = path + std::to_string(i) + ".png";

		olc::Sprite* sprite = new olc::Sprite(temp);
		olc::Decal* normal = new olc::Decal(sprite);
		BrightenSprite(sprite);

		list.push_back({ normal, new olc::Decal(sprite), new olc::Decal(new olc::Sprite(path + std::to_string(i) +  "_dark.png")) });
		if (list.back()[0]->sprite->width <= 0)
		{
			list.pop_back();
			return;
		}
	}
}
void DnDTool::OnLoadDecalsMasked(std::vector<std::vector<olc::Decal*>>& list, std::string path)
{
	for (int i = 1; i > 0; i++)
	{
		std::string temp = path + std::to_string(i) + ".png";

		olc::Sprite* sprite = new olc::Sprite(temp);
		olc::Decal* unMasked = new olc::Decal(sprite);
		MaskSprite(sprite);
		olc::Decal* Masked = new olc::Decal(sprite);
		GrayscaleSprite(sprite);

		olc::Sprite* sprite_gray_crossed = new olc::Sprite(*sprite);
		FuseSprites(sprite_gray_crossed, deathCross);
		MaskSprite(sprite_gray_crossed);

		list.push_back(std::vector<olc::Decal*>{unMasked, Masked, new olc::Decal(sprite), new olc::Decal(sprite_gray_crossed)});
		if (list.back()[0]->sprite->width <= 0)
		{
			list.pop_back();
			return;
		}
	}
}
void DnDTool::LoadDecals()
{
	gridTile = new olc::Decal(new olc::Sprite("./Assets/Backgrounds/Tile.png"));
	selection = new olc::Decal(new olc::Sprite("./Assets/Tokens/Selection.png"));
	measuringLine = new olc::Decal(new olc::Sprite("./Assets/Cursors/Measuring_Line.png"));
	debugSquare = new olc::Decal(new olc::Sprite("./Assets/Debug/Debug_Square.png"));
	deathCross = new olc::Sprite("./Assets/Tokens/Dead.png");

	OnLoadDecals(buttonIcons, "./Assets/UI/Buttons/");
	OnLoadDecals(buttonIcons_Special, "./Assets/UI/Buttons/Special_");
	OnLoadDecals(cursors, "./Assets/Cursors/");

	iconMask = Gdiplus::Bitmap::FromFile(gnu::ConvertS2W("./Assets/Tokens/Mask_100.png").c_str());
	eraserMask = Gdiplus::Bitmap::FromFile(gnu::ConvertS2W("./Assets/Cursors/Eraser.png").c_str());
	drawIndicator = new olc::Decal(new olc::Sprite("./Assets/Cursors/DrawEraseCursor.png"));

	OnLoadDecalsMasked(icons, "./Assets/Tokens/");
}
void DnDTool::LoadCharacters()
{
	std::ifstream in("./Assets/SaveData/tokens.txt", std::ifstream::ate | std::ifstream::binary);
	uint8_t fileSize = in.tellg();

	FILE* file;
	fopen_s(&file, "./Assets/SaveData/tokens.txt", "rb");
	if (!file) return;
	std::vector<uint8_t> fileData = {};
	fileData.resize(fileSize);
	uint8_t bytesRead = 0;

	fread(fileData.data(), 1, fileSize, file);

	while (bytesRead < fileSize)
	{
		std::string name((const char*)(fileData.data() + bytesRead));
		bytesRead += strlen(name.c_str())+1;

		int index = *(fileData.data() + bytesRead);
		bytesRead += sizeof(int);

		Characters.push_back(token(icons[index], olc::WHITE, name, index));
	}
	fclose(file);
}

void DnDTool::ConstructMaps()
{
	fogOfWarSprite = new olc::Sprite(width, height);
	FillFog(olc::BLANK);
	/*maps =
	{
		map("MapName", 1, std::vector<map::link> { map::link("Hello", {5,5}) }, 4)
	};*/
	//scaleUnaffectedByUI = maps[currentMap].Width() > maps[currentMap].Height() ? (width / (maps[currentMap].Width())) : (height / maps[currentMap].Height());
}

void DnDTool::LoadUI()
{
	windows =
	{
		window //The inquiry window, asking yes or no
		(
			{
				button(buttonIcons_Special[1], {3, 65}, [&]() {*currentEvent.second = false; }),
				button(buttonIcons_Special[2], {63, 65}, [&]() {currentEvent.first = Event::EVENT_NONE; })
			},
			new olc::Decal(new olc::Sprite("./Assets/UI/Windows/Window_Inquiry.png")),
			{0,0}, "Do you want \nto quit?", {0,0}
		)
	};
	for (size_t i = 0; i < windows.size(); i++)
	{
		windows[i].position = { width / 2 - windows[i].background->sprite->width / 2, height / 2 - windows[i].background->sprite->height / 2 };
	}
	screens =
	{
		canvas //screen 0 is the map UI
		(
			std::vector<window>
			{
				window //The UI frame. Window[0] is always the frame
				(
					{
						button(buttonIcons[3], {752,15}, [&]() 
							{
								interactionMode = InteractionMode::MOVE; 
								screens[0].windows[0].buttons[0].currentIcon = 2;
								screens[0].windows[0].buttons[1].currentIcon = 0;
								screens[0].windows[0].buttons[2].currentIcon = 0;
								if (selectedToken->icon != nullptr)
								{
									screens[0].windows[0].buttons[4].active = true;
									screens[0].windows[0].buttons[5].active = true;
									screens[0].windows[0].buttons[6].active = true;
								}
							}),
						button(buttonIcons[1], {752,15 + 17},[&]() 
							{
								interactionMode = InteractionMode::DRAW; 
								screens[0].windows[0].buttons[0].currentIcon = 0;
								screens[0].windows[0].buttons[1].currentIcon = 2;
								screens[0].windows[0].buttons[2].currentIcon = 0;
								screens[0].windows[0].buttons[4].active = false;
								screens[0].windows[0].buttons[5].active = false;
								screens[0].windows[0].buttons[6].active = false;
							}),
						button(buttonIcons[2], {752,15 + 17 * 2},[&]() 
							{
								interactionMode = InteractionMode::MEASURE; 
								screens[0].windows[0].buttons[0].currentIcon = 0;
								screens[0].windows[0].buttons[1].currentIcon = 0;
								screens[0].windows[0].buttons[2].currentIcon = 2;
								screens[0].windows[0].buttons[4].active = false;
								screens[0].windows[0].buttons[5].active = false;
								screens[0].windows[0].buttons[6].active = false;
							}),
						button(buttonIcons[5], {752,15 + 17 * 3},[&]() { screens[0].windows[1].ToggleReveal(); }),
						button(buttonIcons[8], {4,92}, [&]() {TEA(selectedToken->angle, -90, ? 0 : -90); }, false),
						button(buttonIcons[4], {4 + 16,92},[&]() {selectedToken->dead = !selectedToken->dead; }, false),
						button(buttonIcons[9], {4 + 16 * 2,92},[&]() 
						{
							forall (i, 0, maps[currentMap].characters.size()) 
							{ 
								if (maps[currentMap].characters[i].position == selectedToken->position) 
								{
									maps[currentMap].characters.erase(maps[currentMap].characters.begin() + i);
									selectedTile = -1;
									screens[0].windows[0].buttons[4].active = false;
									screens[0].windows[0].buttons[5].active = false;
									screens[0].windows[0].buttons[6].active = false;
									break;
								}
							}
							selectedToken = nullptr; 
						}, false)

					},new olc::Decal(new olc::Sprite("./Assets/UI/Windows/UI_Border.png")),
					{0,0}, "", {0,0}
				),
				window //The token selection window. Appears when a button is pressed
				(
					{GetTokenButtons()}, //Fill up the window with all of the tokens
					new olc::Decal(new olc::Sprite("./Assets/UI/Windows/Window_TokenSelect.png")),
					{width, 15}, "", {width - 100, 15}, 3
				)
			},
			{0,0}
		)
		//screen 1 is the start UI
	};
}
std::vector<DnDTool::button> DnDTool::GetTokenButtons()
{
	std::vector<DnDTool::button> buttons = {};

	int width = 2;
	for (size_t i = 0; i < Characters.size(); i++)
	{
		int x = i % width; int y = i / width; float scale = 0.385f;
		buttons.push_back(button(Characters[i].icon_unmasked, {(float) Characters[i].icon_unmasked->sprite->width * scale * x + 2 * x + 2,(float)  Characters[i].icon_unmasked->sprite->height * scale * y + 2*y + 2}, {scale, scale}, [&, i]()
			{
				if (heldToken == nullptr)
				{
					heldToken = new token();
					maps[currentMap].characters.push_back(Characters[i]);
					heldToken = &maps[currentMap].characters.back();
					heldToken->position = { -1,-1 };
				}
			}
		));
	}
	return buttons;
}

void DnDTool::LoadMap(std::string name_in)
{
	std::ifstream in("./Assets/SaveData/maps/" + name_in + ".txt", std::ifstream::ate | std::ifstream::binary);
	uint8_t fileSize = in.tellg();

	FILE* file;
	fopen_s(&file, ("./Assets/SaveData/maps/" + name_in + ".txt").c_str(), "rb");
	if (!file) return;
	std::vector<uint8_t> fileData = {};
	fileData.resize(fileSize);
	uint8_t bytesRead = 0;

	fread(fileData.data(), 1, fileSize, file);

	std::string name((const char*)(fileData.data() + bytesRead));
	bytesRead += strlen(name.c_str()) + 1;
	int backgroundIndex = *(fileData.data() + bytesRead);
	bytesRead += sizeof(int);
	int commonDivisorIndex = *(fileData.data() + bytesRead);
	bytesRead += sizeof(int);

	std::vector<token> tokens = {};

	while (bytesRead < fileSize)
	{
		std::string token_name((const char*)(fileData.data() + bytesRead));
		bytesRead += strlen(token_name.c_str()) + 1;
		int token_icon_index = *(fileData.data() + bytesRead);
		bytesRead += sizeof(int);
		float token_angle = *(fileData.data() + bytesRead);
		bytesRead += sizeof(float);
		float token_x = *(fileData.data() + bytesRead);
		bytesRead += sizeof(float);
		float token_y = *(fileData.data() + bytesRead);
		bytesRead += sizeof(float);
		olc::vf2d token_position = { token_x, token_y };
		bool token_dead = *(fileData.data() + bytesRead);
		bytesRead += sizeof(bool);
		tokens.push_back(token(token_name, icons[token_icon_index], token_icon_index, token_angle, token_position, token_dead));
	}

	maps.push_back
	(
		map(name, backgroundIndex, tokens ,commonDivisorIndex)
	);

	fclose(file);

	scaleUnaffectedByUI = maps[currentMap].Width() > maps[currentMap].Height() ? (width / (maps[currentMap].Width())) : (height / maps[currentMap].Height());
	SetScaleAffectedByUI();
	if (commonDivisor == 0)
	{
		std::vector<int> commonDivisors = gnu::findCommonDivisors(maps[currentMap].Width(), maps[currentMap].Height());
		int gridWidth = (scaleUnaffectedByUI / commonDivisors[maps[currentMap].commonDivisorIndex + 1] * maps[currentMap].Width());
		for (size_t i = 0; i < commonDivisors.size(); i++)
		{
			if (commonDivisors[i] >= 50)
			{
				maps[currentMap].commonDivisorIndex = i;
				break;
			}
		}
		commonDivisor = commonDivisors[maps[currentMap].commonDivisorIndex];
	}
}
#include "dndtool.h"
bool DnDTool::OnUserCreate()
{
	LoadDecals();
	LoadCharacters();
	ConstructMaps();
	//ToggleUI();
	LoadUI();
	LoadMap();
	return true;
}

void DnDTool::OnLoadDecals(std::vector<olc::Decal*> &list, std::string path)
{
	for (int i = 1; i > 0; i++)
	{
		std::string temp = path + std::to_string(i) + ".png";
		list.push_back(new olc::Decal(new olc::Sprite(temp.c_str())));
		//list.emplace_back(new olc::Sprite(path.c_str()));
		if (list.back()->sprite->width <= 0)
		{
			list.pop_back();
			return;
		}
	}
}
void DnDTool::OnLoadDecalsMasked(std::vector<std::pair<olc::Decal*, olc::Decal*>>& list, std::string path)
{
	for (int i = 1; i > 0; i++)
	{
		std::string temp = path + std::to_string(i) + ".png";

		olc::Sprite* sprite = new olc::Sprite(temp);
		olc::Decal* unMasked = new olc::Decal(sprite);
		MaskSprite(sprite);

		list.push_back(std::pair<olc::Decal*, olc::Decal*>(unMasked, new olc::Decal(sprite)));
		//list.emplace_back(new olc::Sprite(path.c_str()));
		if (list.back().first->sprite->width <= 0)
		{
			list.pop_back();
			return;
		}
	}
}
void DnDTool::LoadDecals()
{
	gridTile = new olc::Decal(new olc::Sprite("./Assets/Tile.png"));
	selection = new olc::Decal(new olc::Sprite("./Assets/Selection.png"));
	measuringLine = new olc::Decal(new olc::Sprite("./Assets/Measuring_Line.png"));
	debugSquare = new olc::Decal(new olc::Sprite("./Assets/Debug_Square.png"));

	OnLoadDecals(backgrounds, "./Assets/Backgrounds/");
	OnLoadDecals(buttonIcons, "./Assets/UI/");
	OnLoadDecals(buttonIcons_Special, "./Assets/UI/Special_");

	iconMask = Gdiplus::Bitmap::FromFile(gnu::ConvertS2W("./Assets/Mask_100.png").c_str());
	eraserMask = Gdiplus::Bitmap::FromFile(gnu::ConvertS2W("./Assets/Eraser.png").c_str());

	std::string cursor_paths[5] = { "Cursor_Hover.png", "Cursor_Grab.png", "Cursor_Grabbable.png", "Cursor_Draw.png" , "Cursor_Eraser.png" };
	for (int i = 0; i < 5; i++)
	{
		std::string path = "./Assets/" + cursor_paths[i];
		cursors.push_back(new olc::Decal(new olc::Sprite(path)));
	}

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
void DnDTool::MaskSprite(olc::Sprite* sprite)
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
void DnDTool::ConstructMaps()
{
	fogOfWarSprite = new olc::Sprite(width, height);
	FillFog(olc::BLANK);
	maps =
	{
		map("MapName", backgrounds[0], std::vector<map::link> { map::link("Hello", {5,5}) }, 1)
	};
	scaleUnaffectedByUI = maps[currentMap].Width() > maps[currentMap].Height() ? (width / (maps[currentMap].Width())) : (height / maps[currentMap].Height());
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
			new olc::Decal(new olc::Sprite("./Assets/UI/Window_Inquiry.png")),
			{0,0}, "Do you want \nto quit?", {0,0}
		)
	};
	for (int i = 0; i < windows.size(); i++)
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
						button(buttonIcons[3], {752,15}, [&]() {interactionMode = InteractionMode::MOVE; }),
						button(buttonIcons[1], {752,15 + 17},[&]() {interactionMode = InteractionMode::DRAW; }),
						button(buttonIcons[2], {752,15 + 17 * 2},[&]() {interactionMode = InteractionMode::MEASURE; }),
						button(buttonIcons[5], {752,15 + 17 * 3},[&]() { screens[0].windows[1].ToggleReveal(); }),
					},new olc::Decal(new olc::Sprite("./Assets/UI/UI_Border.png")),
					{0,0}, "", {0,0}
				),
				window //The token selection window. Appears when a button is pressed
				(
					{GetTokenButtons()}, //Fill up the window with all of the tokens
					new olc::Decal(new olc::Sprite("./Assets/UI/Window_TokenSelect.png")),
					{width, 0}, "", {width - 100, 0}
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

	for (size_t i = 0; i < Characters.size(); i++)
	{
		buttons.push_back(button(Characters[i].icon, { 0,0 }, [&, i]()
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

void DnDTool::LoadMap()
{
	SetScaleAffectedByUI();
	std::vector<int> commonDivisors = gnu::findCommonDivisors(maps[currentMap].Width(), maps[currentMap].Height());
	int gridWidth = (scaleUnaffectedByUI / commonDivisors[commonDivisorIndex + 1] * maps[currentMap].Width());
	for (int i = 0; i < commonDivisors.size(); i++)
	{
		if (commonDivisors[i] >= 50)
		{
			commonDivisorIndex = i;
			break;
		}
	}
	commonDivisor = commonDivisors[commonDivisorIndex];
}
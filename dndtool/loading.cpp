#include "dndtool.h"
bool DnDTool::OnUserCreate()
{
	LoadDecals();
	LoadPlayers();
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
void DnDTool::LoadDecals()
{
	OnLoadDecals(backgrounds, "./Assets/Backgrounds/");
	gridTile = new olc::Decal(new olc::Sprite("./Assets/Tile.png"));
	selection = new olc::Decal(new olc::Sprite("./Assets/Selection.png"));
	measuringLine = new olc::Decal(new olc::Sprite("./Assets/Measuring_Line.png"));
	OnLoadDecals(buttonIcons, "./Assets/UI/");
	OnLoadDecals(buttonIcons_Special, "./Assets/UI/Special_");

	debugSquare = new olc::Decal(new olc::Sprite("./Assets/Debug_Square.png"));

	iconMask = Gdiplus::Bitmap::FromFile(gnu::ConvertS2W("./Assets/Mask_100.png").c_str());
	eraserMask = Gdiplus::Bitmap::FromFile(gnu::ConvertS2W("./Assets/Eraser.png").c_str());

	std::string cursor_paths[5] = { "Cursor_Hover.png", "Cursor_Grab.png", "Cursor_Grabbable.png", "Cursor_Draw.png" , "Cursor_Eraser.png" };
	for (int i = 0; i < 5; i++)
	{
		std::string path = "./Assets/" + cursor_paths[i];
		cursors.push_back(new olc::Decal(new olc::Sprite(path)));
	}

	std::string mugshot_paths[6] = { "Test.png", "Player_Haiku.png", "Player_Terrahin.png", "Player_Nym.png", "Player_Bob.png", "Player_Isk.png" };
	int size = sizeof(mugshot_paths) / sizeof(mugshot_paths[0]);
	for (int i = 0; i < size; i++)
	{
		std::string path = "./Assets/Tokens/" + mugshot_paths[i];
		olc::Sprite* sprite = new olc::Sprite(path);
		olc::Decal* unMasked = new olc::Decal(sprite);
		MaskSprite(sprite);
		icons.push_back(std::pair<olc::Decal*, olc::Decal*>(unMasked, new olc::Decal(sprite)));
	}
}
void DnDTool::LoadPlayers()
{
	NPCs.push_back(token(icons[5], { 0,0 }, olc::WHITE, "Isk"));
	NPCs.push_back(token(icons[0], { 1,0 }, olc::WHITE, "Cinder"));
	NPCs.push_back(token(icons[2], { 2,0 }, olc::WHITE, "Tarrehin"));
	NPCs.push_back(token(icons[4], { 3,0 }, olc::WHITE, "Bob"));
	NPCs.push_back(token(icons[3], { 4,0 }, olc::WHITE, "Nym"));
	NPCs.push_back(token(icons[1], { 5,0 }, olc::WHITE, "Haiku"));
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
			{0,0}, "Do you want \nto quit?"
		)
	};
	for (int i = 0; i < windows.size(); i++)
	{
		windows[i].position = { width / 2 - windows[i].background->sprite->width / 2, height / 2 - windows[i].background->sprite->height / 2 };
	}
	screens =
	{
		canvas()
		//screen 0 is the map UI
		//screen 1 is the start UI
	};
	float distanceBetweenButtons = 17;
	screens[currentUI].buttons =
	{
		button(buttonIcons[3], {752,15}, [&]() {interactionMode = InteractionMode::MOVE; }),
		button(buttonIcons[1], {752,15 + distanceBetweenButtons},[&]() {interactionMode = InteractionMode::DRAW; }),
		button(buttonIcons[2], {752,15 + distanceBetweenButtons * 2},[&]() {interactionMode = InteractionMode::MEASURE; }),
	};
	screens[0].windows.push_back(new olc::Decal(new olc::Sprite("./Assets/UI/UI_Border.png")));
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
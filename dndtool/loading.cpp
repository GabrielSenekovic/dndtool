#include "dndtool.h"
bool DnDTool::OnUserCreate()
{
	LoadDecals();
	LoadPlayers();
	ConstructMaps();
	ToggleUI();
	return true;
}
void DnDTool::LoadDecals()
{
	gridTile = new olc::Decal(new olc::Sprite("./Assets/Tile.png"));
	selection = new olc::Decal(new olc::Sprite("./Assets/Selection.png"));
	backgrounds.push_back(new olc::Decal(new olc::Sprite("./Assets/1.png")));
	measuringLine = new olc::Decal(new olc::Sprite("./Assets/Measuring_Line.png"));
	UIBorder = new olc::Decal(new olc::Sprite("./Assets/UI_Border.png"));
	UIButton = new olc::Decal(new olc::Sprite("./Assets/UI_Button_Background.png"));

	iconMask = Gdiplus::Bitmap::FromFile(olc::ConvertS2W("./Assets/Mask_100.png").c_str());
	eraserMask = Gdiplus::Bitmap::FromFile(olc::ConvertS2W("./Assets/Eraser.png").c_str());

	std::string cursor_paths[5] = { "Cursor_Hover.png", "Cursor_Grab.png", "Cursor_Grabbable.png", "Cursor_Draw.png" , "Cursor_Eraser.png" };
	for (int i = 0; i < 5; i++)
	{
		std::string path = "./Assets/" + cursor_paths[i];
		cursors.push_back(new olc::Decal(new olc::Sprite(path)));
	}

	std::string mugshot_paths[5] = { "Test.png", "Player_Haiku.png", "Player_Terrahin.png", "Player_Nym.png", "Player_Bob.png" };
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
	NPCs.push_back(token(icons[0], { 0,0 }, olc::WHITE, "Isk"));
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
	scaleUnaffectedByUI = maps[currentMap].background->sprite->width > maps[currentMap].background->sprite->height ? (width / (maps[currentMap].background->sprite->width)) : (height / maps[currentMap].background->sprite->height);
}
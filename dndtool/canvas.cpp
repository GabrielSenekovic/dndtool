#include "dndtool.h"

DnDTool::canvas::canvas(std::vector<window> windows_in, olc::vf2d UIoffset_in):windows(windows_in), UIoffset(UIoffset_in){}

void DnDTool::canvas::Update(float fElapsedTime)
{
	for (size_t i = 0; i < windows.size(); i++)
	{
		windows[i].Update(fElapsedTime);
	}
}

void DnDTool::canvas::Render(DnDTool* dndTool)
{
	olc::vf2d scale = { dndTool->width / FrameWidth(), dndTool->height / FrameHeight() };
	for (size_t i = windows.size(); i > 0; i--)
	{
		windows[i-1].Render(dndTool);
	}

	if (dndTool->selectedToken && dndTool->selectedToken->icon)
	{
		float widthOfPortraitFrame = 62.0f;
		float ratio = widthOfPortraitFrame / dndTool->selectedToken->icon->sprite->width * scale.x;
		dndTool->DrawDecal({ 4 * scale.x,16 * scale.y}, dndTool->selectedToken->icon_unmasked, { ratio,ratio });
		dndTool->DrawStringDecal({ 4 * scale.x, 86 * scale.y }, dndTool->selectedToken->name, olc::WHITE, { 0.5f, 0.5f });
	}
	RenderText(dndTool);
}
void DnDTool::canvas::RenderText(DnDTool* dndTool)
{
	dndTool->DrawStringDecal({ 2,2 },
		"[Grid Coord X: " + std::to_string(dndTool->GetMousePositionInXY().x) + " Y: " + std::to_string(dndTool->GetMousePositionInXY().y) + "]" +
		"[F_Grid Coord X: " + std::to_string(dndTool->GetMousePositionInXYFloat().x) + " Y: " + std::to_string(dndTool->GetMousePositionInXYFloat().y) + "]" +
		" [Selected tile Index: " + std::to_string(dndTool->selectedTile) +
		" [Selected tile X: " + std::to_string(dndTool->selectedTile % (int)(dndTool->maps[dndTool->currentMap].Width() * (dndTool->scaleUnaffectedByUI / dndTool->commonDivisor))) + " Y: " + std::to_string((int)(dndTool->selectedTile / (dndTool->maps[dndTool->currentMap].Width() * (dndTool->scaleUnaffectedByUI / dndTool->commonDivisor))))
		, olc::WHITE, { 0.5f, 0.5f });
	dndTool->DrawStringDecal({ 2,7 }, "[Mouse X: " + std::to_string(dndTool->GetMouseX()) + " Y: " + std::to_string(dndTool->GetMouseY()) + "]" +
		" [Zoom:" + std::to_string(dndTool->zoom.x) + "]"
		, olc::WHITE, { 0.5f, 0.5f });
}
float DnDTool::canvas::FrameWidth()
{
	return windows[0].background->sprite->width;
}
float DnDTool::canvas::FrameHeight()
{
	return windows[0].background->sprite->height;
}
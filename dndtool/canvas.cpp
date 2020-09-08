#include "dndtool.h"

void DnDTool::canvas::Render(DnDTool* dndTool)
{
	olc::vf2d scale = { dndTool->width / UIBorder->sprite->width, dndTool->height / UIBorder->sprite->height };
	dndTool->DrawDecal({ 0,0 }, UIBorder, scale);
	if (dndTool->selectedToken.icon != nullptr)
	{
		float widthOfPortraitFrame = 62.0f;
		float ratio = widthOfPortraitFrame / dndTool->selectedToken.icon->sprite->width * scale.x;
		dndTool->DrawDecal({ 4 * scale.x,16 * scale.y}, dndTool->selectedToken.icon_unmasked, { ratio,ratio });
		dndTool->DrawStringDecal({ 4 * scale.x, 86 * scale.y }, dndTool->selectedToken.name, olc::WHITE, { 0.5f, 0.5f });
	}
	RenderButtons(dndTool, scale);
	RenderText(dndTool);
}
void DnDTool::canvas::RenderButtons(DnDTool* dndTool, olc::vf2d scale)
{
	for (int i = 0; i < modeButtons.size(); i++)
	{
		dndTool->DrawDecal(modeButtons[i].position * scale, modeButtons[i].icons[0], scale);
	}
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
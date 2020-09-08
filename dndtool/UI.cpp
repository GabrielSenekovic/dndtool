#include "dndtool.h"

void DnDTool::UI::Render(DnDTool dndTool)
{
	olc::vf2d scale = { dndTool.width / dndTool.UIBorder->sprite->width, dndTool.height / dndTool.UIBorder->sprite->height };
	dndTool.DrawDecal({ 0,0 }, dndTool.UIBorder, scale);
	if (dndTool.selectedToken.icon != nullptr)
	{
		float ratio = (62.0f / dndTool.selectedToken.icon->sprite->width) * dndTool.width / dndTool.UIBorder->sprite->width;
		dndTool.DrawDecal({ 3,12 }, dndTool.selectedToken.icon_unmasked, { ratio,ratio });
		dndTool.DrawStringDecal({ 3, 64 }, dndTool.selectedToken.name, olc::WHITE, { 0.5f, 0.5f });
	}
	RenderButtons(dndTool, scale);
	RenderText(dndTool);
}
void DnDTool::UI::RenderButtons(DnDTool dndTool, olc::vf2d scale)
{
	for (int i = 0; i < dndTool.modeButtons.size(); i++)
	{
		dndTool.DrawDecal(dndTool.modeButtons[i].position, dndTool.modeButtons[i].icon, scale);
	}
}
void DnDTool::UI::RenderText(DnDTool dndTool)
{
	dndTool.DrawStringDecal({ 2,2 },
		"[Grid Coord X: " + std::to_string(dndTool.GetMousePositionInXY().x) + " Y: " + std::to_string(dndTool.GetMousePositionInXY().y) + "]" +
		"[F_Grid Coord X: " + std::to_string(dndTool.GetMousePositionInXYFloat().x) + " Y: " + std::to_string(dndTool.GetMousePositionInXYFloat().y) + "]" +
		" [Selected tile Index: " + std::to_string(dndTool.selectedTile) +
		" [Selected tile X: " + std::to_string(dndTool.selectedTile % (int)(dndTool.maps[dndTool.currentMap].Width() * (dndTool.scaleUnaffectedByUI / dndTool.commonDivisor))) + " Y: " + std::to_string((int)(dndTool.selectedTile / (dndTool.maps[dndTool.currentMap].Width() * (dndTool.scaleUnaffectedByUI / dndTool.commonDivisor))))
		, olc::WHITE, { 0.5f, 0.5f });
	dndTool.DrawStringDecal({ 2,7 }, "[Mouse X: " + std::to_string(dndTool.GetMouseX()) + " Y: " + std::to_string(dndTool.GetMouseY()) + "]" +
		" [Zoom:" + std::to_string(dndTool.zoom.x) + "]"
		, olc::WHITE, { 0.5f, 0.5f });
}
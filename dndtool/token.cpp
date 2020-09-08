#include "dndtool.h"

void DnDTool::token::Render(DnDTool* dndTool, float tileWidthRatio, float tileableSize, float gridWidth, olc::vf2d scale, float iconToTileRatio)
{
	iconToTileRatio *= dndTool->iconSizeAdjustment; //Shrinks token to be smaller than tile
	scale = scale * iconToTileRatio;

	float centeringModifier = ((dndTool->gridTile->sprite->width * tileableSize) - ((dndTool->gridTile->sprite->width * tileableSize) * dndTool->iconSizeAdjustment)) / 2;

	olc::vf2d renderPosition = { (position.x * dndTool->gridTile->sprite->width * tileableSize + centeringModifier) / tileWidthRatio, (position.y * dndTool->gridTile->sprite->height * tileableSize + centeringModifier) / tileWidthRatio};

	dndTool->RenderImage(icon, renderPosition, scale, olc::WHITE);

	bool isSelected = dndTool->selectedTile == (int)(position.y * gridWidth + position.x);
	bool isHoveringOver = dndTool->GetMousePositionInXY().x == position.x && dndTool->GetMousePositionInXY().y == position.y;

	if (isHoveringOver || isSelected)
	{
		RenderText(dndTool, tileWidthRatio, tileableSize, isSelected, renderPosition);
	}
}
void DnDTool::token::RenderText(DnDTool* dndTool, float tileWidthRatio, float tileableSize, bool isSelected, olc::vf2d renderPosition)
{
	float lengthOfName = name.length() * 8;
	olc::Pixel tint = isSelected ? olc::RED : olc::WHITE;

	float yPositionModifier = isSelected ? 24 : 20; //Makes sure the text doesnt overlap with the red selection circle
	float lengthToSubtract = ((lengthOfName - dndTool->gridTile->sprite->width) / 4) + dndTool->gridTile->sprite->width * tileableSize * 2;

	dndTool->RenderText(name, { renderPosition.x - lengthToSubtract, renderPosition.y + yPositionModifier }, { 0.5f, 0.5f }, tint);
}
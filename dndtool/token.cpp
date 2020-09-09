#include "dndtool.h"

void DnDTool::token::Render(DnDTool* dndTool, float tileWidthRatio, float tileableSize, float gridWidth, olc::vf2d scale, float iconToTileRatio)
{
	iconToTileRatio *= dndTool->iconSizeAdjustment; //Shrinks token to be smaller than tile
	scale = scale * iconToTileRatio;

	float centeringModifier = ((dndTool->gridTile->sprite->width * tileableSize) - ((dndTool->gridTile->sprite->width * tileableSize) * dndTool->iconSizeAdjustment)) / 2;

	olc::vf2d renderPosition = { position.x * dndTool->gridTile->sprite->width * tileableSize, position.y * dndTool->gridTile->sprite->height * tileableSize};
	olc::vf2d renderPosition_icon = { (renderPosition.x + centeringModifier) / tileWidthRatio, (renderPosition.y + centeringModifier) / tileWidthRatio };
	olc::vf2d renderPosition_text = { renderPosition.x / tileWidthRatio, renderPosition.y / tileWidthRatio };

	dndTool->RenderImage(icon, renderPosition_icon, scale, olc::WHITE);

	bool isSelected = dndTool->selectedTile == (int)(position.y * gridWidth + position.x);
	bool isHoveringOver = dndTool->GetMousePositionInXY().x == position.x && dndTool->GetMousePositionInXY().y == position.y;

	if (isHoveringOver || isSelected)
	{
		RenderText(dndTool, tileWidthRatio, tileableSize, isSelected, renderPosition_text);
	}
}
void DnDTool::token::RenderText(DnDTool* dndTool, float tileWidthRatio, float tileableSize, bool isSelected, olc::vf2d renderPosition)
{
	float scale = 0.8f;
	olc::Pixel tint = isSelected ? olc::RED : olc::WHITE;

	float yPositionModifier = isSelected ? 290 : 280; //Makes sure the text doesnt overlap with the red selection circle
	float nameLength = name.length() * 8 * scale;

	float centerer = (dndTool->gridTile->sprite->width * tileableSize / tileWidthRatio - nameLength) / 2;

	dndTool->RenderText(name, { renderPosition.x + centerer, renderPosition.y + yPositionModifier * tileableSize}, { scale,  scale }, tint);

	dndTool->RenderImage(dndTool->debugSquare, { renderPosition.x + centerer, renderPosition.y + yPositionModifier * tileableSize }, {0.02,0.02}, tint);
	dndTool->RenderImage(dndTool->debugSquare, { renderPosition.x + nameLength + centerer, renderPosition.y + yPositionModifier * tileableSize }, { 0.02,0.02 }, tint);
}
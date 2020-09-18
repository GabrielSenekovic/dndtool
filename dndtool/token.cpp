#include "dndtool.h"

DnDTool::token::token(){}
DnDTool::token::token(std::vector<olc::Decal*> icon_in, olc::Pixel tint_in, const std::string& name_in, int index_in): 
icon(icon_in[1]), icon_unmasked(icon_in[0]), icon_dead(icon_in[2]), icon_deadcrossed(icon_in[3]), tint(tint_in), name(name_in), icon_index(index_in){}
DnDTool::token::token(const std::string& name_in, std::vector<olc::Decal*> icon_in, int index_in, float angle_in, olc::vf2d position_in, bool dead_in)
	:name(name_in), angle(angle_in), position(position_in), dead(dead_in), icon_index(index_in),
	icon_unmasked(icon_in[0]), icon(icon_in[1]), icon_dead(icon_in[2]), icon_deadcrossed(icon_in[3])
{
}

void DnDTool::token::Render(DnDTool* dndTool, float tileWidthRatio, float tileableSize, float gridWidth, olc::vf2d scale, float iconToTileRatio)
{
	iconToTileRatio *= dndTool->iconSizeAdjustment; //Shrinks token to be smaller than tile
	scale = scale * iconToTileRatio;

	float centeringModifier = (dndTool->gridTile->sprite->width * tileableSize * ( 1 - dndTool->iconSizeAdjustment)) * 2; // x - xy == x(1-y)
	//*2 if its rotating, otherwise /2

	olc::vf2d spriteDimensions = dndTool->gridTile->sprite->dim();
	olc::vf2d renderPosition = position * spriteDimensions * tileableSize;
	olc::vf2d renderPosition_icon = (renderPosition + centeringModifier) / tileWidthRatio;
	olc::vf2d renderPosition_text = renderPosition / tileWidthRatio;

	dndTool->RenderImage(dead?icon_deadcrossed:icon, renderPosition_icon, scale, olc::WHITE, angle * 3.1415f / 180, icon->sprite->dim()/2);

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

	float yPositionModifier = isSelected ? 280 : 270; //Makes sure the text doesnt overlap with the red selection circle
	float nameLength = name.length() * 8 * scale;

	float centerer = (dndTool->gridTile->sprite->width * tileableSize / tileWidthRatio - nameLength) / 2;

	dndTool->RenderText(name, { renderPosition.x + centerer, renderPosition.y + yPositionModifier * tileableSize}, { scale,  scale }, tint);

	//dndTool->RenderImage(dndTool->debugSquare, { renderPosition.x + centerer, renderPosition.y + yPositionModifier * tileableSize }, {0.02,0.02}, tint);
	//dndTool->RenderImage(dndTool->debugSquare, { renderPosition.x + nameLength + centerer, renderPosition.y + yPositionModifier * tileableSize }, { 0.02,0.02 }, tint);
}
bool DnDTool::token::Save(FILE* file, std::vector<uint8_t>& fileData, uint8_t& bytesWritten)
{
	fileData.resize(bytesWritten + name.size() + 1 + sizeof(int) + sizeof(float)*3 + sizeof(bool));

	memcpy(fileData.data() + bytesWritten, name.data(), name.size());
	bytesWritten += name.size() + 1;
	memcpy(fileData.data() + bytesWritten, &icon_index, sizeof(int));
	bytesWritten += sizeof(int);
	memcpy(fileData.data() + bytesWritten, &angle, sizeof(float));
	bytesWritten += sizeof(float);
	memcpy(fileData.data() + bytesWritten, &position.x, sizeof(float));
	bytesWritten += sizeof(float);
	memcpy(fileData.data() + bytesWritten, &position.y, sizeof(float));
	bytesWritten += sizeof(float);
	memcpy(fileData.data() + bytesWritten, &dead, sizeof(bool));
	bytesWritten += sizeof(bool);

	return true;
}
#include "dndtool.h"

olc::vf2d DnDTool::GetScaleUIOffset()
{
	return UIoffset.x == 0 ? olc::vf2d{ 0,0 } : olc::vf2d{ 48 * width / UIBorder->sprite->width, 32 * height / UIBorder->sprite->height };
}
olc::vi2d DnDTool::GetMousePositionInXY()
{
	return olc::vi2d({ (int)GetMousePositionInXYFloat().x, (int)GetMousePositionInXYFloat().y });
}
olc::vf2d DnDTool::GetMousePositionInXYFloat()
{
	olc::vf2d scaleUIOffset = GetScaleUIOffset();
	float commonDivisor = gnu::findCommonDivisors(maps[currentMap].background->sprite->width * scaleUnaffectedByUI, maps[currentMap].background->sprite->height * scaleUnaffectedByUI)[commonDivisorIndex];
	float mapWidthScaledByUI = maps[currentMap].background->sprite->width * scaleAffectedByUI;
	float tileableSize = commonDivisor / tileDivisor;
	float amountOfColumns = scaleUnaffectedByUI / commonDivisor;
	int gridWidth = (maps[currentMap].background->sprite->width * amountOfColumns);
	float adjustedTileWidth = (mapWidthScaledByUI / gridWidth) / tileableSize;

	olc::vf2d downScaledTileDimensions = { commonDivisor * adjustedTileWidth, commonDivisor * adjustedTileWidth };

	return olc::vf2d({ (float)(GetMouseX() - UIoffset.x) * (float)tileDivisor / downScaledTileDimensions.x, (float)(GetMouseY() - UIoffset.y) * (float)tileDivisor / downScaledTileDimensions.y });
}
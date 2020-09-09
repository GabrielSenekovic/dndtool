#include "dndtool.h"

void DnDTool::SetScaleUIOffset()
{
	screens[currentUI].scaleUIOffset = screens[currentUI].UIoffset.x == 0 ? olc::vf2d{ 0,0 } : olc::vf2d{ 48 * width / screens[currentUI].windows[0]->sprite->width, 32 * height / screens[currentUI].windows[0]->sprite->height };
	SetScaleAffectedByUI();
}
void DnDTool::SetScaleAffectedByUI()
{
	//Only needs to be calculated when UIoffset change or when the map changes, so it shouldnt be here!
	scaleAffectedByUI = maps[currentMap].Width() > maps[currentMap].Height() ? //Is the width bigger than the height?
		((width - screens[currentUI].scaleUIOffset.x) / (maps[currentMap].Height())) : //If its wider than tall, then scale based on width
		((height - screens[currentUI].scaleUIOffset.y) / maps[currentMap].Height()); //Otherwise, scale by height 
}
olc::vi2d DnDTool::GetMousePositionInXY()
{
	return olc::vi2d({ (int)GetMousePositionInXYFloat().x, (int)GetMousePositionInXYFloat().y });
}
olc::vf2d DnDTool::GetMousePositionInXYFloat()
{
	SetScaleUIOffset();
	float mapWidthScaledByUI = maps[currentMap].Width() * scaleAffectedByUI;
	float tileableSize = commonDivisor / tileDivisor;
	float amountOfColumns = scaleUnaffectedByUI / commonDivisor;
	int gridWidth = (maps[currentMap].Width() * amountOfColumns);
	float adjustedTileWidth = (mapWidthScaledByUI / gridWidth) / tileableSize;

	olc::vf2d downScaledTileDimensions = { commonDivisor * adjustedTileWidth, commonDivisor * adjustedTileWidth };

	return olc::vf2d({ (float)(GetMouseX() - screens[currentUI].UIoffset.x) * (float)tileDivisor / downScaledTileDimensions.x, (float)(GetMouseY() - screens[currentUI].UIoffset.y) * (float)tileDivisor / downScaledTileDimensions.y });
}
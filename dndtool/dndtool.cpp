#include "dndtool.h"

bool DnDTool::OnUserUpdate(float fElepsedTime)
{
	if (selectedTile > -1)
	{
		selectionAngle += selectionRotationSpeed;
	}
	screens[currentUI].Update(fElepsedTime);
	RenderAll();
	CheckInput();
	return playing;
}	
void DnDTool:: Quit()
{
	SaveCharacters();
	for (size_t i = 0; i < icons.size(); i++)
	{
		delete icons[i].first;
		delete icons[i].second;
	}
	for (size_t i = 0; i < cursors.size(); i++){delete cursors[i];}
	for (size_t i = 0; i < backgrounds.size(); i++){delete backgrounds[i];}
	for (size_t i = 0; i < buttonIcons.size(); i++)
	{
		for (size_t j = 0; j < buttonIcons[i].size(); j++)
		{
			delete buttonIcons[i][j];
		}
	}
	for (size_t i = 0; i < buttonIcons_Special.size(); i++)
	{
		for (size_t j = 0; j < buttonIcons_Special[i].size(); j++)
		{
			delete buttonIcons_Special[i][j];
		}
	}
	delete gridTile;
	delete fogOfWarSprite;
	delete fogOfWarDecal;
	delete eraserMask;
	delete drawIndicator;
	delete measuringLine;
	delete selection;
	delete heldToken;
	delete iconMask;
	delete hoveredButton;
	delete debugSquare;
}

int main()
{
	DnDTool dndtool;
	if (dndtool.Construct((int32_t)dndtool.width, (int32_t)dndtool.height, 2, 2, true))
	{
		dndtool.Start();
	}
	dndtool.Quit();
	return 0;
}
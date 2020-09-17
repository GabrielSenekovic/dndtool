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
	SaveMaps();
	for (size_t i = 0; i < icons.size(); i++)
	{
		for (size_t j = 0; j < icons[i].size(); j++)
		{
			DEL(icons[i][j]);
		}
	}
	for (size_t i = 0; i < cursors.size(); i++) { DEL(cursors[i]) }
	for (size_t i = 0; i < buttonIcons.size(); i++)
	{
		for (size_t j = 0; j < buttonIcons[i].size(); j++)
		{
			DEL(buttonIcons[i][j]);
		}
	}
	for (size_t i = 0; i < buttonIcons_Special.size(); i++)
	{
		for (size_t j = 0; j < buttonIcons_Special[i].size(); j++)
		{
			DEL(buttonIcons_Special[i][j]);
		}
	}
	DEL(gridTile);
	DEL(fogOfWarSprite);
	DEL(fogOfWarDecal);
	DEL(eraserMask);
	DEL(drawIndicator);
	DEL(measuringLine);
	DEL(selection);
	if (heldToken) { delete heldToken; }
	DEL(iconMask);
	hoveredButton = nullptr;
	DEL(debugSquare);
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
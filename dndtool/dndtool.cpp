#include "dndtool.h"

bool DnDTool::OnUserUpdate(float fElepsedTime)
{
	if (selectedTile > -1)
	{
		selectionAngle += selectionRotationSpeed;
	}
	DrawScene();
	CheckInput();
	return true;
}	
void DnDTool:: Quit()
{
	for (size_t i = 0; i < icons.size(); i++)
	{
		delete icons[i].first;
		delete icons[i].second;
	}
	for (size_t i = 0; i < backgrounds.size(); i++)
	{
		delete backgrounds[i];
	}
	for (size_t i = 0; i < cursors.size(); i++)
	{
		delete cursors[i];
	}
	delete gridTile;
	delete fogOfWarSprite;
}

int main()
{
	DnDTool dndtool;
	if (dndtool.Construct(dndtool.width, dndtool.height, 2, 2))
	{
		dndtool.Start();
	}
	dndtool.Quit();
	return 0;
}
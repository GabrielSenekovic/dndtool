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
	if (dndtool.Construct(dndtool.width, dndtool.height, 2, 2, true))
	{
		dndtool.Start();
	}
	dndtool.Quit();
	return 0;
}
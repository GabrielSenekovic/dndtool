#include "dndtool.h"

void DnDTool::RenderAll()
{
	DrawDecal({ screens[currentUI].UIoffset.x,screens[currentUI].UIoffset.y }, maps[currentMap].background, { scaleAffectedByUI * zoom.x,scaleAffectedByUI * zoom.y}); //Draws the background

	RenderMap();
	
	//DrawLinks(commonDivisor);
	RenderFog();
	if (screens[currentUI].UIoffset.x > 0)
	{
		screens[currentUI].Render(this);
	}
	RenderWindows();
	RenderCursor();
}
void DnDTool::RenderMap()
{
	float tileableSize = commonDivisor / tileDivisor;
	float amountOfColumns = scaleUnaffectedByUI / commonDivisor;
	int gridWidth = maps[currentMap].Width() * amountOfColumns;
	float adjustedTileWidth = ((maps[currentMap].Width() * scaleAffectedByUI) / gridWidth) / tileableSize;
	float tileWidthRatio = gridTile->sprite->width / adjustedTileWidth;

	float iconToTileRatio = (float)gridTile->sprite->width / (float)Characters[0].icon->sprite->width;

	olc::vf2d scale = { tileableSize / tileWidthRatio, tileableSize / tileWidthRatio };

	if (interactionMode == InteractionMode::MOVE || interactionMode == InteractionMode::MEASURE)
	{
		RenderGrid(amountOfColumns, tileWidthRatio, tileableSize, gridWidth, scale, iconToTileRatio);
	}
	//Draw all tokens
	for (unsigned int i = 0; i < maps[currentMap].characters.size(); i++)
	{
		maps[currentMap].characters[i].Render(this, tileWidthRatio, tileableSize, gridWidth, scale, iconToTileRatio);
	}
}
void DnDTool::RenderGrid(float amountOfColumns, float tileWidthRatio, float tileableSize, int gridWidth, olc::vf2d tileScale, float iconToTileRatio)
{
	for (size_t i = 0; i < (maps[currentMap].Width() * amountOfColumns) * (maps[currentMap].Height() * amountOfColumns); i++)
	{
		int x = i % gridWidth; int y = i / gridWidth;

		bool isThisHighlighted = ((GetMousePositionInXY().x == x && (GetMousePositionInXY().y == y)));
		olc::Pixel tint = isThisHighlighted ? olc::RED : gridColor;

		olc::vf2d position = {(float)x * gridTile->sprite->width * tileScale.x,(float)y * gridTile->sprite->height * tileScale.y };

		if (i == selectedTile)
		{
			float centeringModifier = (gridTile->sprite->width * tileableSize) - ((gridTile->sprite->width * tileableSize)) / 2;
			olc::vf2d scale = tileScale * iconToTileRatio;
			olc::vf2d centerOfRotation = { (float)selection->sprite->width / 2 , (float)selection->sprite->height / 2 };
			olc::vf2d selectionPosition = { (x * gridTile->sprite->width * tileableSize + centeringModifier) / tileWidthRatio, (y * gridTile->sprite->height * tileableSize + centeringModifier) / tileWidthRatio };

			RenderImage(selection, selectionPosition, scale, olc::RED, selectionAngle, centerOfRotation);
		}
		RenderImage(gridTile, position, tileScale, tint);
	}

	olc::vf2d cursorPosition = { GetMouseX() * zoom.x, GetMouseY() * zoom.y };
	DrawDecal(cursorPosition, gridTile, tileScale, olc::BLUE);
}

void DnDTool::RenderLinks(float modifier)
{
	for (unsigned int i = 0; i < maps[currentMap].links.size(); i++)
	{
		bool selection = ((int)((GetMouseX() * tileDivisor) / (modifier * gridTile->sprite->width)) == maps[currentMap].links[i].position.x) && ((int)((GetMouseY() * tileDivisor) / (modifier * gridTile->sprite->height)) == maps[currentMap].links[i].position.y);
		olc::Pixel tint = selection ? olc::RED : olc::WHITE;
		olc::vf2d position = { maps[currentMap].links[i].position.x * gridTile->sprite->width * modifier / tileDivisor, maps[currentMap].links[i].position.y * gridTile->sprite->height * modifier / tileDivisor + gridTile->sprite->height * modifier / tileDivisor * 0.8f };

		DrawStringDecal(position, maps[currentMap].links[i].mapIdentifier, tint, { 1, 1 });
	}
}
void DnDTool::RenderCursor()
{
	bool grabbable = false;
	olc::vf2d scale = { 0.5f,0.5f };
	olc::vf2d position = { (GetMouseX() - 10) * zoom.x, (GetMouseY() - 10) * zoom.y };
	for (size_t i = 0; i < Characters.size(); i++)
	{
		if (Characters[i].position.x == GetMousePositionInXY().x && Characters[i].position.y == GetMousePositionInXY().y)
		{
			grabbable = true;
		}
		if (hoveredButton != nullptr)
		{
			grabbable = true;
		}
	}
	switch (interactionMode)
	{
		case InteractionMode::MOVE:
		{
			if (grabbable && heldToken == nullptr)
			{
				DrawDecal(position, cursors[2], scale);
			}
			else if (heldToken != nullptr)
			{
				float tokenScale = (float)gridTile->sprite->width / (float)heldToken->icon->sprite->width;
				float scaleShrinker = 0.75f; //to make sure the token isnt the same size as the tile, its a bit smaller
				tokenScale *= scaleShrinker;
				DrawDecal({ (float)GetMouseX() - 10, (float)GetMouseY() - 10 }, heldToken->icon, { tokenScale * commonDivisor / tileDivisor, tokenScale * commonDivisor / tileDivisor }, heldToken->tint);
				DrawDecal(position, cursors[1], scale);
			}
			else
			{
				DrawDecal(position, cursors[0], scale);
			}
			break;
		}
		case InteractionMode::DRAW:
		{
			int index = GetMouse(1).bHeld ? 4 : 3;
			DrawDecal(position, cursors[index], scale);
			DrawDecal({GetMouseX() - (float)eraserMask->GetWidth() / 2 , GetMouseY() - (float)eraserMask->GetHeight()/2}, drawIndicator, { 1,1 });
			break;
		}
	}
}
void DnDTool::RenderFog()
{
	delete fogOfWarDecal; fogOfWarDecal = nullptr;
	fogOfWarDecal = new olc::Decal(fogOfWarSprite);
	DrawDecal({ 0,0 }, fogOfWarDecal);
}
void DnDTool::RenderWindows()
{
	switch (currentEvent.first)
	{
		case Event::EVENT_INQUIRY:
		{
			windows[0].Render(this); //The Yes or No window
			break;
		}
	}
}

void DnDTool::RenderImage(olc::Decal* image, olc::vf2d position, olc::vf2d scale, olc::Pixel tint)
{
	//This functions adjusts for UI offset and zoom before finally being sent to the drawing function
	DrawDecal({ position.x * zoom.x + screens[currentUI].UIoffset.x, position.y * zoom.y + screens[currentUI].UIoffset.y }, image, { scale.x * zoom.x, scale.y * zoom.y }, tint);
}
void DnDTool::RenderImage(olc::Decal* image, olc::vf2d position, olc::vf2d scale, olc::Pixel tint, float angle, olc::vf2d center)
{
	//This functions adjusts for UI offset and zoom before finally being sent to the drawing function
	DrawRotatedDecal({ position.x * zoom.x + screens[currentUI].UIoffset.x, position.y * zoom.y + screens[currentUI].UIoffset.y },
		image, angle, center, { scale.x * zoom.x, scale.y * zoom.y }, tint);
}
void DnDTool::RenderText(std::string text, olc::vf2d position, olc::vf2d scale, olc::Pixel tint)
{
	DrawStringDecal({ position.x * zoom.x + screens[currentUI].UIoffset.x, position.y * zoom.y + screens[currentUI].UIoffset.y}, text, tint, { scale.x * zoom.x, scale.y * zoom.y });
}
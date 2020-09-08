#include "dndtool.h"

void DnDTool::RenderAll()
{
	olc::vf2d scaleUIOffset = UIoffset.x == 0 ? olc::vf2d{ 0,0 } : olc::vf2d{ 48 * width / UIBorder->sprite->width, 32 * height / UIBorder->sprite->height };

	scaleAffectedByUI = maps[currentMap].background.sprite->width > maps[currentMap].background.sprite->height ? ((width - scaleUIOffset.x) / (maps[currentMap].background.sprite->width)) : ((height - scaleUIOffset.y) / maps[currentMap].background.sprite->height);

	DrawDecal({ UIoffset.x,UIoffset.y }, &maps[currentMap].background, { scaleAffectedByUI * zoom.x,scaleAffectedByUI * zoom.y });
	float scaledWidth = maps[currentMap].background.sprite->width * scaleAffectedByUI * zoom.x;
	commonDivisor = gnu::findCommonDivisors(maps[currentMap].background.sprite->width * scaleUnaffectedByUI, maps[currentMap].background.sprite->height * scaleUnaffectedByUI)[commonDivisorIndex];
	RenderMap();
	
	//DrawLinks(commonDivisor);
	RenderFog();
	if (UIoffset.x > 0)
	{
		RenderHUD();
	}
	RenderCursor();
}
void DnDTool::RenderMap()
{
	float tileableSize = commonDivisor / tileDivisor;
	float amountOfColumns = scaleUnaffectedByUI / commonDivisor;
	int gridWidth = maps[currentMap].background.sprite->width * amountOfColumns;
	float adjustedTileWidth = ((maps[currentMap].background.sprite->width * scaleAffectedByUI) / gridWidth) / tileableSize;
	float tileWidthRatio = gridTile->sprite->width / adjustedTileWidth;

	float iconToTileRatio = (float)gridTile->sprite->width / (float)NPCs[0].icon->sprite->width;

	olc::vf2d scale = { tileableSize / tileWidthRatio, tileableSize / tileWidthRatio };

	if (interactionMode == InteractionMode::MOVE || interactionMode == InteractionMode::MEASURE)
	{
		RenderGrid(amountOfColumns, tileWidthRatio, tileableSize, gridWidth, scale, iconToTileRatio);
	}
	//Draw all tokens
	for (unsigned int i = 0; i < NPCs.size(); i++)
	{
		NPCs[i].Render(this, tileWidthRatio, tileableSize, gridWidth, scale, iconToTileRatio);
	}
}
void DnDTool::RenderGrid(float amountOfColumns, float tileWidthRatio, float tileableSize, int gridWidth, olc::vf2d tileScale, float iconToTileRatio)
{
	for (size_t i = 0; i < (maps[currentMap].background.sprite->width * amountOfColumns) * (maps[currentMap].background.sprite->height * amountOfColumns); i++)
	{
		int x = i % gridWidth; int y = i / gridWidth;

		bool isThisHighlighted = ((GetMousePositionInXY().x == x && (GetMousePositionInXY().y == y)));
		olc::Pixel tint = isThisHighlighted ? olc::RED : gridColor;

		olc::vf2d position = {(float)x * gridTile->sprite->width * tileScale.x,(float)y * gridTile->sprite->height * tileScale.y };

		if (i == selectedTile)
		{
			float centeringModifier = (gridTile->sprite->width * tileableSize) - ((gridTile->sprite->width * tileableSize)) / 2;
			olc::vf2d scale = scale * iconToTileRatio;
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
	for (size_t i = 0; i < NPCs.size(); i++)
	{
		if (NPCs[i].position.x == GetMousePositionInXY().x && NPCs[i].position.y == GetMousePositionInXY().y)
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
void DnDTool::RenderHUD()
{
	DrawDecal({ 0,0 }, UIBorder, { width / UIBorder->sprite->width, height / UIBorder->sprite->height });
	if (selectedToken.icon != nullptr)
	{
		float ratio = (62.0f / selectedToken.icon->sprite->width) * width / UIBorder->sprite->width;
		DrawDecal({ 3,12 }, selectedToken.icon_unmasked, { ratio,ratio });
		DrawStringDecal({ 3, 64 }, selectedToken.name, olc::WHITE, { 0.5f, 0.5f });
	}
	DrawDecal({ 564, 11.5f }, UIButton, { 14.0f / UIButton->sprite->width * width / UIBorder->sprite->width, 14.0f / UIButton->sprite->width * width / UIBorder->sprite->width });
	DrawStringDecal({ 2,2 },
		"[Grid Coord X: " + std::to_string(GetMousePositionInXY().x) + " Y: " + std::to_string(GetMousePositionInXY().y) + "]" +
		"[F_Grid Coord X: " + std::to_string(GetMousePositionInXYFloat().x) + " Y: " + std::to_string(GetMousePositionInXYFloat().y) + "]" +
		" [Selected tile Index: " + std::to_string(selectedTile) +
		" [Selected tile X: " + std::to_string(selectedTile % (int)(maps[currentMap].background.sprite->width * (scaleUnaffectedByUI / commonDivisor))) + " Y: " + std::to_string((int)(selectedTile / (maps[currentMap].background.sprite->width * (scaleUnaffectedByUI / commonDivisor))))
		, olc::WHITE, { 0.5f, 0.5f });
	DrawStringDecal({ 2,7 }, "[Mouse X: " + std::to_string(GetMouseX()) + " Y: " + std::to_string(GetMouseY()) + "]" +
		" [Zoom:" + std::to_string(zoom.x) + "]"
		, olc::WHITE, { 0.5f, 0.5f });
}
void DnDTool::RenderImage(olc::Decal* image, olc::vf2d position, olc::vf2d scale, olc::Pixel tint)
{
	//This functions adjusts for UI offset and zoom before finally being sent to the drawing function
	DrawDecal({ position.x * zoom.x + UIoffset.x, position.y * zoom.y + UIoffset.y }, image, { scale.x * zoom.x, scale.y * zoom.y }, tint);
}
void DnDTool::RenderImage(olc::Decal* image, olc::vf2d position, olc::vf2d scale, olc::Pixel tint, float angle, olc::vf2d center)
{
	//This functions adjusts for UI offset and zoom before finally being sent to the drawing function
	DrawRotatedDecal({ position.x * zoom.x + UIoffset.x, position.y * zoom.y + UIoffset.y }, 
		image, angle, center, { scale.x * zoom.x, scale.y * zoom.y }, tint);
}
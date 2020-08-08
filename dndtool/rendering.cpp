#include "dndtool.h"

void DnDTool::DrawScene()
{
	olc::vf2d scaleUIOffset = UIoffset.x == 0 ? olc::vf2d{ 0,0 } : olc::vf2d{ 48 * width / UIBorder->sprite->width, 32 * height / UIBorder->sprite->height };

	scaleAffectedByUI = maps[currentMap].background->sprite->width > maps[currentMap].background->sprite->height ? ((width - scaleUIOffset.x) / (maps[currentMap].background->sprite->width)) : ((height - scaleUIOffset.y) / maps[currentMap].background->sprite->height);

	DrawDecal({ UIoffset.x,UIoffset.y }, maps[currentMap].background, { scaleAffectedByUI * zoom.x,scaleAffectedByUI * zoom.y });
	float scaledWidth = maps[currentMap].background->sprite->width * scaleAffectedByUI * zoom.x;
	commonDivisor = gnu::findCommonDivisors(maps[currentMap].background->sprite->width * scaleUnaffectedByUI, maps[currentMap].background->sprite->height * scaleUnaffectedByUI)[commonDivisorIndex];

	float tileableSize = commonDivisor / tileDivisor;
	float amountOfColumns = scaleUnaffectedByUI / commonDivisor;
	int gridWidth = maps[currentMap].background->sprite->width * amountOfColumns;
	float adjustedTileWidth = ((maps[currentMap].background->sprite->width * scaleAffectedByUI) / gridWidth) / tileableSize;
	float tileWidthRatio = gridTile->sprite->width / adjustedTileWidth;
	if (interactionMode == InteractionMode::MOVE || interactionMode == InteractionMode::MEASURE)
	{
		DrawGrid(commonDivisor, scaleUnaffectedByUI, tileWidthRatio, tileableSize, gridWidth);
	}
	DrawTokens(tileWidthRatio);
	//DrawLinks(commonDivisor);
	DisplayFog();
	if (UIoffset.x > 0)
	{
		DisplayHUD();
	}
	DrawCursor(commonDivisor);
}
void DnDTool::DrawGrid(float commonDivisor, float scale, float tileWidthRatio, float tileableSize, int gridWidth)
{
	olc::vf2d tileScale = { tileableSize * zoom.x / tileWidthRatio, tileableSize * zoom.y / tileWidthRatio };

	for (size_t i = 0; i < (maps[currentMap].background->sprite->width * scale / commonDivisor) * (maps[currentMap].background->sprite->height * scale / commonDivisor); i++)
	{
		int x = i % gridWidth;
		int y = i / gridWidth;
		bool isThisHighlighted = ((GetMousePositionInXY().x == x && (GetMousePositionInXY().y == y)));
		olc::Pixel tint = isThisHighlighted ? olc::RED : gridColor;
		olc::vf2d adjustedDimensions = { gridTile->sprite->width * tileableSize * zoom.x / tileWidthRatio , gridTile->sprite->height * tileableSize * zoom.y / tileWidthRatio };
		olc::vf2d position = {
			(float)x * adjustedDimensions.x + UIoffset.x,
			(float)y * adjustedDimensions.y + UIoffset.y };

		if (i == selectedTile)
		{
			float centeringModifier = (gridTile->sprite->width * tileableSize) - ((gridTile->sprite->width * tileableSize)) / 2;
			float iconToTileRatio = (float)gridTile->sprite->width / (float)NPCs[0].icon->sprite->width;
			olc::vf2d scale = { tileableSize * zoom.x * iconToTileRatio / tileWidthRatio, tileableSize * zoom.y * iconToTileRatio / tileWidthRatio };
			olc::vf2d centerOfRotation = { (float)selection->sprite->width / 2 , (float)selection->sprite->height / 2 };
			olc::vf2d selectionPosition = { (x * gridTile->sprite->width * tileableSize + centeringModifier) * zoom.x / tileWidthRatio + UIoffset.x, (y * gridTile->sprite->height * tileableSize + centeringModifier) * zoom.y / tileWidthRatio + UIoffset.y };

			DrawRotatedDecal(selectionPosition, selection, selectionAngle, centerOfRotation, scale, olc::RED);
		}
		DrawDecal(position, gridTile, tileScale, tint);
	}

	olc::vf2d cursorPosition = { GetMouseX() * zoom.x, GetMouseY() * zoom.y };
	DrawDecal(cursorPosition, gridTile, tileScale, olc::BLUE);
}
void DnDTool::DrawTokens(float tileWidthRatio)
{
	for (unsigned int i = 0; i < NPCs.size(); i++)
	{
		float tileableSize = commonDivisor / tileDivisor;
		float iconToTileRatio = (float)gridTile->sprite->width / (float)NPCs[i].icon->sprite->width;
		float scaleShrinker = 0.75f; //to make sure the token isnt the same size as the tile, its a bit smaller
		iconToTileRatio *= scaleShrinker;
		float centeringModifier = ((gridTile->sprite->width * tileableSize) - ((gridTile->sprite->width * tileableSize) * scaleShrinker)) / 2;

		olc::vf2d scale = { tileableSize * zoom.x * iconToTileRatio / tileWidthRatio, tileableSize * zoom.y * iconToTileRatio / tileWidthRatio };
		olc::vf2d position = { (NPCs[i].position.x * gridTile->sprite->width * tileableSize + centeringModifier) * zoom.x / tileWidthRatio + UIoffset.x, (NPCs[i].position.y * gridTile->sprite->height * tileableSize + centeringModifier) * zoom.y / tileWidthRatio + UIoffset.y };

		DrawDecal(position, NPCs[i].icon, scale, NPCs[i].tint);
		float gridWidth = (maps[currentMap].background->sprite->width * (scaleUnaffectedByUI / commonDivisor));
		bool isSelected = selectedTile == (int)(NPCs[i].position.y * gridWidth + NPCs[i].position.x);

		if (GetMousePositionInXY().x == NPCs[i].position.x && GetMousePositionInXY().y == NPCs[i].position.y || isSelected)
		{
			//if this NPC is highlighted
			float lengthOfName = NPCs[i].name.length() * 8;
			olc::Pixel tint = isSelected ? olc::RED : olc::WHITE;
			float yPositionModifier = isSelected ? 24 : 20;
			float lengthToSubtract = ((lengthOfName - gridTile->sprite->width) / 4) + gridTile->sprite->width * tileableSize * 2;
			DrawStringDecal({ position.x - lengthToSubtract * zoom.x, position.y + yPositionModifier * zoom.y / tileWidthRatio }, NPCs[i].name, tint, { 0.5f * zoom.x, 0.5f * zoom.y });
		}
	}
}
void DnDTool::DrawLinks(float modifier)
{
	for (unsigned int i = 0; i < maps[currentMap].links.size(); i++)
	{
		bool selection = ((int)((GetMouseX() * tileDivisor) / (modifier * gridTile->sprite->width)) == maps[currentMap].links[i].position.x) && ((int)((GetMouseY() * tileDivisor) / (modifier * gridTile->sprite->height)) == maps[currentMap].links[i].position.y);
		olc::Pixel tint = selection ? olc::RED : olc::WHITE;
		olc::vf2d position = { maps[currentMap].links[i].position.x * gridTile->sprite->width * modifier / tileDivisor, maps[currentMap].links[i].position.y * gridTile->sprite->height * modifier / tileDivisor + gridTile->sprite->height * modifier / tileDivisor * 0.8f };

		DrawStringDecal(position, maps[currentMap].links[i].mapIdentifier, tint, { 1, 1 });
	}
}
void DnDTool::DrawCursor(float modifier)
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
			DrawDecal({ (float)GetMouseX() - 10, (float)GetMouseY() - 10 }, heldToken->icon, { tokenScale * modifier / tileDivisor, tokenScale * modifier / tileDivisor }, heldToken->tint);
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
void DnDTool::DisplayFog()
{
	delete fogOfWarDecal; fogOfWarDecal = nullptr;
	fogOfWarDecal = new olc::Decal(fogOfWarSprite);
	DrawDecal({ 0,0 }, fogOfWarDecal);
}
void DnDTool::DisplayHUD()
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
		" [Selected tile X: " + std::to_string(selectedTile % (int)(maps[currentMap].background->sprite->width * (scaleUnaffectedByUI / commonDivisor))) + " Y: " + std::to_string((int)(selectedTile / (maps[currentMap].background->sprite->width * (scaleUnaffectedByUI / commonDivisor))))
		, olc::WHITE, { 0.5f, 0.5f });
	DrawStringDecal({ 2,7 }, "[Mouse X: " + std::to_string(GetMouseX()) + " Y: " + std::to_string(GetMouseY()) + "]" +
		" [Zoom:" + std::to_string(zoom.x) + "]"
		, olc::WHITE, { 0.5f, 0.5f });
}
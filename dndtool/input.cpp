#include "dndtool.h"

void DnDTool::CheckInput()
{
	if (GetMouse(0).bPressed)
	{
		olc::vf2d UIscale = { width / screens[currentUI].UIBorder->sprite->width, height / screens[currentUI].UIBorder->sprite->height };
		for (int i = 0; i < screens[currentUI].modeButtons.size(); i++)
		{
			if (GetMouseX() > screens[currentUI].modeButtons[i].position.x * UIscale.x && GetMouseX() < (screens[currentUI].modeButtons[i].position.x + screens[currentUI].modeButtons[i].Width()) * UIscale.x &&
				GetMouseY() > screens[currentUI].modeButtons[i].position.y * UIscale.y && GetMouseY() < (screens[currentUI].modeButtons[i].position.y + screens[currentUI].modeButtons[i].Height()) * UIscale.y)
			{
				screens[currentUI].modeButtons[i].OnPress();
			}
		}
		switch (interactionMode)
		{
			case InteractionMode::MOVE:
			{
				PickUpToken(); break;
			}
			case InteractionMode::MEASURE:
			{
				//deselect by right clicking
				int gridWidth = (maps[currentMap].Width() * (scaleUnaffectedByUI / commonDivisor));
				selectedTile = GetMousePositionInXY().y * gridWidth + GetMousePositionInXY().x;
			}
		}
	}
	if (GetMouse(0).bHeld)
	{
		if (interactionMode == InteractionMode::DRAW)
		{
			EraseDraw(DrawMode::DRAWMODE_DRAW);
		}
	}
	if (GetMouse(1).bPressed)
	{
		switch (interactionMode)
		{
		case InteractionMode::MOVE:
		{
			//Should only happen if youre pressing a token, otherwise it will deselect
			int gridWidth = (maps[currentMap].Width() * (scaleUnaffectedByUI / commonDivisor));
			selectionAngle = selectedTile == GetMousePositionInXY().y * gridWidth + GetMousePositionInXY().x ? selectionAngle : 0;
			selectedTile = GetMousePositionInXY().y * gridWidth + GetMousePositionInXY().x;
			for (int i = 0; i < NPCs.size(); i++)
			{
				if (NPCs[i].position.y * gridWidth + NPCs[i].position.x == selectedTile)
				{
					selectedToken = NPCs[i];
				}
			}
			break;
		}
		case InteractionMode::MEASURE:
		{
			//Deselect measurement if you rightclick
			selectedTile = -1;
			break;
		}
		}
	}
	if (GetMouse(1).bHeld)
	{
		if (interactionMode == InteractionMode::DRAW)
		{
			EraseDraw(DrawMode::DRAWMODE_ERASE);
		}
	}
	if (GetKey(olc::Q).bPressed)
	{
		ToggleUI();
	}
	if (GetKey(olc::A).bPressed)
	{
		gridColor = gridColor == olc::WHITE ? olc::BLACK : olc::WHITE;
	}
	if (GetKey(olc::CTRL).bHeld)
	{
		if (GetKey(olc::Q).bPressed)
		{
			interactionMode = InteractionMode::MOVE;
		}
		if (GetKey(olc::W).bPressed)
		{
			interactionMode = InteractionMode::MEASURE;
		}
		if (GetKey(olc::E).bPressed)
		{
			interactionMode = InteractionMode::DRAW;
		}
		if (GetKey(olc::M).bPressed)
		{
			commonDivisorIndex += 1 * (commonDivisorIndex < 9);
		}
		if (GetKey(olc::N).bPressed)
		{
			commonDivisorIndex -= 1 * (commonDivisorIndex > 1);
		}
		if (GetMouse(0).bPressed && interactionMode == InteractionMode::DRAW)
		{
			FillFog(olc::BLACK);
		}
	}
	if (GetKey(olc::ESCAPE).bPressed)
	{
		playing = false;
	}
	/*if (GetKey(olc::M).bHeld)
	{
		zoom.x += 0.001f;
		zoom.y += 0.001f;
	}
	if (GetKey(olc::N).bHeld && zoom.x > 1)
	{
		zoom.x -= 0.001f;
		zoom.y -= 0.001f;
	}*/
}

void DnDTool::FillFog(olc::Pixel color)
{
	for (size_t i = 0; i < width * height; i++)
	{
		fogOfWarSprite->SetPixel({ (int)i % (int)width, (int)i / (int)width }, color);
	}
}
void DnDTool::EraseDraw(DrawMode mode)
{
	for (size_t i = GetMouseX(); i < GetMouseX() + eraserMask->GetWidth(); i++)
	{
		for (size_t j = GetMouseY(); j < GetMouseY() + eraserMask->GetHeight(); j++)
		{
			Gdiplus::Color c;
			eraserMask->GetPixel(i - GetMouseX(), j - GetMouseY(), &c);
			olc::Pixel pixel;
			switch (mode)
			{
			case DrawMode::DRAWMODE_ERASE:
			{
				pixel = olc::Pixel(
					0,
					0,
					0,
					c.GetA() & fogOfWarSprite->GetPixel(i, j).a); break;
			}
			case DrawMode::DRAWMODE_DRAW:
			{
				pixel = olc::Pixel(
					0,
					0,
					0,
					~c.GetA() | fogOfWarSprite->GetPixel(i, j).a); break;
			}
			}
			fogOfWarSprite->SetPixel(i, j, pixel);
		}
	}
}

void DnDTool::PickUpToken()
{
	olc::vf2d MousePositionInXY = GetMousePositionInXY();
	for (int i = 0; i < NPCs.size(); i++)
	{
		if (NPCs[i].position.x == MousePositionInXY.x && NPCs[i].position.y == MousePositionInXY.y)
		{
			//if there is something here
			if (heldToken == nullptr)
			{
				heldToken = &NPCs[i];
				previousTokenPosition = NPCs[i].position;
				NPCs[i].position = { -1,-1 }; //Instead of making the icon invisible, I move it offscreen
			}
			else
			{
				std::swap(NPCs[i], *heldToken);
				NPCs[i].position = { MousePositionInXY.x,MousePositionInXY.y }; //Instead of making the icon invisible, I move it offscreen
			}
			return;
		}
	}
	//nothing is on this tile
	if (heldToken != nullptr)
	{
		int gridWidth = (maps[currentMap].Width() * (scaleUnaffectedByUI / commonDivisor));
		if (selectedTile == previousTokenPosition.y * gridWidth + previousTokenPosition.x)
		{
			selectedTile = GetMousePositionInXY().y * gridWidth + GetMousePositionInXY().x;
		}
		heldToken->position = { MousePositionInXY.x,MousePositionInXY.y };

		heldToken = nullptr;
	}
}

void DnDTool::ToggleUI()
{
	screens[currentUI].UIoffset = { 70.0f * (screens[currentUI].UIoffset.x == 0) * width / screens[currentUI].UIBorder->sprite->width, 16.0f * (screens[currentUI].UIoffset.y == 0) * height / screens[currentUI].UIBorder->sprite->height };
	screens[currentUI].scaleUIOffset = screens[currentUI].UIoffset.x == 0 ? olc::vf2d{ 0,0 } : olc::vf2d{ 48 * width / screens[currentUI].UIBorder->sprite->width, 32 * height / screens[currentUI].UIBorder->sprite->height };

	scaleAffectedByUI = maps[currentMap].Width() > maps[currentMap].Height() ? //Is the width bigger than the height?
		((width - screens[currentUI].scaleUIOffset.x) / (maps[currentMap].Height())) : //If its wider than tall, then scale based on width
		((height - screens[currentUI].scaleUIOffset.y) / maps[currentMap].Height()); //Otherwise, scale by height 
}
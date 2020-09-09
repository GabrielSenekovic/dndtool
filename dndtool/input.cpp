#include "dndtool.h"

void DnDTool::CheckInput()
{
	olc::vf2d UIscale = { width / screens[currentUI].windows[0]->sprite->width, height / screens[currentUI].windows[0]->sprite->height };
	hoveredButton = nullptr;
	for (int i = 0; i < screens[currentUI].buttons.size(); i++)
	{
		if (GetMouseX() > screens[currentUI].buttons[i].position.x * UIscale.x && GetMouseX() < (screens[currentUI].buttons[i].position.x + screens[currentUI].buttons[i].Width()) * UIscale.x &&
			GetMouseY() > screens[currentUI].buttons[i].position.y * UIscale.y && GetMouseY() < (screens[currentUI].buttons[i].position.y + screens[currentUI].buttons[i].Height()) * UIscale.y)
		{
			hoveredButton = &screens[currentUI].buttons[i];
			break;
		}
	}

	//Go through each active window, and stop as soon as hoveredButton isnt nullptr anymore
	hoveredButton = windows[0].CheckButtonCollision({ (float)GetMouseX(), (float)GetMouseY() }, UIscale);

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
			std::vector<int> commonDivisors = gnu::findCommonDivisors(maps[currentMap].Width(), maps[currentMap].Height());

			int gridWidth = (scaleUnaffectedByUI / commonDivisors[commonDivisorIndex + 1] * maps[currentMap].Width());

			commonDivisorIndex += 1 * (gridWidth >= 8);
			commonDivisor = commonDivisors[commonDivisorIndex];
		}
		if (GetKey(olc::N).bPressed)
		{
			std::vector<int> commonDivisors = gnu::findCommonDivisors(maps[currentMap].Width(), maps[currentMap].Height());

			int gridWidth = (scaleUnaffectedByUI / commonDivisors[commonDivisorIndex - 1] * maps[currentMap].Width());

			commonDivisorIndex -= 1 * (gridWidth <= 50);
			commonDivisor = commonDivisors[commonDivisorIndex];
		}
		if (GetMouse(0).bPressed && interactionMode == InteractionMode::DRAW)
		{
			FillFog(olc::BLACK);
		}
	}
	else
	{
		if (GetMouse(0).bPressed)
		{
			olc::vf2d UIscale = { width / screens[currentUI].windows[0]->sprite->width, height / screens[currentUI].windows[0]->sprite->height };
			if (hoveredButton != nullptr) { hoveredButton->OnPress(); };
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
				for (int i = 0; i < Characters.size(); i++)
				{
					if (Characters[i].position.y * gridWidth + Characters[i].position.x == selectedTile)
					{
						selectedToken = Characters[i];
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

		if (GetKey(olc::ESCAPE).bPressed)
		{
			currentEvent = std::pair<Event, bool*>(Event::EVENT_INQUIRY, &playing);
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
	for (int i = 0; i < Characters.size(); i++)
	{
		if (Characters[i].position.x == MousePositionInXY.x && Characters[i].position.y == MousePositionInXY.y)
		{
			//if there is something here
			if (heldToken == nullptr)
			{
				heldToken = &Characters[i];
				previousTokenPosition = Characters[i].position;
				Characters[i].position = { -1,-1 }; //Instead of making the icon invisible, I move it offscreen
			}
			else
			{
				std::swap(Characters[i], *heldToken);
				Characters[i].position = { MousePositionInXY.x,MousePositionInXY.y }; //Instead of making the icon invisible, I move it offscreen
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
	screens[currentUI].UIoffset = { 70.0f * (screens[currentUI].UIoffset.x == 0) * width / screens[currentUI].windows[0]->sprite->width, 16.0f * (screens[currentUI].UIoffset.y == 0) * height / screens[currentUI].windows[0]->sprite->height };
	screens[currentUI].scaleUIOffset = screens[currentUI].UIoffset.x == 0 ? olc::vf2d{ 0,0 } : olc::vf2d{ 48 * width / screens[currentUI].windows[0]->sprite->width, 32 * height / screens[currentUI].windows[0]->sprite->height };

	scaleAffectedByUI = maps[currentMap].Width() > maps[currentMap].Height() ? //Is the width bigger than the height?
		((width - screens[currentUI].scaleUIOffset.x) / (maps[currentMap].Height())) : //If its wider than tall, then scale based on width
		((height - screens[currentUI].scaleUIOffset.y) / maps[currentMap].Height()); //Otherwise, scale by height 
}
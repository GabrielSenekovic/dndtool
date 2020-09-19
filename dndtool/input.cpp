#include "dndtool.h"

void DnDTool::CheckInput()
{
	olc::vf2d UIscale = { width / screens[currentUI].FrameWidth(), height / screens[currentUI].FrameHeight() };
	
	if (hoveredButton != nullptr)
	{
		TEA(hoveredButton->currentIcon, 1, ? 0 : 2);
		hoveredButton = nullptr;
	}

	if (currentEvent.first == Event::EVENT_INQUIRY)
	{
		hoveredButton = windows[0].CheckButtonCollision({ (float)GetMouseX(), (float)GetMouseY() }, UIscale);
	}
	else
	{
		for (size_t i = 0; i < screens[currentUI].windows.size(); i++)
		{
			button* temp = screens[currentUI].windows[i].CheckButtonCollision({ (float)GetMouseX(), (float)GetMouseY() }, UIscale);
			if (temp != nullptr)
			{
				hoveredButton = temp;
				break;
			}
		}
	}
	if (hoveredButton != nullptr)
	{
		TEA(hoveredButton->currentIcon, 0, ? 1 : 2);
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
			std::vector<int> commonDivisors = gnu::findCommonDivisors(maps[currentMap].Width(), maps[currentMap].Height());

			int gridWidth = (scaleUnaffectedByUI / commonDivisors[maps[currentMap].commonDivisorIndex + 1] * maps[currentMap].Width());

			maps[currentMap].commonDivisorIndex += 1 * (gridWidth >= 8);
			commonDivisor = commonDivisors[maps[currentMap].commonDivisorIndex];
		}
		if (GetKey(olc::N).bPressed)
		{
			std::vector<int> commonDivisors = gnu::findCommonDivisors(maps[currentMap].Width(), maps[currentMap].Height());

			int gridWidth = (scaleUnaffectedByUI / commonDivisors[maps[currentMap].commonDivisorIndex - 1] * maps[currentMap].Width());

			maps[currentMap].commonDivisorIndex -= 1 * (gridWidth <= 50);
			commonDivisor = commonDivisors[maps[currentMap].commonDivisorIndex];
		}
		if (GetMouse(0).bPressed && interactionMode == InteractionMode::DRAW)
		{
			FillFog(olc::BLACK);
		}
		if (GetMouse(1).bPressed && interactionMode == InteractionMode::DRAW)
		{
			FillFog(olc::BLANK);
		}
	}
	else
	{
		if (GetMouse(0).bPressed)
		{
			if (hoveredButton != nullptr) { hoveredButton->OnPress(); return; };

			olc::vf2d UIscale = { width / screens[currentUI].FrameWidth(), height / screens[currentUI].FrameHeight() };
			switch (interactionMode)
			{
				case MOVE:
				{
					PickUpToken(); break;
				}
				case MEASURE:
				{
					//deselect by right clicking
					int gridWidth = (maps[currentMap].Width() * (scaleUnaffectedByUI / commonDivisor));
					selectedTile = GetMousePositionInXY().y * gridWidth + GetMousePositionInXY().x; break;
				}
				case BUILD:
				{
					int gridWidth = (maps[currentMap].Width() * (scaleUnaffectedByUI / commonDivisor));
					olc::vi2d position = GetMousePositionInXY();
					forall(i, 0, maps[currentMap].links.size())
					{
						if (position == maps[currentMap].links[i].position)
						{
							return;
						}
					}
					selectedTile = GetMousePositionInXY().y * gridWidth + GetMousePositionInXY().x;
					screens[0].windows[2].state = window::Window_State::WINDOW_NONE;
					screens[0].windows[2].position = { (float)GetMouseX(),(float) GetMouseY() };
					break;
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
				for (size_t i = 0; i < maps[currentMap].characters.size(); i++)
				{
					if (maps[currentMap].characters[i].position.y* gridWidth + maps[currentMap].characters[i].position.x == selectedTile)
					{
						selectedToken = &maps[currentMap].characters[i];
						screens[0].windows[0].buttons[4].active = true;
						screens[0].windows[0].buttons[5].active = true;
						screens[0].windows[0].buttons[6].active = true;
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
	int x = GetMouseX() - eraserMask->GetWidth() / 2; int y = GetMouseY() - eraserMask->GetHeight() / 2;
	for (size_t i = x; i < x + eraserMask->GetWidth(); i++)
	{
		for (size_t j = y; j < y + eraserMask->GetHeight(); j++)
		{
			Gdiplus::Color c;
			eraserMask->GetPixel(i - x, j - y, &c);
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
	for (size_t i = 0; i < maps[currentMap].characters.size(); i++)
	{
		if (maps[currentMap].characters[i].position.x == MousePositionInXY.x && maps[currentMap].characters[i].position.y == MousePositionInXY.y)
		{
			//if there is something here
			if (heldToken == nullptr)
			{
				heldToken = &maps[currentMap].characters[i];
				previousTokenPosition = maps[currentMap].characters[i].position;
				maps[currentMap].characters[i].position = { -1,-1 }; //Instead of making the icon invisible, I move it offscreen
			}
			else
			{
				std::swap(maps[currentMap].characters[i], *heldToken);
				maps[currentMap].characters[i].position = { MousePositionInXY.x,MousePositionInXY.y }; //Instead of making the icon invisible, I move it offscreen
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
	screens[currentUI].UIoffset = { 70.0f * (screens[currentUI].UIoffset.x == 0) * width / screens[currentUI].FrameWidth(), 16.0f * (screens[currentUI].UIoffset.y == 0) * height / screens[currentUI].FrameHeight() };
	screens[currentUI].scaleUIOffset = screens[currentUI].UIoffset.x == 0 ? olc::vf2d{ 0,0 } : olc::vf2d{ 48 * width / screens[currentUI].FrameWidth(), 32 * height / screens[currentUI].FrameHeight()};

	scaleAffectedByUI = maps[currentMap].Width() > maps[currentMap].Height() ? //Is the width bigger than the height?
		((width - screens[currentUI].scaleUIOffset.x) / (maps[currentMap].Height())) : //If its wider than tall, then scale based on width
		((height - screens[currentUI].scaleUIOffset.y) / maps[currentMap].Height()); //Otherwise, scale by height 
}
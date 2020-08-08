#include "dndtool.h"

void DnDTool::CheckInput()
{
	if (GetMouse(0).bPressed)
	{
		switch (interactionMode)
		{
		case InteractionMode::MOVE:
		{
			PickUpToken(); break;
		}
		case InteractionMode::MEASURE:
		{
			//deselect by right clicking
			int gridWidth = (maps[currentMap].background->sprite->width * (scaleUnaffectedByUI / commonDivisor));
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
			int gridWidth = (maps[currentMap].background->sprite->width * (scaleUnaffectedByUI / commonDivisor));
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
		//ToggleUI();
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
void DnDTool::ToggleUI()
{
	UIoffset = { 70.0f * (UIoffset.x == 0) * width / UIBorder->sprite->width, 16.0f * (UIoffset.y == 0) * height / UIBorder->sprite->height };
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
		int gridWidth = (maps[currentMap].background->sprite->width * (scaleUnaffectedByUI / commonDivisor));
		if (selectedTile == previousTokenPosition.y * gridWidth + previousTokenPosition.x)
		{
			selectedTile = GetMousePositionInXY().y * gridWidth + GetMousePositionInXY().x;
		}
		heldToken->position = { MousePositionInXY.x,MousePositionInXY.y };

		heldToken = nullptr;
	}
}
void DnDTool::FillFog(olc::Pixel color)
{
	for (size_t i = 0; i < width * height; i++)
	{
		fogOfWarSprite->SetPixel({ (int)i % (int)width, (int)i / (int)width }, color);
	}
}
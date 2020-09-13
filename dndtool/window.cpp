#include "dndtool.h"

DnDTool::window::window(std::vector<button> buttons_in, olc::Decal* background_in, olc::vf2d position_in, std::string text_in, olc::vf2d revealedPosition_in): 
buttons(buttons_in), background(background_in), position(position_in), text(text_in), state(Window_State::WINDOW_NONE), 
unrevealedPosition(position_in), revealedPosition(revealedPosition_in){}

void DnDTool::window::Update(float fElapsedTime)
{
	if (state == Window_State::WINDOW_NONE){return;}
	switch (state)
	{
	case Window_State::WINDOW_REVEAL:
		position.x--;
		if (position.x >= revealedPosition.x)
		{
			position.x = revealedPosition.x;
			state = Window_State::WINDOW_NONE;
		}
		break;
	case Window_State::WINDOW_DISAPPEAR:
		position.x++;
		if (position.x <= unrevealedPosition.x)
		{
			position.x = unrevealedPosition.x;
			state = Window_State::WINDOW_NONE;
		}
		break;
	}
}
void DnDTool::window::ToggleReveal()
{
	if (position.x == unrevealedPosition.x)
	{
		state = Window_State::WINDOW_REVEAL;
	}
	else if (position.x == revealedPosition.x)
	{
		state = Window_State::WINDOW_DISAPPEAR;
	}
}
void DnDTool::window::Render(DnDTool* dndTool)
{
	olc::vf2d scale = { dndTool->width / dndTool->screens[dndTool->currentUI].windows[0].background->sprite->width, dndTool->height / dndTool->screens[dndTool->currentUI].windows[0].background->sprite->height };
	dndTool->DrawDecal(position * scale, background, scale);
	for (int i = 0; i < buttons.size(); i++)
	{
		dndTool->DrawDecal(buttons[i].position * scale + position * scale, buttons[i].icons[0], scale);
	}
	dndTool->DrawStringDecal(position * scale + 10 * scale, text, olc::WHITE, scale);
}
DnDTool::button* DnDTool::window::CheckButtonCollision(olc::vf2d mouse, olc::vf2d UIscale)
{
	for (int i = 0; i < buttons.size(); i++)
	{
		if (mouse.x > buttons[i].position.x * UIscale.x + position.x * UIscale.x && mouse.x < (buttons[i].position.x + buttons[i].Width()) * UIscale.x + position.x * UIscale.x &&
			mouse.y > buttons[i].position.y * UIscale.y + position.y * UIscale.y && mouse.y < (buttons[i].position.y + buttons[i].Height()) * UIscale.y + position.y * UIscale.y)
		{
			return &buttons[i];
		}
	}
	return nullptr;
}
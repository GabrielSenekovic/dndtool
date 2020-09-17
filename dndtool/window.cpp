#include "dndtool.h"

DnDTool::window::window(const std::vector<button> buttons_in, olc::Decal* background_in, olc::vf2d position_in, std::string text_in, olc::vf2d revealedPosition_in): 
	buttons(buttons_in), background(background_in), position(position_in), text(text_in), state(Window_State::WINDOW_NONE), 
	unrevealedPosition(position_in), revealedPosition(revealedPosition_in){}
DnDTool::window::window(const std::vector<button> buttons_in, olc::Decal* background_in, olc::vf2d position_in, std::string text_in, olc::vf2d revealedPosition_in, float speed_in) :
	buttons(buttons_in), background(background_in), position(position_in), text(text_in), state(Window_State::WINDOW_NONE),
	unrevealedPosition(position_in), revealedPosition(revealedPosition_in), speed(speed_in) {}

void DnDTool::window::Update(float fElapsedTime)
{
	if (state == Window_State::WINDOW_NONE){return;}
	switch (state)
	{
	case Window_State::WINDOW_REVEAL:
		Move(fElapsedTime, position, revealedPosition, unrevealedPosition);
		break;
	case Window_State::WINDOW_DISAPPEAR:
		Move(fElapsedTime, position, unrevealedPosition, revealedPosition);
		break;
	}
}
void DnDTool::window::Move(float fElapsedTime, olc::vf2d& a, olc::vf2d b, olc::vf2d source)
{
	olc::vf2d distance = b - source;
	a.x += distance.x * fElapsedTime * speed; a.y += distance.y * fElapsedTime * speed;
	if (source.x < b.x && a.x >= b.x)
	{
		a.x = b.x;
		state = Window_State::WINDOW_NONE;
	}
	else if (source.x > b.x && a.x <= b.x)
	{
		a.x = b.x;
		state = Window_State::WINDOW_NONE;
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
	if (background) { dndTool->DrawDecal(position * scale, background, scale); }
	for (size_t i = 0; i < buttons.size(); i++)
	{
		if (buttons[i].active){dndTool->DrawDecal(scale *(position + buttons[i].position), buttons[i].icons[buttons[i].currentIcon], scale * buttons[i].scale);}
	}
	dndTool->DrawStringDecal(position * scale + 10 * scale, text, olc::WHITE, scale);
}
DnDTool::button* DnDTool::window::CheckButtonCollision(olc::vf2d mouse, olc::vf2d UIscale)
{
	for (size_t i = 0; i < buttons.size(); i++)
	{
		if (buttons[i].active && mouse.x > buttons[i].position.x * UIscale.x + position.x * UIscale.x && mouse.x < buttons[i].position.x * UIscale.x + buttons[i].Width() * UIscale.x * buttons[i].scale.x + position.x * UIscale.x &&
			mouse.y > buttons[i].position.y * UIscale.y + position.y * UIscale.y && mouse.y < buttons[i].position.y *UIscale.y + buttons[i].Height() * UIscale.y * buttons[i].scale.y + position.y * UIscale.y)
		{
			return &buttons[i];
		}
	}
	return nullptr;
}
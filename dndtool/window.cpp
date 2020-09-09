#include "dndtool.h"

DnDTool::window::window(std::vector<button> buttons_in, olc::Decal* background_in, olc::vf2d position_in, std::string text_in): buttons(buttons_in), background(background_in), position(position_in), text(text_in)
{
}
void DnDTool::window::Render(DnDTool* dndTool)
{
	olc::vf2d scale = { dndTool->width / dndTool->screens[dndTool->currentUI].windows[0]->sprite->width, dndTool->height / dndTool->screens[dndTool->currentUI].windows[0]->sprite->height };
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
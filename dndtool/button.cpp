#include "dndtool.h"

DnDTool::button::button(olc::Decal* icon_in, olc::vf2d position_in, std::function<void()> function_in): position(position_in), function(function_in)
{
	icons[0] = icon_in;
	//icons[1] is highlighted
	//icons[2] is pressed
}

bool DnDTool::button::OnPress()
{
	function();
	return true;
}
float DnDTool::button::Width()
{
	return icons[0]->sprite->width;
}
float DnDTool::button::Height()
{
	return icons[0]->sprite->height;
}
#include "dndtool.h"

DnDTool::button::button(std::array<olc::Decal*,3> icon_in, olc::vf2d position_in, std::function<void()> function_in): position(position_in), function(function_in)
{
	for (size_t i = 0; i < 3; i++)
	{
		icons[i] = icon_in[i];
	}
}
DnDTool::button::button(olc::Decal* icon_in, olc::vf2d position_in, olc::vf2d scale_in, std::function<void()> function_in) : position(position_in), function(function_in), scale(scale_in)
{
	for (size_t i = 0; i < 3; i++)
	{
		icons[i] = icon_in;
	}
}

bool DnDTool::button::OnPress()
{
	function();
	return true;
}
float DnDTool::button::Width()
{
	return (float)icons[0]->sprite->width;
}
float DnDTool::button::Height()
{
	return (float)icons[0]->sprite->height;
}
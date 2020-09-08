#include "dndtool.h"

DnDTool::button::button(olc::Decal* icon_in, olc::vf2d position_in): icon(icon_in), position(position_in){}

bool DnDTool::button::OnPress()
{
	return true;
}
float DnDTool::button::Width()
{
	return icon->sprite->width;
}
float DnDTool::button::Height()
{
	return icon->sprite->height;
}
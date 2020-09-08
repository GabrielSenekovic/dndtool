#include "dndtool.h"

DnDTool::map::map(std::string mapIdentifier_in, olc::Decal* background_in, int tileScale_in)
{
	mapIdentifier = mapIdentifier_in;
	links = {};
	background = background_in;
	tileScale = tileScale_in;
}
DnDTool::map::map(std::string mapIdentifier_in, olc::Decal* background_in, int tileScale_in, int commonDivisor_in)
{
	mapIdentifier = mapIdentifier_in;
	links = {};
	background = background_in;
	tileScale = tileScale_in;
	commonDivisorIndex = commonDivisor_in;
}
DnDTool::map::map(std::string mapIdentifier_in, olc::Decal* background_in, std::vector<link> links_in, int tileScale_in)
{
	mapIdentifier = mapIdentifier_in;
	links = links_in;
	background = background_in;
	tileScale = tileScale_in;
}
DnDTool::map::map(std::string mapIdentifier_in, olc::Decal* background_in, std::vector<link> links_in, int tileScale_in, int commonDivisor_in)
{
	mapIdentifier = mapIdentifier_in;
	links = links_in;
	background = background_in;
	tileScale = tileScale_in;
	commonDivisorIndex = commonDivisor_in;
}
float DnDTool::map::Width()
{
	return background->sprite->width;
}
float DnDTool::map::Height()
{
	return background->sprite->height;
}
#include "dndtool.h"

DnDTool::map::map(std::string mapIdentifier_in, int background_index_in, int tileScale_in):background_index(background_index_in)
{
	mapIdentifier = mapIdentifier_in;
	links = {};
	background = new olc::Decal(new olc::Sprite("./Assets/Backgrounds/" + std::to_string(background_index) + ".png"));
	tileScale = tileScale_in;
}
DnDTool::map::map(std::string mapIdentifier_in, int background_index_in, int tileScale_in, int commonDivisor_in) :background_index(background_index_in)
{
	mapIdentifier = mapIdentifier_in;
	links = {};
	background = new olc::Decal(new olc::Sprite("./Assets/Backgrounds/" + std::to_string(background_index) + ".png"));
	tileScale = tileScale_in;
	commonDivisorIndex = commonDivisor_in;
}
DnDTool::map::map(std::string mapIdentifier_in, int background_index_in, std::vector<link> links_in, int tileScale_in) :background_index(background_index_in)
{
	mapIdentifier = mapIdentifier_in;
	links = links_in;
	background = new olc::Decal(new olc::Sprite("./Assets/Backgrounds/" + std::to_string(background_index) + ".png"));
	tileScale = tileScale_in;
}
DnDTool::map::map(std::string mapIdentifier_in, int background_index_in, std::vector<link> links_in, int tileScale_in, int commonDivisor_in) :background_index(background_index_in)
{
	mapIdentifier = mapIdentifier_in;
	links = links_in;
	background = new olc::Decal(new olc::Sprite("./Assets/Backgrounds/" + std::to_string(background_index) + ".png"));
	tileScale = tileScale_in;
	commonDivisorIndex = commonDivisor_in;
}
DnDTool::map::~map()
{
	delete background;
}
float DnDTool::map::Width()
{
	return background->sprite->width;
}
float DnDTool::map::Height()
{
	return background->sprite->height;
}
bool DnDTool::map::Save(std::string path, const DnDTool::map map)
{
	FILE* file;
	fopen_s(&file, (path + map.mapIdentifier + ".txt").c_str(), "wb");
	if (!file) return false;
	std::vector<uint8_t> fileData = {};
	uint8_t bytesWritten = 0;

	memcpy(fileData.data() + bytesWritten, map.mapIdentifier.c_str(), map.mapIdentifier.size());
	bytesWritten += map.mapIdentifier.size() + 1;
	memcpy(fileData.data() + bytesWritten, &map.background_index, sizeof(int));
	bytesWritten += sizeof(int);
	memcpy(fileData.data() + bytesWritten, &map.commonDivisorIndex, sizeof(int));
	bytesWritten += sizeof(int);

	fwrite(fileData.data(), 1, bytesWritten, file);
	fclose(file);
	return true;
}
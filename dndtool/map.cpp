#include "dndtool.h"

DnDTool::map::map(std::string mapIdentifier_in, int background_index_in, std::vector<link> links_in) :background_index(background_index_in)
{
	mapIdentifier = mapIdentifier_in;
	links = links_in;
	background = new olc::Decal(new olc::Sprite("./Assets/Backgrounds/" + std::to_string(background_index) + ".png"));
}
DnDTool::map::map(std::string mapIdentifier_in, int background_index_in, std::vector<link> links_in, int commonDivisor_in) :background_index(background_index_in)
{
	mapIdentifier = mapIdentifier_in;
	links = links_in;
	background = new olc::Decal(new olc::Sprite("./Assets/Backgrounds/" + std::to_string(background_index) + ".png"));
	commonDivisorIndex = commonDivisor_in;
}
DnDTool::map::map(std::string mapIdentifier_in, int background_index_in, std::vector<token> tokens_in, int commonDivisor_in) :background_index(background_index_in)
{
	mapIdentifier = mapIdentifier_in;
	characters = tokens_in;
	background = new olc::Decal(new olc::Sprite("./Assets/Backgrounds/" + std::to_string(background_index) + ".png"));
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
bool DnDTool::map::Save(std::string path, const DnDTool::map map)
{
	FILE* file;
	fopen_s(&file, (path + map.mapIdentifier + ".txt").c_str(), "wb");
	if (!file) return false;
	std::vector<uint8_t> fileData = {};
	uint8_t bytesWritten = 0;

	fileData.resize(map.mapIdentifier.size() + 1 + sizeof(int)*2);

	memcpy(fileData.data() + bytesWritten, map.mapIdentifier.data(), map.mapIdentifier.size());
	bytesWritten += map.mapIdentifier.size() + 1;
	memcpy(fileData.data() + bytesWritten, &map.background_index, sizeof(int));
	bytesWritten += sizeof(int);
	memcpy(fileData.data() + bytesWritten, &map.commonDivisorIndex, sizeof(int));
	bytesWritten += sizeof(int);

	for (DnDTool::token token : map.characters)
	{
		token.Save(file, fileData, bytesWritten);
	}

	fwrite(fileData.data(), 1, bytesWritten, file);
	fclose(file);
	return true;
}
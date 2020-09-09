#include "dndtool.h"
#include <fstream>

bool DnDTool::SaveCharacters()
{
	FILE* file;
	fopen_s(&file, "./Assets/SaveData/tokens.txt", "wb");
	if (!file) return false;
	std::vector<uint8_t> fileData = {};
	uint8_t bytesWritten = 0;

	for (size_t i = 0; i < Characters.size(); i++)
	{
		fileData.resize(fileData.size() + Characters[i].name.size() + 1 + sizeof(Characters[i].icon_index));

		memcpy(fileData.data() + bytesWritten, Characters[i].name.data(), Characters[i].name.size());
		bytesWritten += Characters[i].name.size() + 1;
		memcpy(fileData.data() + bytesWritten, &Characters[i].icon_index, sizeof(int));
		bytesWritten += sizeof(int);
	}

	fwrite(fileData.data(), 1, bytesWritten, file);
	fclose(file);
	return true;
}
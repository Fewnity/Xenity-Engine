#pragma once

class File;

enum FileType
{
	File_Other,
	File_Audio,
	File_Mesh,
	File_Texture,
	File_Scene,
};

class FileReference
{
public:
	int fileId = 0;
	File* file = nullptr;
	FileType fileType = File_Other;
};


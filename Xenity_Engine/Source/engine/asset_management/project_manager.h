#pragma once

#include <unordered_map>

class FileReference;
class File;

class ProjectManager
{
public:
	static void LoadProject();
	static std::unordered_map<int, FileReference*> projectFilesRef;
	static FileReference* GetFileReferenceById(int id);

private:
	struct PairFile {
		File* file;
		File* meta;
	};
};


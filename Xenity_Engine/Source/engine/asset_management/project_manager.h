#pragma once

#include <unordered_map>

class FileReference;
class File;

class ProjectManager
{
public:
	static void LoadProject();
	static std::unordered_map<uint64_t, FileReference*> projectFilesRef;
	static FileReference* GetFileReferenceById(uint64_t id);
	static void SaveMetaFile(FileReference* fileReference);

private:
	static void LoadMetaFile(FileReference* fileReference);
	struct PairFile {
		File* file;
		File* meta;
	};
};


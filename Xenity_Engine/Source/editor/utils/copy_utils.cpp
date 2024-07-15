#include "copy_utils.h"
#include <filesystem>
#include <engine/debug/debug.h>
#include <engine/assertions/assertions.h>

namespace fs = std::filesystem;

std::vector<CopyEntry> CopyUtils::copyEntries;

bool CopyUtils::ExecuteCopyEntries()
{
	bool success = true;
	for (const CopyEntry& entry : copyEntries)
	{
		try
		{
			if (entry.isFolder)
			{
				fs::copy(
					entry.sourcePath,
					entry.destPath,
					fs::copy_options::overwrite_existing | fs::copy_options::recursive
				);
			}
			else
			{
				fs::copy_file(
					entry.sourcePath,
					entry.destPath,
					fs::copy_options::overwrite_existing
				);
			}
		}
		catch (const std::exception&)
		{
			Debug::PrintError("[Compiler::ExecuteCopyEntries] Cannot copy " + entry.sourcePath + " to " + entry.destPath, true);
			success = false;
		}
	}
	copyEntries.clear();
	return success;
}

void CopyUtils::AddCopyEntry(bool isFolder, const std::string& source, const std::string& dest)
{
	XASSERT(!source.empty(), "[Compiler::AddCopyEntry] source is empty");
	XASSERT(!dest.empty(), "[Compiler::AddCopyEntry] dest is empty");

	if (source.empty() || dest.empty())
	{
		return;
	}

	CopyEntry entry;
	entry.isFolder = isFolder;
	entry.sourcePath = source;
	entry.destPath = dest;

	copyEntries.push_back(entry);
}

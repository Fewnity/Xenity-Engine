#pragma once

enum class ProjectLoadingErrors
{
	Success = 0,
	NoAssetFolder = 1,
	NoStartupScene = 2,
	FailedToOpenDataFile = 3,
	FailedToOpenDataBaseFile = 4,
};
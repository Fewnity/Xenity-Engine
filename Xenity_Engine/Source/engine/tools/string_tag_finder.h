#pragma once

#include <string>

class StringTagFinder
{
public:
	static bool FindTag(const std::string& textToSearchIn, const int index, const int textSize, const std::string& textToFind, int& startPosition, int& endPosition);
};


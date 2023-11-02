#pragma once

#include <string>

class StringTagFinder
{
public:
	static bool FindTag(const std::string& textToSearchIn, const size_t index, const size_t textSize, const std::string& textToFind, int& startPosition, int& endPosition);
};


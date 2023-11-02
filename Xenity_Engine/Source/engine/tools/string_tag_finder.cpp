#include "string_tag_finder.h"

bool StringTagFinder::FindTag(const std::string& textToSearchIn, const size_t index, const size_t textSize, const std::string& textToFind, int& startPosition, int& endPosition)
{
	bool found = false;
	int textToFindSize = textToFind.size();
	bool notEquals = false;

	for (int i = 0; i < textToFindSize; i++)
	{
		if (textToSearchIn[index + i] != textToFind[i])
		{
			notEquals = true;
			break;
		}
	}

	if (!notEquals)
	{
		startPosition = index;
		for (int j = index + 1; j < textSize; j++)
		{
			if (textToSearchIn[j] == '}')
			{
				endPosition = j + 2;
				found = true;
				break;
			}
		}
	}
	return found;
}
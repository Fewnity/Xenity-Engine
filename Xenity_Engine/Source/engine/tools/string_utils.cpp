// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "string_utils.h"

bool StringUtils::FindTag(const std::string& textToSearchIn, const size_t index, const size_t textSize, const std::string& textToFind, int& startPosition, int& endPosition)
{
	bool found = false;
	const int textToFindSize = (int)textToFind.size();
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

std::vector<std::string> StringUtils::Split(const std::string& text, const char delimiter)
{
	std::vector<std::string> result;

	size_t offset = 0;
	size_t delimiterLocation = text.find(delimiter, offset);
	while (delimiterLocation != -1)
	{
		const std::string sub = text.substr(offset, delimiterLocation - offset);
		if (sub.size() > 0)
		{
			result.push_back(sub);
		}
		offset = delimiterLocation + 1;
		delimiterLocation = text.find(delimiter, offset);
	}

	// Add the last part of the string if there is one
	const std::string sub = text.substr(offset, delimiterLocation - offset);
	if (sub.size() > 0)
	{
		result.push_back(sub);
	}

	return result;
}

std::string StringUtils::ToLower(const std::string& text)
{
	std::string result = text;

	size_t size = result.size();
	for (size_t i = 0; i < size; i++)
	{
		result[i] = std::tolower(result[i]);
	}

	return result;
}

std::string StringUtils::ToUpper(const std::string& text)
{
	std::string result = text;

	size_t size = result.size();
	for (size_t i = 0; i < size; i++)
	{
		result[i] = std::toupper(result[i]);
	}

	return result;
}

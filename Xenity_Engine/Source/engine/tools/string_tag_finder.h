// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <string>

class StringTagFinder
{
public:
	/**
	* @brief Find the position of a tag in a string
	* @param textToSearchIn The text to search in
	* @param index The char index to check in the text
	* @param Size of the text to search in
	* @param textToFind The tag to find
	* @param startPosition The position of the start of the tag (if found)
	* @param endPosition The position of the end of the tag (if found)
	* @return True if the tag was found, false otherwise
	*/
	static bool FindTag(const std::string& textToSearchIn, const size_t index, const size_t textSize, const std::string& textToFind, int& startPosition, int& endPosition);
};


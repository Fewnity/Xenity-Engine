// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#pragma once

#include <string>
#include <vector>

class StringUtils
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
	[[nodiscard]] static bool FindTag(const std::string& textToSearchIn, const size_t index, const size_t textSize, const std::string& textToFind, size_t& startPosition, size_t& endPosition);

	/**
	* @brief Split a string into a vector of strings
	* @param text The text to split
	* @param delimiter The delimiter to split the text with
	*/
	[[nodiscard]] static std::vector<std::string> Split(const std::string& text, const char delimiter);

	/**
	* @brief Take a string and convert it to lowercase
	*/
	[[nodiscard]] static std::string ToLower(const std::string& text);

	/**
	* @brief Take a string and convert it to uppercase
	*/
	[[nodiscard]] static std::string ToUpper(const std::string& text);
};


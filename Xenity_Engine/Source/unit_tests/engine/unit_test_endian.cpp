// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "../unit_test_manager.h"

#include <engine/debug/debug.h>
#include <engine/tools/endian_utils.h>

#include <iostream> 

TestResult EndianCheckTest::Start(std::string& errorOut)
{
	BEGIN_TEST();

	const bool isBigEndian = EndianUtils::IsBigEndian();
#if defined(__PSP__)
	EXPECT_EQUALS(isBigEndian, false, "Big Endian Check should return false on PSP");
#elif defined(__vita__)
	EXPECT_EQUALS(isBigEndian, false, "Big Endian Check should return false on PsVita");
#elif defined(__PS3__)
	EXPECT_EQUALS(isBigEndian, true, "Big Endian Check should return true on PS3");
#endif

	END_TEST();
}

TestResult EndianSwapTest::Start(std::string& errorOut)
{
	BEGIN_TEST();

	const uint8_t before8 = 0b00100000;
	const uint8_t after8 = 0b00100000;
	EXPECT_EQUALS(EndianUtils::SwapEndian(before8), after8, "Endian Swap failed for 8 bits");

	const uint16_t before16 = 0b0010000000000001;
	const uint16_t after16 =  0b0000000100100000;
	EXPECT_EQUALS(EndianUtils::SwapEndian(before16), after16, "Endian Swap failed for 16 bits");

	const uint32_t before32 = 0xAABBCCDD;
	const uint32_t after32 = 0xDDCCBBAA;
	EXPECT_EQUALS(EndianUtils::SwapEndian(before32), after32, "Endian Swap failed for 32 bits");

	const uint64_t before64 = 0xAABBCCDD00112233;
	const uint64_t after64 = 0x33221100DDCCBBAA;
	EXPECT_EQUALS(EndianUtils::SwapEndian(before64), after64, "Endian Swap failed for 64 bits");

	END_TEST();
}
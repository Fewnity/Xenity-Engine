// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "../unit_test_manager.h"

#include <engine/debug/debug.h>
#include <engine/graphics/color/color.h>

TestResult ColorConstructorTest::Start(std::string& errorOut)
{
	BEGIN_TEST();

	Color color;
	EXPECT_EQUALS(color.GetUnsignedIntABGR(), 0xffffffff, "Bad Color constructor (GetUnsignedIntABGR)");
	EXPECT_EQUALS(color.GetUnsignedIntRGBA(), 0xffffffff, "Bad Color constructor (GetUnsignedIntRGBA)");
	EXPECT_EQUALS(color.GetUnsignedIntARGB(), 0xffffffff, "Bad Color constructor (GetUnsignedIntARGB)");

	const RGBA& rgbaRef = color.GetRGBA();
	EXPECT_NEAR(rgbaRef.r, 1.0f, "Bad Color RGBA constructor (red)");
	EXPECT_NEAR(rgbaRef.g, 1.0f, "Bad Color RGBA constructor (green)");
	EXPECT_NEAR(rgbaRef.b, 1.0f, "Bad Color RGBA constructor (blue)");
	EXPECT_NEAR(rgbaRef.a, 1.0f, "Bad Color RGBA constructor (alpha)");

	END_TEST();
}

TestResult ColorSetTest::Start(std::string& errorOut)
{
	BEGIN_TEST();

	const int r = 50;
	const int g = 101;
	const int b = 5;
	const int a = 200;

	const float rFloat = r / 255.0f;
	const float gFloat = g / 255.0f;
	const float bFloat = b / 255.0f;
	const float aFloat = a / 255.0f;

	Color color = Color::CreateFromRGBA(r, g, b, a);
	EXPECT_EQUALS(color.GetUnsignedIntABGR(), 0xc8056532, "Bad CreateFromRGBA GetUnsignedIntABGR");
	EXPECT_EQUALS(color.GetUnsignedIntRGBA(), 0x326505c8, "Bad CreateFromRGBA GetUnsignedIntRGBA");
	EXPECT_EQUALS(color.GetUnsignedIntARGB(), 0xc8326505, "Bad CreateFromRGBA GetUnsignedIntARGB");

	EXPECT_NEAR(color.GetRGBA().r, rFloat, "Bad CreateFromRGBA GetRGBA (red)");
	EXPECT_NEAR(color.GetRGBA().g, gFloat, "Bad CreateFromRGBA GetRGBA (green)");
	EXPECT_NEAR(color.GetRGBA().b, bFloat, "Bad CreateFromRGBA GetRGBA (blue)");
	EXPECT_NEAR(color.GetRGBA().a, aFloat, "Bad CreateFromRGBA GetRGBA (alpha)");

	EXPECT_EQUALS(color.GetRGBA().ToVector4(), Vector4(rFloat, gFloat, bFloat, aFloat), "Bad CreateFromRGBA GetRGBA ToVector4");

	Color colorFloat = Color::CreateFromRGBAFloat(rFloat, gFloat, bFloat, aFloat);
	EXPECT_EQUALS(colorFloat.GetUnsignedIntABGR(), 0xc8056532, "Bad CreateFromRGBAFloat GetUnsignedIntABGR");
	EXPECT_EQUALS(colorFloat.GetUnsignedIntRGBA(), 0x326505c8, "Bad CreateFromRGBAFloat GetUnsignedIntRGBA");
	EXPECT_EQUALS(colorFloat.GetUnsignedIntARGB(), 0xc8326505, "Bad CreateFromRGBAFloat GetUnsignedIntARGB");

	EXPECT_NEAR(colorFloat.GetRGBA().r, rFloat, "Bad CreateFromRGBAFloat GetRGBA (red)");
	EXPECT_NEAR(colorFloat.GetRGBA().g, gFloat, "Bad CreateFromRGBAFloat GetRGBA (green)");
	EXPECT_NEAR(colorFloat.GetRGBA().b, bFloat, "Bad CreateFromRGBAFloat GetRGBA (blue)");
	EXPECT_NEAR(colorFloat.GetRGBA().a, aFloat, "Bad CreateFromRGBAFloat GetRGBA (alpha)");

	EXPECT_EQUALS(colorFloat.GetRGBA().ToVector4(), Vector4(rFloat, gFloat, bFloat, aFloat), "Bad CreateFromRGBAFloat GetRGBA ToVector4");

	END_TEST();
}
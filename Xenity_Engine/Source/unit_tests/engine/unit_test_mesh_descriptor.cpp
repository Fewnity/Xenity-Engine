// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "../unit_test_manager.h"

#include <engine/graphics/3d_graphics/vertex_descriptor.h>

TestResult VertexDescriptorFloatTest::Start(std::string& errorOut)
{
	BEGIN_TEST();

	// PSP uses unsigned int for color, it's automatically converted
	int colorBytesCount = sizeof(float[4]);
#if defined(__PSP__)
	colorBytesCount = sizeof(unsigned int);
#endif

	VertexDescriptor vertexDescriptor;

	// Check initial values
	EXPECT_EQUALS(vertexDescriptor.GetPositionIndex(), -1, "Bad intial position index");
	EXPECT_EQUALS(vertexDescriptor.GetNormalIndex(), -1, "Bad intial normal index");
	EXPECT_EQUALS(vertexDescriptor.GetUvIndex(), -1, "Bad intial uv index");
	EXPECT_EQUALS(vertexDescriptor.GetColorIndex(), -1, "Bad intial color index");

	EXPECT_EQUALS(vertexDescriptor.GetVertexSize(), 0, "Bad intial vertex size");
	EXPECT_EQUALS(vertexDescriptor.GetVertexElementList().size(), 0, "Bad intial vertex element count");

	vertexDescriptor.AddVertexElement(VertexElement::POSITION_32_BITS);

	EXPECT_EQUALS(vertexDescriptor.GetPositionIndex(), 0, "Bad position index");
	EXPECT_EQUALS(vertexDescriptor.GetNormalIndex(), -1, "Bad normal index");
	EXPECT_EQUALS(vertexDescriptor.GetUvIndex(), -1, "Bad uv index");
	EXPECT_EQUALS(vertexDescriptor.GetColorIndex(), -1, "Bad color index");

	EXPECT_EQUALS(vertexDescriptor.GetVertexSize(), sizeof(float[3]), "Bad vertex size");
	EXPECT_EQUALS(vertexDescriptor.GetVertexElementList().size(), 1, "Bad vertex element count");

	EXPECT_EQUALS(vertexDescriptor.GetPositionOffset(), 0, "Bad position offset");

	vertexDescriptor.AddVertexElement(VertexElement::COLOR_4_FLOATS);

	EXPECT_EQUALS(vertexDescriptor.GetPositionIndex(), 0, "Bad position index");
	EXPECT_EQUALS(vertexDescriptor.GetNormalIndex(), -1, "Bad normal index");
	EXPECT_EQUALS(vertexDescriptor.GetUvIndex(), -1, "Bad uv index");
	EXPECT_EQUALS(vertexDescriptor.GetColorIndex(), 1, "Bad color index");

	EXPECT_EQUALS(vertexDescriptor.GetVertexSize(), sizeof(float[3]) + colorBytesCount, "Bad vertex size");
	EXPECT_EQUALS(vertexDescriptor.GetVertexElementList().size(), 2, "Bad vertex element count");

	EXPECT_EQUALS(vertexDescriptor.GetPositionOffset(), 0, "Bad position offset");
	EXPECT_EQUALS(vertexDescriptor.GetColorOffset(), sizeof(float[3]), "Bad color offset");

	vertexDescriptor.AddVertexElement(VertexElement::NORMAL_32_BITS);

	EXPECT_EQUALS(vertexDescriptor.GetPositionIndex(), 0, "Bad position index");
	EXPECT_EQUALS(vertexDescriptor.GetNormalIndex(), 2, "Bad normal index");
	EXPECT_EQUALS(vertexDescriptor.GetUvIndex(), -1, "Bad uv index");
	EXPECT_EQUALS(vertexDescriptor.GetColorIndex(), 1, "Bad color index");

	EXPECT_EQUALS(vertexDescriptor.GetVertexSize(), sizeof(float[3]) + colorBytesCount + sizeof(float[3]), "Bad vertex size");
	EXPECT_EQUALS(vertexDescriptor.GetVertexElementList().size(), 3, "Bad vertex element count");

	EXPECT_EQUALS(vertexDescriptor.GetPositionOffset(), 0, "Bad position offset");
	EXPECT_EQUALS(vertexDescriptor.GetColorOffset(), sizeof(float[3]), "Bad color offset");
	EXPECT_EQUALS(vertexDescriptor.GetNormalOffset(), sizeof(float[3]) + colorBytesCount, "Bad normal offset");

	vertexDescriptor.AddVertexElement(VertexElement::UV_32_BITS);

	EXPECT_EQUALS(vertexDescriptor.GetPositionIndex(), 0, "Bad position index");
	EXPECT_EQUALS(vertexDescriptor.GetNormalIndex(), 2, "Bad normal index");
	EXPECT_EQUALS(vertexDescriptor.GetUvIndex(), 3, "Bad uv index");
	EXPECT_EQUALS(vertexDescriptor.GetColorIndex(), 1, "Bad color index");

	EXPECT_EQUALS(vertexDescriptor.GetVertexSize(), sizeof(float[3]) + colorBytesCount + sizeof(float[3]) + sizeof(float[2]), "Bad vertex size");
	EXPECT_EQUALS(vertexDescriptor.GetVertexElementList().size(), 4, "Bad vertex element count");

	EXPECT_EQUALS(vertexDescriptor.GetPositionOffset(), 0, "Bad position offset");
	EXPECT_EQUALS(vertexDescriptor.GetColorOffset(), sizeof(float[3]), "Bad color offset");
	EXPECT_EQUALS(vertexDescriptor.GetNormalOffset(), sizeof(float[3]) + colorBytesCount, "Bad normal offset");
	EXPECT_EQUALS(vertexDescriptor.GetUvOffset(), sizeof(float[3]) + colorBytesCount + sizeof(float[3]), "Bad normal offset");

	END_TEST();
}

TestResult VertexDescriptor16BitsTest::Start(std::string& errorOut)
{
	BEGIN_TEST();

	VertexDescriptor vertexDescriptor;

	vertexDescriptor.AddVertexElement(VertexElement::POSITION_16_BITS);

	EXPECT_EQUALS(vertexDescriptor.GetVertexSize(), sizeof(int16_t[3]), "Bad vertex size");
	EXPECT_EQUALS(vertexDescriptor.GetVertexElementList().size(), 1, "Bad vertex element count");

	vertexDescriptor.AddVertexElement(VertexElement::COLOR_32_BITS_UINT); // Not 16 bits color

	EXPECT_EQUALS(vertexDescriptor.GetVertexSize(), sizeof(int16_t[3]) + sizeof(unsigned int), "Bad vertex size");
	EXPECT_EQUALS(vertexDescriptor.GetVertexElementList().size(), 2, "Bad vertex element count");

	vertexDescriptor.AddVertexElement(VertexElement::NORMAL_16_BITS);

	EXPECT_EQUALS(vertexDescriptor.GetVertexSize(), sizeof(int16_t[3]) + sizeof(unsigned int) + sizeof(int16_t[3]), "Bad vertex size");
	EXPECT_EQUALS(vertexDescriptor.GetVertexElementList().size(), 3, "Bad vertex element count");

	vertexDescriptor.AddVertexElement(VertexElement::UV_16_BITS);

	EXPECT_EQUALS(vertexDescriptor.GetVertexSize(), sizeof(int16_t[3]) + sizeof(unsigned int) + sizeof(int16_t[3]) + sizeof(int16_t[2]), "Bad vertex size");
	EXPECT_EQUALS(vertexDescriptor.GetVertexElementList().size(), 4, "Bad vertex element count");

	END_TEST();
}

TestResult VertexDescriptor8BitsTest::Start(std::string& errorOut)
{
	BEGIN_TEST();

	VertexDescriptor vertexDescriptor;

	vertexDescriptor.AddVertexElement(VertexElement::POSITION_8_BITS);

	EXPECT_EQUALS(vertexDescriptor.GetVertexSize(), sizeof(int8_t[3]), "Bad vertex size");
	EXPECT_EQUALS(vertexDescriptor.GetVertexElementList().size(), 1, "Bad vertex element count");

	vertexDescriptor.AddVertexElement(VertexElement::COLOR_32_BITS_UINT); // Not 16 bits color

	EXPECT_EQUALS(vertexDescriptor.GetVertexSize(), sizeof(int8_t[3]) + sizeof(unsigned int), "Bad vertex size");
	EXPECT_EQUALS(vertexDescriptor.GetVertexElementList().size(), 2, "Bad vertex element count");

	vertexDescriptor.AddVertexElement(VertexElement::NORMAL_8_BITS);

	EXPECT_EQUALS(vertexDescriptor.GetVertexSize(), sizeof(int8_t[3]) + sizeof(unsigned int) + sizeof(int8_t[3]), "Bad vertex size");
	EXPECT_EQUALS(vertexDescriptor.GetVertexElementList().size(), 3, "Bad vertex element count");

	vertexDescriptor.AddVertexElement(VertexElement::UV_8_BITS);

	EXPECT_EQUALS(vertexDescriptor.GetVertexSize(), sizeof(int8_t[3]) + sizeof(unsigned int) + sizeof(int8_t[3]) + sizeof(int8_t[2]), "Bad vertex size");
	EXPECT_EQUALS(vertexDescriptor.GetVertexElementList().size(), 4, "Bad vertex element count");

	END_TEST();
}

TestResult VertexDescriptorWrongTest::Start(std::string& errorOut)
{
	BEGIN_TEST();

	int colorBytesCount = sizeof(float[4]);
	// PSP uses unsigned int for color, it's automatically converted
#if defined(__PSP__)
	colorBytesCount = sizeof(unsigned int);
#endif

	VertexDescriptor vertexDescriptor;

	vertexDescriptor.AddVertexElement(VertexElement::NONE);

	EXPECT_EQUALS(vertexDescriptor.GetPositionIndex(), -1, "Bad position index");
	EXPECT_EQUALS(vertexDescriptor.GetNormalIndex(), -1, "Bad normal index");
	EXPECT_EQUALS(vertexDescriptor.GetUvIndex(), -1, "Bad uv index");
	EXPECT_EQUALS(vertexDescriptor.GetColorIndex(), -1, "Bad color index");

	EXPECT_EQUALS(vertexDescriptor.GetVertexSize(), 0, "Bad vertex size");
	EXPECT_EQUALS(vertexDescriptor.GetVertexElementList().size(), 0, "Bad vertex element count");

	vertexDescriptor.AddVertexElement(VertexElement::COLOR_4_FLOATS);

	EXPECT_EQUALS(vertexDescriptor.GetPositionIndex(), -1, "Bad position index");
	EXPECT_EQUALS(vertexDescriptor.GetNormalIndex(), -1, "Bad normal index");
	EXPECT_EQUALS(vertexDescriptor.GetUvIndex(), -1, "Bad uv index");
	EXPECT_EQUALS(vertexDescriptor.GetColorIndex(), 0, "Bad color index");

	EXPECT_EQUALS(vertexDescriptor.GetVertexSize(), colorBytesCount, "Bad vertex size");
	EXPECT_EQUALS(vertexDescriptor.GetVertexElementList().size(), 1, "Bad vertex element count");

	EXPECT_EQUALS(vertexDescriptor.GetColorOffset(), 0, "Bad color offset");

	vertexDescriptor.AddVertexElement(VertexElement::POSITION_32_BITS);

	EXPECT_EQUALS(vertexDescriptor.GetPositionIndex(), 1, "Bad position index");
	EXPECT_EQUALS(vertexDescriptor.GetNormalIndex(), -1, "Bad normal index");
	EXPECT_EQUALS(vertexDescriptor.GetUvIndex(), -1, "Bad uv index");
	EXPECT_EQUALS(vertexDescriptor.GetColorIndex(), 0, "Bad color index");

	EXPECT_EQUALS(vertexDescriptor.GetVertexSize(), colorBytesCount + sizeof(float[3]), "Bad vertex size");
	EXPECT_EQUALS(vertexDescriptor.GetVertexElementList().size(), 2, "Bad vertex element count");

	EXPECT_EQUALS(vertexDescriptor.GetColorOffset(), 0, "Bad color offset");
	EXPECT_EQUALS(vertexDescriptor.GetPositionOffset(), colorBytesCount, "Bad position offset");

	// Try to add a color again (should not change anything)
	vertexDescriptor.AddVertexElement(VertexElement::COLOR_4_FLOATS);

	EXPECT_EQUALS(vertexDescriptor.GetPositionIndex(), 1, "Bad position index");
	EXPECT_EQUALS(vertexDescriptor.GetNormalIndex(), -1, "Bad normal index");
	EXPECT_EQUALS(vertexDescriptor.GetUvIndex(), -1, "Bad uv index");
	EXPECT_EQUALS(vertexDescriptor.GetColorIndex(), 0, "Bad color index");

	EXPECT_EQUALS(vertexDescriptor.GetVertexSize(), colorBytesCount + sizeof(float[3]), "Bad vertex size");
	EXPECT_EQUALS(vertexDescriptor.GetVertexElementList().size(), 2, "Bad vertex element count");

	EXPECT_EQUALS(vertexDescriptor.GetColorOffset(), 0, "Bad color offset");
	EXPECT_EQUALS(vertexDescriptor.GetPositionOffset(), colorBytesCount, "Bad position offset");

	// Try to add a position again but different size (should not change anything)
	vertexDescriptor.AddVertexElement(VertexElement::POSITION_16_BITS);

	EXPECT_EQUALS(vertexDescriptor.GetPositionIndex(), 1, "Bad position index");
	EXPECT_EQUALS(vertexDescriptor.GetNormalIndex(), -1, "Bad normal index");
	EXPECT_EQUALS(vertexDescriptor.GetUvIndex(), -1, "Bad uv index");
	EXPECT_EQUALS(vertexDescriptor.GetColorIndex(), 0, "Bad color index");

	EXPECT_EQUALS(vertexDescriptor.GetVertexSize(), colorBytesCount + sizeof(float[3]), "Bad vertex size");
	EXPECT_EQUALS(vertexDescriptor.GetVertexElementList().size(), 2, "Bad vertex element count");

	EXPECT_EQUALS(vertexDescriptor.GetColorOffset(), 0, "Bad color offset");
	EXPECT_EQUALS(vertexDescriptor.GetPositionOffset(), colorBytesCount, "Bad position offset");

	END_TEST();
}


TestResult VertexDescriptorGetVertexElementSizeTest::Start(std::string& errorOut)
{
	BEGIN_TEST();
	EXPECT_EQUALS(VertexDescriptor::GetVertexElementSize(VertexElement::POSITION_32_BITS), sizeof(float[3]), "Wrong None size");
	EXPECT_EQUALS(VertexDescriptor::GetVertexElementSize(VertexElement::POSITION_16_BITS), sizeof(int16_t[3]), "Wrong None size");
	EXPECT_EQUALS(VertexDescriptor::GetVertexElementSize(VertexElement::POSITION_8_BITS), sizeof(int8_t[3]), "Wrong None size");
	END_TEST();
}
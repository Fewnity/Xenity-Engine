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

	VertexDescriptor vertexDescriptor;

	// Check initial values
	EXPECT_EQUALS(vertexDescriptor.m_positionIndex, -1, "Bad intial position index");
	EXPECT_EQUALS(vertexDescriptor.m_normalIndex, -1, "Bad intial normal index");
	EXPECT_EQUALS(vertexDescriptor.m_uvIndex, -1, "Bad intial uv index");
	EXPECT_EQUALS(vertexDescriptor.m_colorIndex, -1, "Bad intial color index");

	EXPECT_EQUALS(vertexDescriptor.m_vertexSize, 0, "Bad intial vertex size");
	EXPECT_EQUALS(vertexDescriptor.m_vertexElementInfos.size(), 0, "Bad intial vertex element count");

	vertexDescriptor.AddVertexDescriptor(VertexElements::POSITION_32_BITS);

	EXPECT_EQUALS(vertexDescriptor.m_positionIndex, 0, "Bad position index");
	EXPECT_EQUALS(vertexDescriptor.m_normalIndex, -1, "Bad normal index");
	EXPECT_EQUALS(vertexDescriptor.m_uvIndex, -1, "Bad uv index");
	EXPECT_EQUALS(vertexDescriptor.m_colorIndex, -1, "Bad color index");

	EXPECT_EQUALS(vertexDescriptor.m_vertexSize, sizeof(float[3]), "Bad vertex size");
	EXPECT_EQUALS(vertexDescriptor.m_vertexElementInfos.size(), 1, "Bad vertex element count");

	EXPECT_EQUALS(vertexDescriptor.GetPositionOffset(), 0, "Bad position offset");

	vertexDescriptor.AddVertexDescriptor(VertexElements::COLOR_4_FLOATS);

	EXPECT_EQUALS(vertexDescriptor.m_positionIndex, 0, "Bad position index");
	EXPECT_EQUALS(vertexDescriptor.m_normalIndex, -1, "Bad normal index");
	EXPECT_EQUALS(vertexDescriptor.m_uvIndex, -1, "Bad uv index");
	EXPECT_EQUALS(vertexDescriptor.m_colorIndex, 1, "Bad color index");

	EXPECT_EQUALS(vertexDescriptor.m_vertexSize, sizeof(float[3]) + sizeof(float[4]), "Bad vertex size");
	EXPECT_EQUALS(vertexDescriptor.m_vertexElementInfos.size(), 2, "Bad vertex element count");

	EXPECT_EQUALS(vertexDescriptor.GetPositionOffset(), 0, "Bad position offset");
	EXPECT_EQUALS(vertexDescriptor.GetColorOffset(), sizeof(float[3]), "Bad color offset");

	vertexDescriptor.AddVertexDescriptor(VertexElements::NORMAL_32_BITS);

	EXPECT_EQUALS(vertexDescriptor.m_positionIndex, 0, "Bad position index");
	EXPECT_EQUALS(vertexDescriptor.m_normalIndex, 2, "Bad normal index");
	EXPECT_EQUALS(vertexDescriptor.m_uvIndex, -1, "Bad uv index");
	EXPECT_EQUALS(vertexDescriptor.m_colorIndex, 1, "Bad color index");

	EXPECT_EQUALS(vertexDescriptor.m_vertexSize, sizeof(float[3]) + sizeof(float[4]) + sizeof(float[3]), "Bad vertex size");
	EXPECT_EQUALS(vertexDescriptor.m_vertexElementInfos.size(), 3, "Bad vertex element count");

	EXPECT_EQUALS(vertexDescriptor.GetPositionOffset(), 0, "Bad position offset");
	EXPECT_EQUALS(vertexDescriptor.GetColorOffset(), sizeof(float[3]), "Bad color offset");
	EXPECT_EQUALS(vertexDescriptor.GetNormalOffset(), sizeof(float[3]) + sizeof(float[4]), "Bad normal offset");

	vertexDescriptor.AddVertexDescriptor(VertexElements::UV_32_BITS);

	EXPECT_EQUALS(vertexDescriptor.m_positionIndex, 0, "Bad position index");
	EXPECT_EQUALS(vertexDescriptor.m_normalIndex, 2, "Bad normal index");
	EXPECT_EQUALS(vertexDescriptor.m_uvIndex, 3, "Bad uv index");
	EXPECT_EQUALS(vertexDescriptor.m_colorIndex, 1, "Bad color index");

	EXPECT_EQUALS(vertexDescriptor.m_vertexSize, sizeof(float[3]) + sizeof(float[4]) + sizeof(float[3]) + sizeof(float[2]), "Bad vertex size");
	EXPECT_EQUALS(vertexDescriptor.m_vertexElementInfos.size(), 4, "Bad vertex element count");

	EXPECT_EQUALS(vertexDescriptor.GetPositionOffset(), 0, "Bad position offset");
	EXPECT_EQUALS(vertexDescriptor.GetColorOffset(), sizeof(float[3]), "Bad color offset");
	EXPECT_EQUALS(vertexDescriptor.GetNormalOffset(), sizeof(float[3]) + sizeof(float[4]), "Bad normal offset");
	EXPECT_EQUALS(vertexDescriptor.GetUvOffset(), sizeof(float[3]) + sizeof(float[4]) + sizeof(float[3]), "Bad normal offset");

	END_TEST();
}

TestResult VertexDescriptor16BitsTest::Start(std::string& errorOut)
{
	BEGIN_TEST();

	VertexDescriptor vertexDescriptor;

	vertexDescriptor.AddVertexDescriptor(VertexElements::POSITION_16_BITS);

	EXPECT_EQUALS(vertexDescriptor.m_vertexSize, sizeof(int16_t[3]), "Bad vertex size");
	EXPECT_EQUALS(vertexDescriptor.m_vertexElementInfos.size(), 1, "Bad vertex element count");

	vertexDescriptor.AddVertexDescriptor(VertexElements::COLOR_32_BITS_UINT); // Not 16 bits color

	EXPECT_EQUALS(vertexDescriptor.m_vertexSize, sizeof(int16_t[3]) + sizeof(unsigned int), "Bad vertex size");
	EXPECT_EQUALS(vertexDescriptor.m_vertexElementInfos.size(), 2, "Bad vertex element count");

	vertexDescriptor.AddVertexDescriptor(VertexElements::NORMAL_16_BITS);

	EXPECT_EQUALS(vertexDescriptor.m_vertexSize, sizeof(int16_t[3]) + sizeof(unsigned int) + sizeof(int16_t[3]), "Bad vertex size");
	EXPECT_EQUALS(vertexDescriptor.m_vertexElementInfos.size(), 3, "Bad vertex element count");

	vertexDescriptor.AddVertexDescriptor(VertexElements::UV_16_BITS);

	EXPECT_EQUALS(vertexDescriptor.m_vertexSize, sizeof(int16_t[3]) + sizeof(unsigned int) + sizeof(int16_t[3]) + sizeof(int16_t[2]), "Bad vertex size");
	EXPECT_EQUALS(vertexDescriptor.m_vertexElementInfos.size(), 4, "Bad vertex element count");

	END_TEST();
}

TestResult VertexDescriptor8BitsTest::Start(std::string& errorOut)
{
	BEGIN_TEST();

	VertexDescriptor vertexDescriptor;

	vertexDescriptor.AddVertexDescriptor(VertexElements::POSITION_8_BITS);

	EXPECT_EQUALS(vertexDescriptor.m_vertexSize, sizeof(int8_t[3]), "Bad vertex size");
	EXPECT_EQUALS(vertexDescriptor.m_vertexElementInfos.size(), 1, "Bad vertex element count");

	vertexDescriptor.AddVertexDescriptor(VertexElements::COLOR_32_BITS_UINT); // Not 16 bits color

	EXPECT_EQUALS(vertexDescriptor.m_vertexSize, sizeof(int8_t[3]) + sizeof(unsigned int), "Bad vertex size");
	EXPECT_EQUALS(vertexDescriptor.m_vertexElementInfos.size(), 2, "Bad vertex element count");

	vertexDescriptor.AddVertexDescriptor(VertexElements::NORMAL_8_BITS);

	EXPECT_EQUALS(vertexDescriptor.m_vertexSize, sizeof(int8_t[3]) + sizeof(unsigned int) + sizeof(int8_t[3]), "Bad vertex size");
	EXPECT_EQUALS(vertexDescriptor.m_vertexElementInfos.size(), 3, "Bad vertex element count");

	vertexDescriptor.AddVertexDescriptor(VertexElements::UV_8_BITS);

	EXPECT_EQUALS(vertexDescriptor.m_vertexSize, sizeof(int8_t[3]) + sizeof(unsigned int) + sizeof(int8_t[3]) + sizeof(int8_t[2]), "Bad vertex size");
	EXPECT_EQUALS(vertexDescriptor.m_vertexElementInfos.size(), 4, "Bad vertex element count");

	END_TEST();
}

TestResult VertexDescriptorWrongTest::Start(std::string& errorOut)
{
	BEGIN_TEST();

	VertexDescriptor vertexDescriptor;

	vertexDescriptor.AddVertexDescriptor(VertexElements::NONE);

	EXPECT_EQUALS(vertexDescriptor.m_positionIndex, -1, "Bad position index");
	EXPECT_EQUALS(vertexDescriptor.m_normalIndex, -1, "Bad normal index");
	EXPECT_EQUALS(vertexDescriptor.m_uvIndex, -1, "Bad uv index");
	EXPECT_EQUALS(vertexDescriptor.m_colorIndex, -1, "Bad color index");

	EXPECT_EQUALS(vertexDescriptor.m_vertexSize, 0, "Bad vertex size");
	EXPECT_EQUALS(vertexDescriptor.m_vertexElementInfos.size(), 0, "Bad vertex element count");

	vertexDescriptor.AddVertexDescriptor(VertexElements::COLOR_4_FLOATS);

	EXPECT_EQUALS(vertexDescriptor.m_positionIndex, -1, "Bad position index");
	EXPECT_EQUALS(vertexDescriptor.m_normalIndex, -1, "Bad normal index");
	EXPECT_EQUALS(vertexDescriptor.m_uvIndex, -1, "Bad uv index");
	EXPECT_EQUALS(vertexDescriptor.m_colorIndex, 0, "Bad color index");

	EXPECT_EQUALS(vertexDescriptor.m_vertexSize, sizeof(float[4]), "Bad vertex size");
	EXPECT_EQUALS(vertexDescriptor.m_vertexElementInfos.size(), 1, "Bad vertex element count");

	EXPECT_EQUALS(vertexDescriptor.GetColorOffset(), 0, "Bad color offset");

	vertexDescriptor.AddVertexDescriptor(VertexElements::POSITION_32_BITS);

	EXPECT_EQUALS(vertexDescriptor.m_positionIndex, 1, "Bad position index");
	EXPECT_EQUALS(vertexDescriptor.m_normalIndex, -1, "Bad normal index");
	EXPECT_EQUALS(vertexDescriptor.m_uvIndex, -1, "Bad uv index");
	EXPECT_EQUALS(vertexDescriptor.m_colorIndex, 0, "Bad color index");

	EXPECT_EQUALS(vertexDescriptor.m_vertexSize, sizeof(float[4]) + sizeof(float[3]), "Bad vertex size");
	EXPECT_EQUALS(vertexDescriptor.m_vertexElementInfos.size(), 2, "Bad vertex element count");

	EXPECT_EQUALS(vertexDescriptor.GetColorOffset(), 0, "Bad color offset");
	EXPECT_EQUALS(vertexDescriptor.GetPositionOffset(), sizeof(float[4]), "Bad position offset");

	// Try to add a color again (should not change anything)
	vertexDescriptor.AddVertexDescriptor(VertexElements::COLOR_4_FLOATS);

	EXPECT_EQUALS(vertexDescriptor.m_positionIndex, 1, "Bad position index");
	EXPECT_EQUALS(vertexDescriptor.m_normalIndex, -1, "Bad normal index");
	EXPECT_EQUALS(vertexDescriptor.m_uvIndex, -1, "Bad uv index");
	EXPECT_EQUALS(vertexDescriptor.m_colorIndex, 0, "Bad color index");

	EXPECT_EQUALS(vertexDescriptor.m_vertexSize, sizeof(float[4]) + sizeof(float[3]), "Bad vertex size");
	EXPECT_EQUALS(vertexDescriptor.m_vertexElementInfos.size(), 2, "Bad vertex element count");

	EXPECT_EQUALS(vertexDescriptor.GetColorOffset(), 0, "Bad color offset");
	EXPECT_EQUALS(vertexDescriptor.GetPositionOffset(), sizeof(float[4]), "Bad position offset");

	// Try to add a position again but different size (should not change anything)
	vertexDescriptor.AddVertexDescriptor(VertexElements::POSITION_16_BITS);

	EXPECT_EQUALS(vertexDescriptor.m_positionIndex, 1, "Bad position index");
	EXPECT_EQUALS(vertexDescriptor.m_normalIndex, -1, "Bad normal index");
	EXPECT_EQUALS(vertexDescriptor.m_uvIndex, -1, "Bad uv index");
	EXPECT_EQUALS(vertexDescriptor.m_colorIndex, 0, "Bad color index");

	EXPECT_EQUALS(vertexDescriptor.m_vertexSize, sizeof(float[4]) + sizeof(float[3]), "Bad vertex size");
	EXPECT_EQUALS(vertexDescriptor.m_vertexElementInfos.size(), 2, "Bad vertex element count");

	EXPECT_EQUALS(vertexDescriptor.GetColorOffset(), 0, "Bad color offset");
	EXPECT_EQUALS(vertexDescriptor.GetPositionOffset(), sizeof(float[4]), "Bad position offset");

	END_TEST();
}


TestResult VertexDescriptorGetVertexElementSizeTest::Start(std::string& errorOut)
{
	BEGIN_TEST();
	EXPECT_EQUALS(VertexDescriptor::GetVertexElementSize(VertexElements::POSITION_32_BITS), sizeof(float[3]), "Wrong None size");
	EXPECT_EQUALS(VertexDescriptor::GetVertexElementSize(VertexElements::POSITION_16_BITS), sizeof(int16_t[3]), "Wrong None size");
	EXPECT_EQUALS(VertexDescriptor::GetVertexElementSize(VertexElements::POSITION_8_BITS), sizeof(int8_t[3]), "Wrong None size");
	END_TEST();
}
#pragma once

#include <cstdint>
#include <type_traits>
#include <vector>

#include <engine/assertions/assertions.h>

enum class VertexElements : uint32_t // Do not change the uint32_t type
{
	NONE = 0,
	POSITION_32_BITS = 1 << 0,
	POSITION_16_BITS = 1 << 1, // Used for PSP
	POSITION_8_BITS = 1 << 2, // Used for PSP
	NORMAL_32_BITS = 1 << 3,
	NORMAL_16_BITS = 1 << 4, // Used for PSP
	NORMAL_8_BITS = 1 << 5, // Used for PSP
	UV_32_BITS = 1 << 6,
	UV_16_BITS = 1 << 7, // Used for PSP
	UV_8_BITS = 1 << 8, // Used for PSP
	COLOR_4_FLOATS = 1 << 9, // Used for PSP
	COLOR_32_BITS_UINT = 1 << 10,
};

constexpr VertexElements operator|(VertexElements lhs, VertexElements rhs)
{
	using UnderlyingType = std::underlying_type_t<VertexElements>;
	return static_cast<VertexElements>(
		static_cast<UnderlyingType>(lhs) | static_cast<UnderlyingType>(rhs)
		);
}

constexpr VertexElements& operator|=(VertexElements& lhs, VertexElements rhs)
{
	lhs = lhs | rhs;
	return lhs;
}

constexpr VertexElements operator&(VertexElements lhs, VertexElements rhs)
{
	using UnderlyingType = std::underlying_type_t<VertexElements>;
	return static_cast<VertexElements>(
		static_cast<UnderlyingType>(lhs) & static_cast<UnderlyingType>(rhs)
		);
}

constexpr VertexElements& operator&=(VertexElements& lhs, VertexElements rhs)
{
	lhs = lhs & rhs;
	return lhs;
}

struct VertexElementInfo
{
	VertexElements vertexElement = VertexElements::NONE;
	uint32_t size = 0;
	uint32_t offset = 0;
};

struct VertexDescriptor
{
	static uint16_t GetVertexElementSize(VertexElements vertexElement)
	{
		XASSERT(vertexElement != VertexElements::NONE, "[VertexDescriptor::AddVertexDescriptor] Wrong vertexElement");

		uint16_t vertexSize = 0;
		if ((vertexElement & VertexElements::POSITION_32_BITS) == VertexElements::POSITION_32_BITS)
		{
			vertexSize += sizeof(float[3]);
		}
		else if ((vertexElement & VertexElements::POSITION_16_BITS) == VertexElements::POSITION_16_BITS)
		{
			vertexSize += sizeof(int16_t[3]);
		}
		else if ((vertexElement & VertexElements::POSITION_8_BITS) == VertexElements::POSITION_8_BITS)
		{
			vertexSize += sizeof(int8_t[3]);
		}

		if ((vertexElement & VertexElements::NORMAL_32_BITS) == VertexElements::NORMAL_32_BITS)
		{
			vertexSize += sizeof(float[3]);
		}
		else if ((vertexElement & VertexElements::NORMAL_16_BITS) == VertexElements::NORMAL_16_BITS)
		{
			vertexSize += sizeof(int16_t[3]);
		}
		else if ((vertexElement & VertexElements::NORMAL_8_BITS) == VertexElements::NORMAL_8_BITS)
		{
			vertexSize += sizeof(int8_t[3]);
		}

		if ((vertexElement & VertexElements::UV_32_BITS) == VertexElements::UV_32_BITS)
		{
			vertexSize += sizeof(float[2]);
		}
		else if ((vertexElement & VertexElements::UV_16_BITS) == VertexElements::UV_16_BITS)
		{
			vertexSize += sizeof(int16_t[2]);
		}
		else if ((vertexElement & VertexElements::UV_8_BITS) == VertexElements::UV_8_BITS)
		{
			vertexSize += sizeof(int8_t[2]);
		}

		if ((vertexElement & VertexElements::COLOR_4_FLOATS) == VertexElements::COLOR_4_FLOATS)
		{
			vertexSize += sizeof(float[4]);
		}

		if ((vertexElement & VertexElements::COLOR_32_BITS_UINT) == VertexElements::COLOR_32_BITS_UINT)
		{
			vertexSize += sizeof(uint32_t);
		}

		return vertexSize;
	}

	void AddVertexDescriptor(VertexElements vertexElement)
	{
		if (vertexElement == VertexElements::NONE)
			return;

		if ((vertexElement & VertexElements::POSITION_32_BITS) == VertexElements::POSITION_32_BITS || (vertexElement & VertexElements::POSITION_16_BITS) == VertexElements::POSITION_16_BITS || (vertexElement & VertexElements::POSITION_8_BITS) == VertexElements::POSITION_8_BITS)
		{
			if (m_positionIndex != -1)
			{
				return;
			}
			m_positionIndex = static_cast<int>(m_vertexElementInfos.size());
		}
		else if ((vertexElement & VertexElements::NORMAL_32_BITS) == VertexElements::NORMAL_32_BITS || (vertexElement & VertexElements::NORMAL_16_BITS) == VertexElements::NORMAL_16_BITS || (vertexElement & VertexElements::NORMAL_8_BITS) == VertexElements::NORMAL_8_BITS)
		{
			if (m_normalIndex != -1)
			{
				return;
			}
			m_normalIndex = static_cast<int>(m_vertexElementInfos.size());
		}
		else if ((vertexElement & VertexElements::UV_32_BITS) == VertexElements::UV_32_BITS || (vertexElement & VertexElements::UV_16_BITS) == VertexElements::UV_16_BITS || (vertexElement & VertexElements::UV_8_BITS) == VertexElements::UV_8_BITS)
		{
			if (m_uvIndex != -1)
			{
				return;
			}
			m_uvIndex = static_cast<int>(m_vertexElementInfos.size());
		}
		else if ((vertexElement & VertexElements::COLOR_4_FLOATS) == VertexElements::COLOR_4_FLOATS || (vertexElement & VertexElements::COLOR_32_BITS_UINT) == VertexElements::COLOR_32_BITS_UINT)
		{
			if (m_colorIndex != -1)
			{
				return;
			}
			m_colorIndex = static_cast<int>(m_vertexElementInfos.size());
		}
		else 
		{
			XASSERT(false, "[VertexDescriptor::AddVertexDescriptor] Wrong vertexElement");
			return;
		}

		VertexElementInfo vertexElementInfo;
		vertexElementInfo.vertexElement = vertexElement;
		vertexElementInfo.size = GetVertexElementSize(vertexElement);
		vertexElementInfo.offset = m_vertexSize;
		m_vertexElementInfos.push_back(vertexElementInfo);

		m_vertexSize += vertexElementInfo.size;
	}

	VertexElements GetElementFromIndex(int32_t index) const
	{
		XASSERT(index >= 0 && index < static_cast<int32_t>(m_vertexElementInfos.size()), "[VertexDescriptor::GetOffsetFromIndex] Index out of bound");

		return m_vertexElementInfos[index].vertexElement;
	}

	uint32_t GetOffsetFromIndex(int32_t index) const
	{
		XASSERT(index >= 0 && index < static_cast<int32_t>(m_vertexElementInfos.size()), "[VertexDescriptor::GetOffsetFromIndex] Index out of bound");

		return m_vertexElementInfos[index].offset;
	}

	uint32_t GetPositionOffset() const
	{
		XASSERT(m_positionIndex >= 0 && m_positionIndex < static_cast<int32_t>(m_vertexElementInfos.size()), "[VertexDescriptor::GetPositionOffset] Index out of bound");

		return m_vertexElementInfos[m_positionIndex].offset;
	}

	uint32_t GetNormalOffset() const
	{
		XASSERT(m_normalIndex >= 0 && m_normalIndex < static_cast<int32_t>(m_vertexElementInfos.size()), "[VertexDescriptor::GetNormalOffset] Index out of bound");

		return m_vertexElementInfos[m_normalIndex].offset;
	}

	uint32_t GetUvOffset() const
	{
		XASSERT(m_uvIndex >= 0 && m_uvIndex < static_cast<int32_t>(m_vertexElementInfos.size()), "[VertexDescriptor::GetUvOffset] Index out of bound");

		return m_vertexElementInfos[m_uvIndex].offset;
	}

	uint32_t GetColorOffset() const
	{
		XASSERT(m_colorIndex >= 0 && m_colorIndex < static_cast<int32_t>(m_vertexElementInfos.size()), "[VertexDescriptor::GetColorOffset] Index out of bound");

		return m_vertexElementInfos[m_colorIndex].offset;
	}

	std::vector<VertexElementInfo> m_vertexElementInfos;
	int32_t m_positionIndex = -1;
	int32_t m_normalIndex = -1;
	int32_t m_uvIndex = -1;
	int32_t m_colorIndex = -1;
	uint16_t m_vertexSize = 0;
};
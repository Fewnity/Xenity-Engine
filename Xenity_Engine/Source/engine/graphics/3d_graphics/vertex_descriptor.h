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
	NORMAL_32_BITS = 1 << 2,
	NORMAL_16_BITS = 1 << 3, // Used for PSP
	NORMAL_8_BITS = 1 << 4, // Used for PSP
	UV_32_BITS = 1 << 5,
	UV_16_BITS = 1 << 6, // Used for PSP
	COLOR = 1 << 7,
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

struct VertexDescriptor
{
	VertexElements vertexElement = VertexElements::NONE;
	uint32_t size = 0;
	uint32_t offset = 0;
};

struct VertexDescriptorList
{
	static uint16_t GetVertexElementSize(VertexElements vertexElement)
	{
		uint16_t vertexSize = 0;
		if ((vertexElement & VertexElements::POSITION_32_BITS) == VertexElements::POSITION_32_BITS)
		{
			vertexSize += sizeof(float[3]);
		}
		else if ((vertexElement & VertexElements::POSITION_16_BITS) == VertexElements::POSITION_16_BITS)
		{
			vertexSize += sizeof(uint16_t[3]);
		}

		if ((vertexElement & VertexElements::NORMAL_32_BITS) == VertexElements::NORMAL_32_BITS)
		{
			vertexSize += sizeof(float[3]);
		}
		else if ((vertexElement & VertexElements::NORMAL_16_BITS) == VertexElements::NORMAL_16_BITS)
		{
			vertexSize += sizeof(uint16_t[3]);
		}
		else if ((vertexElement & VertexElements::NORMAL_8_BITS) == VertexElements::NORMAL_8_BITS)
		{
			vertexSize += sizeof(char[3]);
		}

		if ((vertexElement & VertexElements::UV_32_BITS) == VertexElements::UV_32_BITS)
		{
			vertexSize += sizeof(float[2]);
		}
		else if ((vertexElement & VertexElements::UV_16_BITS) == VertexElements::UV_16_BITS)
		{
			vertexSize += sizeof(uint16_t[2]);
		}

		if ((vertexElement & VertexElements::COLOR) == VertexElements::COLOR)
		{
#if defined(__PSP__)
			vertexSize += sizeof(uint32_t);
#else
			vertexSize += sizeof(float[4]);
#endif
		}

		return vertexSize;
	}

	void AddVertexDescriptor(VertexElements vertexElement)
	{
		VertexDescriptor vertexDescriptor;
		vertexDescriptor.vertexElement = vertexElement;
		vertexDescriptor.size = GetVertexElementSize(vertexElement);
		vertexDescriptor.offset = m_vertexSize;
		m_vertexDescriptors.push_back(vertexDescriptor);

		m_vertexSize += vertexDescriptor.size;

		if ((vertexElement & VertexElements::POSITION_32_BITS) == VertexElements::POSITION_32_BITS || (vertexElement & VertexElements::POSITION_16_BITS) == VertexElements::POSITION_16_BITS)
		{
			m_positionIndex = static_cast<int>(m_vertexDescriptors.size() - 1);
		}
		else if ((vertexElement & VertexElements::NORMAL_32_BITS) == VertexElements::NORMAL_32_BITS || (vertexElement & VertexElements::NORMAL_16_BITS) == VertexElements::NORMAL_16_BITS || (vertexElement & VertexElements::NORMAL_8_BITS) == VertexElements::NORMAL_8_BITS)
		{
			m_normalIndex = static_cast<int>(m_vertexDescriptors.size() - 1);
		}
		else if ((vertexElement & VertexElements::UV_32_BITS) == VertexElements::UV_32_BITS || (vertexElement & VertexElements::UV_16_BITS) == VertexElements::UV_16_BITS)
		{
			m_uvIndex = static_cast<int>(m_vertexDescriptors.size() - 1);
		}
		else if ((vertexElement & VertexElements::COLOR) == VertexElements::COLOR)
		{
			m_colorIndex = static_cast<int>(m_vertexDescriptors.size() - 1);
		}
	}

	uint32_t GetOffsetFromIndex(int32_t index) const
	{
		XASSERT(index >= 0 && index < static_cast<int32_t>(m_vertexDescriptors.size()), "[VertexDescriptorList::GetOffsetFromIndex] Index out of bound");

		return m_vertexDescriptors[index].offset;
	}

	uint32_t GetPositionOffset() const
	{
		XASSERT(m_positionIndex >= 0 && m_positionIndex < static_cast<int32_t>(m_vertexDescriptors.size()), "[VertexDescriptorList::GetPositionOffset] Index out of bound");

		return m_vertexDescriptors[m_positionIndex].offset;
	}

	uint32_t GetNormalOffset() const
	{
		XASSERT(m_normalIndex >= 0 && m_normalIndex < static_cast<int32_t>(m_vertexDescriptors.size()), "[VertexDescriptorList::GetNormalOffset] Index out of bound");

		return m_vertexDescriptors[m_normalIndex].offset;
	}

	uint32_t GetUvOffset() const
	{
		XASSERT(m_uvIndex >= 0 && m_uvIndex < static_cast<int32_t>(m_vertexDescriptors.size()), "[VertexDescriptorList::GetUvOffset] Index out of bound");

		return m_vertexDescriptors[m_uvIndex].offset;
	}

	uint32_t GetColorOffset() const
	{
		XASSERT(m_colorIndex >= 0 && m_colorIndex < static_cast<int32_t>(m_vertexDescriptors.size()), "[VertexDescriptorList::GetColorOffset] Index out of bound");

		return m_vertexDescriptors[m_colorIndex].offset;
	}

	std::vector<VertexDescriptor> m_vertexDescriptors;
	int32_t m_positionIndex = -1;
	int32_t m_normalIndex = -1;
	int32_t m_uvIndex = -1;
	int32_t m_colorIndex = -1;
	uint16_t m_vertexSize = 0;
};
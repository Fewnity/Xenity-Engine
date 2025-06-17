#pragma once

#include <cstdint>
#include <type_traits>
#include <vector>

#include <engine/assertions/assertions.h>

enum class VertexElement : uint32_t // Do not change the uint32_t type
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
	COLOR_4_FLOATS = 1 << 9,
	COLOR_32_BITS_UINT = 1 << 10, // Used for PSP
};

constexpr VertexElement operator|(VertexElement lhs, VertexElement rhs)
{
	using UnderlyingType = std::underlying_type_t<VertexElement>;
	return static_cast<VertexElement>(
		static_cast<UnderlyingType>(lhs) | static_cast<UnderlyingType>(rhs)
		);
}

constexpr VertexElement& operator|=(VertexElement& lhs, VertexElement rhs)
{
	lhs = lhs | rhs;
	return lhs;
}

constexpr VertexElement operator&(VertexElement lhs, VertexElement rhs)
{
	using UnderlyingType = std::underlying_type_t<VertexElement>;
	return static_cast<VertexElement>(
		static_cast<UnderlyingType>(lhs) & static_cast<UnderlyingType>(rhs)
		);
}

constexpr VertexElement& operator&=(VertexElement& lhs, VertexElement rhs)
{
	lhs = lhs & rhs;
	return lhs;
}

struct VertexElementInfo
{
	VertexElement vertexElement = VertexElement::NONE;
	uint32_t size = 0;
	uint32_t offset = 0;
};

struct VertexDescriptor
{
	/**
	* Get the size in bytes of a vertex element
	*/
	[[nodiscard]] static uint16_t GetVertexElementSize(VertexElement vertexElement)
	{
		XASSERT(vertexElement != VertexElement::NONE, "[VertexDescriptor::AddVertexElement] Wrong vertexElement");

		uint16_t vertexSize = 0;
		if ((vertexElement & VertexElement::POSITION_32_BITS) == VertexElement::POSITION_32_BITS)
		{
			vertexSize += sizeof(float[3]);
		}
		else if ((vertexElement & VertexElement::POSITION_16_BITS) == VertexElement::POSITION_16_BITS)
		{
			vertexSize += sizeof(int16_t[3]);
		}
		else if ((vertexElement & VertexElement::POSITION_8_BITS) == VertexElement::POSITION_8_BITS)
		{
			vertexSize += sizeof(int8_t[3]);
		}

		if ((vertexElement & VertexElement::NORMAL_32_BITS) == VertexElement::NORMAL_32_BITS)
		{
			vertexSize += sizeof(float[3]);
		}
		else if ((vertexElement & VertexElement::NORMAL_16_BITS) == VertexElement::NORMAL_16_BITS)
		{
			vertexSize += sizeof(int16_t[3]);
		}
		else if ((vertexElement & VertexElement::NORMAL_8_BITS) == VertexElement::NORMAL_8_BITS)
		{
			vertexSize += sizeof(int8_t[3]);
		}

		if ((vertexElement & VertexElement::UV_32_BITS) == VertexElement::UV_32_BITS)
		{
			vertexSize += sizeof(float[2]);
		}
		else if ((vertexElement & VertexElement::UV_16_BITS) == VertexElement::UV_16_BITS)
		{
			vertexSize += sizeof(int16_t[2]);
		}
		else if ((vertexElement & VertexElement::UV_8_BITS) == VertexElement::UV_8_BITS)
		{
			vertexSize += sizeof(int8_t[2]);
		}

		if ((vertexElement & VertexElement::COLOR_4_FLOATS) == VertexElement::COLOR_4_FLOATS)
		{
			vertexSize += sizeof(float[4]);
		}

		if ((vertexElement & VertexElement::COLOR_32_BITS_UINT) == VertexElement::COLOR_32_BITS_UINT)
		{
			vertexSize += sizeof(uint32_t);
		}

		return vertexSize;
	}

	/**
	* @brief Add a vertex element to the descriptor
	*/
	void AddVertexElement(VertexElement vertexElement)
	{
		if (vertexElement == VertexElement::NONE)
			return;

#if defined(__PSP__)
		if (vertexElement == VertexElement::COLOR_4_FLOATS)
		{
			vertexElement = VertexElement::COLOR_32_BITS_UINT;
		}
#endif

		if ((vertexElement & VertexElement::POSITION_32_BITS) == VertexElement::POSITION_32_BITS || (vertexElement & VertexElement::POSITION_16_BITS) == VertexElement::POSITION_16_BITS || (vertexElement & VertexElement::POSITION_8_BITS) == VertexElement::POSITION_8_BITS)
		{
			if (m_positionIndex != -1)
			{
				return;
			}
			m_positionIndex = static_cast<int>(m_vertexElementInfos.size());
		}
		else if ((vertexElement & VertexElement::NORMAL_32_BITS) == VertexElement::NORMAL_32_BITS || (vertexElement & VertexElement::NORMAL_16_BITS) == VertexElement::NORMAL_16_BITS || (vertexElement & VertexElement::NORMAL_8_BITS) == VertexElement::NORMAL_8_BITS)
		{
			if (m_normalIndex != -1)
			{
				return;
			}
			m_normalIndex = static_cast<int>(m_vertexElementInfos.size());
		}
		else if ((vertexElement & VertexElement::UV_32_BITS) == VertexElement::UV_32_BITS || (vertexElement & VertexElement::UV_16_BITS) == VertexElement::UV_16_BITS || (vertexElement & VertexElement::UV_8_BITS) == VertexElement::UV_8_BITS)
		{
			if (m_uvIndex != -1)
			{
				return;
			}
			m_uvIndex = static_cast<int>(m_vertexElementInfos.size());
		}
		else if ((vertexElement & VertexElement::COLOR_4_FLOATS) == VertexElement::COLOR_4_FLOATS || (vertexElement & VertexElement::COLOR_32_BITS_UINT) == VertexElement::COLOR_32_BITS_UINT)
		{
			if (m_colorIndex != -1)
			{
				return;
			}
			m_colorIndex = static_cast<int>(m_vertexElementInfos.size());
		}
		else 
		{
			XASSERT(false, "[VertexDescriptor::AddVertexElement] Wrong vertexElement");
			return;
		}

		VertexElementInfo vertexElementInfo;
		vertexElementInfo.vertexElement = vertexElement;
		vertexElementInfo.size = GetVertexElementSize(vertexElement);
		vertexElementInfo.offset = m_vertexSize;
		m_vertexElementInfos.push_back(vertexElementInfo);

		m_vertexSize += vertexElementInfo.size;
	}

	/**
	* @brief Get the vertex element from the index
	*/
	[[nodiscard]] VertexElement GetElementFromIndex(int32_t index) const
	{
		XASSERT(index >= 0 && index < static_cast<int32_t>(m_vertexElementInfos.size()), "[VertexDescriptor::GetOffsetFromIndex] Index out of bound");

		return m_vertexElementInfos[index].vertexElement;
	}

	/**
	* @brief Get the offset in memory of the vertex element from the index
	*/
	[[nodiscard]] uint32_t GetOffsetFromIndex(int32_t index) const
	{
		XASSERT(index >= 0 && index < static_cast<int32_t>(m_vertexElementInfos.size()), "[VertexDescriptor::GetOffsetFromIndex] Index out of bound");

		return m_vertexElementInfos[index].offset;
	}

	/**
	* @brief Get the index of the position vertex element
	*/
	[[nodiscard]] uint32_t GetPositionIndex() const
	{
		return m_positionIndex;
	}

	/**
	* @brief Get the index of the normal vertex element
	*/
	[[nodiscard]] uint32_t GetNormalIndex() const
	{
		return m_normalIndex;
	}

	/**
	* @brief Get the index of the UV vertex element
	*/
	[[nodiscard]] uint32_t GetUvIndex() const
	{
		return m_uvIndex;
	}

	/**
	* @brief Get the index of the color vertex element
	*/
	[[nodiscard]] uint32_t GetColorIndex() const
	{
		return m_colorIndex;
	}

	/**
	* @brief Get the offset in memory of the position vertex element
	*/
	[[nodiscard]] uint32_t GetPositionOffset() const
	{
		XASSERT(m_positionIndex >= 0 && m_positionIndex < static_cast<int32_t>(m_vertexElementInfos.size()), "[VertexDescriptor::GetPositionOffset] Index out of bound");

		return m_vertexElementInfos[m_positionIndex].offset;
	}

	/**
	* @brief Get the offset in memory of the normal vertex element
	*/
	[[nodiscard]] uint32_t GetNormalOffset() const
	{
		XASSERT(m_normalIndex >= 0 && m_normalIndex < static_cast<int32_t>(m_vertexElementInfos.size()), "[VertexDescriptor::GetNormalOffset] Index out of bound");

		return m_vertexElementInfos[m_normalIndex].offset;
	}

	/**
	* @brief Get the offset in memory of the UV vertex element
	*/
	[[nodiscard]] uint32_t GetUvOffset() const
	{
		XASSERT(m_uvIndex >= 0 && m_uvIndex < static_cast<int32_t>(m_vertexElementInfos.size()), "[VertexDescriptor::GetUvOffset] Index out of bound");

		return m_vertexElementInfos[m_uvIndex].offset;
	}

	/**
	* @brief Get the offset in memory of the color vertex element
	*/
	[[nodiscard]] uint32_t GetColorOffset() const
	{
		XASSERT(m_colorIndex >= 0 && m_colorIndex < static_cast<int32_t>(m_vertexElementInfos.size()), "[VertexDescriptor::GetColorOffset] Index out of bound");

		return m_vertexElementInfos[m_colorIndex].offset;
	}

	/**
	* @brief Get the list of all vertex elements
	*/
	[[nodiscard]] const std::vector<VertexElementInfo>& GetVertexElementList() const
	{
		return m_vertexElementInfos;
	}

	/**
	* @brief Get the size in byte of a vertex
	*/
	[[nodiscard]] uint16_t GetVertexSize() const
	{
		return m_vertexSize;
	}

private:
	std::vector<VertexElementInfo> m_vertexElementInfos;
	int32_t m_positionIndex = -1;
	int32_t m_normalIndex = -1;
	int32_t m_uvIndex = -1;
	int32_t m_colorIndex = -1;
	uint16_t m_vertexSize = 0;
};
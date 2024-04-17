#pragma once

#include "TextAlignments.h"

#include <engine/file_system/file_reference.h>
#include <engine/reflection/reflection.h>

#include <engine/vectors/vector2.h>
#include <glm/glm.hpp>

#include <vector>
#include <string>
#include <memory>


class Texture;
class Vector4;
class Transform;
class MeshData;
class Vector3;
class Color;
class Font;
class Material;

struct Character
{
	glm::ivec2 Size = glm::ivec2(0, 0);          // Size of glyph
	glm::ivec2 Bearing = glm::ivec2(0, 0);       // Offset from baseline to left/top of glyph

	Vector2 rightSize = Vector2(0);
	Vector2 rightBearing = Vector2(0);

	float rightAdvance = 0;
	unsigned int Advance = 0; // Offset to advance to next glyph

	Vector2 uv = Vector2(0);
	Vector2 uvOffet = Vector2(0);
};

/**
 * [Internal]
 */
struct LineInfo
{
	float lenght = 0;
	float y1 = 0;
};

/**
 * [Internal]
 */
struct TextInfo
{
	std::vector<LineInfo> linesInfo;
	float maxLineHeight = 0;
	int lineCount = 0;
};

/**
 * [Internal]
 */
class TextManager
{
public:

	/**
	* @brief [Internal] Init text manager
	*/
	static void Init();

	/**
	* @brief Draw a text
	* @param text Text
	* @param textInfo Text info
	* @param horizontalAlignment Horizontal alignment
	* @param verticalAlignment Vertical alignment
	* @param transform Transform
	* @param color Color
	* @param canvas Is for canvas
	* @param mesh Mesh
	* @param font Font
	* @param material Material
	*/
	static void DrawText(const std::string& text, TextInfo* textInfo, HorizontalAlignment horizontalAlignment, VerticalAlignment verticalAlignment, const std::shared_ptr<Transform>& transform, const Color& color, bool canvas, const std::shared_ptr <MeshData>& mesh, const std::shared_ptr<Font>& font, const std::shared_ptr <Material>& material);

	/**
	* @brief Get informations about a text
	* @param text Text
	* @param textLen Text lenght
	* @param font Font
	* @param scale Test scale
	* @return Text informations
	*/
	static TextInfo* GetTextInfomations(const std::string& text, int textLen, std::shared_ptr<Font> font, float scale);

	/**
	* @brief Create a mesh from a text
	* @param text Text
	* @param textInfo Text informations
	* @param horizontalAlignment Horizontal alignment
	* @param verticalAlignment Vertical alignment
	* @param color Color
	* @param font Font
	* @param scale Scale
	* @return The mesh
	*/
	static std::shared_ptr <MeshData> CreateMesh(std::string& text, TextInfo* textInfo, HorizontalAlignment horizontalAlignment, VerticalAlignment verticalAlignment, Color& color, const std::shared_ptr<Font>& font, float scale);

private:

	/**
	* @brief Add a char to the mesh
	* @param mesh Mesh to modify
	* @param ch Char to add
	* @param x Char X position
	* @param y Char Y position
	* @param letterIndex Letter index in the string
	*/
	static void AddCharToMesh(const std::shared_ptr<MeshData>& mesh, Character* ch, float x, float y, int letterIndex, float scale);

	static std::vector<std::shared_ptr<MeshData>> meshes;
};
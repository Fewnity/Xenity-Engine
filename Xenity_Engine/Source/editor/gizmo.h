#pragma once
#include <engine/api.h>

#include <memory>

class Vector3;
class Vector2;
class Color;
class Texture;

class API Gizmo
{
public:

	/**
	* @brief [Internal] Init Gizmo system
	*/
	static void Init();

	/**
	* @brief Draw a simple line From A to B
	* @param a Start point
	* @param b End point
	*/
	static void DrawLine(Vector3 a, Vector3 b);

	/**
	* @brief Set Gizmo draw color
	* @param newColor New color to set
	*/
	static void SetColor(const Color& newColor);

	/**
	* @brief Draw a billboard sprite
	* @param position Position
	* @param scale Scale
	* @param texture Texture to draw
	* @param color Color
	*/
	static void DrawBillboard(const Vector3& position, const Vector2& scale, const std::shared_ptr<Texture>& texture, const Color& color);

private:
	static Color color;
};


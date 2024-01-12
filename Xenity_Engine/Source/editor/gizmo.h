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
	* [Internal] Init Gizmo system
	*/
	static void Init();

	/**
	* Draw a simple line From A to B
	*/
	static void DrawLine(const Vector3& a, const Vector3& b);

	/**
	* Set Gizmo draw color
	*/
	static void SetColor(const Color& newColor);

	/**
	* Draw a billboard sprite
	*/
	static void DrawBillboard(const Vector3& position, const Vector2& scale, const std::shared_ptr<Texture>& texture, const Color& color);

private:
	static Color color;
};


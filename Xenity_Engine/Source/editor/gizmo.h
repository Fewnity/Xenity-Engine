#pragma once

class Vector3;
class Color;

class Gizmo
{
public:
	static void Init();
	static void DrawLine(const Vector3& a, const Vector3& b);
	static void SetColor(const Color& newColor);
private:
	static Color color;
};


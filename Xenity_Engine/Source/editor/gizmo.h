#pragma once

class Vector3;
class Color;

class Gizmo
{
public:
	static void Init();
	static void DrawLine(Vector3 a, Vector3 b);
	static void SetColor(Color newColor);
private:
	static Color color;
};


#pragma once

#include <engine/api.h>
#include <engine/graphics/iDrawable.h>
#include <engine/vectors/vector2_int.h>

class API Canvas : public IDrawable
{
public:
	Canvas();
	~Canvas();
	void OnDrawGizmos() override;
	ReflectiveData GetReflectiveData() override;
	void OnReflectionUpdated() override;
	int GetDrawPriority() const override;
	void Draw() override;

private:
	Vector2Int lastSize = 0;
};


#include "canvas.h"

#if defined(EDITOR)
#include <editor/gizmo.h>
#include <editor/ui/editor_ui.h>
#endif

#include <engine/game_elements/gameobject.h>
#include <engine/game_elements/transform.h>
#include <engine/graphics/color/color.h>
#include <engine/graphics/graphics.h>
#include <engine/graphics/camera.h>
#include <engine/asset_management/asset_manager.h>
#include <engine/ui/window.h>
#include <engine/game_elements/rect_transform.h>

ReflectiveData Canvas::GetReflectiveData()
{
	ReflectiveData reflectedVariables;
	Reflective::AddVariable(reflectedVariables, lastSize, "lastSize", true);
	return reflectedVariables;
}

void Canvas::OnReflectionUpdated()
{
}

int Canvas::GetDrawPriority() const
{
	return 0;
}

void Canvas::Draw()
{
	/*if (Window::GetWidth() != lastSize.x || Window::GetHeight() != lastSize.y)
	{*/
		lastSize = Vector2Int(Window::GetWidth(), Window::GetHeight());

		for (int i = 0; i < GetGameObject()->childCount; i++)
		{
			std::shared_ptr<RectTransform> rect = GetGameObject()->children[i].lock()->GetComponent<RectTransform>();
			if (rect) 
			{
				rect->UpdatePosition(std::dynamic_pointer_cast<Canvas>(shared_from_this()));
			}
		}
	//}
}

Canvas::Canvas()
{
	componentName = "Canvas";
	type = Draw_UI;

	AssetManager::AddReflection(this);
}

Canvas::~Canvas()
{
	AssetManager::RemoveReflection(this);
}

void Canvas::OnDrawGizmos()
{
#if defined(EDITOR)
	float aspect = Graphics::usedCamera.lock()->GetAspectRatio();
//#if defined(EDITOR)
	if (Editor::lastFocusedGameMenu.lock() != nullptr)
	{
		Vector2Int windowsSize = Editor::lastFocusedGameMenu.lock()->GetWindowSize();
		aspect = (float)windowsSize.x / (float)windowsSize.y;
	}
//#endif

	float xOff = (-aspect * 5) + (GetTransform()->GetPosition().x * (aspect * 10));
	float yOff = (-1 * 5) + (GetTransform()->GetPosition().y * (1 * 10));
	Vector3 pos = Vector3(xOff, -yOff, 1); // Z 1 to avoid issue with near clipping plane

	Color lineColor = Color::CreateFromRGBAFloat(1, 1, 1, 1);
	Gizmo::SetColor(lineColor);

	std::shared_ptr<Camera> camera = std::dynamic_pointer_cast<Camera>(shared_from_this());

	Engine::GetRenderer().SetCameraPosition(Graphics::usedCamera.lock());
	Gizmo::DrawLine(Vector3(xOff, yOff, 0) * -1, Vector3(xOff, -yOff, 0) * -1);
	Gizmo::DrawLine(Vector3(-xOff, yOff, 0) * -1, Vector3(-xOff, -yOff, 0) * -1);

	Gizmo::DrawLine(Vector3(xOff, yOff, 0) * -1, Vector3(-xOff, yOff, 0) * -1);
	Gizmo::DrawLine(Vector3(xOff, -yOff, 0) * -1, Vector3(-xOff, -yOff, 0) * -1);

	//if (Editor::GetSelectedGameObject() && Editor::GetSelectedGameObject() == GetGameObject())
	//{
	//	Color lineColor = Color::CreateFromRGBAFloat(1, 1, 1, 1);
	//	Gizmo::SetColor(lineColor);

	//	std::shared_ptr<Camera> camera = std::dynamic_pointer_cast<Camera>(shared_from_this());

	//	Engine::GetRenderer().SetCameraPosition(Graphics::usedCamera.lock());

	//	Vector3 cameraPosition = camera->GetTransform()->GetPosition();
	//	Vector3 cameraRotation = camera->GetTransform()->GetRotation();
	//	glm::mat4 cameraModelMatrix = glm::mat4(1.0f);
	//	cameraModelMatrix = glm::rotate(cameraModelMatrix, glm::radians(cameraRotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
	//	cameraModelMatrix = glm::rotate(cameraModelMatrix, glm::radians(cameraRotation.x * -1), glm::vec3(1.0f, 0.0f, 0.0f));
	//	cameraModelMatrix = glm::rotate(cameraModelMatrix, glm::radians(cameraRotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	//	cameraModelMatrix = glm::translate(cameraModelMatrix, glm::vec3(-cameraPosition.x, cameraPosition.y, cameraPosition.z));

	//	glm::vec4 screenSizeNorm = glm::vec4(0, 0, 1, 1);

	//	//Top left
	//	glm::vec3 topLeftNear = glm::unProject(glm::vec3(0, 0, 0.5f), cameraModelMatrix, camera->projection, screenSizeNorm);
	//	glm::vec3 topLeftFar = glm::unProject(glm::vec3(0, 0, 1.0f), cameraModelMatrix, camera->projection, screenSizeNorm);

	//	Gizmo::DrawLine(Vector3(topLeftNear.x, topLeftNear.y, topLeftNear.z) * -1, Vector3(topLeftFar.x, topLeftFar.y, topLeftFar.z) * -1);

	//	//Top right
	//	glm::vec3 topRightNear = glm::unProject(glm::vec3(1, 0, 0.5f), cameraModelMatrix, camera->projection, screenSizeNorm);
	//	glm::vec3 topRightFar = glm::unProject(glm::vec3(1, 0, 1.0f), cameraModelMatrix, camera->projection, screenSizeNorm);

	//	Gizmo::DrawLine(Vector3(topRightNear.x, topRightNear.y, topRightNear.z) * -1, Vector3(topRightFar.x, topRightFar.y, topRightFar.z) * -1);

	//	//Bottom left
	//	glm::vec3 bottomLeftNear = glm::unProject(glm::vec3(0, 1, 0.5f), cameraModelMatrix, camera->projection, screenSizeNorm);
	//	glm::vec3 bottomLeftFar = glm::unProject(glm::vec3(0, 1, 1.0f), cameraModelMatrix, camera->projection, screenSizeNorm);

	//	Gizmo::DrawLine(Vector3(bottomLeftNear.x, bottomLeftNear.y, bottomLeftNear.z) * -1, Vector3(bottomLeftFar.x, bottomLeftFar.y, bottomLeftFar.z) * -1);

	//	//Bottom right
	//	glm::vec3 bottomRightNear = glm::unProject(glm::vec3(1, 1, 0.5f), cameraModelMatrix, camera->projection, screenSizeNorm);
	//	glm::vec3 bottomRightFar = glm::unProject(glm::vec3(1, 1, 1.0f), cameraModelMatrix, camera->projection, screenSizeNorm);

	//	Gizmo::DrawLine(Vector3(bottomRightNear.x, bottomRightNear.y, bottomRightNear.z) * -1, Vector3(bottomRightFar.x, bottomRightFar.y, bottomRightFar.z) * -1);


	//	Gizmo::DrawLine(Vector3(topLeftFar.x, topLeftFar.y, topLeftFar.z) * -1, Vector3(topRightFar.x, topRightFar.y, topRightFar.z) * -1);
	//	Gizmo::DrawLine(Vector3(topLeftNear.x, topLeftNear.y, topLeftNear.z) * -1, Vector3(topRightNear.x, topRightNear.y, topRightNear.z) * -1);

	//	Gizmo::DrawLine(Vector3(bottomLeftFar.x, bottomLeftFar.y, bottomLeftFar.z) * -1, Vector3(bottomRightFar.x, bottomRightFar.y, bottomRightFar.z) * -1);
	//	Gizmo::DrawLine(Vector3(bottomLeftNear.x, bottomLeftNear.y, bottomLeftNear.z) * -1, Vector3(bottomRightNear.x, bottomRightNear.y, bottomRightNear.z) * -1);

	//	Gizmo::DrawLine(Vector3(bottomLeftFar.x, bottomLeftFar.y, bottomLeftFar.z) * -1, Vector3(topLeftFar.x, topLeftFar.y, topLeftFar.z) * -1);
	//	Gizmo::DrawLine(Vector3(bottomRightFar.x, bottomRightFar.y, bottomRightFar.z) * -1, Vector3(topRightFar.x, topRightFar.y, topRightFar.z) * -1);

	//	Gizmo::DrawLine(Vector3(bottomLeftNear.x, bottomLeftNear.y, bottomLeftNear.z) * -1, Vector3(topLeftNear.x, topLeftNear.y, topLeftNear.z) * -1);
	//	Gizmo::DrawLine(Vector3(bottomRightNear.x, bottomRightNear.y, bottomRightNear.z) * -1, Vector3(topRightNear.x, topRightNear.y, topRightNear.z) * -1);
	//}
#endif
}
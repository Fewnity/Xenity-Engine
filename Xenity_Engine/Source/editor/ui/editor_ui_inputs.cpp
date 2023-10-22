#if defined(EDITOR)

#include "editor_ui.h"
#include "../editor.h"
#include "../../xenity.h"
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_stdlib.h>
#include "../../engine/tools/shape_spawner.h"
#include <variant>
#include <imgui/imgui_internal.h>
#include "../../engine/asset_management/project_manager.h"
#include <Windows.h>
//#include <Commdlg.h>
#include <ShObjIdl.h>
#include "../../engine/graphics/skybox.h"

void EditorUI::DrawInputTitle(const std::string& title)
{
	ImGui::Text(title.c_str());
	ImGui::SameLine();
	ImGui::SetCursorPosX(150 * uiScale);
	ImGui::SetNextItemWidth(-1);
}

bool EditorUI::DrawInput(const std::string& inputName, float& value)
{
	DrawInputTitle(inputName);
	float oldValue = float(value);
	ImGui::InputFloat(GenerateItemId().c_str(), &value, 0, 0, "%.4f");
	return value != oldValue;
}

bool EditorUI::DrawInput(const std::string& inputName, double& value)
{
	DrawInputTitle(inputName);
	double oldValue = double(value);
	ImGui::InputDouble(GenerateItemId().c_str(), &value, 0, 0, "%0.8f");
	return value != oldValue;
}

bool EditorUI::DrawInput(const std::string& inputName, std::string& value)
{
	DrawInputTitle(inputName);
	std::string oldValue = std::string(value);
	ImGui::InputText(GenerateItemId().c_str(), &value);
	return value != oldValue;
}

bool EditorUI::DrawInput(const std::string& inputName, int& value)
{
	DrawInputTitle(inputName);
	int oldValue = int(value);
	ImGui::InputInt(GenerateItemId().c_str(), &value);
	return value != oldValue;
}

bool EditorUI::DrawInput(const std::string& inputName, bool& value)
{
	DrawInputTitle(inputName);
	bool oldValue = bool(value);
	ImGui::Checkbox(GenerateItemId().c_str(), &value);
	return value != oldValue;
}

bool EditorUI::DrawInput(const std::string& inputName, std::weak_ptr<Component>& value)
{
	std::shared_ptr<Component> oldValue = value.lock();

	std::string inputText = "None (Component)";
	auto ptr = value.lock();
	if (ptr != nullptr)
	{
		inputText = ptr->GetGameObject()->name;
		inputText += " " + std::to_string(ptr->GetUniqueId());
	}

	if (DrawInputButton(inputName, inputText, true) == 2)
	{
		value.reset();
	}

	std::shared_ptr<Component> ref = nullptr;
	std::string payloadName = "Component";
	if (DragDropTarget(payloadName, ref))
	{
		value = ref;
	}

	return oldValue != value.lock();
}

bool EditorUI::DrawInput(const std::string& inputName, std::weak_ptr<Collider>& value)
{
	std::shared_ptr<Collider> oldValue = value.lock();

	std::string inputText = "None (Collider)";
	auto ptr = value.lock();
	if (ptr != nullptr)
	{
		inputText = ptr->GetGameObject()->name;
		inputText += " " + std::to_string(ptr->GetUniqueId());
	}

	if (DrawInputButton(inputName, inputText, true) == 2)
	{
		value.reset();
	}

	std::shared_ptr<Collider> ref = nullptr;
	std::string payloadName = "Collider";
	if (DragDropTarget(payloadName, ref))
	{
		value = ref;
	}

	return oldValue != value.lock();
}

bool EditorUI::DrawInput(const std::string& inputName, std::weak_ptr<Transform>& value)
{
	std::shared_ptr<Transform> oldValue = value.lock();

	std::string inputText = "None (Transform)";
	auto ptr = value.lock();
	if (ptr != nullptr)
	{
		inputText = ptr->GetGameObject()->name;
		inputText += " " + std::to_string(ptr->GetGameObject()->GetUniqueId());
	}

	if (DrawInputButton(inputName, inputText, true) == 2)
	{
		value.reset();
	}

	std::shared_ptr <Transform> ref = nullptr;
	std::string payloadName = "Transform";
	if (DragDropTarget(payloadName, ref))
	{
		value = ref;
	}

	return oldValue != value.lock();
}

bool EditorUI::DrawInput(const std::string& inputName, std::weak_ptr<GameObject>& value)
{
	std::shared_ptr<GameObject> oldValue = value.lock();

	std::string inputText = "None (GameObject)";
	auto ptr = value.lock();
	if (ptr != nullptr)
	{
		inputText = ptr->name;
		inputText += " " + std::to_string(ptr->GetUniqueId());
	}

	if (DrawInputButton(inputName, inputText, true) == 2)
	{
		value.reset();
	}

	std::shared_ptr <GameObject> ref = nullptr;
	std::string payloadName = "GameObject";
	if (DragDropTarget(payloadName, ref))
	{
		value = ref;
	}

	return oldValue != value.lock();
}

bool EditorUI::DrawInput(const std::string& inputName, std::shared_ptr<SkyBox>& value)
{
	bool changed = false;
	auto ref = std::ref(value);
	DrawFileReference(File_Skybox, "Skybox", &ref, changed, inputName);
	return changed;
}

bool EditorUI::DrawInput(const std::string& inputName, Color& value)
{
	ImGui::Text(inputName.c_str());
	ImGui::SameLine();
	ImGui::SetCursorPosX(150 * uiScale);
	Vector4 vec4 = value.GetRGBA().ToVector4();
	ImVec4 color = ImVec4(vec4.x, vec4.y, vec4.z, vec4.w);
	float startAvailSize = ImGui::GetContentRegionAvail().x;
	ImGui::SetNextItemWidth(startAvailSize);

	ImGui::ColorEdit4(GenerateItemId().c_str(), (float*)&color, ImGuiColorEditFlags_NoInputs);
	value.SetFromRGBAfloat(color.x, color.y, color.z, color.w);

	bool valueChanged = false;
	if (vec4.x != color.x || vec4.y != color.y || vec4.z != color.z || vec4.w != color.w)
		valueChanged = true;

	return valueChanged;
}

bool EditorUI::DrawInput(const std::string& inputName, Vector2& value)
{
	Vector2 oldValue = Vector2(value);

	ImGui::Text(inputName.c_str());
	ImGui::SameLine();
	ImGui::SetCursorPosX(150 * uiScale);

	if (ImGui::BeginTable("table", 2, 0))
	{
		ImGui::TableNextRow();
		DrawTableInput("X", GenerateItemId(), 0, value.x);
		DrawTableInput("Y", GenerateItemId(), 1, value.y);
		ImGui::EndTable();
	}

	return value != oldValue;
}

bool EditorUI::DrawInput(const std::string& inputName, Vector2Int& value)
{
	Vector2Int oldValue = Vector2Int(value);

	ImGui::Text(inputName.c_str());
	ImGui::SameLine();
	ImGui::SetCursorPosX(150 * uiScale);

	if (ImGui::BeginTable("table", 2, 0))
	{
		ImGui::TableNextRow();
		DrawTableInput("X", GenerateItemId(), 0, value.x);
		DrawTableInput("Y", GenerateItemId(), 1, value.y);
		ImGui::EndTable();
	}

	return value != oldValue;
}

bool EditorUI::DrawInput(const std::string& inputName, Vector3& value)
{
	Vector3 oldValue = Vector3(value);

	ImGui::Text(inputName.c_str());
	ImGui::SameLine();
	ImGui::SetCursorPosX(150 * uiScale);

	if (ImGui::BeginTable("table", 3, 0))
	{
		ImGui::TableNextRow();
		DrawTableInput("X", GenerateItemId(), 0, value.x);
		DrawTableInput("Y", GenerateItemId(), 1, value.y);
		DrawTableInput("Z", GenerateItemId(), 2, value.z);
		ImGui::EndTable();
	}

	return value != oldValue;
}

bool EditorUI::DrawInput(const std::string& inputName, Vector4& value)
{
	Vector4 oldValue = Vector4(value);

	ImGui::Text(inputName.c_str());
	ImGui::SameLine();
	ImGui::SetCursorPosX(150 * uiScale);

	if (ImGui::BeginTable("table", 4, 0))
	{
		ImGui::TableNextRow();
		DrawTableInput("X", GenerateItemId(), 0, value.x);
		DrawTableInput("Y", GenerateItemId(), 1, value.y);
		DrawTableInput("Z", GenerateItemId(), 2, value.z);
		DrawTableInput("W", GenerateItemId(), 3, value.w);
		ImGui::EndTable();
	}

	return value != oldValue;
}

#endif
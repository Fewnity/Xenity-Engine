// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#if defined(EDITOR)

//#include <Windows.h>
//#include <ShObjIdl.h>

// ImGui
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>

#include <editor/editor.h>
#include <editor/ui/editor_ui.h>

#include <engine/file_system/file.h>
#include <engine/physics/collider.h>
#include <engine/graphics/color/color.h>
#include <engine/graphics/skybox.h>
#include <engine/vectors/vector2.h>
#include <engine/vectors/vector2_int.h>
#include <engine/vectors/vector2.h>
#include <engine/vectors/vector3.h>

void EditorUI::DrawInputTitle(const std::string& title)
{
	float lastCursorX = ImGui::GetCursorPosX();
	ImGui::Text("%s", title.c_str());
	ImGui::SameLine();
	ImGui::SetCursorPosX(lastCursorX + 150 * uiScale);
	ImGui::SetNextItemWidth(-1);
}

#pragma region Old inputs

#pragma endregion

#pragma region New Inputs

ValueInputState EditorUI::DrawEnum(const std::string& inputName, int& newValue, uint64_t enumType)
{
	ValueInputState state = ValueInputState::NO_CHANGE;
	DrawInputTitle(inputName);
	int value = newValue;
	const int oldValue = int(value);
	std::vector<EnumValueName>& names = EnumHelper::GetEnumStringsLists()[enumType];
	std::vector<EnumValueName> mergednames;

	const size_t enumSize = names.size();
	std::string comboTitle = "";
	bool isFirst = true;

	int mergedNameSize = 0;
	for (size_t enumIndex = 0; enumIndex < enumSize; enumIndex++)
	{
		const EnumValueName& currentName = names[enumIndex];
		// If the enum is selected, add the name to the combo's title
		if (value == currentName.value)
		{
			if (!isFirst)
				comboTitle += " | ";
			else
				isFirst = false;

			comboTitle += currentName.name;
		}

		// If there are enums with the same value, merge their names
		bool found = false;
		for (int j = 0; j < mergedNameSize; j++)
		{
			if (mergednames[j].value == currentName.value)
			{
				mergednames[j].name += " | " + currentName.name;
				found = true;
				break;
			}
		}
		if (!found)
		{
			EnumValueName newMergedName = EnumValueName();
			newMergedName.name = currentName.name;
			newMergedName.value = currentName.value;
			mergednames.push_back(newMergedName);
			mergedNameSize++;
		}
	}

	// Draw combo list
	if (ImGui::BeginCombo(GenerateItemId().c_str(), comboTitle.c_str()))
	{
		for (const auto& var : mergednames)
		{
			if (ImGui::Selectable(var.name.c_str()))
			{
				value = var.value;
			}

			if (value == var.value) 
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}

	newValue = value;
	if (value != oldValue)
		state = ValueInputState::APPLIED;
	return state;
}

ValueInputState EditorUI::DrawInput(const std::string& inputName, Vector2& newValue)
{
	ValueInputState state = ValueInputState::NO_CHANGE;
	Vector2 value = newValue;
	const Vector2 oldValue = Vector2(value);

	DrawInputTitle(inputName);

	if (ImGui::BeginTable("table", 2, 0))
	{
		ImGui::TableNextRow();
		DrawTableInput("X", GenerateItemId(), 0, value.x);
		DrawTableInput("Y", GenerateItemId(), 1, value.y);
		ImGui::EndTable();
	}
	newValue = value;
	if (value != oldValue)
		state = ValueInputState::APPLIED;
	return state;
}

ValueInputState EditorUI::DrawInput(const std::string& inputName, Vector2Int& newValue)
{
	ValueInputState state = ValueInputState::NO_CHANGE;
	Vector2Int value = newValue;
	const Vector2Int oldValue = Vector2Int(value);

	DrawInputTitle(inputName);

	if (ImGui::BeginTable("table", 2, 0))
	{
		ImGui::TableNextRow();
		DrawTableInput("X", GenerateItemId(), 0, value.x);
		DrawTableInput("Y", GenerateItemId(), 1, value.y);
		ImGui::EndTable();
	}
	newValue = value;
	if (value != oldValue)
		state = ValueInputState::APPLIED;
	return state;
}

ValueInputState EditorUI::DrawInput(const std::string& inputName, Vector3& newValue)
{
	ValueInputState state = ValueInputState::NO_CHANGE;
	Vector3 value = newValue;
	const Vector3 oldValue = Vector3(value);

	DrawInputTitle(inputName);

	if (ImGui::BeginTable("table", 3, 0))
	{
		ImGui::TableNextRow();
		DrawTableInput("X", GenerateItemId(), 0, value.x);
		DrawTableInput("Y", GenerateItemId(), 1, value.y);
		DrawTableInput("Z", GenerateItemId(), 2, value.z);
		ImGui::EndTable();
	}
	newValue = value;
	if (value != oldValue)
		state = ValueInputState::APPLIED;
	return state;
}

ValueInputState EditorUI::DrawInput(const std::string& inputName,Vector4& newValue)
{
	ValueInputState state = ValueInputState::NO_CHANGE;
	Vector4 value = newValue;
	const Vector4 oldValue = Vector4(value);

	DrawInputTitle(inputName);

	if (ImGui::BeginTable("table", 4, 0))
	{
		ImGui::TableNextRow();
		DrawTableInput("X", GenerateItemId(), 0, value.x);
		DrawTableInput("Y", GenerateItemId(), 1, value.y);
		DrawTableInput("Z", GenerateItemId(), 2, value.z);
		DrawTableInput("W", GenerateItemId(), 3, value.w);
		ImGui::EndTable();
	}
	newValue = value;
	if(value != oldValue)
		state = ValueInputState::APPLIED;
	return state;
}

bool EditorUI::DrawInput(const std::string& inputName, std::weak_ptr<Component>& newValue, uint64_t typeId)
{
	std::weak_ptr<Component> value = newValue;
	const std::shared_ptr<Component> oldValue = value.lock();

	std::string inputText;
	const auto ptr = value.lock();
	if (ptr != nullptr)
	{
		inputText = ptr->GetGameObject()->GetName();
		inputText += " " + std::to_string(ptr->GetUniqueId());
	}
	else 
	{

		inputText = "None (" + ClassRegistry::GetClassNameById(typeId) + ")";
	}

	InputButtonState result = DrawInputButton(inputName, inputText, true);
	if (result == InputButtonState::ResetValue)
	{
		value.reset();
	}

	std::shared_ptr<Component> ref = nullptr;
	const std::string payloadName = "Type" + std::to_string(typeId);
	if (DragDropTarget(payloadName, ref))
	{
		value = ref;
	}
	newValue = value;
	return oldValue != value.lock();
}

bool EditorUI::DrawInput(const std::string& inputName, std::weak_ptr<Collider>& newValue, uint64_t typeId)
{
	std::weak_ptr<Collider> value = newValue;
	const std::shared_ptr<Collider> oldValue = value.lock();

	std::string inputText = "None (Collider)";
	const auto ptr = value.lock();
	if (ptr != nullptr)
	{
		inputText = ptr->GetGameObject()->GetName();
		inputText += " " + std::to_string(ptr->GetUniqueId());
	}

	InputButtonState result = DrawInputButton(inputName, inputText, true);
	if (result == InputButtonState::ResetValue)
	{
		value.reset();
	}

	std::shared_ptr<Collider> ref = nullptr;
	const std::string payloadName = "Type" + std::to_string(typeId);
	if (DragDropTarget(payloadName, ref))
	{
		value = ref;
	}
	newValue = value;
	return oldValue != value.lock();
}

bool EditorUI::DrawInput(const std::string& inputName, std::weak_ptr<Transform>& newValue, uint64_t typeId)
{
	std::weak_ptr<Transform> value = newValue;
	const std::shared_ptr<Transform> oldValue = value.lock();

	std::string inputText = "None (Transform)";
	const auto ptr = value.lock();
	if (ptr != nullptr)
	{
		inputText = ptr->GetGameObject()->GetName();
		inputText += " " + std::to_string(ptr->GetGameObject()->GetUniqueId());
	}

	InputButtonState result = DrawInputButton(inputName, inputText, true);
	if (result == InputButtonState::ResetValue)
	{
		value.reset();
	}

	std::shared_ptr <Transform> ref = nullptr;
	const std::string payloadName = "Type" + std::to_string(typeId);
	if (DragDropTarget(payloadName, ref))
	{
		value = ref;
	}
	newValue = value;
	return oldValue != value.lock();
}

bool EditorUI::DrawInput(const std::string& inputName, std::weak_ptr<GameObject>& newValue, uint64_t typeId)
{
	std::weak_ptr<GameObject> value = newValue;
	const std::shared_ptr<GameObject> oldValue = value.lock();

	std::string inputText = "None (GameObject)";
	auto ptr = value.lock();
	if (ptr != nullptr)
	{
		inputText = ptr->GetName();
		inputText += " " + std::to_string(ptr->GetUniqueId());
	}

	InputButtonState result = DrawInputButton(inputName, inputText, true);
	if (result == InputButtonState::ResetValue)
	{
		value.reset();
	}

	std::shared_ptr <GameObject> ref = nullptr;
	const std::string payloadName = "Type" + std::to_string(typeId);
	if (DragDropTarget(payloadName, ref))
	{
		value = ref;
	}
	newValue = value;
	return oldValue != value.lock();
}

ValueInputState EditorUI::DrawInput(const std::string& inputName, Color& newValue)
{
	ValueInputState state = ValueInputState::NO_CHANGE;
	DrawInputTitle(inputName);

	const Vector4 vec4 = newValue.GetRGBA().ToVector4();
	ImVec4 color = ImVec4(vec4.x, vec4.y, vec4.z, vec4.w);

	bool isEdited = ImGui::ColorEdit4(GenerateItemId().c_str(), (float*)&color, ImGuiColorEditFlags_NoInputs);

	if (ImGui::IsItemClicked()) 
	{
		state = ValueInputState::ON_OPEN;
	}
	if (ImGui::IsItemDeactivatedAfterEdit())
	{
		state = ValueInputState::APPLIED;
	}
	else if (isEdited) 
	{
		state = ValueInputState::CHANGED;
	}

	newValue.SetFromRGBAfloat(color.x, color.y, color.z, color.w);


	/*bool valueChanged = false;
	if (vec4.x != color.x || vec4.y != color.y || vec4.z != color.z || vec4.w != color.w)
		valueChanged = true;*/
	return state;
}

#pragma endregion



#endif
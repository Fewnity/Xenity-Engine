// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#if defined(EDITOR)

// ImGui
#include <imgui/imgui.h>

#include <editor/ui/editor_ui.h>

#include <engine/physics/collider.h>
#include <engine/graphics/color/color.h>
#include <engine/vectors/vector2.h>
#include <engine/vectors/vector2_int.h>
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
		//inputText += " " + std::to_string(ptr->GetUniqueId()); // For debugging
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
		//inputText += " " + std::to_string(ptr->GetUniqueId()); // For debugging
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
		//inputText += " " + std::to_string(ptr->GetGameObject()->GetUniqueId()); // For debugging
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
		//inputText += " " + std::to_string(ptr->GetUniqueId()); // For debugging
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

	newValue.SetFromRGBAFloat(color.x, color.y, color.z, color.w);


	/*bool valueChanged = false;
	if (vec4.x != color.x || vec4.y != color.y || vec4.z != color.z || vec4.w != color.w)
		valueChanged = true;*/
	return state;
}

ValueInputState EditorUI::DrawReflectiveData(ReflectiveDataToDraw& reflectiveDataToDraw, const ReflectiveData& myMap, Event<>* _onValueChangedEvent)
{
	ValueInputState valueInputState = ValueInputState::NO_CHANGE;
	onValueChangedEvent = _onValueChangedEvent;
	for (const ReflectiveEntry& reflectionEntry : myMap)
	{
		if (reflectionEntry.isPublic)
		{
			reflectiveDataToDraw.currentEntry = reflectionEntry;
			reflectiveDataToDraw.name = GetPrettyVariableName(reflectionEntry.variableName);
			reflectiveDataToDraw.entryStack.push_back(reflectionEntry);
			reflectiveDataToDraw.reflectiveDataStack.push_back(myMap);

			ValueInputState tempValueInputState = ValueInputState::NO_CHANGE;
			std::visit([&tempValueInputState, &reflectiveDataToDraw](auto& value)
				{
					tempValueInputState = DrawVariable(reflectiveDataToDraw, value);
				}, reflectiveDataToDraw.currentEntry.variable.value());

			reflectiveDataToDraw.entryStack.erase(reflectiveDataToDraw.entryStack.end() - 1);
			reflectiveDataToDraw.reflectiveDataStack.erase(reflectiveDataToDraw.reflectiveDataStack.end() - 1);

			if (tempValueInputState != ValueInputState::NO_CHANGE)
			{
				valueInputState = tempValueInputState;
			}
		}
	}
	if (onValueChangedEvent && valueInputState != ValueInputState::NO_CHANGE)
	{
		onValueChangedEvent->Trigger();
	}
	ImGui::Separator();
	/*if(valueChanged)
		valueInputState = ValueInputState::APPLIED;*/
	return valueInputState;
}

bool EditorUI::DrawVector(ReflectiveDataToDraw& reflectiveDataToDraw, const std::string& className, const std::reference_wrapper<std::vector<Reflective*>> valuePtr)
{
	bool valueChanged = false;
	const std::string headerName = reflectiveDataToDraw.name + "##ListHeader" + std::to_string((uint64_t)&valuePtr.get());
	if (ImGui::CollapsingHeader(headerName.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed))
	{
		const size_t vectorSize = valuePtr.get().size();

		const std::string tempName = reflectiveDataToDraw.name;
		reflectiveDataToDraw.name = "";
		ReflectiveEntry temp = reflectiveDataToDraw.currentEntry;
		for (size_t vectorI = 0; vectorI < vectorSize; vectorI++)
		{
			Reflective* ptr = valuePtr.get()[vectorI];
			if (ptr)
			{
				ValueInputState valueInputState = ValueInputState::NO_CHANGE;
				////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// Empty name normally here!!!!
				if (auto val = dynamic_cast<Vector2*>(ptr)) // Specific draw
				{
					valueInputState = DrawInputReflective(reflectiveDataToDraw, val);
				}
				else if (auto val = dynamic_cast<Vector2Int*>(ptr)) // Specific draw
				{
					valueInputState = DrawInputReflective(reflectiveDataToDraw, val);
				}
				else if (auto val = dynamic_cast<Vector3*>(ptr)) // Specific draw
				{
					valueInputState = DrawInputReflective(reflectiveDataToDraw, val);
				}
				else if (auto val = dynamic_cast<Vector4*>(ptr)) // Specific draw
				{
					valueInputState = DrawInputReflective(reflectiveDataToDraw, val);
				}
				else if (auto val = dynamic_cast<Color*>(ptr)) // Specific draw
				{
					valueInputState = DrawInputReflective(reflectiveDataToDraw, val);
				}
				else //Basic draw
				{
					const std::string headerName = tempName + "##ListHeader" + std::to_string((uint64_t)ptr);
					if (ImGui::CollapsingHeader(headerName.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed))
					{
						valueInputState = DrawReflectiveData(reflectiveDataToDraw, ptr->GetReflectiveData(), nullptr);
					}
				}
				if (valueInputState != ValueInputState::NO_CHANGE)
				{
					valueChanged = true;
				}
			}
			else
			{
				ImGui::Text("Null element");
			}
		}
		reflectiveDataToDraw.name = tempName;
		const std::string addText = "Add " + GenerateItemId();
		if (ImGui::Button(addText.c_str()))
		{
			valuePtr.get().push_back((Reflective*)temp.typeSpawner->Allocate());
			valueChanged = true;
		}

		const std::string removeText = "Remove " + GenerateItemId();
		if (ImGui::Button(removeText.c_str()))
		{
			if (vectorSize != 0)
			{
				delete valuePtr.get()[vectorSize - 1];
				valuePtr.get().erase(valuePtr.get().begin() + vectorSize - 1);
				valueChanged = true;
			}
		}
		ImGui::Separator();
	}
	return valueChanged;
}

ValueInputState EditorUI::DrawVariable(ReflectiveDataToDraw& reflectiveDataToDraw, const std::reference_wrapper<std::vector<Reflective*>> valuePtr)
{
	ValueInputState valueInputState = ValueInputState::NO_CHANGE;
	bool valueChangedTemp = false;
	valueChangedTemp = DrawVector(reflectiveDataToDraw, "Reflective", valuePtr);

	if (valueChangedTemp)
		valueInputState = ValueInputState::APPLIED;
	return valueInputState;
}

ValueInputState EditorUI::DrawVariable(ReflectiveDataToDraw& reflectiveDataToDraw, const std::reference_wrapper<Reflective> valuePtr)
{
	ValueInputState valueInputState = ValueInputState::NO_CHANGE;

	if (auto val = dynamic_cast<Vector2*>(&valuePtr.get())) // Specific draw
	{
		valueInputState = DrawInputReflective(reflectiveDataToDraw, val);
	}
	else if (auto val = dynamic_cast<Vector2Int*>(&valuePtr.get())) // Specific draw
	{
		valueInputState = DrawInputReflective(reflectiveDataToDraw, val);
	}
	else if (auto val = dynamic_cast<Vector3*>(&valuePtr.get())) // Specific draw
	{
		valueInputState = DrawInputReflective(reflectiveDataToDraw, val);
	}
	else if (auto val = dynamic_cast<Vector4*>(&valuePtr.get())) // Specific draw
	{
		valueInputState = DrawInputReflective(reflectiveDataToDraw, val);
	}
	else if (auto val = dynamic_cast<Color*>(&valuePtr.get())) // Specific draw
	{
		valueInputState = DrawInputReflective(reflectiveDataToDraw, val);
	}
	else //Basic draw
	{
		const std::string headerName = reflectiveDataToDraw.name + "##ListHeader" + std::to_string((uint64_t) & (valuePtr.get()));
		if (ImGui::CollapsingHeader(headerName.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed))
		{
			valueInputState = DrawReflectiveData(reflectiveDataToDraw, valuePtr.get().GetReflectiveData(), nullptr);
		}
	}

	/*if(valueInputState != ValueInputState::NO_CHANGE)
		valueChangedTemp = true;*/

	return valueInputState;
}

#pragma endregion



#endif
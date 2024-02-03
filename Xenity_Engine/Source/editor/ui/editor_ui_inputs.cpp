#if defined(EDITOR)

#include <Windows.h>
#include <ShObjIdl.h>

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
	ImGui::Text(title.c_str());
	ImGui::SameLine();
	ImGui::SetCursorPosX(150 * uiScale);
	ImGui::SetNextItemWidth(-1);
}

#pragma region Old inputs

bool EditorUI::DrawInput(const std::string& inputName, float& value)
{
	DrawInputTitle(inputName);
	const float oldValue = float(value);
	ImGui::InputFloat(GenerateItemId().c_str(), &value, 0, 0, "%.4f");
	return value != oldValue;
}

bool EditorUI::DrawInput(const std::string& inputName, double& value)
{
	DrawInputTitle(inputName);
	const double oldValue = double(value);
	ImGui::InputDouble(GenerateItemId().c_str(), &value, 0, 0, "%0.8f");
	return value != oldValue;
}

bool EditorUI::DrawInput(const std::string& inputName, std::string& value)
{
	DrawInputTitle(inputName);
	const std::string oldValue = std::string(value);
	ImGui::InputText(GenerateItemId().c_str(), &value);
	return value != oldValue;
}

bool EditorUI::DrawInput(const std::string& inputName, int& value)
{
	DrawInputTitle(inputName);
	const int oldValue = int(value);
	ImGui::InputInt(GenerateItemId().c_str(), &value);
	return value != oldValue;
}

bool EditorUI::DrawInput(const std::string& inputName, bool& value)
{
	DrawInputTitle(inputName);
	const bool oldValue = bool(value);
	ImGui::Checkbox(GenerateItemId().c_str(), &value);
	return value != oldValue;
}

bool EditorUI::DrawInput(const std::string& inputName, std::weak_ptr<Component>& value, uint64_t typeId)
{
	const std::shared_ptr<Component> oldValue = value.lock();

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
	const std::string payloadName = "Type" + std::to_string(typeId);
	if (DragDropTarget(payloadName, ref))
	{
		value = ref;
	}

	return oldValue != value.lock();
}

bool EditorUI::DrawInput(const std::string& inputName, std::weak_ptr<Collider>& value, uint64_t typeId)
{
	const std::shared_ptr<Collider> oldValue = value.lock();

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
	const std::string payloadName = "Type" + std::to_string(typeId);
	if (DragDropTarget(payloadName, ref))
	{
		value = ref;
	}

	return oldValue != value.lock();
}

bool EditorUI::DrawInput(const std::string& inputName, std::weak_ptr<Transform>& value, uint64_t typeId)
{
	const std::shared_ptr<Transform> oldValue = value.lock();

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
	const std::string payloadName = "Type" + std::to_string(typeId);
	if (DragDropTarget(payloadName, ref))
	{
		value = ref;
	}

	return oldValue != value.lock();
}

bool EditorUI::DrawInput(const std::string& inputName, std::weak_ptr<GameObject>& value, uint64_t typeId)
{
	const std::shared_ptr<GameObject> oldValue = value.lock();

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
	const std::string payloadName = "Type" + std::to_string(typeId);
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
	DrawFileReference(FileType::File_Skybox, "Skybox", &ref, changed, inputName);
	return changed;
}

bool EditorUI::DrawInput(const std::string& inputName, Color& value)
{
	ImGui::Text(inputName.c_str());
	ImGui::SameLine();
	ImGui::SetCursorPosX(150 * uiScale);
	const Vector4 vec4 = value.GetRGBA().ToVector4();
	ImVec4 color = ImVec4(vec4.x, vec4.y, vec4.z, vec4.w);
	const float startAvailSize = ImGui::GetContentRegionAvail().x;
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
	const Vector2 oldValue = Vector2(value);

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
	const Vector2Int oldValue = Vector2Int(value);

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
	const Vector3 oldValue = Vector3(value);

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
	const Vector4 oldValue = Vector4(value);

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

#pragma endregion

#pragma region New Inputs

bool EditorUI::DrawInput(const std::string& inputName, float value, float& newValue)
{
	DrawInputTitle(inputName);
	const float oldValue = float(value);
	ImGui::InputFloat(GenerateItemId().c_str(), &value, 0, 0, "%.4f");
	newValue = value;
	return value != oldValue;
}

bool EditorUI::DrawInput(const std::string& inputName, double value, double& newValue)
{
	DrawInputTitle(inputName);
	const double oldValue = double(value);
	ImGui::InputDouble(GenerateItemId().c_str(), &value, 0, 0, "%0.8f");
	newValue = value;
	return value != oldValue;
}

bool EditorUI::DrawInput(const std::string& inputName, std::string value, std::string& newValue)
{
	DrawInputTitle(inputName);
	const std::string oldValue = std::string(value);
	ImGui::InputText(GenerateItemId().c_str(), &value);
	newValue = value;
	return value != oldValue;
}

bool EditorUI::DrawInput(const std::string& inputName, int value, int& newValue)
{
	DrawInputTitle(inputName);
	const int oldValue = int(value);
	ImGui::InputInt(GenerateItemId().c_str(), &value);
	newValue = value;
	return value != oldValue;
}

bool EditorUI::DrawEnum(const std::string& inputName, int value, int& newValue, uint64_t enumType)
{
	DrawInputTitle(inputName);
	const int oldValue = int(value);
	std::vector<EnumValueName>& names = EnumHelper::GetEnumStringsLists()[enumType];
	std::vector<EnumValueName> mergednames;

	int enumSize = names.size();
	std::string comboTitle = "";
	bool isFirst = true;

	int mergedNameSize = 0;
	for (int enumIndex = 0; enumIndex < enumSize; enumIndex++)
	{
		EnumValueName& currentName = names[enumIndex];
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
		for (auto& var : mergednames)
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
	return value != oldValue;
}

bool EditorUI::DrawInput(const std::string& inputName, bool value, bool& newValue)
{
	DrawInputTitle(inputName);
	const bool oldValue = bool(value);
	ImGui::Checkbox(GenerateItemId().c_str(), &value);
	newValue = value;
	return value != oldValue;
}

bool EditorUI::DrawInput(const std::string& inputName, Vector2 value, Vector2& newValue)
{
	const Vector2 oldValue = Vector2(value);

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
	newValue = value;
	return value != oldValue;
}

bool EditorUI::DrawInput(const std::string& inputName, Vector2Int value, Vector2Int& newValue)
{
	const Vector2Int oldValue = Vector2Int(value);

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
	newValue = value;
	return value != oldValue;
}

bool EditorUI::DrawInput(const std::string& inputName, Vector3 value, Vector3& newValue)
{
	const Vector3 oldValue = Vector3(value);

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
	newValue = value;
	return value != oldValue;
}

bool EditorUI::DrawInput(const std::string& inputName, Vector4 value, Vector4& newValue)
{
	const Vector4 oldValue = Vector4(value);

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
	newValue = value;
	return value != oldValue;
}

bool EditorUI::DrawInput(const std::string& inputName, std::weak_ptr<Component> value, std::weak_ptr<Component>& newValue, uint64_t typeId)
{
	const std::shared_ptr<Component> oldValue = value.lock();

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
	const std::string payloadName = "Type" + std::to_string(typeId);
	if (DragDropTarget(payloadName, ref))
	{
		value = ref;
	}
	newValue = value;
	return oldValue != value.lock();
}

bool EditorUI::DrawInput(const std::string& inputName, std::weak_ptr<Collider> value, std::weak_ptr<Collider>& newValue, uint64_t typeId)
{
	const std::shared_ptr<Collider> oldValue = value.lock();

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
	const std::string payloadName = "Type" + std::to_string(typeId);
	if (DragDropTarget(payloadName, ref))
	{
		value = ref;
	}
	newValue = value;
	return oldValue != value.lock();
}

bool EditorUI::DrawInput(const std::string& inputName, std::weak_ptr<Transform> value, std::weak_ptr<Transform>& newValue, uint64_t typeId)
{
	const std::shared_ptr<Transform> oldValue = value.lock();

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
	const std::string payloadName = "Type" + std::to_string(typeId);
	if (DragDropTarget(payloadName, ref))
	{
		value = ref;
	}
	newValue = value;
	return oldValue != value.lock();
}

bool EditorUI::DrawInput(const std::string& inputName, std::weak_ptr<GameObject> value, std::weak_ptr<GameObject>& newValue, uint64_t typeId)
{
	const std::shared_ptr<GameObject> oldValue = value.lock();

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
	const std::string payloadName = "Type" + std::to_string(typeId);
	if (DragDropTarget(payloadName, ref))
	{
		value = ref;
	}
	newValue = value;
	return oldValue != value.lock();
}

//bool EditorUI::DrawInput(const std::string& inputName, std::shared_ptr<SkyBox> value, std::weak_ptr<SkyBox>& newValue)
//{
//	bool changed = false;
//	auto ref = std::ref(value);
//	DrawFileReference(File_Skybox, "Skybox", &ref, changed, inputName);
//	newValue = value;
//	return changed;
//}

bool EditorUI::DrawInput(const std::string& inputName, Color value, Color& newValue)
{
	ImGui::Text(inputName.c_str());
	ImGui::SameLine();
	ImGui::SetCursorPosX(150 * uiScale);
	const Vector4 vec4 = value.GetRGBA().ToVector4();
	ImVec4 color = ImVec4(vec4.x, vec4.y, vec4.z, vec4.w);
	const float startAvailSize = ImGui::GetContentRegionAvail().x;
	ImGui::SetNextItemWidth(startAvailSize);

	ImGui::ColorEdit4(GenerateItemId().c_str(), (float*)&color, ImGuiColorEditFlags_NoInputs);
	value.SetFromRGBAfloat(color.x, color.y, color.z, color.w);

	bool valueChanged = false;
	if (vec4.x != color.x || vec4.y != color.y || vec4.z != color.z || vec4.w != color.w)
		valueChanged = true;
	newValue = value;
	return valueChanged;
}

#pragma endregion



#endif
#if defined(_WIN32) || defined(_WIN64)

#include "editor_ui.h"
#include "../editor.h"
#include "../../xenity.h"
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_stdlib.h>
#include "../../engine/tools/shape_spawner.h"
#include <variant>
#include <imgui/imgui_internal.h>

int EditorUI::uiId = 0;
bool EditorUI::showProfiler = true;
bool EditorUI::showEditor = true;
bool EditorUI::showEngineSettings = false;
Texture* EditorUI::folderIcon = nullptr;
Texture* EditorUI::fileIcon = nullptr;
Texture* EditorUI::sceneIcon = nullptr;
float EditorUI::uiScale = 1;

#pragma region Initialisation

void EditorUI::Init()
{
	//Set round corners
	ImGui::GetStyle().WindowRounding = 10;

	ImGuiIO& io = ImGui::GetIO();
	io.Fonts->AddFontFromFileTTF("Roboto Regular.ttf", 30);

	fileIcon = new Texture("icons/text.png", "", true);
	folderIcon = new Texture("icons/folder.png", "", true);
	sceneIcon = new Texture("icons/belt.png", "", true);

	Debug::Print("---- Editor UI initiated ----");
}

void EditorUI::Draw()
{
	NewFrame();
	Render();
}

#pragma endregion

#pragma region Update

std::string EditorUI::GetPrettyVariableName(std::string variableName)
{
	variableName[0] = toupper(variableName[0]);
	int nameSize = variableName.size();
	bool addSpace = false;
	for (int i = 1; i < nameSize - 1; i++)
	{
		if (isupper(variableName[i]))
		{
			if (addSpace)
				addSpace = false;
			else
				addSpace = true;
		}
		else if (addSpace)
		{
			variableName.insert(i - 1, " ");
			nameSize++;
			i++;
			addSpace = false;
		}
	}
	return variableName;
}

/**
* Create a new frame for the editor's UI
*/
void EditorUI::NewFrame()
{
	EditorUI::UpdateUIScale();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
	uiId = 0;
}

void EditorUI::UpdateUIScale()
{
	ImGuiIO& io = ImGui::GetIO();
	//float y = io.DisplaySize.y;

	SDL_DisplayMode DM;
	int index = SDL_GetWindowDisplayIndex(Window::window);
	float oldUiScale = uiScale;
	if (index >= 0)
	{
		float dpi = 0;
		float unused;
		int result = SDL_GetDisplayDPI(index, &dpi, &unused, &unused);
		if (result == 0)
		{
			uiScale = dpi / 96.0f;
		}
		else
		{
			SDL_GetCurrentDisplayMode(index, &DM);
			auto y = DM.h;
			uiScale = y / 1080.0f;
		}
	}

	if (oldUiScale != uiScale)
	{
		//io.Fonts->Clear();
		ImGui::GetIO().FontGlobalScale = 0.5f * uiScale;
	}
}

/**
* Render the editor's UI
*/
void EditorUI::Render()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

#pragma endregion

#pragma region Low Level Draw Functions

std::string EditorUI::GenerateItemId()
{
	std::string itemId = "##" + std::to_string(uiId);
	uiId++;
	return itemId;
}

void EditorUI::DrawTreeItem(std::weak_ptr<GameObject> child)
{
	auto childLock = child.lock();
	if (childLock)
	{
		int childCount = (int)childLock->children.size();
		int flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
		if (Engine::selectedGameObject.lock() == childLock)
			flags |= ImGuiTreeNodeFlags_Selected;

		if (childCount == 0)
			flags |= ImGuiTreeNodeFlags_Leaf;

		if (childLock->GetLocalActive())
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1.0f));
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5, 0.5f, 0.5f, 1.0f));
		}

		bool opened = ImGui::TreeNodeEx(childLock->name.c_str(), flags);
		ImGui::PopStyleColor();
		if (ImGui::IsItemActivated() && ImGui::IsItemClicked())
		{
			Engine::SetSelectedGameObject(child);
		}
		if (opened)
		{
			for (int i = 0; i < childCount; i++)
			{
				DrawTreeItem(childLock->children[i]);
			}
			ImGui::TreePop();
		}
	}
}

void EditorUI::DrawReflection(Reflection& reflection)
{
	auto t = reflection.GetReflection();
	for (const auto& kv : t)
	{
		std::string variableName = GetPrettyVariableName(kv.first);

		Variable& variableRef = t.at(kv.first);
		if (auto valuePtr = std::get_if< std::reference_wrapper<int>>(&variableRef)) // Supported basic type
			DrawInput(variableName, valuePtr->get());
		else if (auto valuePtr = std::get_if<std::reference_wrapper<float>>(&variableRef))// Supported basic type
			DrawInput(variableName, valuePtr->get());
		else if (auto valuePtr = std::get_if< std::reference_wrapper<double>>(&variableRef))// Supported basic type
			DrawInput(variableName, valuePtr->get());
		else if (auto valuePtr = std::get_if< std::reference_wrapper<std::string>>(&variableRef))// Supported basic type
			DrawInput(variableName, valuePtr->get());
		else if (auto valuePtr = std::get_if< std::reference_wrapper<bool>>(&variableRef)) // Supported basic type
			DrawInput(variableName, valuePtr->get());
		else if (auto valuePtr = std::get_if<std::reference_wrapper<Reflection>>(&variableRef))
		{
			if (auto val = dynamic_cast<Vector2*>(&valuePtr->get())) // Specific draw
				DrawInput(variableName, *val);
			else if (auto val = dynamic_cast<Vector2Int*>(&valuePtr->get())) // Specific draw
				DrawInput(variableName, *val);
			else if (auto val = dynamic_cast<Vector3*>(&valuePtr->get())) // Specific draw
				DrawInput(variableName, *val);
			else if (auto val = dynamic_cast<Vector4*>(&valuePtr->get())) // Specific draw
				DrawInput(variableName, *val);
			else if (auto val = dynamic_cast<Color*>(&valuePtr->get())) // Specific draw
				DrawInput(variableName, *val);
			else //Basic draw
				DrawReflection(valuePtr->get());
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<MeshData*>>(&variableRef))
		{
			std::string inputText = "Empty MeshData...";
			if (valuePtr->get() != nullptr)
				inputText = valuePtr->get()->file->GetFileName();

			DrawInputButton(variableName, inputText);
			FileReference* ref = nullptr;
			std::string payloadName = "Files" + std::to_string(FileType::File_Mesh);
			if (DragDropTarget(payloadName, ref))
			{
				valuePtr->get() = (MeshData*)ref;
			}
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<AudioClip*>>(&variableRef))
		{
			std::string inputText = "Empty Audio...";
			if (valuePtr->get() != nullptr)
				inputText = valuePtr->get()->file->GetFileName();

			DrawInputButton(variableName, inputText);
			FileReference* ref = nullptr;
			std::string payloadName = "Files" + std::to_string(FileType::File_Audio);
			if (DragDropTarget(payloadName, ref))
			{
				valuePtr->get() = (AudioClip*)ref;
			}
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<Texture*>>(&variableRef))
		{
			std::string inputText = "Empty Texture...";
			if (valuePtr->get() != nullptr)
				inputText = valuePtr->get()->file->GetFileName();

			DrawInputButton(variableName, inputText);
			FileReference* ref = nullptr;
			std::string payloadName = "Files" + std::to_string(FileType::File_Texture);
			if (DragDropTarget(payloadName, ref))
			{
				valuePtr->get() = (Texture*)ref;
			}
		}
	}
}

void EditorUI::DrawTableInput(std::string inputName, std::string inputId, int columnIndex, float& value)
{
	ImGui::TableSetColumnIndex(columnIndex);
	ImGui::Text(inputName.c_str());
	ImGui::SameLine();
	ImGui::SetNextItemWidth(-1);
	ImGui::InputFloat(inputId.c_str(), &value, 0, 0, "%.4f");
}

void EditorUI::DrawTableInput(std::string inputName, std::string inputId, int columnIndex, int& value)
{
	ImGui::TableSetColumnIndex(columnIndex);
	ImGui::Text(inputName.c_str());
	ImGui::SameLine();
	ImGui::SetNextItemWidth(-1);
	ImGui::InputInt(inputId.c_str(), &value, 0, 0);
}

void EditorUI::DrawTextCentered(std::string text)
{
	float windowWidth = ImGui::GetWindowSize().x;
	float textWidth = ImGui::CalcTextSize(text.c_str()).x;
	ImGui::SetCursorPosX((windowWidth - textWidth) * 0.5f);
	ImGui::Text(text.c_str());
}

bool EditorUI::DrawInputButton(std::string inputName, std::string text)
{
	DrawInputTitle(inputName);
	ImGui::Button(text.c_str(), ImVec2(-1, 0));
	return false;
}

bool EditorUI::DragDropTarget(std::string name, FileReference*& ref)
{
	if (ImGui::BeginDragDropTarget())
	{
		ImGuiDragDropFlags target_flags = 0;
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(name.c_str(), target_flags))
		{
			FileReference* movedFile = (FileReference*)payload->Data;
			Debug::Print(movedFile->file->GetPath());

			int c = Editor::fileRefs.size();
			for (int i = 0; i < c; i++)
			{
				if (Editor::fileRefs[i]->fileId == movedFile->fileId)
				{
					ref = Editor::fileRefs[i];
					return true;
				}
			}
		}
		ImGui::EndDragDropTarget();
	}
	return false;
}

void EditorUI::DrawInputTitle(std::string title)
{
	ImGui::Text(title.c_str());
	ImGui::SameLine();
	ImGui::SetCursorPosX(150);
	ImGui::SetNextItemWidth(-1);
}

bool EditorUI::DrawInput(std::string inputName, float& value)
{
	DrawInputTitle(inputName);
	float oldValue = float(value);
	ImGui::InputFloat(GenerateItemId().c_str(), &value, 0, 0, "%.4f");
	return value != oldValue;
}

bool EditorUI::DrawInput(std::string inputName, double& value)
{
	DrawInputTitle(inputName);
	double oldValue = double(value);
	ImGui::InputDouble(GenerateItemId().c_str(), &value, 0, 0, "%0.8f");
	return value != oldValue;
}

bool EditorUI::DrawInput(std::string inputName, std::string& value)
{
	DrawInputTitle(inputName);
	std::string oldValue = std::string(value);
	ImGui::InputText(GenerateItemId().c_str(), &value);
	return value != oldValue;
}

bool EditorUI::DrawInput(std::string inputName, int& value)
{
	DrawInputTitle(inputName);
	int oldValue = int(value);
	ImGui::InputInt(GenerateItemId().c_str(), &value);
	return value != oldValue;
}

bool EditorUI::DrawInput(std::string inputName, bool& value)
{
	DrawInputTitle(inputName);
	bool oldValue = bool(value);
	ImGui::Checkbox(GenerateItemId().c_str(), &value);
	return value != oldValue;
}

bool EditorUI::DrawInput(std::string inputName, Color& value)
{
	ImGui::Text(inputName.c_str());
	ImGui::SameLine();
	ImGui::SetCursorPosX(150);
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

bool EditorUI::DrawInput(std::string inputName, Vector2& value)
{
	Vector2 oldValue = Vector2(value);

	ImGui::Text(inputName.c_str());
	ImGui::SameLine();
	ImGui::SetCursorPosX(150);

	if (ImGui::BeginTable("table", 2, 0))
	{
		ImGui::TableNextRow();
		DrawTableInput("X", GenerateItemId(), 0, value.x);
		DrawTableInput("Y", GenerateItemId(), 1, value.y);
		ImGui::EndTable();
	}

	return value != oldValue;
}

bool EditorUI::DrawInput(std::string inputName, Vector2Int& value)
{
	Vector2Int oldValue = Vector2Int(value);

	ImGui::Text(inputName.c_str());
	ImGui::SameLine();
	ImGui::SetCursorPosX(150);

	if (ImGui::BeginTable("table", 2, 0))
	{
		ImGui::TableNextRow();
		DrawTableInput("X", GenerateItemId(), 0, value.x);
		DrawTableInput("Y", GenerateItemId(), 1, value.y);
		ImGui::EndTable();
	}

	return value != oldValue;
}

bool EditorUI::DrawInput(std::string inputName, Vector3& value)
{
	Vector3 oldValue = Vector3(value);

	ImGui::Text(inputName.c_str());
	ImGui::SameLine();
	ImGui::SetCursorPosX(150);

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

bool EditorUI::DrawInput(std::string inputName, Vector4& value)
{
	Vector4 oldValue = Vector4(value);

	ImGui::Text(inputName.c_str());
	ImGui::SameLine();
	ImGui::SetCursorPosX(150);

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

#endif
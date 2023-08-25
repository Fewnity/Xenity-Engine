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
//#include <ShObjIdl.h>

int EditorUI::uiId = 0;
bool EditorUI::showProfiler = true;
bool EditorUI::showEditor = true;
bool EditorUI::showEngineSettings = false;
bool EditorUI::showProjectsSettings = false;
Texture* EditorUI::folderIcon = nullptr;
Texture* EditorUI::fileIcon = nullptr;
Texture* EditorUI::sceneIcon = nullptr;
Texture* EditorUI::imageIcon = nullptr;
Texture* EditorUI::meshIcon = nullptr;
Texture* EditorUI::codeIcon = nullptr;
Texture* EditorUI::headerIcon = nullptr;

float EditorUI::uiScale = 1;

#pragma region Initialisation

void EditorUI::Init()
{
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigWindowsMoveFromTitleBarOnly = true;
	io.Fonts->AddFontFromFileTTF("Roboto Regular.ttf", 30);

	fileIcon = new Texture("icons/text.png", "", true);
	fileIcon->SetWrapMode(Texture::ClampToEdge);
	folderIcon = new Texture("icons/folder.png", "", true);
	folderIcon->SetWrapMode(Texture::ClampToEdge);
	sceneIcon = new Texture("icons/belt.png", "", true);
	sceneIcon->SetWrapMode(Texture::ClampToEdge);
	imageIcon = new Texture("icons/image.png", "", true);
	imageIcon->SetWrapMode(Texture::ClampToEdge);
	meshIcon = new Texture("icons/3d.png", "", true);
	meshIcon->SetWrapMode(Texture::ClampToEdge);
	codeIcon = new Texture("icons/code.png", "", true);
	codeIcon->SetWrapMode(Texture::ClampToEdge);
	headerIcon = new Texture("icons/header.png", "", true);
	headerIcon->SetWrapMode(Texture::ClampToEdge);

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
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
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

	//if (oldUiScale != uiScale)
	//{
		//io.Fonts->Clear();
	ImGui::GetIO().FontGlobalScale = 0.5f * uiScale;
	//}
}

/**
* Render the editor's UI
*/
void EditorUI::Render()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	}
}

void EditorUI::SetRoundedCorner(int value)
{
	ImGui::GetStyle().WindowRounding = value;
}

#pragma endregion

#pragma region Low Level Draw Functions

std::string EditorUI::GenerateItemId()
{
	std::string itemId = "##" + std::to_string(uiId);
	uiId++;
	return itemId;
}

std::string EditorUI::OpenFolderDialog(std::string title)
{
	std::string path = "";

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	IFileOpenDialog* pFileOpen;

	HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pFileOpen));

	if (SUCCEEDED(hr)) {
		DWORD dwOptions;
		pFileOpen->GetOptions(&dwOptions);

		/*COMDLG_FILTERSPEC fileTypes[] = {
			{ L"Json Files and csv", L"*.json;*.dat" },
	{ L"Json Files", L"*.json" },
	{ L"CSV Files", L"*.*" }
		};
		pFileOpen->SetFileTypes(ARRAYSIZE(fileTypes), fileTypes);*/
		pFileOpen->SetOptions(dwOptions | FOS_PICKFOLDERS);

		// Initializing an object of wstring
		std::wstring tempTitle = std::wstring(title.begin(), title.end());

		// Applying c_str() method on temp
		LPCWSTR wideString = tempTitle.c_str();

		pFileOpen->SetTitle(tempTitle.c_str());
		hr = pFileOpen->Show(NULL);

		if (SUCCEEDED(hr))
		{
			IShellItem* pItem;
			hr = pFileOpen->GetResult(&pItem);
			if (SUCCEEDED(hr))
			{
				PWSTR pszFolderPath;
				hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFolderPath);
				if (SUCCEEDED(hr))
				{
					std::wstringstream ss;
					ss << pszFolderPath;
					std::wstring wst = ss.str();
					path = std::string(wst.begin(), wst.end());
					path += "\\";
					CoTaskMemFree(pszFolderPath);
				}
				pItem->Release();
			}
		}

		pFileOpen->Release();
	}

	CoUninitialize();

	return path;
}

std::string EditorUI::OpenFileDialog(std::string title)
{
	std::string path = "";

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	IFileSaveDialog* pFileOpen;

	HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pFileOpen));

	if (SUCCEEDED(hr)) 
	{
		DWORD dwOptions;
		pFileOpen->GetOptions(&dwOptions);

		/*COMDLG_FILTERSPEC fileTypes[] = {
			{ L"Json Files and csv", L"*.json;*.dat" },
	{ L"Json Files", L"*.json" },
	{ L"CSV Files", L"*.*" }
		};*/

		COMDLG_FILTERSPEC fileTypes[] = {
			{ L"Scene", L"*.xen" },
			//{ L"Scene2", L"*.xen2" },
		};

		//pFileOpen->SetFileTypes(ARRAYSIZE(fileTypes), fileTypes);
		//pFileOpen->SetOptions(dwOptions | FOS_CREATEPROMPT);



		// Initializing an object of wstring
		std::string title = title;
		std::wstring tempTitle = std::wstring(title.begin(), title.end());

		// Applying c_str() method on temp
		LPCWSTR wideString = tempTitle.c_str();

		pFileOpen->SetTitle(tempTitle.c_str());
		hr = pFileOpen->Show(NULL);

		if (SUCCEEDED(hr))
		{
			IShellItem* pItem;
			hr = pFileOpen->GetResult(&pItem);
			if (SUCCEEDED(hr))
			{
				PWSTR pszFolderPath;
				hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFolderPath);

				if (SUCCEEDED(hr))
				{
					unsigned int fileTypeIndex = 0;
					hr = pFileOpen->GetFileTypeIndex(&fileTypeIndex);
					fileTypeIndex--; // The index starts at 1
					if (SUCCEEDED(hr)) 
					{
						std::wstringstream ss;
						ss << pszFolderPath;
						std::wstring wst = ss.str();

						path = std::string(wst.begin(), wst.end());
						CoTaskMemFree(pszFolderPath);
					}
				}
				pItem->Release();
			}
		}

		pFileOpen->Release();
	}

	CoUninitialize();

	return path;
}

std::string EditorUI::SaveFileDialog(std::string title)
{
	std::string path = "";

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	IFileSaveDialog* pFileOpen;

	HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pFileOpen));

	if (SUCCEEDED(hr)) 
	{
		DWORD dwOptions;
		pFileOpen->GetOptions(&dwOptions);

		/*COMDLG_FILTERSPEC fileTypes[] = {
			{ L"Json Files and csv", L"*.json;*.dat" },
	{ L"Json Files", L"*.json" },
	{ L"CSV Files", L"*.*" }
		};*/

		COMDLG_FILTERSPEC fileTypes[] = 
		{
			{ L"Scene", L"*.xen" },
			//{ L"Scene2", L"*.xen2" },
		};

		pFileOpen->SetFileTypes(ARRAYSIZE(fileTypes), fileTypes);
		//pFileOpen->SetOptions(dwOptions | FOS_CREATEPROMPT);



		// Initializing an object of wstring
		std::string title = title;
		std::wstring tempTitle = std::wstring(title.begin(), title.end());

		// Applying c_str() method on temp
		LPCWSTR wideString = tempTitle.c_str();

		pFileOpen->SetTitle(tempTitle.c_str());
		hr = pFileOpen->Show(NULL);

		if (SUCCEEDED(hr))
		{
			IShellItem* pItem;
			hr = pFileOpen->GetResult(&pItem);
			if (SUCCEEDED(hr))
			{
				PWSTR pszFolderPath;
				hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFolderPath);

				if (SUCCEEDED(hr))
				{
					unsigned int fileTypeIndex = 0;
					hr = pFileOpen->GetFileTypeIndex(&fileTypeIndex);
					fileTypeIndex--; // The index starts at 1
					if (SUCCEEDED(hr)) 
					{
						std::wstringstream ss;
						ss << pszFolderPath;
						std::wstring wst = ss.str();

						std::wstring fileExtChoosed = fileTypes[fileTypeIndex].pszSpec;
						fileExtChoosed = fileExtChoosed.substr(1);

						int pathSize = wst.size();
						int extSize = fileExtChoosed.size();

						std::wstring endOfTheFile = wst.substr(pathSize - extSize);

						bool foundExt = true;
						for (int i2 = 0; i2 < extSize; i2++)
						{
							if (endOfTheFile[i2] != fileExtChoosed[i2])
							{
								foundExt = false;
								break;
							}
						}
						if (!foundExt)
						{
							wst += fileExtChoosed;
						}

						path = std::string(wst.begin(), wst.end());
						CoTaskMemFree(pszFolderPath);
					}
				}
				pItem->Release();
			}
		}

		pFileOpen->Release();
	}

	CoUninitialize();

	return path;
}

bool EditorUI::DrawTreeItem(ProjectDirectory* projectDir)
{
	bool objectClicked = false;
	if (projectDir)
	{
		int childCount = (int)projectDir->subdirectories.size();
		int flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanFullWidth;
		if (Engine::currentProjectDirectory == projectDir)
			flags |= ImGuiTreeNodeFlags_Selected;

		if (childCount == 0)
			flags |= ImGuiTreeNodeFlags_Leaf;

		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 1, 1.0f));

		bool opened = ImGui::TreeNodeEx(projectDir->GetFolderName().c_str(), flags);
		ImGui::PopStyleColor();

		if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(0))
		{
			Engine::currentProjectDirectory = projectDir;
			objectClicked = true;
		}
		if (opened)
		{
			for (int i = 0; i < childCount; i++)
			{
				bool clickedTemp = DrawTreeItem(projectDir->subdirectories[i]);
				if (clickedTemp)
					objectClicked = true;
			}
			ImGui::TreePop();
		}
	}
	return objectClicked;
}

bool EditorUI::DrawTreeItem(std::weak_ptr<GameObject> child)
{
	bool objectClicked = false;
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
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			std::string payloadName = "GameObject";
			ImGui::SetDragDropPayload(payloadName.c_str(), childLock.get(), sizeof(GameObject));
			ImGui::Text(childLock->name.c_str());
			ImGui::EndDragDropSource();
		}
		ImGui::PopStyleColor();

		GameObject* droppedGameObject = nullptr;
		if (DragDropTarget("GameObject", droppedGameObject))
		{
			droppedGameObject->SetParent(childLock);
		}
		else
		{
			if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(0) && !ImGui::IsDragDropActive())
			{
				Engine::SetSelectedGameObject(child);
				Engine::selectedFileReference = nullptr;
				objectClicked = true;
			}
		}
		if (opened)
		{
			for (int i = 0; i < childCount; i++)
			{
				bool clickedTemp = DrawTreeItem(childLock->children[i]);
				if (clickedTemp)
					objectClicked = true;
			}
			ImGui::TreePop();
		}
	}
	return objectClicked;
}

bool EditorUI::DrawMap(std::unordered_map<std::string, Variable> myMap)
{

	bool valueChanged = false;
	for (const auto& kv : myMap)
	{
		bool valueChangedTemp = false;
		std::string variableName = GetPrettyVariableName(kv.first);

		Variable variableRef = kv.second;
		if (auto valuePtr = std::get_if< std::reference_wrapper<int>>(&variableRef)) // Supported basic type
			valueChangedTemp = DrawInput(variableName, valuePtr->get());
		else if (auto valuePtr = std::get_if<std::reference_wrapper<float>>(&variableRef))// Supported basic type
			valueChangedTemp = DrawInput(variableName, valuePtr->get());
		else if (auto valuePtr = std::get_if< std::reference_wrapper<double>>(&variableRef))// Supported basic type
			valueChangedTemp = DrawInput(variableName, valuePtr->get());
		else if (auto valuePtr = std::get_if< std::reference_wrapper<std::string>>(&variableRef))// Supported basic type
			valueChangedTemp = DrawInput(variableName, valuePtr->get());
		else if (auto valuePtr = std::get_if< std::reference_wrapper<bool>>(&variableRef)) // Supported basic type
			valueChangedTemp = DrawInput(variableName, valuePtr->get());
		else if (auto valuePtr = std::get_if< std::reference_wrapper<std::weak_ptr<Component>>>(&variableRef)) // Supported basic type
			valueChangedTemp = DrawInput(variableName, valuePtr->get());
		else if (auto valuePtr = std::get_if< std::reference_wrapper<std::weak_ptr<GameObject>>>(&variableRef)) // Supported basic type
			valueChangedTemp = DrawInput(variableName, valuePtr->get());
		else if (auto valuePtr = std::get_if< std::reference_wrapper<std::weak_ptr<Transform>>>(&variableRef)) // Supported basic type
			valueChangedTemp = DrawInput(variableName, valuePtr->get());
		else if (auto valuePtr = std::get_if<std::reference_wrapper<Reflection>>(&variableRef))
		{
			if (auto val = dynamic_cast<Vector2*>(&valuePtr->get())) // Specific draw
				valueChangedTemp = DrawInput(variableName, *val);
			else if (auto val = dynamic_cast<Vector2Int*>(&valuePtr->get())) // Specific draw
				valueChangedTemp = DrawInput(variableName, *val);
			else if (auto val = dynamic_cast<Vector3*>(&valuePtr->get())) // Specific draw
				valueChangedTemp = DrawInput(variableName, *val);
			else if (auto val = dynamic_cast<Vector4*>(&valuePtr->get())) // Specific draw
				valueChangedTemp = DrawInput(variableName, *val);
			else if (auto val = dynamic_cast<Color*>(&valuePtr->get())) // Specific draw
				valueChangedTemp = DrawInput(variableName, *val);
			else //Basic draw
				DrawReflection(valuePtr->get());
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<MeshData*>>(&variableRef))
		{
			std::string inputText = "None (MeshData)";
			auto ptr = valuePtr->get();
			if (ptr != nullptr)
			{
				if (ptr->file != nullptr)
					inputText = ptr->file->GetFileName();
				else
					inputText = "Filled but invalid file reference (MeshData)";

				inputText += " " + std::to_string(ptr->fileId) + " ";
				if (ptr->file)
					inputText += " " + std::to_string(ptr->file->GetUniqueId()) + " ";
			}

			if (DrawInputButton(variableName, inputText, true) == 2) 
			{
				valuePtr->get() = nullptr;
			}

			FileReference* ref = nullptr;
			std::string payloadName = "Files" + std::to_string(FileType::File_Mesh);
			if (DragDropTarget(payloadName, ref))
			{
				valuePtr->get() = (MeshData*)ref;
				valueChangedTemp = true;
			}
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<AudioClip*>>(&variableRef))
		{
			std::string inputText = "None (AudioClip)";
			auto ptr = valuePtr->get();
			if (ptr != nullptr)
			{
				if (ptr->file != nullptr)
					inputText = ptr->file->GetFileName();
				else
					inputText = "Filled but invalid file reference (AudioClip)";

				inputText += " " + std::to_string(ptr->fileId) + " ";
				if (ptr->file)
					inputText += " " + std::to_string(ptr->file->GetUniqueId()) + " ";
			}

			if (DrawInputButton(variableName, inputText, true) == 2)
			{
				valuePtr->get() = nullptr;
			}

			FileReference* ref = nullptr;
			std::string payloadName = "Files" + std::to_string(FileType::File_Audio);
			if (DragDropTarget(payloadName, ref))
			{
				valuePtr->get() = (AudioClip*)ref;
				valueChangedTemp = true;
			}
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<Texture*>>(&variableRef))
		{
			std::string inputText = "None (Texture)";
			auto ptr = valuePtr->get();
			if (ptr != nullptr)
			{
				if (ptr->file != nullptr)
					inputText = ptr->file->GetFileName();
				else
					inputText = "Filled but invalid file reference (Texture)";

				inputText += " " + std::to_string(ptr->fileId) + " ";
				if (ptr->file)
					inputText += " " + std::to_string(ptr->file->GetUniqueId()) + " ";
			}

			if (DrawInputButton(variableName, inputText, true) == 2)
			{
				valuePtr->get() = nullptr;
			}

			FileReference* ref = nullptr;
			std::string payloadName = "Files" + std::to_string(FileType::File_Texture);
			if (DragDropTarget(payloadName, ref))
			{
				valuePtr->get() = (Texture*)ref;
				valueChangedTemp = true;
			}
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<Scene*>>(&variableRef))
		{
			std::string inputText = "None (Scene)";
			auto ptr = valuePtr->get();
			if (ptr != nullptr)
			{
				if (ptr->file != nullptr)
					inputText = ptr->file->GetFileName();
				else
					inputText = "Filled but invalid file reference (Scene)";

				inputText += " " + std::to_string(ptr->fileId) + " ";
				if (ptr->file)
					inputText += " " + std::to_string(ptr->file->GetUniqueId()) + " ";
			}

			if (DrawInputButton(variableName, inputText, true) == 2)
			{
				valuePtr->get() = nullptr;
			}

			FileReference* ref = nullptr;
			std::string payloadName = "Files" + std::to_string(FileType::File_Scene);
			if (DragDropTarget(payloadName, ref))
			{
				valuePtr->get() = (Scene*)ref;
				valueChangedTemp = true;
			}
		}
		else if (auto valuePtr = std::get_if<std::reference_wrapper<std::vector<Texture*>>>(&variableRef))
		{
			int vectorSize = valuePtr->get().size();
			for (int vectorI = 0; vectorI < vectorSize; vectorI++)
			{
				std::string inputText = "None (Texture)";
				auto ptr = valuePtr->get()[vectorI];
				if (ptr != nullptr)
				{
					if (ptr->file != nullptr)
						inputText = ptr->file->GetFileName();
					else
						inputText = "Filled but invalid file reference (Texture)";

					inputText += " " + std::to_string(ptr->fileId) + " ";
					if (ptr->file)
						inputText += " " + std::to_string(ptr->file->GetUniqueId()) + " ";
				}

				if (DrawInputButton(variableName, inputText, true) == 2)
				{
					valuePtr->get()[vectorI] = nullptr;
				}

				FileReference* ref = nullptr;
				std::string payloadName = "Files" + std::to_string(FileType::File_Texture);
				if (DragDropTarget(payloadName, ref))
				{
					valuePtr->get()[vectorI] = (Texture*)ref;
					valueChangedTemp = true;
				}
			}
			if (ImGui::Button("Add Texture"))
			{
				valuePtr->get().push_back(nullptr);
			}
			if (ImGui::Button("Remove Texture"))
			{
				int textureSize = valuePtr->get().size();
				if (textureSize != 0) 
				{
					valuePtr->get().erase(valuePtr->get().begin() + textureSize -1);
				}
			}
		}
		if (valueChangedTemp)
		{
			valueChanged = true;
		}
	}
	return valueChanged;
}

bool EditorUI::DrawReflection(Reflection& reflection)
{
	auto t = reflection.GetReflection();
	return DrawMap(t);
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

int EditorUI::DrawInputButton(std::string inputName, std::string text, bool addUnbindButton)
{
	int returnValue = 0;
	DrawInputTitle(inputName);
	float w = ImGui::GetContentRegionAvail().x;
	if (addUnbindButton) 
	{
		w -= 25 * uiScale;
	}
	ImGui::BeginGroup();
	std::string id = text + GenerateItemId();
	if (ImGui::Button(id.c_str(), ImVec2(w, 0)))
	{
		returnValue = 1;
	}
	if (addUnbindButton)
	{
		ImGui::SameLine();
		std::string id2 = "X" + GenerateItemId();
		if (ImGui::Button(id2.c_str()))
		{
			returnValue = 2;
		}
	}
	ImGui::EndGroup();
	return returnValue;
}

bool EditorUI::DragDropTarget(std::string name, FileReference*& ref)
{
	if (ImGui::BeginDragDropTarget())
	{
		ImGuiDragDropFlags target_flags = 0;
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(name.c_str(), target_flags))
		{
			FileReference* movedFile = (FileReference*)payload->Data;

			FileReference* file = ProjectManager::GetFileReferenceById(movedFile->fileId);
			if (file)
			{
				file->LoadFileReference();
				ref = file;
				return true;
			}
		}
		ImGui::EndDragDropTarget();
	}
	return false;
}

bool EditorUI::DragDropTarget(std::string name, Component*& ref)
{
	if (ImGui::BeginDragDropTarget())
	{
		ImGuiDragDropFlags target_flags = 0;
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(name.c_str(), target_flags))
		{
			Component* comp = ((Component*)payload->Data);

			if (comp)
			{
				ref = comp;
				return true;
			}
		}
		ImGui::EndDragDropTarget();
	}
	return false;
}

bool EditorUI::DragDropTarget(std::string name, GameObject*& ref)
{
	if (ImGui::BeginDragDropTarget())
	{
		ImGuiDragDropFlags target_flags = 0;
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(name.c_str(), target_flags))
		{
			GameObject* gameObject = ((GameObject*)payload->Data);

			if (gameObject)
			{
				ref = gameObject;
				return true;
			}
		}
		ImGui::EndDragDropTarget();
	}
	return false;
}

bool EditorUI::DragDropTarget(std::string name, Transform*& ref)
{
	if (ImGui::BeginDragDropTarget())
	{
		ImGuiDragDropFlags target_flags = 0;
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(name.c_str(), target_flags))
		{
			Transform* trans = ((Transform*)payload->Data);

			if (trans)
			{
				ref = trans;
				return true;
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
	ImGui::SetCursorPosX(150 * uiScale);
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

bool EditorUI::DrawInput(std::string inputName, std::weak_ptr<Component>& value)
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

	Component* ref = nullptr;
	std::string payloadName = "Component";
	if (DragDropTarget(payloadName, ref))
	{
		value = ref->shared_from_this();
	}

	return oldValue != value.lock();
}

bool EditorUI::DrawInput(std::string inputName, std::weak_ptr<Transform>& value)
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

	Transform* ref = nullptr;
	std::string payloadName = "Transform";
	if (DragDropTarget(payloadName, ref))
	{
		value = ref->shared_from_this();
	}

	return oldValue != value.lock();
}

bool EditorUI::DrawInput(std::string inputName, std::weak_ptr<GameObject>& value)
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

	GameObject* ref = nullptr;
	std::string payloadName = "GameObject";
	if (DragDropTarget(payloadName, ref))
	{
		value = ref->shared_from_this();
	}

	return oldValue != value.lock();
}

bool EditorUI::DrawInput(std::string inputName, Color& value)
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

bool EditorUI::DrawInput(std::string inputName, Vector2& value)
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

bool EditorUI::DrawInput(std::string inputName, Vector2Int& value)
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

bool EditorUI::DrawInput(std::string inputName, Vector3& value)
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

bool EditorUI::DrawInput(std::string inputName, Vector4& value)
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

#pragma endregion

#endif
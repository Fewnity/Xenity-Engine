// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2025 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "select_project_menu.h"

#include <filesystem>

#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

#include <engine/asset_management/project_manager.h>
#include <editor/editor.h>
#include <editor/ui/editor_ui.h>

#include <engine/debug/debug.h>

SelectProjectMenu::SelectProjectMenu()
{
	group = MenuGroup::Menu_Select_Project;
}

void SelectProjectMenu::Init()
{
	m_projectsList = ProjectManager::GetProjectsList();
}

// Code from https://github.com/ocornut/imgui/issues/1901
bool SelectProjectMenu::BufferingBar(const char* label, float value, const ImVec2& size_arg, const uint32_t& bg_col, const uint32_t& fg_col) 
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size = size_arg;
	size.x -= style.FramePadding.x * 2;

	const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
	ImGui::ItemSize(bb, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id))
		return false;

	// Render
	const float circleStart = size.x * 0.85f;
	const float circleEnd = size.x;
	const float circleWidth = circleEnd - circleStart;

	const float t = static_cast<float>(g.Time);
	const float r = size.y / 2;
	const float speed = 2.f;

	const float a = speed * 0;
	const float b = speed * 0.333f;
	const float c = speed * 0.666f;

	const float o1 = (circleWidth + r) * (t + a - speed * (int)((t + a) / speed)) / speed;
	const float o2 = (circleWidth + r) * (t + b - speed * (int)((t + b) / speed)) / speed;
	const float o3 = (circleWidth + r) * (t + c - speed * (int)((t + c) / speed)) / speed;

	window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o1, bb.Min.y + r), r, bg_col);
	window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o2, bb.Min.y + r), r, bg_col);
	window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o3, bb.Min.y + r), r, bg_col);

	window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart, bb.Max.y), bg_col);
	window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart * value, bb.Max.y), fg_col);
	return true;
}

void SelectProjectMenu::Draw()
{
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);

	const bool visible = ImGui::Begin("Select Project", 0, ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
	if (visible)
	{
		OnStartDrawing();

		//Increase font size
		ImFont* font = ImGui::GetFont();
		const float oldScale = font->Scale;
		font->Scale *= 2;
		ImGui::PushFont(font);

		//Draw text
		const std::string projectsText = "Projects";
		const ImVec2 textSize = ImGui::CalcTextSize(projectsText.c_str());

		ImGui::SetCursorPos(ImVec2((viewport->WorkSize.x - textSize.x) / 2.0f, 10));
		ImGui::Text("%s", projectsText.c_str());

		ImGui::PopFont();

		font->Scale = oldScale * 1.5f;
		ImGui::PushFont(font);

		const bool isLoadingBegValue = 
			ProjectManager::GetProjectState() == ProjectState::Loading ||
			ProjectManager::GetProjectState() == ProjectState::WaitingForScene;

		if (isLoadingBegValue)
		{
			ImGui::BeginDisabled();
		}

		if (ImGui::Button("Create project"))
		{
			Editor::s_currentMenu = MenuGroup::Menu_Create_Project;
		}
		ImGui::SameLine();
		if (ImGui::Button("Load project"))
		{
			OnLoadButtonClick();
		}

		if (isLoadingBegValue)
		{
			ImGui::EndDisabled();
		}

		DrawProjectsList();

		ImGui::PopFont();

		//Reset font
		font->Scale = oldScale;
		ImGui::PushFont(font);
		ImGui::PopFont();

		CalculateWindowValues();
	}
	else
	{
		ResetWindowValues();
	}

	ImGui::End();
}

void SelectProjectMenu::OnLoadButtonClick()
{
	const std::string projectPath = EditorUI::OpenFolderDialog("Select project folder", "");
	if (!projectPath.empty())
	{
		ProjectLoadingErrors result = ProjectManager::LoadProject(projectPath);
		if (result == ProjectLoadingErrors::Success)
		{
			// Check if the project is already in the opened projects list
			bool projectAlreadyInList = false;
			const size_t projectsCount = m_projectsList.size();
			for (size_t i = 0; i < projectsCount; i++)
			{
				if (m_projectsList[i].path == projectPath)
				{
					projectAlreadyInList = true;
					break;
				}
			}

			// If not, add the project to the list
			if (!projectAlreadyInList)
			{
				// Create new item
				ProjectListItem newProjectListItem;
				newProjectListItem.name = ProjectManager::GetProjectName();
				newProjectListItem.path = projectPath;
				m_projectsList.push_back(newProjectListItem);

				ProjectManager::SaveProjectsList(m_projectsList);
			}

			Editor::s_currentMenu = MenuGroup::Menu_Editor;
		}
		else
		{
			ShowProjectError(result);
		}
	}
}

void SelectProjectMenu::DrawProjectsList()
{
	ImGui::Separator();

	const bool isLoadingBegValue =
		ProjectManager::GetProjectState() == ProjectState::Loading ||
		ProjectManager::GetProjectState() == ProjectState::WaitingForScene;

	if (isLoadingBegValue)
	{
		ImGui::BeginDisabled();
	}

	size_t projectListSize = m_projectsList.size();
	for (size_t i = 0; i < projectListSize; i++)
	{
		ProjectListItem& project = m_projectsList[i];
		ImGui::BeginGroup();
		ImVec2 cursorPos = ImGui::GetCursorPos();
		ImGui::Text("%s", project.name.c_str());
		ImGui::Text("%s", project.path.c_str());
		float availWidth = ImGui::GetContentRegionAvail().x;
		ImGui::SameLine();
		ImGui::SetCursorPos(ImVec2(availWidth - 50 * GetUIScale(), cursorPos.y + 15 * GetUIScale()));
		if (ImGui::Button((std::string("...") + EditorUI::GenerateItemId()).c_str()))
		{
			m_selectedProject = &project;
			ImGui::OpenPopup(std::to_string(*(size_t*)m_selectedProject).c_str());
		}
		if (m_selectedProject == &project)
		{
			if (ImGui::BeginPopup(std::to_string(*(size_t*)m_selectedProject).c_str()))
			{
				if (ImGui::MenuItem("Remove from list"))
				{
					DialogResult result = EditorUI::OpenDialog("Remove " + project.name, "Are you sure you want to remove the " + project.name + " project from the list?\n(Files won't be deleted)", DialogType::Dialog_Type_YES_NO_CANCEL);
					if (result == DialogResult::Dialog_YES)
					{
						DeleteProject(i, false);
						i--;
						projectListSize--;
					}
					m_selectedProject = nullptr;
					ImGui::CloseCurrentPopup();
				}
				if (ImGui::MenuItem("Delete"))
				{
					DialogResult result = EditorUI::OpenDialog("Delete " + project.name, "Are you sure you want to delete the " + project.name + " project?\n(Files will be deleted)", DialogType::Dialog_Type_YES_NO_CANCEL);
					if (result == DialogResult::Dialog_YES)
					{
						DeleteProject(i, true);
						i--;
						projectListSize--;
					}
					m_selectedProject = nullptr;
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
		}
		if (m_projectToLoad == &project)
		{
			if (ProjectManager::GetProjectState() == ProjectState::NotLoaded)
			{
				m_projectToLoad = nullptr;
			}
			else 
			{
				ImGui::SetCursorPos(ImVec2(cursorPos.x, cursorPos.y + 53 * GetUIScale()));
				BufferingBar("##buffer_bar", ProjectManager::GetLoadingProgress(), ImVec2(400 * GetUIScale(), 6 * GetUIScale()), ImGui::GetColorU32(ImGuiCol_Button), ImGui::GetColorU32(ImGuiCol_ButtonHovered));
			}
		}
		ImGui::EndGroup();
		ImGui::SetCursorPos(ImVec2(cursorPos.x, cursorPos.y));
		ImVec2 butSize = ImGui::GetItemRectSize();
		const bool buttonClicked = ImGui::InvisibleButton(EditorUI::GenerateItemId().c_str(), ImVec2(butSize.x, butSize.y));
		if (buttonClicked)
		{
			if (ProjectManager::GetProjectState() == ProjectState::NotLoaded)
			{
				m_projectToLoad = &project;
				m_loadingThread = std::thread([this, project]()
					{
						ProjectLoadingErrors result = ProjectManager::LoadProject(project.path);
						if (result != ProjectLoadingErrors::Success)
						{
							ShowProjectError(result);
						}
						m_loadingThread.detach();
					});
			}
		}
		ImGui::Separator();
	}

	if (isLoadingBegValue)
	{
		ImGui::EndDisabled();
	}
}

void SelectProjectMenu::DeleteProject(size_t projectIndex, bool deleteFiles)
{
	ProjectListItem& project = m_projectsList[projectIndex];
	if (deleteFiles && std::filesystem::exists(project.path))
	{
		try
		{
			std::filesystem::remove_all(project.path);
		}
		catch (const std::exception&)
		{
		}
	}
	m_projectsList.erase(m_projectsList.begin() + projectIndex);
	ProjectManager::SaveProjectsList(m_projectsList);
}

void SelectProjectMenu::ShowProjectError(ProjectLoadingErrors error)
{
	if (error == ProjectLoadingErrors::NoAssetFolder)
	{
		EditorUI::OpenDialog("Error", "This is not a Xenity Project, no asset folder found.", DialogType::Dialog_Type_OK);
		Debug::PrintError("[SelectProjectMenu::DrawProjectsList] This is not a Xenity Project", true);
	}
	else 
	{
		EditorUI::OpenDialog("Error", "Cannot open project.", DialogType::Dialog_Type_OK);
		Debug::PrintError("[SelectProjectMenu::ShowProjectError] Cannot open project", true);
	}
}

// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "menu_builder.h"
#include <imgui/imgui.h>
#include <engine/debug/debug.h>

bool RightClickMenu::isFocusCorrect = false;
bool RightClickMenu::isDrawn = false;
std::string RightClickMenu::isDrawnName = "";

RightClickMenu::RightClickMenu(const std::string& uniqueName) : nameId(uniqueName)
{
}

RightClickMenu::~RightClickMenu()
{
	size_t itemCount = items.size();
	for (size_t i = 0; i < itemCount; i++)
	{
		delete items[i];
	}
	items.clear();
}

void RightClickMenu::DrawRecursive(const RightClickMenuItem& item) const
{
	if (!item.GetIsVisible())
		return;

	const size_t itemsCount = item.onHoverItems.size();
	if (itemsCount != 0) 
	{
		if (ImGui::BeginMenu(item.GetTitle().c_str(), item.IsEnabled()))
		{
			for (size_t i = 0; i < itemsCount; i++)
			{
				DrawRecursive(*item.onHoverItems[i]);
			}
			ImGui::EndMenu();
		}
	}
	else
	{
		if (ImGui::MenuItem(item.GetTitle().c_str(), nullptr, nullptr, item.IsEnabled()))
		{
			if (item.onClicked)
				item.onClicked();

			ImGui::CloseCurrentPopup();
		}
	}
}

RightClickMenuState RightClickMenu::Check(const bool blockOpen)
{
	RightClickMenuState state = RightClickMenuState::Closed;
	bool isHovered = ImGui::IsItemHovered();
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && isHovered)
	{
		isFocusCorrect = true;
	}

	bool isClicked = ImGui::IsMouseReleased(ImGuiMouseButton_Right);
	isHovered = ImGui::IsItemHovered();

	if (isClicked && isHovered)
	{
		if (!blockOpen && isFocusCorrect)
		{
			ImGui::OpenPopup(nameId.c_str());
			state = RightClickMenuState::JustOpened;
			isDrawnName = nameId;
		}
		isFocusCorrect = false;
	}
	if (state == RightClickMenuState::Closed && isDrawn && isDrawnName == nameId)
	{
		state = RightClickMenuState::Opened;
	}
	return state;
}

bool RightClickMenu::Draw()
{
	bool drawn = false;

	if (isDrawnName == nameId)
		isDrawn = false;

	if (ImGui::BeginPopup(nameId.c_str()))
	{
		drawn = true;
		isDrawn = true;
		const size_t itemsCount = items.size();
		for (size_t i = 0; i < itemsCount; i++)
		{
			DrawRecursive(*items[i]);
		}
		ImGui::EndPopup();
	}

	return drawn;
}

RightClickMenuItem* RightClickMenu::AddItem(const std::string& title, const std::function<void()>& onClickFunction)
{
	RightClickMenuItem* newItem = new RightClickMenuItem();
	newItem->SetTitle(title);
	newItem->onClicked = onClickFunction;
	items.push_back(newItem);

	return items[items.size() - 1];
}

RightClickMenuItem* RightClickMenu::AddItem(const std::string& title)
{
	RightClickMenuItem* newItem = new RightClickMenuItem();
	newItem->SetTitle(title);
	items.push_back(newItem);

	return items[items.size() - 1];
}

RightClickMenuItem::~RightClickMenuItem()
{
	size_t itemCount = onHoverItems.size();
	for (size_t i = 0; i < itemCount; i++)
	{
		delete onHoverItems[i];
	}
	onHoverItems.clear();
}

RightClickMenuItem* RightClickMenuItem::AddItem(const std::string& title, std::function<void()> onClickFunction)
{
	RightClickMenuItem* newItem = new RightClickMenuItem();
	newItem->text = title;
	newItem->onClicked = onClickFunction;
	onHoverItems.push_back(newItem);

	return onHoverItems[onHoverItems.size() - 1];
}

RightClickMenuItem* RightClickMenuItem::AddItem(const std::string& title)
{
	RightClickMenuItem* newItem = new RightClickMenuItem();
	newItem->text = title;
	onHoverItems.push_back(newItem);

	return onHoverItems[onHoverItems.size() - 1];
}

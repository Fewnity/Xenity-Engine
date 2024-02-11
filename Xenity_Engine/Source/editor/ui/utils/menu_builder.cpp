#include "menu_builder.h"
#include <imgui/imgui.h>
#include <engine/debug/debug.h>

bool RightClickMenu::isFocusCorrect = false;
bool RightClickMenu::isDrawn = false;
std::string RightClickMenu::isDrawnName = "";

RightClickMenu::RightClickMenu(const std::string& uniqueName)
{
	this->nameId = uniqueName;
}

RightClickMenu::~RightClickMenu()
{
	int itemCount = items.size();
	for (int i = 0; i < itemCount; i++)
	{
		delete items[i];
	}
	items.clear();
}

void RightClickMenu::DrawRecursive(const RightClickMenuItem& item) const
{
	if (!item.GetIsVisible())
		return;

	const int itemsCount = item.onHoverItems.size();
	if (itemsCount != 0) 
	{
		if (ImGui::BeginMenu(item.GetTitle().c_str(), item.GetIsEnabled()))
		{
			for (int i = 0; i < itemsCount; i++)
			{
				DrawRecursive(*item.onHoverItems[i]);
			}
			ImGui::EndMenu();
		}
	}
	else
	{
		if (ImGui::MenuItem(item.GetTitle().c_str(), nullptr, nullptr, item.GetIsEnabled()))
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
		const int itemsCount = items.size();
		for (int i = 0; i < itemsCount; i++)
		{
			DrawRecursive(*items[i]);
		}
		ImGui::EndPopup();
	}

	return drawn;
}

RightClickMenuItem* RightClickMenu::AddItem(const std::string& title, const std::function<void()> onClickFunction)
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
	int itemCount = onHoverItems.size();
	for (int i = 0; i < itemCount; i++)
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

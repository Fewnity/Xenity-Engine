#include "menu_builder.h"
#include <imgui/imgui.h>
#include <engine/debug/debug.h>

bool RightClickMenu::isFocusCorrect = false;

RightClickMenu::RightClickMenu(std::string uniqueName)
{
	this->nameId = uniqueName;
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
				DrawRecursive(item.onHoverItems[i]);
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

RightClickMenuState RightClickMenu::Draw(const bool blockOpen)
{
	RightClickMenuState state = RightClickMenuState::Closed;

	if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImGui::IsItemHovered())
	{
		isFocusCorrect = true;
	}
	if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
	{
		if (ImGui::IsItemHovered() && !blockOpen && isFocusCorrect)
		{
			ImGui::OpenPopup(nameId.c_str());
			state = RightClickMenuState::JustOpened;
		}
		isFocusCorrect = false;
	}

	if (ImGui::BeginPopup(nameId.c_str()))
	{
		if (state != RightClickMenuState::JustOpened)
			state = RightClickMenuState::Opened;

		const int itemsCount = items.size();
		for (int i = 0; i < itemsCount; i++)
		{
			DrawRecursive(items[i]);
		}
		ImGui::EndPopup();
	}

	return state;
}

RightClickMenuItem& RightClickMenu::AddItem(const std::string& title, std::function<void()> onClickFunction)
{
	RightClickMenuItem newItem;
	newItem.SetTitle(title);
	newItem.onClicked = onClickFunction;
	items.push_back(newItem);

	return items[items.size() - 1];
}

RightClickMenuItem& RightClickMenu::AddItem(const std::string& title)
{
	RightClickMenuItem newItem;
	newItem.SetTitle(title);
	items.push_back(newItem);

	return items[items.size() - 1];
}

RightClickMenuItem& RightClickMenuItem::AddItem(const std::string& title, std::function<void()> onClickFunction)
{
	RightClickMenuItem newItem;
	newItem.text = title;
	newItem.onClicked = onClickFunction;
	onHoverItems.push_back(newItem);

	return onHoverItems[onHoverItems.size() - 1];
}

RightClickMenuItem& RightClickMenuItem::AddItem(const std::string& title)
{
	RightClickMenuItem newItem;
	newItem.text = title;
	onHoverItems.push_back(newItem);

	return onHoverItems[onHoverItems.size() - 1];
}

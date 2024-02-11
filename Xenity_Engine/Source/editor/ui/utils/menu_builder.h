#pragma once

//class MenuBuilder
//{
//};

#include <vector>
#include <string>
#include <functional>

enum class RightClickMenuState
{
	Closed,
	JustOpened,
	Opened,
};

class RightClickMenuItem
{
public:
	~RightClickMenuItem();
	std::vector<RightClickMenuItem*> onHoverItems;
	std::function<void()> onClicked;
	RightClickMenuItem* AddItem(const std::string& title, std::function<void()> onClickFunction);
	RightClickMenuItem* AddItem(const std::string& title);

	void SetTitle(const std::string& title) 
	{
		this->text = title;
	}

	void SetIsEnabled(const bool isEnabled) 
	{
		this->isEnabled = isEnabled;
	}

	void SetIsVisible(const bool isVisible) 
	{
		this->isVisible = isVisible;
	}

	std::string GetTitle() const
	{
		return text;
	}

	bool GetIsEnabled() const
	{
		return isEnabled;
	}

	bool GetIsVisible() const
	{
		return isVisible;
	}

private:
	std::string text;
	bool isEnabled = true;
	bool isVisible = true;
};

class RightClickMenu
{
public:
	RightClickMenu() = delete;
	RightClickMenu(const std::string& uniqueName);
	~RightClickMenu();
	std::vector<RightClickMenuItem*> items;
	RightClickMenuState Check(const bool blockOpen);
	bool Draw();
	RightClickMenuItem* AddItem(const std::string& title, const std::function<void()> onClickFunction);
	RightClickMenuItem* AddItem(const std::string& title);
private:
	static bool isDrawn;
	static std::string isDrawnName;
	static bool isFocusCorrect;
	void DrawRecursive(const RightClickMenuItem& item) const;
	std::string nameId;
};

namespace MenuBuilder
{
	void DrawText(const std::string );
}
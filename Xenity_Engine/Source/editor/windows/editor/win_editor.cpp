#include <editor/editor.h>

#include <ShObjIdl.h>

void Editor::OpenLinkInWebBrowser(const std::string& link)
{
	const std::wstring wLink = std::wstring(link.begin(), link.end());
	ShellExecute(0, 0, wLink.c_str(), 0, 0, SW_SHOW);
}

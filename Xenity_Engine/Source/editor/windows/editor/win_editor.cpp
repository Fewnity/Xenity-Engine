// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include <editor/editor.h>

#if defined(_WIN32) || defined(_WIN64)
#include <ShObjIdl.h>
#endif

void Editor::OpenLinkInWebBrowser(const std::string& link)
{
	#if defined(_WIN32) || defined(_WIN64)
	const std::wstring wLink = std::wstring(link.begin(), link.end());
	ShellExecute(0, 0, wLink.c_str(), 0, 0, SW_SHOW);
	#endif
}

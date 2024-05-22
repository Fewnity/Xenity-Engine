// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Grégory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#if defined(EDITOR)

#include <variant>
#include <Windows.h>
#include <ShObjIdl.h>

// ImGui
#include <imgui/imgui.h>
#include <imgui/imgui_stdlib.h>

#include <editor/ui/editor_ui.h>
#include <editor/editor.h>

std::string EditorUI::OpenFolderDialog(const std::string& title, const std::string& defaultLocation)
{
	std::string path = "";

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	IFileOpenDialog* pFileOpen;

	HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pFileOpen));


	if (SUCCEEDED(hr)) 
	{
		// Set default location
		if (!defaultLocation.empty())
		{
			IShellItem* pCurFolder = NULL;
			const std::wstring wstringDefaultLocTemp = std::wstring(defaultLocation.begin(), defaultLocation.end());
			const LPCWSTR wstringDefaultLoc = wstringDefaultLocTemp.c_str();
			HRESULT hr = SHCreateItemFromParsingName(wstringDefaultLoc, NULL, IID_PPV_ARGS(&pCurFolder));
			if (SUCCEEDED(hr))
			{
				pFileOpen->SetFolder(pCurFolder);
				pCurFolder->Release();
			}
		}
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
		const std::wstring tempTitle = std::wstring(title.begin(), title.end());

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
					const std::wstring wst = ss.str();
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

std::string EditorUI::OpenFileDialog(const std::string& title, const std::string& defaultLocation)
{
	std::string path = "";

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	IFileSaveDialog* pFileOpen;

	HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pFileOpen));

	if (SUCCEEDED(hr))
	{
		// Set default location
		if (!defaultLocation.empty())
		{
			IShellItem* pCurFolder = NULL;
			const std::wstring wstringDefaultLocTemp = std::wstring(defaultLocation.begin(), defaultLocation.end());
			const LPCWSTR wstringDefaultLoc = wstringDefaultLocTemp.c_str();
			HRESULT hr = SHCreateItemFromParsingName(wstringDefaultLoc, NULL, IID_PPV_ARGS(&pCurFolder));
			if (SUCCEEDED(hr))
			{
				pFileOpen->SetFolder(pCurFolder);
				pCurFolder->Release();
			}
		}
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
		std::wstring tempTitle = std::wstring(title.begin(), title.end());

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
						const std::wstring wst = ss.str();

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

std::string EditorUI::SaveFileDialog(const std::string& title, const std::string& defaultLocation)
{
	std::string path = "";

	CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	IFileSaveDialog* pFileOpen;

	HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog, NULL, CLSCTX_ALL, IID_PPV_ARGS(&pFileOpen));

	if (SUCCEEDED(hr))
	{
		// Set default location
		if (!defaultLocation.empty())
		{
			IShellItem* pCurFolder = NULL;
			const std::wstring wstringDefaultLocTemp = std::wstring(defaultLocation.begin(), defaultLocation.end());
			const LPCWSTR wstringDefaultLoc = wstringDefaultLocTemp.c_str();
			HRESULT hr = SHCreateItemFromParsingName(wstringDefaultLoc, NULL, IID_PPV_ARGS(&pCurFolder));
			if (SUCCEEDED(hr))
			{
				pFileOpen->SetFolder(pCurFolder);
				pCurFolder->Release();
			}
		}
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
		const std::wstring tempTitle = std::wstring(title.begin(), title.end());

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

						const size_t pathSize = wst.size();
						const size_t extSize = fileExtChoosed.size();

						const std::wstring endOfTheFile = wst.substr(pathSize - extSize);

						bool foundExt = true;
						for (size_t i2 = 0; i2 < extSize; i2++)
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

#endif // #if defined(EDITOR)
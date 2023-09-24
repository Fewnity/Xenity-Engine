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
#include "../../engine/graphics/skybox.h"

std::string EditorUI::OpenFolderDialog(const std::string& title)
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

std::string EditorUI::OpenFileDialog(const std::string& title)
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

std::string EditorUI::SaveFileDialog(const std::string& title)
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

#endif // #if defined(EDITOR)
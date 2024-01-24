#if defined(EDITOR)

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#include <editor/ui/editor_ui.h>

DialogResult EditorUI::OpenDialog(const std::string& title, const std::string& message, DialogType type)
{
	DialogResult dialogResult = DialogResult::Dialog_CANCEL;
#if defined(_WIN32) || defined(_WIN64)
	int windowsType = MB_OK;
	switch (type)
	{
	case DialogType::Dialog_Type_OK:
		windowsType = MB_OK;
		break;
	case DialogType::Dialog_Type_OK_CANCEL:
		windowsType = MB_OKCANCEL;
		break;
	case DialogType::Dialog_Type_ABORT_RETRY_IGNORE:
		windowsType = MB_ABORTRETRYIGNORE;
		break;
	case DialogType::Dialog_Type_YES_NO_CANCEL:
		windowsType = MB_YESNOCANCEL;
		break;
	case DialogType::Dialog_Type_YES_NO:
		windowsType = MB_YESNO;
		break;
	case DialogType::Dialog_Type_RETRY_CANCEL:
		windowsType = MB_RETRYCANCEL;
		break;
	default:
		break;
	}

	const int result = MessageBoxA(NULL, message.c_str(), title.c_str(), windowsType | MB_ICONEXCLAMATION); // 6 7 2 (cross is 2)

	if (result == 6) 
	{
		dialogResult = DialogResult::Dialog_YES;
	}
	else if (result == 7) 
	{
		dialogResult = DialogResult::Dialog_NO;
	}
	else if (result == 2) 
	{
		dialogResult = DialogResult::Dialog_CANCEL;
	}
#endif
	return dialogResult;
}

#endif
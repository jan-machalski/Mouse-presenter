#define NOMINMAX
#include "app_presenter.h"
# include "resource.h"
#include <windows.h>
#include <stdexcept>

#define ID_CLOSE_DIALOG_HOTKEY 52

INT_PTR CALLBACK DialogProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    
    switch (uMsg) {
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
            // Zamkniêcie okna dialogowego po naciœniêciu przycisku OK lub Anuluj
            EndDialog(hwnd, LOWORD(wParam));
            return TRUE;
        }
        break;
    case WM_CLOSE:
        // Zamkniêcie okna dialogowego przy próbie zamkniêcia przez Alt+F4
        EndDialog(hwnd, IDCANCEL);
        return TRUE;
    case WM_HOTKEY:
        if (wParam == ID_CLOSE_DIALOG_HOTKEY) {
            // Zamkniêcie okna dialogowego po naciœniêciu Ctrl+F12
            DestroyWindow(hwnd);
            return TRUE;
        }
        break;
    }
    return NULL;
}

int WINAPI wWinMain(HINSTANCE instance,
	HINSTANCE /*prevInstance*/,
	LPWSTR /*command_line*/,
	int show_command)
{
    try
    {
        HWND hwndDlg = CreateDialogParam(instance, MAKEINTRESOURCE(IDD_DIALOG2), NULL, DialogProc, 0);
        RegisterHotKey(hwndDlg, ID_CLOSE_DIALOG_HOTKEY, MOD_CONTROL, VK_F12);
        SetWindowLong(hwndDlg, GWL_EXSTYLE, GetWindowLong(hwndDlg, GWL_EXSTYLE) | WS_EX_TOOLWINDOW);
        ShowWindow(hwndDlg, SW_SHOWNORMAL);
        app_presenter app{ instance };
        return app.run(show_command);
    }
    catch (const std::exception& e) {
        int errorCode = GetLastError();
        wchar_t errorMsg[256];
        swprintf(errorMsg,256, L"Failed with error %d:\n%s", errorCode, e.what());
        MessageBox(NULL, errorMsg, L"B³¹d", MB_OK | MB_ICONERROR);

    }
}



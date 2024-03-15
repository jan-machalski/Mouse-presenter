#include "app_presenter.h"
#include "resource.h"
#include <stdexcept>


#define ID_CLOSE_HOTKEY 1
#define ID_PULSING_HOTKEY 2
#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_EXIT 11
#define ID_TRAY_COLOR_PICKER 12
#define ID_TRAY_INI_OPEN 13

std::wstring const app_presenter::s_class_name{ L"Presenter Window" };

bool app_presenter::register_class()
{
	WNDCLASSEXW desc{};
	if (GetClassInfoExW(m_instance, s_class_name.c_str(),
		&desc) != 0)
		return true;
	desc = {
	.cbSize = sizeof(WNDCLASSEXW),
	.lpfnWndProc = window_proc_static,
	.hInstance = m_instance,
	.hIcon = static_cast<HICON>(LoadImageW(m_instance,
		MAKEINTRESOURCEW(ID_APPICON),
		IMAGE_ICON,
		0, 0,
		LR_SHARED | LR_DEFAULTSIZE)),
	.hCursor = LoadCursorW(nullptr, L"IDC_ARROW"),
	.lpszClassName = s_class_name.c_str()
	};

	return RegisterClassExW(&desc) != 0;
}

HWND app_presenter::create_window()
{
	HWND hwnd = CreateWindowExW(
		WS_EX_LAYERED | WS_EX_TOPMOST |WS_EX_TRANSPARENT| WS_EX_TOOLWINDOW,
		s_class_name.c_str(),
		L"Presenter",
		WS_POPUP | WS_VISIBLE,
		0,
		0,
		GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
		nullptr,
		nullptr,
		m_instance,
		this);
		SetLayeredWindowAttributes(hwnd, 0,51, LWA_ALPHA);
		RegisterHotKey(hwnd, ID_PULSING_HOTKEY, MOD_ALT | MOD_SHIFT, 'C');

		RegisterHotKey(hwnd, ID_CLOSE_HOTKEY, MOD_ALT | MOD_SHIFT, VK_F4);
	return hwnd;
}

LRESULT app_presenter::window_proc_static(
	HWND window,
	UINT message,
	WPARAM wparam,
	LPARAM lparam)
{
	app_presenter* app = nullptr;
	if (message == WM_NCCREATE)
	{
		app = static_cast<app_presenter*>(
			reinterpret_cast<LPCREATESTRUCTW>(lparam)
			-> lpCreateParams);
		SetWindowLongPtrW(window, GWLP_USERDATA,
			reinterpret_cast<LONG_PTR>(app));
	}
	else
		app = reinterpret_cast<app_presenter*>(
			GetWindowLongPtrW(window, GWLP_USERDATA));
	LRESULT res = app ?
		app-> window_proc(window, message, wparam, lparam) :
		DefWindowProcW(window, message, wparam, lparam);
	if (message == WM_NCDESTROY)
		SetWindowLongPtrW(window, GWLP_USERDATA, 0);
	return res;
}

LRESULT app_presenter::window_proc(
	HWND window,
	UINT message,
	WPARAM wparam,
	LPARAM lparam)
{
	switch (message)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(window, &ps);
		POINT pos;
		GetCursorPos(&pos);

		HDC memDC = CreateCompatibleDC(hdc);
		HBITMAP memBmp = CreateCompatibleBitmap(hdc, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
		HBITMAP oldBmp = reinterpret_cast<HBITMAP>(SelectObject(memDC, memBmp));

		COLORREF backgroundColor = RGB(0, 0, 0);
		RECT clientRect;
		GetClientRect(window, &clientRect);

		HBRUSH backgroundBrush = CreateSolidBrush(backgroundColor);
		FillRect(memDC, &clientRect, backgroundBrush);

		DeleteObject(backgroundBrush);

		HBRUSH brush = CreateSolidBrush(color);
		HBRUSH old_brush = (HBRUSH)SelectObject(memDC, brush);
		HPEN pen = CreatePen(PS_SOLID, 1, color);
		HPEN old_pen = (HPEN)SelectObject(memDC, pen);
		Ellipse(memDC, pos.x - radius, pos.y-radius, pos.x + radius, pos.y + radius);

		SelectObject(memDC, old_brush);
		SelectObject(memDC, old_pen);
		DeleteObject(brush);
		DeleteObject(pen);

		BitBlt(hdc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), memDC, 0, 0, SRCCOPY);
		DeleteObject(SelectObject(memDC, oldBmp));
		DeleteDC(memDC);


		DeleteDC(hdc);
		EndPaint(window, &ps);

	}
	break;
	case WM_TRAYICON:
		if (LOWORD(lparam) == WM_RBUTTONUP) {
			HMENU hPopupMenu = CreatePopupMenu();
			AppendMenu(hPopupMenu, MF_STRING, ID_TRAY_EXIT, L"Exit");
			AppendMenu(hPopupMenu, MF_STRING, ID_TRAY_COLOR_PICKER, L"Choose color");
			AppendMenu(hPopupMenu, MF_STRING, ID_TRAY_INI_OPEN, L"Open .ini file");

			POINT pt;
			GetCursorPos(&pt);

			TrackPopupMenu(hPopupMenu, TPM_LEFTALIGN | TPM_RIGHTBUTTON, pt.x, pt.y, 0, window, NULL);
			DestroyMenu(hPopupMenu);
		}
		break;
	case WM_COMMAND:
	{
		switch (LOWORD(wparam))
		{
			case WM_CREATE:
			case ID_TRAY_EXIT:
				if (window == m_main)
					PostQuitMessage(EXIT_SUCCESS);
				return 0;
			case ID_TRAY_COLOR_PICKER:
			{
				//https://cpp0x.pl/kursy/Kurs-WinAPI-C++/Podstawy/Okna-dialogowe-cz-4/187
				CHOOSECOLOR ccl;
				COLORREF TabKol[16];
				BOOL bResult;

				ZeroMemory(&ccl, sizeof(CHOOSECOLOR));
				ccl.lStructSize = sizeof(CHOOSECOLOR);
				ccl.hwndOwner = window;
				ccl.lpCustColors = TabKol;
				ccl.Flags = CC_ANYCOLOR;
				if(ChooseColor(&ccl));
					color = ccl.rgbResult;
			}
			break;
			case ID_TRAY_INI_OPEN:
			{
				HINSTANCE hInstance = ShellExecute(NULL, L"open", L"settings.ini", NULL, NULL, SW_SHOWNORMAL);
				if ((int)hInstance <= 32) {
					MessageBox(NULL, L"Nie mo¿na otworzyæ pliku .ini", L"B³¹d", MB_ICONERROR);
				}
			}
			break;
		}
	}
	case WM_HOTKEY:
		if (wparam == ID_CLOSE_HOTKEY)
			DestroyWindow(window);
		else if (wparam == ID_PULSING_HOTKEY)
		{
			if (pulsing == true)
			{
				pulsing = false;
				radius = max_radius;
			}
			else
			{
				pulsing = true;
				radius = max_radius;
				radius_diff = -1;
			}
		}
		break;
	case WM_TIMER:
		if(wparam == 1)
			InvalidateRect(window, NULL, FALSE);
		else if (wparam == 2)
		{
			if (pulsing)
			{
				radius += radius_diff;
				if (radius == min_radius || radius == max_radius)
					radius_diff = (-1) * radius_diff;
			}
		}
		break;
	case WM_CLOSE:
		DestroyWindow(m_main);
		return 0;
	
	case WM_DESTROY:
		if (window == m_main)
			PostQuitMessage(EXIT_SUCCESS);
		return 0;
	}
	return DefWindowProcW(window, message, wparam, lparam);
}

app_presenter::app_presenter(HINSTANCE instance)
	: m_instance{ instance }, m_main{}
{
	register_class();

	/*start_color = GetPrivateProfileInt(L"Settings", L"StartColor", RGB(0, 0, 0), L"settings.ini");
	max_radius = GetPrivateProfileInt(L"Settings", L"MaxRadius", 0, L"settings.ini");
	min_radius = GetPrivateProfileInt(L"Settings", L"MinRadius", 0, L"settings.ini");
	pulse_cycle_length = GetPrivateProfileInt(L"Settings", L"PulseCycleLength", 0, L"settings.ini");*/

	m_main = create_window();
	color = start_color;
	radius = 50;
	radius_diff = -1; 
	pulsing = true;
}

int app_presenter::run(int show_command)
{
	SetTimer(m_main, 1, 10, NULL);
	SetTimer(m_main, 2, pulse_cycle_length/2/(max_radius-min_radius), NULL);
	ShowWindow(m_main, show_command);

	NOTIFYICONDATA nid = { sizeof(NOTIFYICONDATA) };
	nid.hWnd = m_main;
	nid.uID = 1;
	nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
	nid.uCallbackMessage = WM_TRAYICON;
	nid.hIcon = static_cast<HICON>(LoadImageW(m_instance,
		MAKEINTRESOURCEW(ID_APPICON),
		IMAGE_ICON,
		0, 0,
		LR_SHARED | LR_DEFAULTSIZE));
	wcscpy_s(nid.szTip, L"Presenter");
	Shell_NotifyIcon(NIM_ADD, &nid);

	MSG msg{};
	BOOL result = TRUE;
	while ((result = GetMessageW(&msg, nullptr, 0, 0)) != 0)
	{
		if (result == -1)
			return EXIT_FAILURE;
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}

	Shell_NotifyIcon(NIM_DELETE, &nid);
	return EXIT_SUCCESS;
}




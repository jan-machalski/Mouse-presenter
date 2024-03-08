#include "app.h"
#include <stdexcept>
std::wstring const app::s_class_name{ L" Window" };

bool app::register_class()
{
	WNDCLASSEXW desc{};
	if (GetClassInfoExW(m_instance, s_class_name.c_str(),
		&desc) != 0)
		return true;
	desc = {
	.cbSize = sizeof(WNDCLASSEXW),
	.lpfnWndProc = window_proc_static,
	.hInstance = m_instance,
	.hCursor = LoadCursorW(nullptr, L"IDC_ARROW"),
	.hbrBackground =
		CreateSolidBrush(RGB(255, 255, 0)),
	.lpszClassName = s_class_name.c_str()
	};
	return RegisterClassExW(&desc) != 0;
}

HWND app::create_window()
{
	HWND hwnd = CreateWindowExW(
		WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW,
		s_class_name.c_str(),
		L"app",
		WS_CAPTION,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		nullptr,
		nullptr,
		m_instance,
		this);
	SetLayeredWindowAttributes(hwnd, 0, 255 /5, LWA_ALPHA);
	HRGN circle = CreateEllipticRgn(0, 0, 100, 100);
	SetWindowRgn(hwnd, circle, true);
	SetWindowLong(hwnd, GWL_STYLE, 0);
	return hwnd;
}

LRESULT app::window_proc_static(
	HWND window,
	UINT message,
	WPARAM wparam,
	LPARAM lparam)
{
	app* ap = nullptr;
	if (message == WM_NCCREATE)
	{
		ap = static_cast<app*>(
			reinterpret_cast<LPCREATESTRUCTW>(lparam)
			->lpCreateParams);
		SetWindowLongPtrW(window, GWLP_USERDATA,
			reinterpret_cast<LONG_PTR>(ap));
	}
	else
		ap = reinterpret_cast<app*>(
			GetWindowLongPtrW(window, GWLP_USERDATA));
	LRESULT res = ap ?
		ap->window_proc(window, message, wparam, lparam) :
		DefWindowProcW(window, message, wparam, lparam);
	if (message == WM_NCDESTROY)
		SetWindowLongPtrW(window, GWLP_USERDATA, 0);
	return res;
}

LRESULT app::window_proc(
	HWND window,
	UINT message,
	WPARAM wparam,
	LPARAM lparam)
{
	LPPOINT lpPoint = (POINT*)malloc(sizeof(POINT));
	switch (message)
	{
	case WM_CLOSE:
		DestroyWindow(m_main);
		return 0;
	case WM_MOUSEMOVE:
			GetCursorPos(lpPoint);
			move_window(m_main,lpPoint);
			break;
	case WM_DESTROY:
		if (window == m_main)
			PostQuitMessage(EXIT_SUCCESS);
		return 0;
	}
	return DefWindowProcW(window, message, wparam, lparam);
}

app::app(HINSTANCE instance) : m_instance{ instance }, m_main{}
{
	register_class();
	m_main = create_window();
}

int app::run(int show_command)
{
	ShowWindow(m_main, show_command);
	SetLayeredWindowAttributes(m_main, 0, 255 * 50 / 100, LWA_ALPHA);
	MSG msg{};
	BOOL result = TRUE;
	while ((result = GetMessageW(&msg, nullptr, 0, 0)) != 0)
	{
		if (result == -1)
			return EXIT_FAILURE;
		TranslateMessage(&msg);
		DispatchMessageW(&msg);
	}
	return EXIT_SUCCESS;
}

void app::move_window(HWND window,
	LPPOINT params)
{
	SetWindowPos(
		window,
		nullptr,
		params->x - 50,
		params->y - 50,
		0,
		0,
		SWP_NOSIZE
	);
}
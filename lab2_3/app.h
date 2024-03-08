#pragma once


#include <Windows.h>
#include <string>
class app
{
private:
	bool register_class();
	static std::wstring const s_class_name;
	static LRESULT CALLBACK window_proc_static(
		HWND window,
		UINT message,
		WPARAM wparam,
		LPARAM lparam);
	LRESULT window_proc(
		HWND window,
		UINT message,
		WPARAM wparam,
		LPARAM lparam);
	HWND create_window();
	HINSTANCE m_instance;
	HWND m_main, m_popup;
	void move_window(HWND window, LPPOINT params);
public:
	app(HINSTANCE instance);
	int run(int show_command);
};

#pragma once
#include <Windows.h>
#include <string>
class app_presenter
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
	HWND m_main;
	COLORREF color;
	int radius;
	COLORREF start_color = RGB(255, 255, 0);
	int max_radius = 50;
	int min_radius = 25;
	int pulse_cycle_length = 2000;//ms
	int radius_diff;
	bool pulsing;
public:
	app_presenter(HINSTANCE instance);
	int run(int show_command);
};
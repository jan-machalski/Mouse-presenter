#pragma once
#include <Windows.h>
#include <string>
#define MAX_PATH 512

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
	COLORREF start_color;
	int max_radius;
	int min_radius;
	int pulse_cycle_length;//ms
	int radius_diff;
	bool pulsing;
	void reload_config();
	wchar_t* ini_path;
	void get_ini_path();
public:
	app_presenter(HINSTANCE instance);
	int run(int show_command);
};
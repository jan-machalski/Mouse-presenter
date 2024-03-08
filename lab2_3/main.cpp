#define NOMINMAX
#include <windows.h>
#include "app.h"
int WINAPI wWinMain(HINSTANCE instance,
	HINSTANCE /*prevInstance*/,
	LPWSTR /*command_line*/,
	int show_command)
{	app ap{ instance };
	return ap.run(show_command);}
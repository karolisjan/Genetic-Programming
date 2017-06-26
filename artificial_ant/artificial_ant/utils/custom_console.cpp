#include <Windows.h>
#include <string>
#include <iostream>
#include "custom_console.h"

using namespace std;

namespace Console
{
	void SetUp(int x , int y, int cx, int cy, char* backgroundColor, char* textColor)
	{
		SetWindowPos(GetConsoleWindow(), 0, x, y, cx, cy, SWP_NOSIZE | SWP_NOZORDER);

		RECT rectDesktop;
		HWND hDesktop = GetDesktopWindow();
		CONSOLE_FONT_INFOEX fontOut;
		fontOut.cbSize = sizeof(CONSOLE_FONT_INFOEX);
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

		GetWindowRect(hDesktop, &rectDesktop);
		GetCurrentConsoleFontEx(hConsole, FALSE, &fontOut);

		string consoleColors = "COLOR ";
		consoleColors.append(backgroundColor);
		consoleColors.append(textColor);

		system(consoleColors.c_str());

		COORD fontSize;
		CONSOLE_FONT_INFOEX fontIn;
		fontIn.cbSize = sizeof(CONSOLE_FONT_INFOEX);

		int desktopWidth = GetSystemMetrics(SM_CXSCREEN);
		int dekstopHeight = GetSystemMetrics(SM_CYSCREEN);

		if (rectDesktop.right == 1920) {
			fontSize.X = 80;
			fontSize.Y = 24;
			fontIn.dwFontSize = fontSize;
		}
		else {
			fontSize.X = 80;
			fontSize.Y = 36;
			fontIn.dwFontSize = fontSize;
		}

		fontIn.nFont = 1;
		fontIn.FontFamily = 54;
		fontIn.FontWeight = 400;

		wchar_t consolasFont[] = L"Consolas";
		wcsncpy(fontIn.FaceName, consolasFont, sizeof(consolasFont));

		BOOL bResult = SetCurrentConsoleFontEx(hConsole, FALSE, &fontIn);
	}

	bool HideCursor()
	{
		HANDLE myconsole = GetStdHandle(STD_OUTPUT_HANDLE);

		CONSOLE_CURSOR_INFO CURSOR;

		BOOL result;

		CURSOR.dwSize = 1;
		CURSOR.bVisible = FALSE;

		return SetConsoleCursorInfo(myconsole, &CURSOR);
	}

	void SetCursorPosition(int x, int y)
	{
		static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		std::cout.flush();
		COORD coord = { (SHORT)x, (SHORT)y };
		SetConsoleCursorPosition(hOut, coord);
	}

	void Maximise()
	{
		system("mode 650");
		ShowWindow(GetConsoleWindow(), SW_MAXIMIZE);
	}

	void Move(int x, int y)
	{
		SetWindowPos(GetConsoleWindow(), 0, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER);
	}

	void Resize(int x, int y, int cx, int cy)
	{
		SetWindowPos(GetConsoleWindow(), 0, x, y, cx, cy, SWP_NOSIZE | SWP_NOZORDER);
	}

	void cls()
	{
		// Get the Win32 handle representing standard output.
		// This generally only has to be done once, so we make it static.
		static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

		CONSOLE_SCREEN_BUFFER_INFO csbi;
		COORD topLeft = { 0, 0 };

		// std::cout uses a buffer to batch writes to the underlying console.
		// We need to flush that to the console because we're circumventing
		// std::cout entirely; after we clear the console, we don't want
		// stale buffered text to randomly be written out.
		std::cout.flush();

		// Figure out the current width and height of the console window
		if (!GetConsoleScreenBufferInfo(hOut, &csbi)) {
			// TODO: Handle failure!
			abort();
		}
		DWORD length = csbi.dwSize.X * csbi.dwSize.Y;

		DWORD written;

		// Flood-fill the console with spaces to clear it
		FillConsoleOutputCharacter(hOut, TEXT(' '), length, topLeft, &written);

		// Reset the attributes of every character to the default.
		// This clears all background colour formatting, if any.
		FillConsoleOutputAttribute(hOut, csbi.wAttributes, length, topLeft, &written);

		// Move the cursor back to the top left for the next sequence of writes
		SetConsoleCursorPosition(hOut, topLeft);
	}
}

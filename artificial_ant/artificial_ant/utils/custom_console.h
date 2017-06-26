#pragma once

namespace Console
{
	void SetUp(int x = 0, int y = 0, int cx = 0, int cy = 0, char* backgroundColor = "F", char* textColor = "0");

	bool HideCursor();

	void SetCursorPosition(int x, int y);

	void Maximise();

	void Move(int x, int y);

	void Resize(int x, int y, int cx, int cy);

	void cls();
}

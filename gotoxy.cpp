#include "gotoxy.h"
#include <conio.h>
#include <windows.h>

void gotoxy() {
	CONSOLE_SCREEN_BUFFER_INFO presentCur;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &presentCur);

	COORD Cur;
	Cur.X = 0;
	Cur.Y = presentCur.dwCursorPosition.Y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cur);
}

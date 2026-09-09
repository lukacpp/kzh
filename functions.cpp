#include "functions.h"

void SimulateMouse(int iMWheelCode)
{
	INPUT Input = {0};

	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_WHEEL;
	Input.mi.dwExtraInfo = 0;

	Input.mi.time = 0;
	Input.mi.mouseData = iMWheelCode;
	SendInput(1, &Input, sizeof(INPUT));

	ZeroMemory(&Input, sizeof(INPUT));
}
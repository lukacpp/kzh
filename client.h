#pragma once

#include "main.h"

extern bool FirstFrame;

void ConsolePrintColor(BYTE R, BYTE G, BYTE B, char* string);

void HookUserMessages();
void HookEngineMessages();
void HookFunction();
#pragma once

#include <stdio.h>
#include <Windows.h>

inline void WriteLog(const char* message)
{
	OutputDebugStringA(message);
	printf(message);
}
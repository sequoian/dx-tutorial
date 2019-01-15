#pragma once

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdarg.h>
#include <Windows.h>
#include <ctime>

enum LogType {
	LOG_TYPE_PRINT,
	LOG_TYPE_WARNING,
	LOG_TYPE_ERROR
};

// Print log to console
int WriteLog(LogType type, const char* fmt, ...);

// Macros
#ifdef _DEBUG
#define DEBUG_PRINT(fmt, ...) WriteLog(LOG_TYPE_PRINT, fmt, __VA_ARGS__)
#define DEBUG_WARN(fmt, ...) WriteLog(LOG_TYPE_WARNING, fmt, __VA_ARGS__)
#define DEBUG_ERROR(fmt, ...) WriteLog(LOG_TYPE_ERROR, fmt, __VA_ARGS__)
#else
#define DEBUG_PRINT(fmt, ...) 0
#define DEBUG_WARN(fmt, ...) 0
#define DEBUG_ERROR(fmt, ...) 0
#endif
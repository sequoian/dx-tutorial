#include "Assert.h"

// needed to use vsprintf in visual studio
#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "WriteLog.h"
#include <stdarg.h>
#include <stdio.h>
#include <cstring>


void PrintAssertMessage(const char* func, const char* file, int line, const char* format, ...)
{
	va_list argList;
	va_start(argList, format);
		
	char buffer[900];
	char func_str[200];
	char file_str[200];
	char msg[500];

	sprintf(func_str, "Assertion failed in %s.\n\t", func);
	strcpy(buffer, func_str);

	sprintf(file_str, "File: %s (Line %i)\n\tMessage: ", file, line);
	strcat(buffer, file_str);

	if (format)
	{
		vsprintf(msg, format, argList);
		strcat(buffer, msg);
	}

	buffer[899] = '\0';

	int charsWritten = DEBUG_ERROR(buffer);
	va_end(argList);
}
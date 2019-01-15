#include "WriteLog.h"

int WriteLog(const char* fmt, ...)
{
	const unsigned int maxChars = 1023;
	char buffer[maxChars + 1];

	va_list args;
	va_start(args, fmt);
	int charsWritten = vsprintf(buffer, fmt, args);
	buffer[maxChars] = '\0';
	OutputDebugStringA(buffer);
	printf(buffer);
	va_end(args);

	return charsWritten;
}
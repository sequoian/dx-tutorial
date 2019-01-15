#include "WriteLog.h"


char* GetTimeStr()
{
	time_t rawTime;
	struct tm* timeInfo;
	char buffer[80];

	time(&rawTime);
	timeInfo = localtime(&rawTime);

	strftime(buffer, 80, "[%F %T] ", timeInfo);

	return buffer;
}


int WriteLog(LogType type, const char* fmt, ...)
{
	const unsigned int maxChars = 1023;
	char buffer[maxChars + 1];
	char msg[maxChars - 100];

	va_list args;
	va_start(args, fmt);
	int charsWritten = vsprintf(msg, fmt, args);

	// timestamp
	strcpy(buffer, GetTimeStr());

	// log type
	switch (type)
	{
	case LOG_TYPE_PRINT:
		strcat(buffer, "[DEBUG_PRINT] ");
		break;
	case LOG_TYPE_WARNING:
		strcat(buffer, "[DEBUG_WARNING] ");
		break;
	case LOG_TYPE_ERROR:
		strcat(buffer, "[DEBUG_ERROR] ");
		break;
	default:
		break;
	}

	// message
	strcat(buffer, msg);
	// newline
	strcat(buffer, "\n");

	buffer[maxChars] = '\0';
	OutputDebugStringA(buffer);
	printf(buffer);
	va_end(args);

	return charsWritten;
}
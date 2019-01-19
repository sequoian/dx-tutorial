#include "WriteLog.h"

// needed to use vsprintf in visual studio
#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <Windows.h>
#include <ctime>
#include <stdarg.h>
#include <stdio.h>
#include "Assert.h"

static FILE* file = nullptr;

const wchar_t* folderName = L"Logs";


char* GetTimeStr(const char* format)
{
	time_t rawTime;
	struct tm* timeInfo;
	char buffer[80];

	time(&rawTime);
	timeInfo = localtime(&rawTime);

	strftime(buffer, 80, format, timeInfo);

	return buffer;
}


bool StartUpLogger()
{
	// Create console for printf output on debug
#ifdef _DEBUG
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
#endif

	ASSERT(file == nullptr);

	if (CreateDirectory(folderName, NULL) || ERROR_ALREADY_EXISTS == GetLastError())
	{
		char fileName[100];
		strcpy(fileName, "Logs/Output-");
		strcat(fileName, GetTimeStr("[%F]-[%H-%M-%S].log"));

		// open file stream
		file = fopen(fileName, "w");
		if (file == nullptr)
		{
			DEBUG_ERROR("Could not open file: %s", fileName);
			return false;
		}
	}
	else
	{
		DEBUG_ERROR("Could not create directory: %s", folderName);
		return false;
	}

	return true;
}


void ShutDownLogger()
{
	// close file stream
	if (file != nullptr)
	{
		fclose(file);
		file = nullptr;
	}

	// free console on debug
#ifdef _DEBUG
	FreeConsole();
#endif
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
	strcpy(buffer, GetTimeStr("[%F %T]"));
	strcat(buffer, ": ");

	// log type
	switch (type)
	{
	case LOG_TYPE_WARNING:
		strcat(buffer, "[WARNING] ");
		break;
	case LOG_TYPE_ERROR:
		strcat(buffer, "[ERROR] ");
		break;
	default:
		break;
	}

	// message
	strcat(buffer, msg);
	// newline
	strcat(buffer, "\n");
	// null character at end
	buffer[maxChars] = '\0';

	// print to VS output
	OutputDebugStringA(buffer);

	// print to console
	printf(buffer);

	// print to file
	if (file != nullptr)
	{
		fprintf(file, buffer);
	}
	
	va_end(args);

	return charsWritten;
}
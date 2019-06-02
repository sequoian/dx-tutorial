#pragma once

#include <string>


inline std::wstring StringToWideString(const char* s)
{
	std::string str(s);
	std::wstring ws(str.length(), ' ');
	std::copy(str.begin(), str.end(), ws.begin());

	return ws;
}


inline std::string WideStringToString(const wchar_t* ws)
{
	std::wstring wstr(ws);
	std::string s(wstr.length(), ' ');
	std::copy(wstr.begin(), wstr.end(), s.begin());

	return s;
}
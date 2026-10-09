#pragma once
#include <atlconv.h>
#include <string>

inline CString StdToCString(const std::string& str)
{
	return CString(CA2W(str.c_str(), CP_UTF8));
}

inline std::string CStringToStd(const CString& str)
{
	return std::string(CW2A(str, CP_UTF8));
}

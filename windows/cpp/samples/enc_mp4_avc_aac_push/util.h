#pragma once

template<class Ch>
int to_int(const std::basic_string<Ch>& str)
{
	int num;
	basic_istringstream<Ch>(str) >> num;
	return num;
}

template<class Ch>
int to_int(const Ch* pch)
{
	return to_int(std::basic_string<Ch>(pch));
}

template<class Ch>
double to_double(const std::basic_string<Ch>& str)
{
	double num;
	std::basic_istringstream<Ch>(str) >> num;
	return num;
}

template<class Ch>
double to_double(const Ch* pch)
{
	return to_double(std::basic_string<Ch>(pch));
}

// Windows specific

inline bool compareNoCase(const wchar_t* arg1, const wchar_t* arg2)
{
	return (0 == _wcsicmp(arg1, arg2));
}

inline bool compareNoCase(const char* arg1, const char* arg2)
{
	return (0 == _stricmp(arg1, arg2));
}

inline std::wstring getExeDir()
{
	WCHAR exedir[MAX_PATH];
	GetModuleFileName(NULL, exedir, MAX_PATH);
	PathRemoveFileSpec(exedir);
	return std::wstring(exedir);
}

inline void deleteFile(const wchar_t* file)
{
	DeleteFile(file);
}

inline void printError(const wchar_t* action, const primo::error::ErrorInfo* e)
{
	using namespace std;

	if (action)
	{
		wcout << action << L": ";
	}

	if (primo::error::ErrorFacility::Success == e->facility())
	{
		wcout << L"Success" << endl;
		return;
	}

	if (e->message())
	{
		wcout << e->message() << L", ";
	}

	wcout << L"facility:" << e->facility() << L", error:" << e->code() << endl;
}
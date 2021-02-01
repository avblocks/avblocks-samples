#pragma once

// Windows specific

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
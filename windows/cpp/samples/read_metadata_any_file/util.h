#pragma once

using namespace std;

inline std::wstring getExeDir()
{
    WCHAR exedir[MAX_PATH];
    GetModuleFileName(NULL, exedir, MAX_PATH);
    PathRemoveFileSpec(exedir);
    return std::wstring(exedir);
}

class stdout_utf16
{
public:
	stdout_utf16()
	{
		// change stdout to Unicode. Cyrillic and Ideographic characters will appear in the console (console font is unicode).
		_setmode(_fileno(stdout), _O_U16TEXT);
	}

	~stdout_utf16()
	{
		// restore ANSI mode
		_setmode(_fileno(stdout), _O_TEXT); 
	}
};
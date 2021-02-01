#pragma once

using namespace std;

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

inline void deleteFile(const wchar_t* file)
{
	DeleteFile(file);
}

inline vector<uint8_t> readFileBytes(const wchar_t *name)
{  
    ifstream f(name, ios::binary);
    vector<uint8_t> bytes;
    if (f)
    {
        f.seekg(0, ios::end);
        size_t filesize = f.tellg();
        bytes.resize(filesize);
        f.seekg(0, ios::beg);        
        f.read((char*)&bytes[0], filesize);
    }

    return bytes;
} 

inline wstring getExeDir()
{
    WCHAR exedir[MAX_PATH];
    GetModuleFileName(NULL, exedir, MAX_PATH);
    PathRemoveFileSpec(exedir);
    return wstring(exedir);
}






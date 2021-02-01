/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/

#pragma once

/**
	This simple file based stream provides implementation of primo::Stream interface.
	It does nothing more than giving access to the original file through primo::Stream
	This implementation is for demonstration purposes only.
*/


class FileStream: public primo::Stream
{
	HANDLE m_hFile;
	std::wstring m_filePath;
	int m_openMode;

	mutable volatile LONG m_refCount;

protected:
	FileStream() :m_hFile(INVALID_HANDLE_VALUE), m_openMode(OpenModeRead), m_refCount(0)
	{

	}

	~FileStream()
	{
		close();
	}

public:

	static FileStream* create()
	{
		FileStream *obj = new FileStream();
		obj->retain();
		return obj;
	}

	int32_t retain() const 
	{ 
		return ::InterlockedIncrement(&m_refCount);
	}

	int32_t release() const 
	{ 
		int32_t ref = ::InterlockedDecrement(&m_refCount);

		if (0 == ref)
		{
			delete this;
		}

		return ref;
	}

	int32_t retainCount() const 
	{ 
		return m_refCount;
	}

	enum OpenMode
	{
		// Opens an existing file for reading.
		OpenModeRead  = 1,

		// Opens a file for writing and truncates the contents if the file already exists.
		OpenModeWrite = 2
	};

	void setFile(const wchar_t *filePath, OpenMode openMode)
	{
		m_filePath.clear();

		if(filePath)
			m_filePath = filePath;

		m_openMode = openMode;
	}

	// primo::codecs::Stream interface
	bool_t open()
	{
		if (isOpen())
			return seek(0);

		switch(m_openMode)
		{
		case OpenModeRead:
			m_hFile = CreateFile(m_filePath.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			break;
		case OpenModeWrite:
			m_hFile = CreateFile(m_filePath.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			break;
		default:
			return FALSE;
		}

		return isOpen();
	}

	void  close()
	{
		if(isOpen())
		{
			CloseHandle(m_hFile);
			m_hFile = INVALID_HANDLE_VALUE;
		}
	}

	bool_t read(void* buffer, int32_t bufferSize, int32_t* totalRead)
	{
		if(!isOpen())
			return FALSE;

		return TRUE == ReadFile(m_hFile, buffer, bufferSize, (DWORD*)totalRead, NULL);
	}

	int64_t size() const
	{
		bool closeWhenDone = false;
		
		const int64_t error = -1LL;

		if(!isOpen())
		{
			if (!((FileStream*)this)->open())
				return error;

			closeWhenDone = true;
		}

		LARGE_INTEGER fileSize;
		BOOL res = GetFileSizeEx(m_hFile, &fileSize);

		if (closeWhenDone)
			((FileStream*)this)->close();

		if (!res)
			return error;

		return fileSize.QuadPart;
	}

	int64_t position() const
	{
		const int64_t error = -1LL;

		if(!isOpen())
			return error;

		LARGE_INTEGER newPosition;
		LARGE_INTEGER offset;
		offset.QuadPart = 0;
		BOOL res = SetFilePointerEx(m_hFile, offset, &newPosition, FILE_CURRENT);

		if (!res)
			return error;

		return newPosition.QuadPart;
	}

	bool_t seek(int64_t ddwOffset)
	{
		if(!isOpen())
			return false;

		LARGE_INTEGER offset;
		offset.QuadPart = ddwOffset;
		return SetFilePointerEx(m_hFile, offset, NULL, FILE_BEGIN);
	}

	bool_t canRead() const
	{
		return (OpenModeRead & m_openMode) == OpenModeRead;
	}

	bool_t canWrite() const
	{
		return (OpenModeWrite & m_openMode) == OpenModeWrite;
	}

	bool_t canSeek() const
	{
		return TRUE;
	}

	bool_t write(const void* buffer, int32_t dataSize)
	{
		if(!isOpen())
			return FALSE;

		DWORD bytesWritten;
		return TRUE == WriteFile(m_hFile, buffer, dataSize, &bytesWritten, NULL);
	}

	bool_t isOpen() const { return (m_hFile != INVALID_HANDLE_VALUE); }
};

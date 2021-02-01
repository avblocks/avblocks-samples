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


// turn off_t into a 64-bit type.
#define _FILE_OFFSET_BITS 64


class FileStream: public primo::Stream
{
	FILE* m_hFile;
	std::string m_filePath;
	int m_openMode;

	mutable volatile int32_t m_refCount;

protected:
	FileStream(): m_hFile(NULL), m_openMode(OpenModeRead), m_refCount(0)
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
		return __sync_add_and_fetch(&m_refCount, 1);
	}

	int32_t release() const
	{
		int32_t ref = __sync_sub_and_fetch(&m_refCount, 1);

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
		OpenModeRead = 1,

		// Opens a file for writing and truncates the contents if the file already exists.
		OpenModeWrite = 2
	};


	void setFile(const char *filePath, OpenMode openMode)
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
			m_hFile = fopen(m_filePath.c_str(), "rb");
			break;
		case OpenModeWrite:
			m_hFile = fopen(m_filePath.c_str(), "wb");
			break;
		default:
			return 0;
		}

		return isOpen();
	}

	void  close()
	{
		if(isOpen())
		{
			fclose(m_hFile);
			m_hFile = NULL;
		}
	}

	bool_t read(void* buffer, int32_t bufferSize, int32_t* totalRead)
	{
		if(!isOpen())
			return 0;

		size_t bytesRead = fread(buffer, 1, bufferSize, m_hFile);

		if(totalRead)
			*totalRead = bytesRead;

		return (bytesRead == bufferSize) || (ferror(m_hFile) == 0);
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

		off_t oldPos = ftello(m_hFile);

		bool res1 = fseeko(m_hFile, 0, SEEK_END) == 0;

		off_t fileSize = ftello(m_hFile);

		bool res2 = fseeko(m_hFile, oldPos, SEEK_SET) == 0;

		if (closeWhenDone)
			((FileStream*)this)->close();

		if (!res1 || !res2)
			return error;

		return fileSize;
	}

	int64_t position() const
	{
		const int64_t error = -1LL;

		if(!isOpen())
			return error;

		return ftello(m_hFile);
	}

	bool_t seek(int64_t ddwOffset)
	{
		if(!isOpen())
			return false;

		return fseeko(m_hFile, ddwOffset, SEEK_SET) == 0;
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
		return 1;
	}

	bool_t write(const void* buffer, int32_t dataSize)
	{
		if(!isOpen())
			return 0;

		return fwrite(buffer, 1, dataSize, m_hFile) == dataSize;
	}

	bool_t isOpen() const { return (m_hFile != NULL); }
};

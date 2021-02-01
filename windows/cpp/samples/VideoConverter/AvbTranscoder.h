/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.  
*/
#pragma once

#include <string>

struct PresetDescriptor
{
	const char* const Id;
	bool AudioOnly;
	char * FileExtension;
};

// a collection of known AVBlocks presets
extern PresetDescriptor avb_presets[];

class AvbTranscoder
{
private:
	std::wstring m_errorMessage;
	std::wstring m_inputFile;
	std::wstring m_outputFile;
	std::string m_outputPreset;
	primo::avblocks::TranscoderCallback * m_callback;

	void FormatErrorMessage(const primo::error::ErrorInfo* e);

public:
	AvbTranscoder(void) : m_callback(NULL) {}
	virtual ~AvbTranscoder(void) {}

	void SetCallback(primo::avblocks::TranscoderCallback * callback)
	{
		m_callback = callback;
	}

	void SetInputFile(const wchar_t* filename)
	{
		if (filename)
			m_inputFile = filename;
		else
			m_inputFile.clear(); 
	}
	
	void SetOutputFile(const wchar_t* filename)
	{
		if (filename)
			m_outputFile = filename;
		else
			m_outputFile.clear();
	}
	
	void SetOutputPreset(const char* preset)
	{
		m_outputPreset = preset;
	}

	const wchar_t* GetErrorMessage()
	{
		return m_errorMessage.c_str();
	}

	bool Convert();

};

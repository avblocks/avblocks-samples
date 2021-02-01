/*
 *  Copyright (c) 2013 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.
*/

#pragma once

#include <string>

// stringstream
#include <sstream>

//dirname
#include <libgen.h>

#include <ApplicationServices/ApplicationServices.h>

#include <vector>
#include <fstream>

// Mac OSX / Unix specific implementations

inline bool compareNoCase(const char* arg1, const char* arg2)
{
    return 0 == strcasecmp(arg1, arg2);
}

inline std::string getExeDir()
{
    std::string dir;
	
	OSStatus status;
    CFDictionaryRef processInfoDict = NULL;
    CFStringRef processExecutable = NULL;
    
    ProcessSerialNumber psn;
    status = GetCurrentProcess(&psn);
    
    if (noErr == status)
    {
        processInfoDict = ProcessInformationCopyDictionary(&psn, kProcessDictionaryIncludeAllInformationMask);
        if (processInfoDict != NULL)
        {
			char exec_str[PATH_MAX];
			
            processExecutable = (CFStringRef)CFDictionaryGetValue(processInfoDict, kCFBundleExecutableKey);
            
            if (processExecutable != NULL)
			{
				processExecutable = (CFStringRef)CFRetain(processExecutable);
				
				CFStringGetCString(processExecutable, exec_str, PATH_MAX, kCFStringEncodingUTF8 );
                
                dir.assign(dirname(exec_str));
                
                CFRelease(processExecutable);
			}
            
            CFRelease(processInfoDict);
        }
    }
	
	return dir;
    
}

inline void deleteFile(const char* file)
{
    remove(file);
}

inline std::vector<uint8_t> readFileBytes(const char *name)
{
    std::ifstream f(name, std::ios::binary);
    std::vector<uint8_t> bytes;
    if (f)
    {
        f.seekg(0, std::ios::end);
        size_t filesize = f.tellg();
        bytes.resize(filesize);
        f.seekg(0, std::ios::beg);
        f.read((char*)&bytes[0], filesize);
    }
    
    return bytes;
}

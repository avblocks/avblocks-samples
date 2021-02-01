/*
 *  Copyright (c) 2016 Primo Software. All Rights Reserved.
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
#include <AVBlocks.h>
#include <PrimoUString.h>

#include <ApplicationServices/ApplicationServices.h>

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

inline void printError(const char* action, const primo::error::ErrorInfo* e)
{
    using namespace std;
    
    if (action)
    {
        cout << action << ": ";
    }
    
    if (primo::error::ErrorFacility::Success == e->facility())
    {
        cout << "Success" << endl;
        return;
    }
    
    if (e->message())
    {
        cout << primo::ustring(e->message()) << ", ";
    }
    
    cout << "facility:" << e->facility()
		  << ", error:" << e->code()
		  << ", hint:" << primo::ustring(e->hint())
		  << endl;
}

#pragma once

using namespace std;

#include <string>

#include <iostream>

// stringstream
#include <sstream>

//getpid
#include <unistd.h>

//dirname
#include <libgen.h>

// remove, sprintf
#include <stdio.h>

#include <sys/stat.h>

// strcasecmp
#include <strings.h>
#include <fstream>
#include <AVBlocks.h>

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

inline bool makeDir(std::string dir)
{
    if (mkdir(dir.c_str(), ACCESSPERMS) == 0 || errno == EEXIST)
        return true;
    
    return false;
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
        cout << e->message() << ", ";
    }
    
    cout << "facility:" << e->facility()
		  << ", error:" << e->code()
		  << ", hint:" << e->hint()
		  << endl;
}

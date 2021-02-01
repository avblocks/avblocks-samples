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
#include <vector>
#include <fstream>
#include <AVBlocks.h>
#include <PrimoReference++.h>
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
		  << endl;
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

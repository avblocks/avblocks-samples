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

#include <iostream>

//getpid
#include <unistd.h>

//dirname
#include <libgen.h>

// PATH_MAX
#include <linux/limits.h>

// remove, sprintf
#include <stdio.h>

// strcasecmp
#include <strings.h>

template<class Ch>
int to_int(const std::basic_string<Ch>& str)
{
	int num;
    std::basic_istringstream<Ch>(str) >> num;
	return num;
}

template<class Ch>
int to_int(const Ch* pch)
{
	return to_int(std::basic_string<Ch>(pch));
}

template<class Ch>
double to_double(const std::basic_string<Ch>& str)
{
	double num;
	std::basic_istringstream<Ch>(str) >> num;
	return num;
}

template<class Ch>
double to_double(const Ch* pch)
{
	return to_double(std::basic_string<Ch>(pch));
}

// Mac OSX / Unix specific implementations

inline bool compareNoCase(const char* arg1, const char* arg2)
{
    return 0 == strcasecmp(arg1, arg2);
}

inline std::string getExeDir()
{	
   /*
     Linux:
    /proc/<pid>/exe

    Solaris:
    /proc/<pid>/object/a.out (filename only)
    /proc/<pid>/path/a.out (complete pathname)

    BSD:
    /proc/<pid>/file

    */

    pid_t pid = getpid();
    
    char proc_link[256];
    sprintf(proc_link,"/proc/%d/exe",pid);

    char exe_path[PATH_MAX];
    int len = readlink(proc_link, exe_path, sizeof(exe_path) - 1);
    if(len > 0)
    {
        exe_path[len] = 0;
    }
    else
    {
        return std::string();
    }
    
    char * exe_dir = dirname(exe_path);    
    return std::string(exe_dir);
}

inline void deleteFile(const char* file)
{
    remove(file);
}

inline void printError(const primo::error::ErrorInfo* e)
{
	using namespace std;

	if (primo::error::ErrorFacility::Success == e->facility())
	{
		std::cout << "Success";
	}
	else
	{
		std::cout << "facility: " << e->facility() << ", error: " << e->code();

		if (e->message())
		{
			std::cout << ", " << primo::ustring(e->message());
		}

		if (e->hint())
		{
			std::cout << ", " << primo::ustring(e->hint());
		}
	}

	std::cout << std::endl;
}

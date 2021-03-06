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

#include <PrimoUString.h>

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

inline void printError(const char* action, const primo::error::ErrorInfo* e)
{
	if (action)
	{
		std::cout << action << ": ";
	}

	if (primo::error::ErrorFacility::Success == e->facility())
	{
		std::cout << "Success" << std::endl;
		return;
	}

	if (e->message())
	{
		std::cout << primo::ustring(e->message()) << " ";
	}

	std::cout << "facility:" << e->facility() << " error:" << e->code() << "" << std::endl;
}

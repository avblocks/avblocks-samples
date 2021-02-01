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

// PATH_MAX
#include <linux/limits.h>

// strcasecmp
#include <strings.h>
#include <fstream>
#include <AVBlocks.h>

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
        cout << primo::ustring(e->message()) << ", ";
    }
    
    cout << "facility:" << e->facility()
	 << ", error:" << e->code()
	 << ", hint:" << primo::ustring(e->hint())
	 << endl;
}

inline void deleteFile(const char* file)
{
    remove(file);
}

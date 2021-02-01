/*
 *  Copyright (c) 2015 Primo Software. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree.
 */

#include <string>
#include "options.h"
#include "program_options.h"
#include "util.h"

using namespace std;
using namespace primo::program_options;

void printUsage(OptionsConfig<char>& optionsConfig)
{
    cout << endl << "Usage: read_metadata_any_file --input <file>" << endl;
    doHelp(cout, optionsConfig);
}

void setDefaultOptions(Options& opt)
{
    opt.inputFile = getExeDir() + "/../assets/aud/Hydrate-Kenny_Beltrey.ogg";
    cout << "Using default option: read_metadata_any_file --input " << opt.inputFile << endl << endl;
}

ErrorCodes prepareOptions(Options& opt, int argc, char* argv[])
{
    if(argc == 1)
    {
        setDefaultOptions(opt);
        return Parsed;
    }
    OptionsConfig<char> optionsConfig;
    optionsConfig.addOptions()
    (("help,?"), opt.help, (""))
    (("input,i"), opt.inputFile, string(), ("input file"));
    
    try
    {
        scanArgv(optionsConfig, argc, argv);
    }
    catch (ParseFailure<char> &ex)
    {
        cout << ex.message() << endl;
        printUsage(optionsConfig);
        return Error;
    }
    
    if(opt.help)
    {
        printUsage(optionsConfig);
        return Command;
    }
    
    return Parsed;
}

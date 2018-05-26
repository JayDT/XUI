/*
* Copyright (C) 2017-2018 Tauri JayD <https://www.>
* Copyright (c) 2016 Austin Brunkhorst, All Rights Reserved.
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the
* Free Software Foundation; either version 2 of the License, or (at your
* option) any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "standard/Platform/Common.h"
#include "lpt/call_stack.hpp"
#include "Log/Log.h"

#include "Reflection/Runtime/ReflectionModule.h"
#include "Reflection/Runtime/TypeConfig.h"
#include "Reflection/Runtime/Type.h"

#include <signal.h>

#include <chrono>
#include <fstream>
#include <future>
#include <algorithm>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "ResourceBuilder.h"

boost::program_options::options_description program("System::Resource Compiler");
boost::program_options::variables_map cmdLine;

bool kIsInTestMode = false;

void AddCommandLineOptions()
{
    program.add_options()
        (
        SWITCH_OPTION(Help),
        "Displays help information."
        )
        (
        SWITCH_OPTION(TargetName),
        boost::program_options::value<std::string>()->required(),
        "Input target project name."
        )
        (
        SWITCH_OPTION(SourceRoot),
        boost::program_options::value<std::string>()->required(),
        "Root source directory that is shared by all header files."
        )
        (
        SWITCH_OPTION(InputSource),
        boost::program_options::value<std::string>()->required(),
        "Source file (header) to compile reflection data from."
        )
        (
        SWITCH_OPTION(OutputModuleSource),
        boost::program_options::value<std::string>()->required(),
        "Output generated C++ module source file."
        )
        (
        SWITCH_OPTION(OutputModuleFileDirectory),
        boost::program_options::value<std::string>()->required(),
        "Output directory for generated C++ module file, header/source files."
        )
        (
        SWITCH_OPTION(ForceRebuild),
        "Whether or not to ignore cache and write the header / source files."
        )
        (
        SWITCH_OPTION(DisplayDiagnostics),
        "Whether or not to display diagnostics from clang."
        );
}

void ParseCommandLineArgs(int argc, char *argv[])
{
    if (argc == 1) // In test mode
    {
        char** iargv = argv;
        std::cout << "Arguments: ";
        while (*iargv)
        {
            std::cout << *iargv << std::endl;
            ++iargv;
        }

        argc = 12;
        argv = new char*[13]
        {
            "C:/Project/XUI/XUI/win/bin/Debug/rs.exe",
            "--target-name",
            "XUI",
            "--source-root",
            "C:/Project/XUI/XUI/src/XUI",
            "--in-source",
            "C:/Project/XUI/XUI/win/src/XUI/.Resource/Resource_XUI.dsc",
            "--out-source",
            "C:/Project/XUI/XUI/win/src/XUI/.Resource/Resource_Modul_XUI.cpp",
            "--out-dir",
            "C:/Project/XUI/XUI/win/src/XUI/.Resource",
            "--display-diagnostics",
            nullptr
        };

        kIsInTestMode = true;
    }

    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, program), cmdLine);
}

int main(int argc, char *argv[])
{
    // path to the executable
    auto exeDir = boost::filesystem::path(argv[0]).parent_path();
    
    // set the working directory to the executable directory
    if (!exeDir.empty())
        boost::filesystem::current_path(exeDir);
    
    setlocale(LC_ALL, "");
    // inherit locale from environment
    setlocale(LC_NUMERIC, "French_Canada.1252");
    //setlocale(LC_NUMERIC, "");
    
    /* initialize random seed: */
    srand(time(NULL));
    
    AddCommandLineOptions();
    ParseCommandLineArgs(argc, argv);
    
    if (cmdLine.count("help"))
    {
        std::cout << program << std::endl;
        return EXIT_SUCCESS;
    }
    
    boost::program_options::notify(cmdLine);
    
    System::Resource::ResourceBuilder::Parse(cmdLine);

    return EXIT_SUCCESS;
}

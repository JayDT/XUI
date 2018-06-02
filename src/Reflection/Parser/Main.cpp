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

#include "Precompiled.h"

#include "ReflectionOptions.h"
#include "ReflectionParser.h"

#include "Switches.h"

#include "standard/collection.h"
#include "standard/concurrentQueue.h"

#include <chrono>
#include <fstream>
#include <future>
#include <algorithm>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include <common_data.h>
#include <buildin_data.h>

std::string standardMSVCIncludePath = "";
std::vector<std::string> otherSystemIncludes;

unsigned long internalID = 0;

bool kIsInTestMode = false;
bool kGenerateBaseModul = false;

void parse(const boost::program_options::variables_map &cmdLine);

#define _TEST_MODUL "XUI"

int main(int argc, char *argv[])
{
    auto start = std::chrono::system_clock::now( );

    if (argc == 1) // In test mode
    {
        char** iargv = argv;
        std::cout << "Arguments: ";
        while (*iargv)
        {
            std::cout << *iargv << std::endl;
            ++iargv;
        }

        argc = 18;
        argv = new char*[19]
        {
            _SOURCE_DIRECTORY "/win64/bin/Debug/MetaParser.exe",
            "--target-name",
			_TEST_MODUL,
            "--source-root",
            _SOURCE_DIRECTORY "/moduls/XUI/src/" _TEST_MODUL,
            "--in-source",
            _SOURCE_DIRECTORY "/win64/moduls/XUI/src/" _TEST_MODUL "/.Assembly/Assembly_" _TEST_MODUL ".dsc",
            "--module-header",
            _SOURCE_DIRECTORY "/moduls/XUI/src/" _TEST_MODUL "AssemblyModul.h",
            "--out-source",
            _SOURCE_DIRECTORY "/win64/moduls/XUI/src/" _TEST_MODUL "/.Assembly/Assembly_Modul_" _TEST_MODUL ".cpp",
            "--out-dir",
            _SOURCE_DIRECTORY "/win64/moduls/XUI/src/" _TEST_MODUL "/.Assembly",
            "--includes",
            _SOURCE_DIRECTORY "/win64/moduls/XUI/src/" _TEST_MODUL "/.Assembly/Module." _TEST_MODUL ".Includes.txt",
            "--pch",
            _SOURCE_DIRECTORY "/moduls/XUI/src/" _TEST_MODUL "/StdAfx.h",
            "--display-diagnostics",
            nullptr
        };

        kIsInTestMode = true;
    }
    else
        kGenerateBaseModul = false;

    // parse command line
    try 
    {
        // path to the executable
        auto exeDir = boost::filesystem::path( argv[ 0 ] ).parent_path( );

        // set the working directory to the executable directory
        if (!exeDir.empty( ))
            boost::filesystem::current_path( exeDir );

        boost::program_options::options_description program( "System::Reflection Meta Parser" );

        program.add_options( )
        ( 
            SWITCH_OPTION( Help ), 
            "Displays help information." 
        )
        ( 
            SWITCH_OPTION( TargetName ), 
            boost::program_options::value<std::string>( )->required( ),
            "Input target project name." 
        )
        ( 
            SWITCH_OPTION( SourceRoot ), 
            boost::program_options::value<std::string>( )->required( ),
            "Root source directory that is shared by all header files." 
        )
        ( 
            SWITCH_OPTION( InputSource ), 
            boost::program_options::value<std::string>( )->required( ), 
            "Source file (header) to compile reflection data from." 
        )
            ( 
            SWITCH_OPTION( ModuleHeaderFile ), 
            boost::program_options::value<std::string>( )->required( ), 
            "Header file that declares this reflection module." 
        )
        ( 
            SWITCH_OPTION( OutputModuleSource ), 
            boost::program_options::value<std::string>( )->required( ), 
            "Output generated C++ module source file." 
        )
        ( 
            SWITCH_OPTION( OutputModuleFileDirectory ), 
            boost::program_options::value<std::string>( )->required( ), 
            "Output directory for generated C++ module file, header/source files." 
        )
        ( 
            SWITCH_OPTION( TemplateDirectory ), 
            boost::program_options::value<std::string>( )->default_value( "Resources/Templates/" ), 
            "Directory that contains the mustache templates." 
        )
        ( 
            SWITCH_OPTION( PrecompiledHeader ), 
            boost::program_options::value<std::string>( ), 
            "Optional name of the precompiled header file for the project." 
        )
        (
            SWITCH_OPTION( ForceRebuild ),
            "Whether or not to ignore cache and write the header / source files."
        )
        (
            SWITCH_OPTION( DisplayDiagnostics ),
            "Whether or not to display diagnostics from clang."
        )
        ( 
            SWITCH_OPTION( CompilerIncludes ), 
            boost::program_options::value<std::string>( ),
            "Optional file that includes the include directories for this target." 
        )
        (
            SWITCH_OPTION( CompilerDefines ),
            boost::program_options::value<std::vector<std::string>>( )->multitoken( ),
            "Optional list of definitions to include for the compiler."
        );

        boost::program_options::variables_map cmdLine;

        boost::program_options::store( boost::program_options::parse_command_line( argc, argv, program ), cmdLine );

        if (cmdLine.count( "help" )) 
        {
            std::cout << program << std::endl;

            return EXIT_SUCCESS;
        }

        boost::program_options::notify( cmdLine );

        parse( cmdLine );
    }
    catch (std::exception &e)
    {
        std::cout << "input error" << std::endl;
        utils::FatalError( e.what( ) );
    }
    catch (...) 
    {
        utils::FatalError( "Unhandled exception occurred!" );
    }

    auto duration = std::chrono::system_clock::now( ) - start;

    std::cout << "Completed in " 
              << std::chrono::duration_cast<std::chrono::milliseconds>( duration ).count( ) 
              << "ms" << std::endl;

    return EXIT_SUCCESS;
}

void parse(const boost::program_options::variables_map &cmdLine)
{
    System::String system_includes(_SYSTEM_INCLUDE_DIRECTORY);
    auto systemIncludeArray = system_includes.Split(';');
    for (auto s : systemIncludeArray)
    {
        if (s.find("Redist/MSVC") != std::string::npos)
            standardMSVCIncludePath = s;
        else
            otherSystemIncludes.push_back(s);
    }

    if (standardMSVCIncludePath.empty())
    {
        std::cout << "Not found standard library includes" << std::endl;
        return;
    }

    std::cout << std::endl << "Using standard library includes: " << standardMSVCIncludePath << std::endl;

    ReflectionOptions options;

    options.forceRebuild = 
        cmdLine.count( kSwitchForceRebuild ) > 0 || kIsInTestMode;

    options.displayDiagnostics = 
        cmdLine.count( kSwitchDisplayDiagnostics ) > 0 /*|| kIsInTestMode*/;

    options.targetName = 
        cmdLine.at( kSwitchTargetName ).as<std::string>( );

    options.sourceRoot = 
        cmdLine.at( kSwitchSourceRoot ).as<std::string>( );

    options.inputSourceFile = 
        cmdLine.at( kSwitchInputSource ).as<std::string>( );

    options.moduleHeaderFile = 
        cmdLine.at( kSwitchModuleHeaderFile ).as<std::string>( );

    options.outputModuleSource = 
        cmdLine.at( kSwitchOutputModuleSource ).as<std::string>( );

    options.outputModuleFileDirectory = 
        cmdLine.at( kSwitchOutputModuleFileDirectory ).as<std::string>( );

    // default arguments
    options.arguments =
    { {
        "-x",
        "c++",
        "-g0",
        "-O0",
        "-fms-compatibility",
        "-fms-compatibility-version=19.20",
        "-fms-extensions",
        "-fdeclspec",
        "-std=c++1z",
        "-D__REFLECTION_PARSER__",
        "-DWIN32" ,
        "-fshort-wchar",
        "-fno-inline-functions",
        "-fdelayed-template-parsing",
        "-fno-experimental-new-pass-manager",
        "-fno-crash-diagnostics",
        "-fmodules-disable-diagnostic-validation",
        "-fno-standalone-debug",
        "-fno-stack-protector",
        "-fsyntax-only"
     } };

    if (cmdLine.count( kSwitchPrecompiledHeader ))
    {
        options.precompiledHeader = 
            cmdLine.at( kSwitchPrecompiledHeader ).as<std::string>( );
    
        options.arguments.emplace_back(std::string("-include") + options.precompiledHeader);
        std::cout << "Using pch force header: " << options.precompiledHeader << std::endl;

        //options.arguments.emplace_back(std::string("-include-pch"));
        //options.arguments.emplace_back(options.precompiledHeader + std::string(".pch"));
    }
    
    if (cmdLine.count( kSwitchCompilerIncludes ))
    {
        auto includes = 
            cmdLine.at( kSwitchCompilerIncludes ).as<std::string>( );

        std::ifstream includesFile( includes );

        std::string include;

        // TODO: implement auto detect path
        options.arguments.emplace_back("-I" + standardMSVCIncludePath); // STRINGIFY(__VC_VC_IncludePath__)); //

        while (std::getline( includesFile, include ))
            options.arguments.emplace_back( "-I"+ include );

    }

    if (kGenerateBaseModul)
    {
        options.arguments.emplace_back("-I" + options.sourceRoot);
        options.arguments.emplace_back("-I" + options.sourceRoot + "/Common");
        options.arguments.emplace_back("-I" + options.sourceRoot + "/Common/Lib");
        options.arguments.emplace_back("-I" + options.sourceRoot + "/..");
        options.arguments.emplace_back("-I" + options.sourceRoot + "/../..");
        options.arguments.emplace_back("-I" + options.sourceRoot + "/../../../CFramework");
        // TODO: implement auto detect path
        options.arguments.emplace_back("-I" + standardMSVCIncludePath);
        options.arguments.emplace_back("-IC:/mingw/include");
        options.arguments.emplace_back("-IC:/local/boost_1_64_0");
    }

    if (cmdLine.count( kSwitchCompilerDefines ))
    {
        auto defines = 
            cmdLine.at( kSwitchCompilerDefines ).as<std::vector<std::string>>( );

        for (auto &define : defines)
            options.arguments.emplace_back( "-D"+ define );
    }

    options.templateDirectory = 
        cmdLine.at( kSwitchTemplateDirectory ).as<std::string>( );
    
    std::cout << "Parsing reflection data for target \"" 
              << options.targetName << "\"" 
              << std::endl;

    std::ifstream includesFile(cmdLine.at(kSwitchInputSource).as<std::string>());

    ReflectionModulParser modulParser(options);

    if (kGenerateBaseModul)
    {
        modulParser.include_headers.push_back("Object.h");
        modulParser.include_headers.push_back("MetaProperty.h");
        modulParser.use_headers.insert(options.sourceRoot + "/" + "Object.h");
        modulParser.use_headers.insert(options.sourceRoot + "/" + "MetaProperty.h");
    }
    else
    {
        std::string sourcefile;
        while (std::getline(includesFile, sourcefile))
        {
            modulParser.use_headers.insert(options.sourceRoot + "/" + sourcefile);
            modulParser.include_headers.push_back(sourcefile);
        }
    }

    boost::filesystem::path sourcePath(options.outputModuleSource);
    auto outputFileHeader = change_extension(boost::filesystem::path(sourcePath), ".h");
    options.moduleHeaderFile = outputFileHeader.string();

    ReflectionModulParser::Instance = &modulParser;

    std::cout << "Compile:" << std::endl;

    const auto hardware_threads = std::min(int(std::thread::hardware_concurrency()), 1);

    int remainingWorkers = hardware_threads;
    std::hash<std::string> hasher;
    System::Concurrent::Queue<std::string const*> resultSource;
    //if (kIsInTestMode)
    {
        static std::string test;

        for (std::string const& sourcefile : modulParser.include_headers)
        {
            if (test.empty())
                test = sourcefile;
            else
            {
                ReflectionOptions parser_options(options);
                parser_options.inputSourceFile = parser_options.sourceRoot + "/" + sourcefile;
                ReflectionParser* parser = modulParser.CreateReflectionFileParser(parser_options);
                parser->Filename = sourcefile;
                parser->modulDescriptor.name = parser_options.inputSourceFile;
                ReflectionModulParser::Instance->m_modulMutex.lock();
                boost::filesystem::path path(parser_options.inputSourceFile);
                ReflectionModulParser::Instance->m_moduleFileStorage[hasher(path.filename().string())/*parser->SourceDesc.GetHash()*/] = &parser->modulDescriptor;
                ReflectionModulParser::Instance->m_modulMutex.unlock();
                if (kGenerateBaseModul && parser->Filename == "Object.h")
                    parser->isBaseModul = true;

                options.arguments.emplace_back(std::string("-include") + sourcefile);
                //std::cout << parser->Filename << std::endl;
            }
        }

        resultSource.push(&test);
    }
    //else
    //{
    //    for (std::string const& sourcefile : modulParser.include_headers)
    //        resultSource.push(&sourcefile);
    //}
    System::Concurrent::Queue<ReflectionParser*> resultParser;

    // todo: https://stackoverflow.com/questions/40805197/parallel-for-each-more-than-two-times-slower-than-stdfor-each
    //std::function<void(void)> task([&modulParser, &resultSource, &resultParser, &remainingWorkers, &options]()
    {
        uint8 ithr = 0;
        std::string const* sourcefile;
        while (resultSource.pop(sourcefile))
        {
            ReflectionOptions parser_options(options);
            parser_options.inputSourceFile = parser_options.sourceRoot + "/" + *sourcefile;
            ReflectionParser* parser = modulParser.CreateReflectionFileParser(parser_options);
            parser->Filename = *sourcefile;
            parser->modulDescriptor.name = parser_options.inputSourceFile;
            ReflectionModulParser::Instance->m_modulMutex.lock();
            boost::filesystem::path path(parser_options.inputSourceFile);
            ReflectionModulParser::Instance->m_moduleFileStorage[hasher(path.filename().string())/*parser->SourceDesc.GetHash()*/] = &parser->modulDescriptor;
            ReflectionModulParser::Instance->m_modulMutex.unlock();
            if (kGenerateBaseModul && parser->Filename == "Object.h")
                parser->isBaseModul = true;

            for (std::string const& sourcefile : modulParser.include_headers)
                parser->modulDescriptor.includes.insert("\"" + sourcefile + "\"");

            //modulParser.SetActiveModul(parser);
            //parser->Parse();
            //std::cout << parser->Filename << std::endl;
            resultParser.push(parser);
            //break;
        }

        --remainingWorkers;
    }//);

    //std::vector<std::shared_ptr<std::thread>> threads;
    //for (int i = 0; i != hardware_threads; ++i)
    //    threads.push_back(std::make_shared<std::thread>(task));
    //
    //while (remainingWorkers > 0)
    {
        ReflectionParser* parser;
        while (resultParser.pop(parser))
        {
            modulParser.PrepareTranslatorUnit(parser);
            modulParser.SetActiveModul(parser);
            parser->Parse();
            //std::cout << parser->Filename << std::endl;
        }
    }
    
    //for (auto& thr : threads)
    //    if (thr->joinable())
    //        thr->join();
    //threads.clear();

    std::cout << "Generate templates" << std::endl;
    modulParser.PrepareObjectsForAssembly();

    std::cout << "Generate assembly" << std::endl;
    modulParser.GenerateFiles();
}

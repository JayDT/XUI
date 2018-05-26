/*
* Copyright (C) 2017-2018 Tauri JayD <https://www.>
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

#include "ResourceBuilder.h"
#include "ResourceCompiler.h"
#include <standard/client/DataSource_Standard.h>

#include <chrono>
#include <fstream>
#include <future>
#include <algorithm>

#include <boost/program_options.hpp>

#include <Wrappers/Mustache.h>

const char* kVersion = "0.0.1a";

namespace MustacheExt
{
    void FatalError(const std::string &error);
    void LoadText(const std::string &filename, std::string &output);
    Mustache::Mustache<std::string> LoadTemplate(const std::string & name);
    void WriteText(const std::string &filename, const std::string &text, bool writeIfChecksumDif);
    boost::filesystem::path MakeRelativePath(const boost::filesystem::path &from, const boost::filesystem::path &to);
    boost::filesystem::path change_extension(const boost::filesystem::path & p, const boost::filesystem::path & new_extension);
}

namespace
{
    const boost::regex kSpecialCharsRegex("[^a-zA-Z0-9]+");
}

System::IO::StandardDataSource fileSystem;
ResourceOptions options;
Mustache::Mustache<std::string>* fileTemplate;

System::Concurrent::Queue<System::Resource::ResourceBuilder*> System::Resource::ResourceBuilder::_activeTasks;
System::Concurrent::Queue<System::Resource::ResourceBuilder*> System::Resource::ResourceBuilder::_resultTasks;

System::Resource::ResourceBuilder::ResourceBuilder(ResourceOptions options, std::string const& sourcefile)
    : _options(options)
{
    _options.inputSourceFile = sourcefile;
}

void System::Resource::ResourceBuilder::Parse(const boost::program_options::variables_map & cmdLine)
{
    extern bool kIsInTestMode;

    options.forceRebuild =
        cmdLine.count(kSwitchForceRebuild) > 0 || kIsInTestMode;

    options.displayDiagnostics =
        cmdLine.count(kSwitchDisplayDiagnostics) > 0 /*|| kIsInTestMode*/;

    options.targetName =
        cmdLine.at(kSwitchTargetName).as<std::string>();

    options.sourceRoot =
        cmdLine.at(kSwitchSourceRoot).as<std::string>();

    options.inputSourceFile =
        cmdLine.at(kSwitchInputSource).as<std::string>();

    options.outputModuleSource =
        cmdLine.at(kSwitchOutputModuleSource).as<std::string>();

    options.outputModuleFileDirectory =
        cmdLine.at(kSwitchOutputModuleFileDirectory).as<std::string>();


    std::ifstream includesFile(cmdLine.at(kSwitchInputSource).as<std::string>());

    std::list<std::string> resource_files;
    std::string sourcefile;
    while (std::getline(includesFile, sourcefile))
        resource_files.push_back(sourcefile);

    for (std::string const& sourcefile : resource_files)
    {
        ResourceBuilder* builder = new ResourceBuilder(options, sourcefile);
        _activeTasks.push(builder);
    }

    //NOTE: can be async after
    ResourceBuilder* builder;
    while (_activeTasks.pop(builder))
    {
        builder->Compile();
        _resultTasks.push(builder);
    }

    Generate();
}

void System::Resource::ResourceBuilder::Compile()
{
    std::string fileName = System::String::format("%s/%s", _options.sourceRoot.c_str(), _options.inputSourceFile.c_str());

    System::IO::IFileReader* resourceFile = fileSystem.OpenFile(fileName.c_str());

    _name = boost::regex_replace(
        _options.inputSourceFile,
        kSpecialCharsRegex,
        "_"
    );

    uint64 inputsize = resourceFile->Size();

    while (inputsize > 0)
    {
        size_t chuckSize = std::min(size_t(inputsize), ResourceCompiler::MaxChunkSize);
        inputsize -= chuckSize;
        std::vector<byte>* buffer = new std::vector<byte>(chuckSize);
        resourceFile->Read(buffer->data(), chuckSize);
        _compiler.m_input.push_back(buffer);
    }

    delete resourceFile;

    _compiler.Compile(_resourceType);
}

void ConvertFileToResourceFile(boost::filesystem::path const& sourceRootDirectory, boost::filesystem::path const& outputFileDirectory, std::string const& _filename, std::string& resourcefile)
{
    System::String normalizedFn = _filename;
    boost::filesystem::path filePath(normalizedFn.Replace('/', '\\').ToLowerFirst().c_str());

    // path relative to the source root
    auto relativeDir = MustacheExt::MakeRelativePath(sourceRootDirectory, filePath)
        .replace_extension("").string();

    if (relativeDir.find_first_of("..") != std::string::npos)
        return;

    auto outputFile = outputFileDirectory / relativeDir;

    auto pt = outputFile.parent_path();
    auto fn = outputFile.filename();
    auto filename = std::wstring(pt.c_str()) + L"/" + std::wstring(L"Resource_") + fn.c_str();
    resourcefile = MustacheExt::change_extension(boost::filesystem::path(filename), ".cpp").string(); //change_extension( outputFile, "Generated.cpp" );

    // module file name
    //resourcefile = boost::regex_replace(
    //    relativeDir,
    //    kSpecialCharsRegex,
    //    "_"
    //);
}

void System::Resource::ResourceBuilder::GenerateFile()
{
    boost::filesystem::path sourceRootDirectory(System::String(_options.sourceRoot).ToLowerFirst().c_str());
    boost::filesystem::path outputFileDirectory(_options.outputModuleFileDirectory);

    Mustache::Data<std::string> sourceData{ Mustache::Data<std::string>::Type::Object };
    Mustache::Data<std::string> resourceList{ Mustache::Data<std::string>::Type::List };

    sourceData["version"] = kVersion;
    sourceData["moduleFileName"] = _name;

    // ToDo: store multiple resource on one module file
    {
        Mustache::Data<std::string> resourceData{ Mustache::Data<std::string>::Type::Object };
        Mustache::Data<std::string> binaryData{ Mustache::Data<std::string>::Type::List };

        resourceData["name"] = _options.inputSourceFile;
        for (size_t id = 0; id < _compiler.m_output.size(); ++id)
        {
            Mustache::Data<std::string> rowData{ Mustache::Data<std::string>::Type::Object };
            rowData["id"] = std::to_string(id);
            rowData["size"] = std::to_string(_compiler.m_output[id]->size());
            rowData["total_size"] = std::to_string(_compiler.m_input[id]->size());

            std::string binary;
            for (size_t bi = 0; bi < _compiler.m_output[id]->size(); ++bi)
            {
                binary += System::String::format("0x%X, ", _compiler.m_output[id]->at(bi));
            }

            delete _compiler.m_output[id];
            delete _compiler.m_input[id];

            binary.resize(binary.size() - 2);

            rowData["binary"] = binary;

            binaryData << rowData;
        }

        _compiler.m_output.clear();
        _compiler.m_input.clear();

        resourceData["data"] = binaryData;

        resourceList << resourceData;
    }

    sourceData["resource"] = resourceList;

    std::string sourceFile;
    ConvertFileToResourceFile(sourceRootDirectory, outputFileDirectory, _options.sourceRoot + "/" + _options.inputSourceFile, sourceFile);

    boost::filesystem::create_directory(boost::filesystem::path(sourceFile).parent_path());

    MustacheExt::WriteText(
        sourceFile,
        fileTemplate->render(sourceData),
        true
    );
}

void System::Resource::ResourceBuilder::Generate()
{
    Mustache::Data<std::string> sourceData{ Mustache::Data<std::string>::Type::Object };
    
    auto modulTemplate = MustacheExt::LoadTemplate("rs-module-source.mustache");
    auto _fileTemplate = MustacheExt::LoadTemplate("rs-module-file-source.mustache");
    fileTemplate = &_fileTemplate;

    sourceData["version"] = kVersion;
    sourceData["targetName"] = options.targetName;
    
    Mustache::Data<std::string> resourceFilesData{ Mustache::Data<std::string>::Type::List };

    ResourceBuilder* builder;
    while (_resultTasks.pop(builder))
    {
        Mustache::Data<std::string> moduleFile{ Mustache::Data<std::string>::Type::Object };

        moduleFile["name"] = builder->_name;

        builder->GenerateFile();

        delete builder;

        resourceFilesData << moduleFile;
    }

    fileTemplate = nullptr;

    sourceData["moduleFile"] = resourceFilesData;

    boost::filesystem::path sourcePath(options.outputModuleSource);

    boost::filesystem::create_directory(sourcePath.parent_path());

    MustacheExt::WriteText(
        sourcePath.string(),
        modulTemplate.render(sourceData),
        true
    );
}

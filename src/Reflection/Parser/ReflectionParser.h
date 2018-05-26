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

#pragma once

#include "ReflectionOptions.h"
#include "MetaDataManager.h"

#include "Cursor.h"
#include "Namespace.h"

#include "Templates.h"

#include "Module/ModuleFile.h"

#include "Wrappers/Mustache.h"
#include <boost/filesystem.hpp>

#include <set>
#include <mutex>

class LanguageType;
class Class;
class External;
class Global;
class Function;
class Enum;
class ReflectionModulParser;

class ReflectionParser
{
    friend class Class;
    friend class ReflectionModulParser;
public:
    ReflectionParser(const ReflectionOptions &options);
    ~ReflectionParser(void);

    void Parse(void);
    void GenerateFiles(void);

    Mustache::Mustache<std::string> LoadTemplate(const std::string &name) const;

    Mustache::Data<std::string>::PartialType LoadTemplatePartial(
        const std::string &name
    ) const;

    void HandleNull(const Cursor &cursor, Namespace &currentNamespace, LanguageType* object);
    void HandleNameSpace(const Cursor& cursor, Namespace &currentNamespace, LanguageType* object);
    void HandleClasses(const Cursor &cursor, Namespace &currentNamespace, LanguageType* object);
    void HandleTemplateClasses(const Cursor &cursor, Namespace &currentNamespace, LanguageType* object);
    void HandleGlobals(const Cursor &cursor, Namespace &currentNamespace, LanguageType* object);
    void HandleGlobalFunctions(const Cursor &cursor, Namespace &currentNamespace, LanguageType* object);
    void HandleEnums(const Cursor &cursor, Namespace &currentNamespace, LanguageType* object);

    void HandleBaseSpecifier(const Cursor &cursor, Namespace &currentNamespace, LanguageType* object);
    void HandleConstructor(const Cursor &cursor, Namespace &currentNamespace, LanguageType* object);
    void HandleFieldDecl(const Cursor &cursor, Namespace &currentNamespace, LanguageType* object);
    void HandleCXXMethod(const Cursor &cursor, Namespace &currentNamespace, LanguageType* object);
    void HandleAnnotateAttr(const Cursor &cursor, Namespace &currentNamespace, LanguageType* object);

    void HandleEnumConstantDecl(const Cursor &cursor, Namespace &currentNamespace, LanguageType* object);
    void HandleParmDecl(const Cursor &cursor, Namespace &currentNamespace, LanguageType* object);
    void HandleVisbility(const Cursor &cursor, Namespace &currentNamespace, LanguageType* object);
	void HandleOverride(const Cursor &cursor, Namespace &currentNamespace, LanguageType* object);

    SourceFileDescriptor SourceDesc;

    ModuleFile modulDescriptor;
    std::string Filename;
    bool isBaseModul = false;
private:
    ReflectionOptions m_options;
    void TraverseAST(const Cursor &cursor, Namespace &currentNamespace, LanguageType* object);

    CXIndex m_index;
    CXTranslationUnit m_translationUnit;

    std::string sourceFile;

    Mustache::Mustache<std::string> m_moduleFileHeaderTemplate;
    Mustache::Mustache<std::string> m_moduleFileSourceTemplate;

    std::unordered_map<std::string, std::string> m_moduleFilesDesc;
    //Mustache::Data<std::string> moduleFilesData;

    mutable std::unordered_map<
        std::string, 
        std::string
    > m_templatePartialCache;

    std::vector<std::shared_ptr<External>> m_externals;

    void addGlobalTemplateData(Mustache::Data<std::string> &data);

    void generateModuleFile(
        const boost::filesystem::path &fileHeader, 
        const boost::filesystem::path &fileSource, 
        const std::string &sourceHeader,
        const ModuleFile &file
    );
};

class ReflectionModulParser : public MetaDataManager
{
public:
    static ReflectionModulParser* Instance;

    typedef std::unordered_map<std::string, std::shared_ptr<Class>> ClassStorage;

    typedef ClassStorage TemplateClassStorage;
    //typedef std::unordered_map<std::string, std::shared_ptr<Function>> TemplateFunctionStorage;
    //typedef std::unordered_map<std::string, std::shared_ptr<Method>> TemplateMethodStorage;

    typedef std::unordered_map<size_t /*Cursor Hash GetUID*/, std::shared_ptr<Class>> ClassHashStorage;

    ReflectionModulParser(const ReflectionOptions &options);

    virtual ~ReflectionModulParser();

    //void AddModul(std::string file, ReflectionModulDesc& modul);

    void generateModuleFile(
        const boost::filesystem::path &fileHeader,
        const boost::filesystem::path &fileSource,
        const std::string &sourceHeader,
        const ModuleFile &file
    );

    void PrepareObjectsForAssembly();
    void GenerateModulFiles(void);
    void GenerateFiles(void);
    void CreatePCH(void);

    Mustache::Mustache<std::string> LoadTemplate(const std::string &name) const;

    ReflectionParser* CreateReflectionFileParser(const ReflectionOptions &options);
    void PrepareTranslatorUnit(ReflectionParser*);

    Class* GetClassDesc(Cursor const& cursor);
    ModuleFile* GetModulFile(SourceFileDescriptor const& fileDesc);

    Class* GetClassDescByName(const char* name);
    Class* GetClassDesc(size_t cursorHash);
    //ModuleFile* GetModulFile(size_t fileDescHash);

    std::list<std::shared_ptr<ReflectionParser>>& GetModulFiles() { return m_moduleFiles; }
    ReflectionParser* GetActiveModul() const { return m_activeModul; }
    void SetActiveModul(ReflectionParser* modul) { m_activeModul = modul; }

    std::list<std::string> include_headers;
    //std::unordered_map<std::string, std::shared_ptr<Class>> m_classes;
    std::unordered_map<size_t /*SourceFileDescriptor*/, ModuleFile*> m_moduleFileStorage;
    std::set<std::string> use_headers;

    static ClassStorage sClassObjectStorage;
    static TemplateClassStorage sTemplateStorage;
    //static TemplateMethodStorage sTemplateMethodStorage;
    //static TemplateFunctionStorage sTemplateFunctionStorage;

    std::mutex s_lock;
    ClassHashStorage m_classes;
    std::mutex m_modulMutex;
    std::list<std::shared_ptr<ReflectionParser>> m_moduleFiles;
private:
    ReflectionParser* m_activeModul;
    ReflectionOptions const& m_options;

    CXIndex m_index;
};

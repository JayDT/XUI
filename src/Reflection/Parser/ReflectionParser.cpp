#include "Precompiled.h"

#include "clang-c/Index.h"
#include "ReflectionParser.h"
#include "Version.h"
#include "MetaDataConfig.h"
#include "MetaUtils.h"
#include "ReservedTypes.h"

#include "LanguageTypes/Class.h"
#include "LanguageTypes/External.h"
#include "LanguageTypes/Global.h"
#include "LanguageTypes/Function.h"
#include "LanguageTypes/Enum.h"

#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include "System/rtString.h"

#include <fstream>

extern bool kGenerateBaseModul;
extern bool isNativeType(const std::string &qualifiedName);
extern std::string standardMSVCIncludePath;

std::set<std::string> _bannedIncludes = { "xxatomic" };

struct ReflectionParseHandler
{
    CXCursorKind CursorKind;
    void (ReflectionParser::*Handler)(const Cursor &, Namespace &, LanguageType*);
};

std::vector<std::string> nativeTypes
{
    kTypeObject,
    kTypeMetaProperty
};

bool isNativeType(const std::string &qualifiedName)
{
    return std::find(
        nativeTypes.begin(),
        nativeTypes.end(),
        qualifiedName
    ) != nativeTypes.end();
}

ReflectionParseHandler AttributeParser
{
    CXCursorKind::CXCursor_UnexposedDecl, &ReflectionParser::HandleAnnotateAttr
};

ReflectionParseHandler pReflectionParseHandler[] =
{
    // Global
    { CXCursorKind::CXCursor_UnexposedDecl,                         &ReflectionParser::HandleNull },
    { CXCursorKind::CXCursor_TypedefDecl,                           &ReflectionParser::HandleNull },
    { CXCursorKind::CXCursor_NamespaceAlias,                        &ReflectionParser::HandleNull },
    { CXCursorKind::CXCursor_ClassDecl,                             &ReflectionParser::HandleClasses },
    { CXCursorKind::CXCursor_StructDecl,                            &ReflectionParser::HandleClasses },
    { CXCursorKind::CXCursor_ClassTemplate,                         &ReflectionParser::HandleTemplateClasses },
    { CXCursorKind::CXCursor_ClassTemplatePartialSpecialization,    &ReflectionParser::HandleTemplateClasses },
    { CXCursorKind::CXCursor_Namespace,                             &ReflectionParser::HandleNameSpace },
    { CXCursorKind::CXCursor_VarDecl,                               &ReflectionParser::HandleGlobals },
    { CXCursorKind::CXCursor_FunctionDecl,                          &ReflectionParser::HandleGlobalFunctions },
    { CXCursorKind::CXCursor_FunctionTemplate,                      &ReflectionParser::HandleGlobalFunctions },
    { CXCursorKind::CXCursor_EnumDecl,                              &ReflectionParser::HandleEnums },
    { CXCursorKind::CXCursor_AnnotateAttr,                          &ReflectionParser::HandleAnnotateAttr },
    { CXCursorKind::CXCursor_ConversionFunction,                    &ReflectionParser::HandleNull }, // example: operator int() const { return 7; }
    { CXCursorKind::CXCursor_CXXAccessSpecifier,                    &ReflectionParser::HandleAnnotateAttr },
    { CXCursorKind::CXCursor_ParmDecl,                              &ReflectionParser::HandleParmDecl },
    { CXCursorKind::CXCursor_TemplateTypeParameter,                 &ReflectionParser::HandleParmDecl },
    { CXCursorKind::CXCursor_NonTypeTemplateParameter,              &ReflectionParser::HandleParmDecl },
    { CXCursorKind::CXCursor_TemplateTemplateParameter,             &ReflectionParser::HandleParmDecl },
    { CXCursorKind::CXCursor_DLLExport,                             &ReflectionParser::HandleVisbility },
    { CXCursorKind::CXCursor_VisibilityAttr,                        &ReflectionParser::HandleVisbility },
    { CXCursorKind::CXCursor_CXXOverrideAttr,                       &ReflectionParser::HandleOverride },
    // Class
    { CXCursorKind::CXCursor_CXXBaseSpecifier,                      &ReflectionParser::HandleBaseSpecifier },
    { CXCursorKind::CXCursor_FieldDecl,                             &ReflectionParser::HandleFieldDecl },
    { CXCursorKind::CXCursor_CXXMethod,                             &ReflectionParser::HandleCXXMethod },
    { CXCursorKind::CXCursor_Constructor,                           &ReflectionParser::HandleConstructor },
    { CXCursorKind::CXCursor_Destructor,                            &ReflectionParser::HandleParmDecl },
    { CXCursorKind::CXCursor_FriendDecl,                            &ReflectionParser::HandleParmDecl },
    // Enum
    { CXCursorKind::CXCursor_EnumConstantDecl,                      &ReflectionParser::HandleEnumConstantDecl },
    // End
    { CXCursorKind(0),                                              nullptr },
};

ReflectionParseHandler const* GetRelfParseHandler(CXCursorKind kind)
{
    ReflectionParseHandler const* itr = pReflectionParseHandler;
    while (itr->Handler)
    {
        if (itr->CursorKind == kind)
            return itr;

        ++itr;
    }

    return &AttributeParser;
}

template<class TDest, class TSource>
void COMPILE_TYPE_TEMPLATES(ReflectionParser* parser, TDest& data, std::string const& listName, TSource const& container)
{
    Mustache::Data<std::string> typeData { Mustache::Data<std::string>::Type::List };
    for (auto &handle : container)
        if (handle->ShouldCompile())
            typeData << handle->CompileTemplate( parser );
    data[ listName ] = typeData;
}

ReflectionModulParser* ReflectionModulParser::Instance = nullptr;

ReflectionModulParser::ClassStorage             ReflectionModulParser::sClassObjectStorage;
ReflectionModulParser::TemplateClassStorage     ReflectionModulParser::sTemplateStorage;
//ReflectionModulParser::TemplateMethodStorage    ReflectionModulParser::sTemplateMethodStorage;
//ReflectionModulParser::TemplateFunctionStorage  ReflectionModulParser::sTemplateFunctionStorage;

namespace
{
    const boost::regex kSpecialCharsRegex( "[^a-zA-Z0-9]+" );
}

ReflectionParser::ReflectionParser(const ReflectionOptions &options)
    : m_options( options )
    , m_index( nullptr )
    , m_translationUnit( nullptr )
    , m_moduleFileHeaderTemplate( "" )
    , m_moduleFileSourceTemplate( "" )
{
    // replace special characters in target name with underscores
    m_options.targetName = boost::regex_replace(
        m_options.targetName, 
        kSpecialCharsRegex, 
        "_" 
    );
}

ReflectionParser::~ReflectionParser(void)
{
    //if (m_index)
    //    clang_disposeIndex(m_index);
    if (m_translationUnit)
        clang_disposeTranslationUnit( m_translationUnit );
}

void ReflectionParser::Parse(void)
{
    // Define Base Types
    if (kGenerateBaseModul && Filename == "Object.h")
    {
        modulDescriptor.classes.push_back(std::make_shared<Class>("void"));
        modulDescriptor.classes.push_back(std::make_shared<Class>("char"));
        modulDescriptor.classes.push_back(std::make_shared<Class>("unsigned char"));
        modulDescriptor.classes.push_back(std::make_shared<Class>("short"));
        modulDescriptor.classes.push_back(std::make_shared<Class>("unsigned short"));
        modulDescriptor.classes.push_back(std::make_shared<Class>("int"));
        modulDescriptor.classes.push_back(std::make_shared<Class>("unsigned int"));
        modulDescriptor.classes.push_back(std::make_shared<Class>("long long"));
        modulDescriptor.classes.push_back(std::make_shared<Class>("unsigned long long"));
        modulDescriptor.classes.push_back(std::make_shared<Class>("bool"));
        modulDescriptor.classes.push_back(std::make_shared<Class>("float"));
        modulDescriptor.classes.push_back(std::make_shared<Class>("double"));
        modulDescriptor.classes.push_back(std::make_shared<Class>("long double"));
        modulDescriptor.classes.push_back(std::make_shared<Class>("std::string"));
        modulDescriptor.classes.push_back(std::make_shared<Class>("wchar_t"));
        modulDescriptor.classes.push_back(std::make_shared<Class>("std::wstring"));
    }

    Cursor cursor = clang_getTranslationUnitCursor( m_translationUnit );

    Namespace globalScope;
    TraverseAST(cursor, globalScope, nullptr);
    clang_disposeTranslationUnit( m_translationUnit );
    m_translationUnit = nullptr;
}

void ReflectionParser::HandleNull(const Cursor & cursor, Namespace & currentNamespace, LanguageType * object)
{
}

void ReflectionParser::HandleNameSpace(const Cursor & cursor, Namespace & currentNamespace, LanguageType* object)
{
    std::string displayName = cursor.GetDisplayName();

    if (displayName.empty())
    {
        MetaDataManager globalScopeAttrs;
        globalScopeAttrs.Parse(cursor);

        if (globalScopeAttrs.GetNsAttributes("assembly"))
        {
            if (displayName.empty() && !ReflectionModulParser::Instance->GetNsAttributes("assembly"))
            {
                *static_cast<MetaDataManager*>(ReflectionModulParser::Instance) = globalScopeAttrs; //->MetaDataManager::Parse(cursor);
            }
        }

        if (auto modulAttrs = globalScopeAttrs.GetNsAttributes("modul"))
        {
            auto fileDesc = cursor.GetFileDesc();
            auto IModulFile = ReflectionModulParser::Instance->GetModulFile(fileDesc);
            auto reflIncRange = modulAttrs->equal_range(native_attribute::ReflIncludeType);
            for (auto inc = reflIncRange.first; inc != reflIncRange.second; ++inc)
            {
                if (!inc->second.empty())
                {
                    if (IModulFile)
                        IModulFile->includes.insert(inc->second);
                    else
                        modulDescriptor.includes.insert(inc->second);
                }
            }
        }
    }
    else
    {
        currentNamespace.emplace_back(displayName);
        TraverseAST(cursor, currentNamespace, object);
        currentNamespace.pop_back();
    }
}

void ReflectionParser::HandleClasses(const Cursor & cursor, Namespace & currentNamespace, LanguageType* object)
{
    if (!cursor.IsDefinition())
        return;

    // TODO: implement support for nested class in template class
    Class* parent = object != nullptr ? object->ToClass() : nullptr;
    if (parent && parent->IsTemplate())
        return;

    auto fileDesc = cursor.GetFileDesc();
    auto kind = cursor.GetKind();
    auto declType = cursor.GetType();
    auto decl = declType.GetDeclaration().GetUID();

    bool isLocalSource = ReflectionModulParser::Instance->GetModulFile(fileDesc); // true; // SourceDesc == fileDesc;

    if (kGenerateBaseModul && !isLocalSource && Filename == "Object.h")
    {
        if (cursor.GetDisplayName().find("basic_string") != std::string::npos)
            isLocalSource = true;
    }

    std::unique_lock<std::mutex> g(ReflectionModulParser::Instance->s_lock);
    std::shared_ptr<Class>& klass = ReflectionModulParser::Instance->m_classes[cursor.GetUID()];
    if (klass && SourceDesc != fileDesc)
        return;

    g.unlock();

    if (!klass)
    {
        klass = std::make_shared<Class>(cursor, currentNamespace);
        ReflectionModulParser::Instance->m_classes[cursor.GetUID()] = klass;
    }

    if (!klass->isMetaInitialized)
    {
        TraverseAST(cursor, currentNamespace, klass.get());
        klass->MetaInit(cursor, currentNamespace);
        klass->isMetaInitialized = true;
    }

    if (!isLocalSource)
        return;

    g.lock();
    auto IModulFile = ReflectionModulParser::Instance->GetModulFile(fileDesc);
    if (IModulFile)
    {
        IModulFile->classes.emplace_back(klass);
        klass->m_modulFile = IModulFile;
        for (auto const inc : klass->extra_includes)
            if (!inc.empty())
            {
                bool banned = false;
                for (auto const& binc : _bannedIncludes)
                    if (strstr(inc.c_str(), binc.c_str()))
                    {
                        banned = true;
                        break;
                    }

                if (banned)
                    continue;

                if (inc.find('.') == std::string::npos)
                    klass->m_modulFile->includes.insert("<" + inc + ">");
                else
                    klass->m_modulFile->includes.insert("\"" + inc + "\"");
            }
    }
    else
    {
        modulDescriptor.classes.emplace_back(klass);
        klass->m_modulFile = &modulDescriptor;
        for (auto const inc : klass->extra_includes)
            if (!inc.empty())
            {
                bool banned = false;
                for (auto const& binc : _bannedIncludes)
                    if (strstr(inc.c_str(), binc.c_str()))
                    {
                        banned = true;
                        break;
                    }

                if (banned)
                    continue;

                if (inc.find('.') == std::string::npos)
                    klass->m_modulFile->includes.insert("<" + inc + ">");
                else
                    klass->m_modulFile->includes.insert("\"" + inc + "\"");
            }
    }
}

void ReflectionParser::HandleTemplateClasses(const Cursor & cursor, Namespace & currentNamespace, LanguageType* object)
{
    if (!cursor.IsDefinition())
        return;

    // TODO: implement support for nested class in template class
    Class* parent = object != nullptr ? object->ToClass() : nullptr;
    if (parent && parent->IsTemplate())
        return;

    auto fileDesc = cursor.GetFileDesc();
    auto kind = cursor.GetKind();

    bool isLocalSource = ReflectionModulParser::Instance->GetModulFile(fileDesc); // true; // SourceDesc == fileDesc;

    if (kGenerateBaseModul && !isLocalSource && Filename == "Object.h")
    {
        if (cursor.GetDisplayName().find("basic_string") != std::string::npos)
            isLocalSource = true;
    }

    std::unique_lock<std::mutex> g(ReflectionModulParser::Instance->s_lock);
    std::shared_ptr<Class>& klass = ReflectionModulParser::Instance->m_classes[cursor.GetUID()];
    if (klass && SourceDesc != fileDesc)
        return;

    g.unlock();

    if (!klass)
    {
        klass = std::make_shared<Class>(cursor, currentNamespace);
        ReflectionModulParser::Instance->m_classes[cursor.GetUID()] = klass;
    }


    if (!klass->isMetaInitialized)
    {
        TraverseAST(cursor, currentNamespace, klass.get());
        klass->MetaInit(cursor, currentNamespace);

        if (klass->GetName().empty())
            return;

        if (klass->IsTemplate())
        {
            auto& rKlass = ReflectionModulParser::sTemplateStorage[klass->GetDisplayName().c_str()];
            //assert(rKlass == nullptr); // ToDo: qualified name
            rKlass = klass;
            rKlass->m_templateBaseModulFile = cursor.GetNormalizedSourceFile();
            rKlass->m_isSystemClass = !isLocalSource; // || fileDesc.InSystemHeader();
        }
        klass->isMetaInitialized = true;
    }

    if (!isLocalSource)
        return;

    g.lock();
    auto IModulFile = ReflectionModulParser::Instance->GetModulFile(fileDesc);
    if (IModulFile)
    {
        IModulFile->classes.emplace_back(klass);
        klass->m_modulFile = IModulFile;
        for (auto const inc : klass->extra_includes)
            if (!inc.empty())
            {
                bool banned = false;
                for (auto const& binc : _bannedIncludes)
                    if (strstr(inc.c_str(), binc.c_str()))
                    {
                        banned = true;
                        break;
                    }

                if (banned)
                    continue;

                klass->m_modulFile->includes.insert("\"" + inc + "\"");
            }
    }
    else
    {
        modulDescriptor.classes.emplace_back(klass);
        klass->m_modulFile = &modulDescriptor;
        for (auto const inc : klass->extra_includes)
            if (!inc.empty())
            {
                bool banned = false;
                for (auto const& binc : _bannedIncludes)
                    if (strstr(inc.c_str(), binc.c_str()))
                    {
                        banned = true;
                        break;
                    }

                if (banned)
                    continue;

                klass->m_modulFile->includes.insert("\"" + inc + "\"");
            }
    }
}

void ReflectionParser::HandleGlobals(const Cursor &cursor, Namespace &currentNamespace, LanguageType* object)
{
    // actual definition
    if (!object)
    {
        if (!cursor.IsDefinition())
            return;

        auto fileDesc = cursor.GetFileDesc();
        bool isLocalSource = ReflectionModulParser::Instance->GetModulFile(fileDesc); //SourceDesc == fileDesc;
        if (!isLocalSource)
            return;
    }

    // skip static globals (hidden)
    if (!object && cursor.GetStorageClass( ) == CX_SC_Static)
        return;

    Class* parent = object != nullptr ? object->ToClass() : nullptr;

    // variable declaration, which is global

    if (parent)
    {
        auto staticvar = std::make_shared<Global>(cursor, currentNamespace, parent);
        TraverseAST(cursor, currentNamespace, staticvar.get());
        staticvar->MetaInit(cursor, currentNamespace);

        parent->m_staticFields.emplace_back(
            staticvar
        );

        Cursor type_decl = staticvar->m_fieldType.GetElaboratedType().GetCanonicalType().GetDeclaration();
        if (type_decl.GetKind() != CXCursorKind::CXCursor_NoDeclFound)
        {
            if (type_decl.GetDefinition().GetKind() != CXCursorKind::CXCursor_InvalidFile)
                type_decl = type_decl.GetDefinition();

            if (!type_decl.GetFileDesc().InSystemHeader())
            {
                System::String path = type_decl.GetSourceFile();
                path = path.Replace(standardMSVCIncludePath + "\\", "");
                parent->extra_includes.insert(path);
            }
        }
    }
    else
    {
        auto global = std::make_shared<Global>(cursor, currentNamespace, parent);
        TraverseAST(cursor, currentNamespace, global.get());
        global->MetaInit(cursor, currentNamespace);

        if (global->ShouldCompile())
        {
            std::lock_guard<std::mutex> g(ReflectionModulParser::Instance->s_lock);
            auto fileDesc = cursor.GetFileDesc();
            auto IModulFile = ReflectionModulParser::Instance->GetModulFile(fileDesc);
            if (IModulFile)
                IModulFile->globals.emplace_back(global);
            else
                modulDescriptor.globals.emplace_back(global);
        }
    };
}

void ReflectionParser::HandleGlobalFunctions(const Cursor &cursor, Namespace &currentNamespace, LanguageType* object)
{
    // actual definition
    if (!object)
    {
        if (!cursor.IsDefinition())
            return;

        auto fileDesc = cursor.GetFileDesc();
        bool isLocalSource = ReflectionModulParser::Instance->GetModulFile(fileDesc); //SourceDesc == fileDesc;
        if (!isLocalSource)
            return;
    }

    // skip static globals (hidden)
    if (!object && cursor.GetStorageClass() == CX_SC_Static)
        return;

    Class* parent = object != nullptr ? object->ToClass() : nullptr;

    // function declaration, which is global
    auto function = std::make_shared<Function>(cursor, currentNamespace, parent);
    TraverseAST(cursor, currentNamespace, function.get());
    function->MetaInit(cursor, currentNamespace);

    if (parent && cursor.IsStatic())
    {
		auto iPropertyMethod = parent->m_unResolvedPropertyMethods.find(function->GetName());
		if (iPropertyMethod != parent->m_unResolvedPropertyMethods.end())
		{
			bool isgetter = function->GetName()[0] == 'G';
			if (isgetter)
				iPropertyMethod->second->_getter_method = function;
			else
				iPropertyMethod->second->_setter_method = function;
		}

        parent->m_staticMethods.emplace_back(
            function
        );

        return;
    }

    //if (function->IsTemplate())
    //{
    //    auto& rFunction = ReflectionModulParser::sTemplateFunctionStorage[function->GetDisplayName().c_str()];
    //    assert(rFunction == nullptr);
    //    rFunction = function;
    //    rFunction->m_modulFile = &modulDescriptor;
    //    auto fileDesc = cursor.GetFileDesc();
    //    auto IModulFile = ReflectionModulParser::Instance->m_moduleFileStorage.find(fileDesc);
    //    if (IModulFile != ReflectionModulParser::Instance->m_moduleFileStorage.end())
    //        rFunction->m_modulFile = IModulFile->second;
    //}

    if (function->ShouldCompile())
    {
        std::lock_guard<std::mutex> g(ReflectionModulParser::Instance->s_lock);
        auto fileDesc = cursor.GetFileDesc();
        auto IModulFile = ReflectionModulParser::Instance->GetModulFile(fileDesc);
        if (IModulFile)
            IModulFile->globalFunctions.emplace_back(function);
        else
            modulDescriptor.globalFunctions.emplace_back(function);
    };
}

void ReflectionParser::HandleEnums(const Cursor &cursor, Namespace &currentNamespace, LanguageType* object)
{
    // actual definition and an enum
    auto fileDesc = cursor.GetFileDesc();
    bool isLocalSource = ReflectionModulParser::Instance->GetModulFile(fileDesc); //SourceDesc == fileDesc;
    if (!isLocalSource)
        return;

    if (!cursor.IsDefinition())
        return;

    // anonymous enum if the underlying type display name contains this
    if (cursor.GetType().GetDisplayName().find("anonymous enum at") != std::string::npos)
    {
        // anonymous enums are just loaded as 
        // globals with each of their values
        for (auto &enumChild : cursor.GetChildren())
        {
            if (enumChild.GetKind() == CXCursorKind::CXCursor_EnumConstantDecl)
            {
                auto global = std::make_shared<Global>(enumChild, currentNamespace, nullptr);
                TraverseAST(cursor, currentNamespace, global.get());
                global->MetaInit(cursor, currentNamespace);

                if (global->ShouldCompile())
                {
                    std::lock_guard<std::mutex> g(ReflectionModulParser::Instance->s_lock);
                    auto fileDesc = cursor.GetFileDesc();
                    auto IModulFile = ReflectionModulParser::Instance->GetModulFile(fileDesc);
                    if (IModulFile)
                        IModulFile->globals.emplace_back(global);
                    else
                        modulDescriptor.globals.emplace_back(global);
                };
            }
        }
    }
    else
    {
        auto enewm = std::make_shared<Enum>(cursor, currentNamespace);
        TraverseAST(cursor, currentNamespace, enewm.get());
        enewm->MetaInit(cursor, currentNamespace);

        if (enewm->ShouldCompile())
        {
            std::lock_guard<std::mutex> g(ReflectionModulParser::Instance->s_lock);
            auto fileDesc = cursor.GetFileDesc();
            auto IModulFile = ReflectionModulParser::Instance->GetModulFile(fileDesc);
            if (IModulFile)
                IModulFile->enums.emplace_back(enewm);
            else
                modulDescriptor.enums.emplace_back(enewm);
        }
    }
}

void ReflectionParser::HandleBaseSpecifier(const Cursor & cursor, Namespace & currentNamespace, LanguageType * object)
{
    Class* parent = object != nullptr ? object->ToClass() : nullptr;
    if (!parent)
        return;

    auto baseClass = std::make_shared<BaseClass>(cursor);
    baseClass->m_metaData.Parse(cursor);

    if (baseClass->name.empty())
        return;


    bool enableType = false;
    Class* klass = ReflectionModulParser::Instance->GetClassDesc(baseClass->type.GetDeclaration());
    if (klass && (klass->m_enabled || klass->m_qualifiedName == kTypeObject))
    {
        if (!klass->m_enabled)
            klass->m_enabled = true;
        enableType = true;
    }


	baseClass->classType = baseClass->astType->FindClassLangType();
	if (!baseClass->classType)
	{
		baseClass->astType->InitializeDeclaration(baseClass->type);
		baseClass->classType = baseClass->astType->FindClassLangType();
	}

    parent->AddBaseClass(baseClass);

    // automatically enable the type if not explicitly disabled
    if (isNativeType(baseClass->class_name) || enableType)
    {
        parent->m_enabled = !parent->m_metaData.GetFlag(native_attribute::Disable);
        if (parent->m_enabled)
        {
            nativeTypes.push_back(parent->m_qualifiedName);
        }
    }

    size_t pos = baseClass->name.find('<');
    if (pos != std::string::npos)
    {
        size_t tpos = baseClass->name.find("type-parameter-");
        if (tpos == std::string::npos)
        {
            std::string type = baseClass->type.GetElaboratedType().GetDisplayName();

            if (Class* klass = ReflectionModulParser::Instance->GetClassDesc(baseClass->type.GetDeclaration()))
                klass->DeclareTemplate(*baseClass->astType, &cursor);
        }
    }
}

void ReflectionParser::HandleConstructor(const Cursor & cursor, Namespace & currentNamespace, LanguageType * object)
{
    Class* parent = object != nullptr ? object->ToClass() : nullptr;
    if (!parent)
        return;

    std::shared_ptr<Constructor> constructor = std::make_shared<Constructor>(cursor, currentNamespace, parent);
    TraverseAST(cursor, currentNamespace, constructor.get());
    constructor->MetaInit(cursor, currentNamespace);

    if (constructor->isDefaultConstructor && !constructor->isDeleted)
        parent->defaultConstructor = constructor.get();

    if (constructor->isCopyConstructor)
    {
        parent->copyConstructor = constructor.get();
        if (parent->m_isCanCopy)
            parent->m_isCanCopy = !parent->copyConstructor->isDeleted;
    }

    if (constructor->isMoveConstructor)
        parent->moveConstructor = constructor.get();

    parent->m_constructors.emplace_back(
        constructor
    );
}

void ReflectionParser::HandleFieldDecl(const Cursor & cursor, Namespace & currentNamespace, LanguageType * object)
{
    Class* parent = object != nullptr ? object->ToClass() : nullptr;
    if (!parent)
        return;

    if (cursor.IsStatic())
    {
        HandleGlobals(cursor, currentNamespace, object);
        return;
    }

    auto field = std::make_shared<Field>(cursor, currentNamespace, parent);
    TraverseAST(cursor, currentNamespace, field.get());
    field->MetaInit(cursor, currentNamespace);

    if (field->IsStatic())
    {
        EXCEPT(false && "critical failure");
        //parent->m_staticFields.emplace_back(
        //    field
        //);
    }
    else
    {
        parent->m_fields.emplace_back(
            field
        );
    }

    if (!parent->m_fields.back()->IsProperty() && parent->m_fields.back()->IsReference())
        parent->m_isAbstract = true;

    Cursor type_decl = parent->m_fields.back()->m_fieldType.GetElaboratedType().GetCanonicalType().GetDeclaration();
    if (type_decl.GetKind() != CXCursorKind::CXCursor_NoDeclFound)
    {
        //std::shared_ptr<Class>& klass = ReflectionModulParser::Instance->m_classes[type_decl.GetUID()];
        //if (!klass)
        //{
        //    Namespace ns;
        //    klass = std::make_shared<Class>(type_decl, ns);
        //    ReflectionModulParser::Instance->m_classes[cursor.GetUID()] = klass;
        //
        //    TraverseAST(cursor, currentNamespace, klass.get());
        //    klass->MetaInit(cursor, currentNamespace);
        //    klass->isMetaInitialized = true;
        //}
        //
        //if (klass->m_isAbstract)
        //{
        //    field->m_NoDefaultClassCopyMethod = true;
        //}

        if (type_decl.GetDefinition().GetKind() != CXCursorKind::CXCursor_InvalidFile)
            type_decl = type_decl.GetDefinition();

        if (!type_decl.GetFileDesc().InSystemHeader())
        {
            System::String path = type_decl.GetSourceFile();
            path = path.Replace(standardMSVCIncludePath + "\\", "");
            parent->extra_includes.insert(path);
        }
    }
}

void ReflectionParser::HandleCXXMethod(const Cursor & cursor, Namespace & currentNamespace, LanguageType * object)
{
    Class* parent = object != nullptr ? object->ToClass() : nullptr;
    if (!parent)
        return;

    if (cursor.IsStatic())
    {
        HandleGlobalFunctions(cursor, currentNamespace, object);
        return;
    }

    std::shared_ptr<Method> method = std::make_shared<Method>(cursor, currentNamespace, parent);
    TraverseAST(cursor, currentNamespace, method.get());
    method->MetaInit(cursor, currentNamespace);

    if (method->IsStatic())
    {
        EXCEPT(false && "critical failure");
        //parent->m_staticMethods.emplace_back(
        //    method
        //);
    }
    else
    {
        parent->m_methods.emplace_back(
            method
        );
    }

    if (parent->m_methods.back()->IsPureVirtual())
        parent->m_pureVirtualMethods.push_back(parent->m_methods.back());

    Cursor type_decl = method->m_returnTypeInfo.GetElaboratedType().GetCanonicalType().GetDeclaration();
    if (type_decl.GetKind() != CXCursorKind::CXCursor_NoDeclFound)
    {
        if (type_decl.GetDefinition().GetKind() != CXCursorKind::CXCursor_InvalidFile)
            type_decl = type_decl.GetDefinition();

        if (!type_decl.GetFileDesc().InSystemHeader())
        {
            System::String path = type_decl.GetSourceFile();
            path = path.Replace(standardMSVCIncludePath + "\\", "");
            parent->extra_includes.insert(path);
        }
    }

    auto type = cursor.GetType();
    unsigned count = type.GetArgumentCount();

    for (unsigned i = 0; i < count; ++i)
    {
        auto argument = type.GetArgument(i);
        type_decl = argument.GetDeclaration();

        if (type_decl.GetKind() != CXCursorKind::CXCursor_NoDeclFound)
        {
            if (type_decl.GetDefinition().GetKind() != CXCursorKind::CXCursor_InvalidFile)
                type_decl = type_decl.GetDefinition();

            if (!type_decl.GetFileDesc().InSystemHeader())
            {
                System::String path = type_decl.GetSourceFile();
                path = path.Replace(standardMSVCIncludePath + "\\", "");
                parent->extra_includes.insert(path);
            }
        }
    }
}

void ReflectionParser::HandleAnnotateAttr(const Cursor & cursor, Namespace & currentNamespace, LanguageType * object)
{
    if (!object)
        return;

    object->m_metaData.m_specifications.insert(std::make_pair(cursor.GetKind(), cursor));

    std::string ns;
    for (auto &prop : object->m_metaData.extractProperties(cursor, ns))
    {
        object->m_metaData.m_attributes.emplace(cursor.GetKind(), prop);
        if (!ns.empty())
            object->m_metaData.m_namespacedProperties[ns].emplace(prop.first, prop.second);

        if (cursor.GetKind() == CXCursorKind::CXCursor_AnnotateAttr)
            object->m_metaData.m_properties[prop.first] = prop.second;
    }
}

void ReflectionParser::HandleEnumConstantDecl(const Cursor & cursor, Namespace & currentNamespace, LanguageType * object)
{
    Enum* parent = object != nullptr ? object->ToEnum() : nullptr;
    if (!parent)
        return;

    MetaDataManager valueMeta;
    valueMeta.Parse(cursor);

    // don't add disabled values
    if (!valueMeta.GetFlag(native_attribute::Disable))
        parent->m_values.emplace_back(parent, cursor);

}

void ReflectionParser::HandleParmDecl(const Cursor & cursor, Namespace & currentNamespace, LanguageType * object)
{
    ReflectionParser::HandleAnnotateAttr(cursor, currentNamespace, object);
}

void ReflectionParser::HandleVisbility(const Cursor & cursor, Namespace & currentNamespace, LanguageType * object)
{
    ReflectionParser::HandleAnnotateAttr(cursor, currentNamespace, object);
}

void ReflectionParser::HandleOverride(const Cursor & cursor, Namespace & currentNamespace, LanguageType * object)
{
	ReflectionParser::HandleAnnotateAttr(cursor, currentNamespace, object);
}

void ReflectionParser::TraverseAST(const Cursor & cursor, Namespace & currentNamespace, LanguageType * object)
{
    for (auto &child : cursor.GetChildren())
    {
        ReflectionParseHandler const* reflHandler = GetRelfParseHandler(child.GetKind());
        if (reflHandler && reflHandler->Handler)
            (this->*reflHandler->Handler)(child, currentNamespace, object);
    }
}

void ReflectionParser::GenerateFiles(void)
{
    boost::filesystem::path sourceRootDirectory( System::String(m_options.sourceRoot).ToLowerFirst().c_str() );
    boost::filesystem::path outputFileDirectory( m_options.outputModuleFileDirectory );

    m_moduleFileHeaderTemplate = LoadTemplate( kTemplateModuleFileDefSource );

    if (!m_moduleFileHeaderTemplate.isValid( ))
    {
        std::stringstream error;

        error << "Unable to compile module file header template." << std::endl;
        error << m_moduleFileHeaderTemplate.errorMessage( );

        throw std::runtime_error( error.str( ) );
    }

    m_moduleFileSourceTemplate = LoadTemplate( kTemplateModuleFileSource );

    if (!m_moduleFileSourceTemplate.isValid( ))
    {
        std::stringstream error;

        error << "Unable to compile module file source template." << std::endl;
        error << m_moduleFileSourceTemplate.errorMessage( );

        throw std::runtime_error( error.str( ) );
    }

    boost::filesystem::path metaCacheFileName = m_options.outputModuleFileDirectory;

    metaCacheFileName /= ".meta-cache";

    auto metaCacheFileExists = exists( metaCacheFileName );

    std::string moduleFileCache;

    auto &file = modulDescriptor; //for (auto &file : m_moduleFiles)
    {
        System::String normalizedFn = file.name;
        boost::filesystem::path filePath(normalizedFn.Replace('/', '\\').ToLowerFirst().c_str());

        // path relative to the source root
        auto relativeDir = utils::MakeRelativePath( sourceRootDirectory, filePath )
            .replace_extension( "" ).string( );

        if (relativeDir.find_first_of( ".." ) != std::string::npos)
            return;

        auto outputFile = outputFileDirectory / relativeDir;

        auto pt = outputFile.parent_path();
        auto fn = outputFile.filename();
        auto filename = std::wstring(pt.c_str()) + L"/" + std::wstring(L"Assembly_") + fn.c_str();
        auto outputFileHeader = change_extension(boost::filesystem::path(filename), ".definition.cpp"); //change_extension( outputFile, "Generated.h" );
        auto outputFileSource = change_extension(boost::filesystem::path(filename), ".cpp"); //change_extension( outputFile, "Generated.cpp" );

        std::string source_header = file.name;

        // module file name
        file.name = boost::regex_replace(
            relativeDir,
            kSpecialCharsRegex,
            "_"
        );

        moduleFileCache += file.name + "\n";

        m_moduleFilesDesc[outputFileHeader.string()] = file.name;

        //Mustache::Data<std::string> moduleFileData{ Mustache::Data<std::string>::Type::Object };
        //
        //moduleFileData["name"] = file.name;
        //moduleFileData["header"] = outputFileHeader.string();
        //
        //moduleFilesData << moduleFileData;

        // if the generated file header/source doesn't exist, we need to regenerate
        if (m_options.forceRebuild || 
            !metaCacheFileExists || 
            !exists( outputFileHeader ) ||
            !exists( outputFileSource )
        )
        {
            generateModuleFile( outputFileHeader, outputFileSource, source_header, file );
            return;
        }

        auto lastSourceWrite = last_write_time( filePath );
        auto lastGeneratedHeaderWrite = last_write_time( outputFileHeader );
        auto lastGeneratedSourceWrite = last_write_time( outputFileSource );

        // if the generated file is older than the source file, we need to regenerate
        //if (lastSourceWrite > lastGeneratedHeaderWrite || lastSourceWrite > lastGeneratedSourceWrite)
        generateModuleFile( outputFileHeader, outputFileSource, source_header, file );
    }

    if (!m_options.forceRebuild && metaCacheFileExists)
    {
        std::ifstream cacheFile( metaCacheFileName.string( ) );

        std::istreambuf_iterator<char> cacheFileBegin( cacheFile );
        std::istreambuf_iterator<char> cacheFileEnd( nullptr );
        
        // the cache is the same, so we don't need to write the source files
        if (utils::RangeEqual( 
                moduleFileCache.begin( ), moduleFileCache.end( ), 
                cacheFileBegin, cacheFileEnd 
            ))
        {
            return;
        }
    }

    // update the cache
    utils::WriteText(
        metaCacheFileName.string( ),
        moduleFileCache,
        true
    );
}

Mustache::Mustache<std::string> ReflectionParser::LoadTemplate(const std::string &name) const
{
    auto path = boost::filesystem::path( m_options.templateDirectory );

    path.append( name );

    try
    {
        std::string text;

        utils::LoadText( path.string( ), text );

        return text;
    }
    catch (std::exception &e)
    {
        std::stringstream error;

        error << "Unable to load template ";
        error << name << "." << std::endl;
        error << "-- " << e.what( );

        utils::FatalError( error.str( ) );
    }

    // this will never happen
    return { "" };
}

Mustache::Data<std::string>::PartialType ReflectionParser::LoadTemplatePartial( const std::string &name ) const
{
    auto path = 
        boost::filesystem::path( m_options.templateDirectory ).append( name ).string( );

    try
    {
        auto partialLoader = [=]()
        {
            auto cache = m_templatePartialCache.find( path );

            if (cache == m_templatePartialCache.end( ))
            {
                std::string text;

                utils::LoadText( path, text );

                m_templatePartialCache[ path ] = text;

                return text;
            }
                
            return cache->second;
        };

        return partialLoader;
    }
    catch(std::exception &e)
    {
        std::stringstream error;

        error << "Unable to load template partial ";
        error << name << "." << std::endl;
        error << "-- " << e.what( );

        utils::FatalError( error.str( ) );
    }

    // this will never happen
    return nullptr;
}

void ReflectionParser::addGlobalTemplateData(Mustache::Data<std::string> &data)
{
    data[ "version" ] = kMetaGeneratorVersion;
    data[ "targetName" ] = m_options.targetName;
    data[ "inputSourceFile" ] = m_options.inputSourceFile;
    data[ "moduleHeaderFile" ] = m_options.moduleHeaderFile;
    data[ "outputModuleSourceFile" ] = m_options.outputModuleSource;

    data[ "usingPrecompiledHeader" ] =
        utils::TemplateBool( !m_options.precompiledHeader.empty( ) );

    data[ "precompiledHeader" ] = m_options.precompiledHeader;
}

void ReflectionParser::generateModuleFile(
    const boost::filesystem::path &fileHeader,
    const boost::filesystem::path &fileSource,
    const std::string &sourceHeader,
    const ModuleFile &file
)
{
    // header file
    //{
    //    Mustache::Data<std::string> headerData { Mustache::Data<std::string>::Type::Object };
    //
    //    addGlobalTemplateData( headerData );
    //
    //    headerData[ "moduleFileName" ] = file.name;
    //
    //    boost::filesystem::create_directory( fileHeader.parent_path( ) );
    //
    //    utils::WriteText( 
    //        fileHeader.string( ), 
    //        m_moduleFileHeaderTemplate.render( headerData ) 
    //    );
    //}

    // source file
    {
        std::vector<std::string> standard_includes = {
            "\"Reflection/Runtime/ReflectionDatabase.h\"",
            "\"Reflection/Runtime/ReflectionModule.h\"",
            "\"Reflection/Runtime/MetaProperty.h\"",
            "\"Reflection/Runtime/MethodInvokerBase.h\"",
            "\"Reflection/Runtime/ArrayWrapper.h\"",
            "<typeinfo>",
            "<type_traits>",
            "<functional>"
        };

        standard_includes.insert(standard_includes.end(), file.includes.begin(), file.includes.end());

        Mustache::Data<std::string> sourceData { Mustache::Data<std::string>::Type::Object };
        Mustache::Data<std::string> includeData{ Mustache::Data<std::string>::Type::List };

        addGlobalTemplateData( sourceData );

        sourceData[ "hasReflType" ] = utils::TemplateBool(!file.classes.empty() || !file.globals.empty() || !file.globalFunctions.empty() || !file.enums.empty());
        sourceData[ "moduleFileName" ] = file.name;
        sourceData[ "moduleFileSourceHeader" ] = sourceHeader;
        sourceData[ "moduleFileHeader" ] = fileHeader.string( );
        sourceData[ "moduleHeaderFile" ] = m_options.moduleHeaderFile;

        for (auto const& header : standard_includes)
        {
            Mustache::Data<std::string> moduleFile{ Mustache::Data<std::string>::Type::Object };

            moduleFile["header"] = header;

            includeData << moduleFile;
        }

        sourceData["standardIncludes"] = includeData;

        COMPILE_TYPE_TEMPLATES( this, sourceData, "class", file.classes );
        COMPILE_TYPE_TEMPLATES( this, sourceData, "global", file.globals );
        COMPILE_TYPE_TEMPLATES( this, sourceData, "globalFunction", file.globalFunctions );
        COMPILE_TYPE_TEMPLATES( this, sourceData, "enum", file.enums );

        boost::filesystem::create_directory( fileSource.parent_path( ) );

        utils::WriteText( 
            fileSource.string( ),
            m_moduleFileSourceTemplate.render( sourceData ),
            true
        );

        boost::filesystem::create_directory(fileHeader.parent_path());

        utils::WriteText(
            fileHeader.string(),
            m_moduleFileHeaderTemplate.render( sourceData ),
            true
        );
    }
}

ReflectionModulParser::ReflectionModulParser(const ReflectionOptions & options)
    : m_options(options)
{
    m_index = clang_createIndex(1, m_options.displayDiagnostics ? 1 : 0);
    //clang_CXIndex_setGlobalOptions(m_index, 0);
}

ReflectionModulParser::~ReflectionModulParser()
{
    m_moduleFiles.clear();

    if (m_index)
        clang_disposeIndex(m_index);
}

void ReflectionModulParser::generateModuleFile(const boost::filesystem::path & fileHeader, const boost::filesystem::path & fileSource, const std::string & sourceHeader, const ModuleFile & file)
{
}

void ReflectionModulParser::PrepareObjectsForAssembly()
{
    bool foundNewRefType;
    do
    {
        foundNewRefType = false;

        for (auto& file : m_moduleFileStorage)
        {
            auto modul = file.second;
            {
                //for (auto& klass : modul->classes)
                for (auto itr = modul->classes.begin(); itr != modul->classes.end();)
                {
                    auto klass = *itr;
                    ++itr;

                    if (!klass->ShouldCompile() && !klass->m_enabled)
                    {
                        for (auto& base : klass->m_baseClasses)
                        {
                            klass->VisitBaseClassesEx([&klass, &foundNewRefType](Class* owner, BaseClass const* base) -> void
                            {
                                if (owner->m_enabled)
                                {
                                    klass->m_enabled = true;
                                    return;
                                }

                                Class* baseClass = ReflectionModulParser::Instance->GetClassDesc(base->astType->GetDeclarationHash());
                                if (baseClass && baseClass->m_enabled)
                                {
                                    owner->m_enabled = true;
                                    klass->m_enabled = true;
                                    //foundNewRefType = true;
                                }
                            });
                        }
                    }

                    if (klass->m_enabled)
                        klass->PrepareForAssembly();

                    {
                        for (auto& field : klass->m_fields)
                        {
                            auto templateRange = field->GetMetaData().GetAttribute(CXCursorKind::CXCursor_TemplateRef);
                            if (templateRange.first == templateRange.second)
                                continue;

                            if (Class* klass = ReflectionModulParser::Instance->GetClassDesc(field->GetFieldAstType()->GetDeclarationHash()))
                            {
                                klass->DeclareTemplate(*field->GetFieldAstType(), nullptr);

                                if (!klass->copyConstructor)
                                {
                                    field->m_NoDefaultClassCopyMethod = true;
                                }
                            }
                        }

                        for (auto& method : klass->m_methods)
                        {
                            auto templateRange = method->GetMetaData().GetAttribute(CXCursorKind::CXCursor_TemplateRef);
                            if (templateRange.first == templateRange.second)
                                continue;

                            if (Class* klass = ReflectionModulParser::Instance->GetClassDesc(method->GetReturnAstType()->GetDeclarationHash()))
                                klass->DeclareTemplate(*method->GetReturnAstType(), nullptr);

                            for (auto& arg : method->GetArgumentTypes())
                                if (Class* klass = ReflectionModulParser::Instance->GetClassDesc(arg->GetDeclarationHash()))
                                    klass->DeclareTemplate(*arg, nullptr); 
                        }
                    }
                }
            }
        }
    } while (foundNewRefType);

    for (auto kvp : sTemplateStorage)
    {
        auto templateClass = kvp.second;
        if (!templateClass->m_enabled)
            continue;

        ModuleFile* modul = templateClass->m_modulFile;
        if (!modul)
            continue;

        templateClass->PrepareForAssembly();

        for (auto& decl : templateClass->m_definations)
        {
            decl->hasVirtualBase = templateClass->hasVirtualBase;
            decl->isReflObject = templateClass->isReflObject;
            decl->m_enabled = templateClass->m_enabled;
            decl->BuildTemplateDefinition();
            if (!decl->IsPreparedForAssembly())
                decl->PrepareForAssembly();
            if (decl->m_modulFile)
                decl->m_modulFile->classes.push_back(decl);
            else
                modul->classes.push_back(decl);
        }
    }

    //for (auto kvp : sTemplateMethodStorage)
    //{
    //    auto templateMethod = kvp.second;
    //    if (!templateMethod->m_enabled || !templateMethod->m_parent)
    //        continue;
    //
    //    for (auto& decl : templateMethod->m_definations)
    //    {
    //        decl->m_enabled = templateMethod->m_enabled;
    //        decl->BuildTemplateDefinition();
    //        decl->m_parent->m_methods.push_back(decl);
    //    }
    //}
    //
    //for (auto kvp : sTemplateFunctionStorage)
    //{
    //    auto templateFunction = kvp.second;
    //    if (!templateFunction->m_enabled)
    //        continue;
    //
    //    ModuleFile* modul = templateFunction->m_modulFile;
    //    if (!modul && !templateFunction->m_parent)
    //        continue;
    //
    //    for (auto& decl : templateFunction->m_definations)
    //    {
    //        decl->m_enabled = templateFunction->m_enabled;
    //        decl->BuildTemplateDefinition();
    //        if (decl->m_parent)
    //            decl->m_parent->m_staticMethods.push_back(decl);
    //        else
    //            modul->globalFunctions.push_back(decl);
    //    }
    //}
}

void ReflectionModulParser::GenerateModulFiles(void)
{
    for (auto parser : GetModulFiles())
    {
        try
        {
            parser->GenerateFiles();
        }
        catch (std::exception &e)
        {
            utils::FatalError(e.what());
        }
    }
}

void ReflectionModulParser::GenerateFiles(void)
{
    GenerateModulFiles();

    Mustache::Data<std::string> moduleFilesData{ Mustache::Data<std::string>::Type::List };
    Mustache::Data<std::string> externalData{ Mustache::Data<std::string>::Type::List };

    for (auto const& modul : m_moduleFiles)
    {
        for (auto modulfile : modul->m_moduleFilesDesc)
        {
            Mustache::Data<std::string> moduleFile{ Mustache::Data<std::string>::Type::Object };

            moduleFile["name"] = modulfile.second;
            moduleFile["header"] = modulfile.first;

            moduleFilesData << moduleFile;
        }

        for (auto &handle : modul->m_externals)
            externalData << handle->CompileTemplate(modul.get());
    }

    // modul header file
    {
        auto headerTemplate = LoadTemplate(kTemplateModuleHeader);

        if (!headerTemplate.isValid())
        {
            std::stringstream error;

            error << "Unable to compile module source template." << std::endl;
            error << headerTemplate.errorMessage();

            throw std::runtime_error(error.str().c_str());
        }

        Mustache::Data<std::string> sourceData{ Mustache::Data<std::string>::Type::Object };

        sourceData["version"] = kMetaGeneratorVersion;
        sourceData["targetName"] = m_options.targetName;
        sourceData["inputSourceFile"] = m_options.inputSourceFile;
        sourceData["moduleHeaderFile"] = m_options.moduleHeaderFile;
        sourceData["outputModuleSourceFile"] = m_options.outputModuleSource;
        sourceData["moduleFile"] = moduleFilesData;

        boost::filesystem::path sourcePath(m_options.outputModuleSource);

        auto outputFileHeader = change_extension(boost::filesystem::path(sourcePath), ".h");
        boost::filesystem::create_directory(outputFileHeader.parent_path());

        utils::WriteText(
            outputFileHeader.string(),
            headerTemplate.render(sourceData),
            true
        );

        printf(m_options.moduleHeaderFile.c_str());

        //const_cast<ReflectionOptions&>(m_options).moduleHeaderFile = outputFileHeader.string();
    }

    // module source file
    {
        auto sourceTemplate = LoadTemplate(kTemplateModuleSource);

        if (!sourceTemplate.isValid())
        {
            std::stringstream error;

            error << "Unable to compile module source template." << std::endl;
            error << sourceTemplate.errorMessage();

            throw std::runtime_error(error.str().c_str());
        }

        Mustache::Data<std::string> sourceData{ Mustache::Data<std::string>::Type::Object };

        if (!m_moduleFiles.empty())
        {
            Mustache::Data<std::string> data{ Mustache::Data<std::string>::Type::Object };
            CompileTemplateData(data, m_moduleFiles.begin()->get(), "assembly");
            sourceData["assembly"] = data;
        }

        sourceData["version"] = kMetaGeneratorVersion;
        sourceData["targetName"] = m_options.targetName;
        sourceData["inputSourceFile"] = m_options.inputSourceFile;
        sourceData["moduleHeaderFile"] = m_options.moduleHeaderFile;
        sourceData["outputModuleSourceFile"] = m_options.outputModuleSource;

        sourceData["usingPrecompiledHeader"] =
            utils::TemplateBool(!m_options.precompiledHeader.empty());

        sourceData["precompiledHeader"] = m_options.precompiledHeader;

        sourceData["moduleFile"] = moduleFilesData;
        sourceData["external"] = externalData;

        //COMPILE_TYPE_TEMPLATES( this, sourceData, "external", m_externals );

        boost::filesystem::path sourcePath(m_options.outputModuleSource);

        boost::filesystem::create_directory(sourcePath.parent_path());

        utils::WriteText(
            sourcePath.string(),
            sourceTemplate.render(sourceData),
            true
        );
    }
}

void ReflectionModulParser::CreatePCH(void)
{
    // excludeDeclsFromPCH = 1, displayDiagnostics=1
    auto Idx = clang_createIndex(1, 1);

    // IndexTest.pch was produced with the following command:
    // "clang -x c IndexTest.h -emit-ast -o IndexTest.pch"
    auto TU = clang_createTranslationUnit(Idx, "IndexTest.pch");

    // This will load all the symbols from 'IndexTest.pch'
    //clang_visitChildren(clang_getTranslationUnitCursor(TU),
    //    TranslationUnitVisitor, 0);

    clang_disposeTranslationUnit(TU);

    // This will load all the symbols from 'IndexTest.c', excluding symbols
    // from 'IndexTest.pch'.
    char *args[] = { "-Xclang", "-include-pch=IndexTest.pch" };
    TU = clang_createTranslationUnitFromSourceFile(Idx, "IndexTest.c", 2, args,
        0, 0);
    //clang_visitChildren(clang_getTranslationUnitCursor(TU),
    //    TranslationUnitVisitor, 0);
    clang_disposeTranslationUnit(TU);

}

Mustache::Mustache<std::string> ReflectionModulParser::LoadTemplate(const std::string & name) const
{
    auto path = boost::filesystem::path(m_options.templateDirectory);

    path.append(name);

    try
    {
        std::string text;

        utils::LoadText(path.string(), text);

        return text;
    }
    catch (std::exception &e)
    {
        std::stringstream error;

        error << "Unable to load template ";
        error << name << "." << std::endl;
        error << "-- " << e.what();

        utils::FatalError(error.str());
    }

    // this will never happen
    return { "" };
}

ReflectionParser * ReflectionModulParser::CreateReflectionFileParser(const ReflectionOptions & options)
{
    auto parser = std::make_shared<ReflectionParser>(options);

    //parser->m_index = m_index;
    m_modulMutex.lock();
    parser->m_index = m_index; // clang_createIndex(true, m_options.displayDiagnostics);
    clang_CXIndex_setGlobalOptions(parser->m_index, CXGlobalOpt_None);
    m_modulMutex.unlock();

    const_cast<ReflectionOptions&>(m_options).inputSourceFile = options.inputSourceFile;
    parser->sourceFile = m_options.inputSourceFile;

    m_modulMutex.lock();
    m_moduleFiles.push_back(parser);
    m_modulMutex.unlock();
    return parser.get();
}

void ReflectionModulParser::PrepareTranslatorUnit(ReflectionParser * parser)
{
    std::vector<const char *> arguments;

#if defined(SYSTEM_INCLUDE_DIRECTORY)
    arguments.emplace_back("-I" SYSTEM_INCLUDE_DIRECTORY);
#endif

    for (auto &argument : parser->m_options.arguments)
        arguments.emplace_back(argument.c_str());


    if (!parser->m_options.displayDiagnostics)
        arguments.push_back("-Wno-everything");

    //if (parser->m_options.displayDiagnostics)
    //{
    //    for (auto *argument : arguments)
    //        std::cout << argument << std::endl;
    //}

    parser->m_translationUnit = clang_parseTranslationUnit(
        m_index,
        parser->m_options.inputSourceFile.c_str(),
        arguments.data(),
        static_cast<int>(arguments.size()),
        nullptr,
        0,
        CXTranslationUnit_Flags::CXTranslationUnit_SkipFunctionBodies | CXTranslationUnit_Flags::CXTranslationUnit_KeepGoing
    );

    //CXIndexAction action = clang_IndexAction_create(index);
    //clang_indexTranslationUnit(action, NULL, &indexerCallbacks, sizeof(indexerCallbacks), CXIndexOpt_SuppressWarnings, translationUnit);
    //clang_IndexAction_dispose(action);

    auto cursor = clang_getTranslationUnitCursor(parser->m_translationUnit);
    parser->SourceDesc = SourceFileDescriptor(parser->m_translationUnit, parser->m_options.inputSourceFile.c_str());

}

Class * ReflectionModulParser::GetClassDesc(Cursor const & cursor)
{
    if (cursor.GetKind() != CXCursor_NoDeclFound)
    {
        std::lock_guard<std::mutex> g(ReflectionModulParser::Instance->s_lock);
        auto KlassDesc = m_classes.find(cursor.GetUID());
        if (KlassDesc != m_classes.end())
            return KlassDesc->second.get();
    }
    return nullptr;
}

ModuleFile * ReflectionModulParser::GetModulFile(SourceFileDescriptor const & fileDesc)
{
    static std::hash<std::string> hasher;
    std::string filename;

    utils::ToString(clang_getFileName(fileDesc.file), filename);
    boost::filesystem::path path(filename);

    auto KlassDesc = m_moduleFileStorage.find(hasher(path.filename().string())/*fileDesc.GetHash()*/);
    if (KlassDesc != m_moduleFileStorage.end())
        return KlassDesc->second;
    return nullptr;
}

Class * ReflectionModulParser::GetClassDescByName(const char* name)
{
    std::lock_guard<std::mutex> g(ReflectionModulParser::Instance->s_lock);
    for (auto& klass : m_classes)
    {
        if (klass.second->GetName() == name)
            return klass.second.get();
    }
    return nullptr;
}

Class * ReflectionModulParser::GetClassDesc(size_t cursorHash)
{
    std::lock_guard<std::mutex> g(ReflectionModulParser::Instance->s_lock);
    auto KlassDesc = m_classes.find(cursorHash);
    if (KlassDesc != m_classes.end())
        return KlassDesc->second.get();
    return nullptr;
}

//ModuleFile * ReflectionModulParser::GetModulFile(size_t fileDescHash)
//{
//    auto KlassDesc = m_moduleFileStorage.find(fileDescHash);
//    if (KlassDesc != m_moduleFileStorage.end())
//        return KlassDesc->second;
//    return nullptr;
//}

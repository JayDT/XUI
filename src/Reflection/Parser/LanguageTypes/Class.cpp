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

#include "LanguageTypes/Class.h"

#include "ReservedTypes.h"
#include "MetaUtils.h"
#include "MetaDataConfig.h"
#include "System/rtString.h"

#include <boost/algorithm/string/join.hpp>

extern std::set<std::string> _bannedIncludes;

BaseClass::BaseClass(const Cursor &cursor)
    : m_metaData()
    , type(cursor.GetType())
    , IsVirtualBase(cursor.IsVirtualBase())
    , m_accessModifier(cursor.GetAccessModifier())
    , astType(AstType::Create(cursor.GetType()))
{
    name = cursor.GetType().GetCanonicalType().GetDisplayName();
    size_t pos = name.find('<');
    if (pos != std::string::npos)
    {
        //name = type.GetDisplayName(); //GetDeclaration().
        class_name = name.substr(0, pos);
    }
    else
        class_name = name;
}

Class::Class(std::string const & name)
    : LanguageType()
    , m_templateClass(nullptr)
    , m_name(name)
    , m_displayName(name)
    , m_spellingName(name)
    , m_qualifiedName(name)
    , m_isTemplate(false)
    , m_noDestructor(true)
    , m_isAbstract(false)
{
    m_arrayTypeEnabled = name != "void";
    if (!m_arrayTypeEnabled)
        m_isAbstract = true;
    m_modulFile = nullptr;
    isReflObject = false;
    hasVirtualBase = false;
    m_enabled = true;
}

Class::Class(const Cursor &cursor, const Namespace &currentNamespace)
    : LanguageType(cursor, currentNamespace)
    , m_templateClass(nullptr)
    , m_type(cursor.GetType())
    , m_name(cursor.GetDisplayName())
    , m_spellingName(cursor.GetSpelling())
    , m_qualifiedName(m_type.GetDisplayName())
    , m_isTemplate(false)
    , m_noDestructor(false)
    , m_isAbstract(false)
{
    m_astType = AstType::Create(cursor.GetType());
    m_modulFile = nullptr;
    m_namespace = utils::GetNamespaceParent(cursor);
    hasVirtualBase = false;
    isReflObject = m_qualifiedName == kTypeObject;
    m_definitionHeader = cursor.GetSourceFile();
    m_isTemplate = cursor.GetKind() == CXCursorKind::CXCursor_ClassTemplate;
}

Class::~Class()
{

}

bool Class::ShouldCompile(void) const
{
    return isAccessible() /*&& !isNativeType( m_qualifiedName )*/ && !m_isTemplate;
}

void Class::MetaInit(const Cursor & cursor, const Namespace &currentNamespace)
{
    LanguageType::MetaInit(cursor, currentNamespace);

    if (m_metaData.GetFlag(native_attribute::ReflIncludeType))
    {
        std::string incl = m_metaData.GetNativeString(native_attribute::ReflIncludeType);
        extra_includes.insert(incl);
    }
    m_isAbstract = m_metaData.GetFlag(native_attribute::AbstractType); // Workaround until not implement pure virtual test
    m_arrayTypeEnabled = m_metaData.GetFlag(native_attribute::EnableArrayType);
    auto displayName = m_metaData.GetNativeString(native_attribute::DisplayName);

    if (displayName.empty())
    {

        m_displayName = m_qualifiedName;
        if (m_qualifiedName.empty())
        {
            m_displayName = cursor.GetSpelling();
            m_qualifiedName = m_displayName;

            auto TemplateTypes = m_metaData.GetAttribute(CXCursorKind::CXCursor_TemplateTypeParameter);
            for (auto iT = TemplateTypes.first; iT != TemplateTypes.second; ++iT)
            {
                m_templateTypeNames.push_back(iT->second.first);
            }
        }
    }
    else
    {
        m_displayName = displayName;
    }
}

Mustache::Data<std::string> Class::CompileTemplate(const ReflectionParser *context) const
{
    assert(!m_isTemplate);
    if (!IsPreparedForAssembly())
        std::cout << "Logic error: not prepared class: " << m_displayName << std::endl;

	m_isAbstract = m_isAbstract || !m_unResolvedMethodOverrides.empty();

    Mustache::Data<std::string> data{ Mustache::Data<std::string>::Type::Object };

    data["internalID"] = std::to_string(++internalID);
    data["isReflObject"] = utils::TemplateBool(IsReflObject());
    data["isDynamicCast"] = utils::TemplateBool(HasVirtualBase());
    data["LinkerVisibility"] = std::to_string(m_visibility);
    data["destructorID"] = std::to_string(++internalID);
    data["arrayConstructorID"] = !m_isAbstract && (defaultConstructor || m_constructors.empty()) ? std::to_string(++internalID) : std::to_string(0);
    data["ConstructorID"] = !m_isAbstract && (defaultConstructor || m_constructors.empty()) ? std::to_string(++internalID) : std::to_string(0);

    data["Name"] = m_name;
    data["SpellingName"] = m_spellingName;
    data["displayName"] = m_displayName;
    data["qualifiedName"] = m_qualifiedName;
    data["namespace"] = boost::join(m_namespace, "::");
    data["ptrTypeEnabled"] = utils::TemplateBool(m_ptrTypeEnabled);

    data["constPtrTypeEnabled"] =
        utils::TemplateBool(m_constPtrTypeEnabled);

    data["arrayTypeEnabled"] =
        utils::TemplateBool(m_arrayTypeEnabled);

    data["HasDefaultConstructor"] =
        utils::TemplateBool(!m_isAbstract && (defaultConstructor || m_constructors.empty()));

    data["hasDestructor"] =
        utils::TemplateBool(!m_noDestructor);

    m_metaData.CompileTemplateData(data, context);

    // base classes
    {
        Mustache::Data<std::string> baseClasses{ Mustache::Data<std::string>::Type::List };

        int i = 0;

        for (auto &baseClass : m_baseClasses)
        {
            Mustache::Data<std::string> item{ Mustache::Data<std::string>::Type::Object };

            item["name"] = baseClass->name;
            item["virtual"] = std::to_string(baseClass->IsVirtualBase);
            item["accessLevel"] = std::to_string(int(m_accessModifier));

            item["isLast"] =
                utils::TemplateBool(i == m_baseClasses.size() - 1);

            baseClasses << item;

            ++i;
        }

        data["baseClass"] = baseClasses;
    }

    data["TemplateDefinationClass"] = utils::TemplateBool(!m_templateTypes.empty());

    if (!m_templateTypes.empty())
    {
        Mustache::Data<std::string> templateClass{ Mustache::Data<std::string>::Type::List };

        int i = 0;

        for (auto &tName : m_templateTypeNames)
        {
            // Skip invalid types (Example: non typed template)
            if (tName.empty())
                continue;

            Mustache::Data<std::string> item{ Mustache::Data<std::string>::Type::Object };

            item["name"] = tName;

            item["isLast"] =
                utils::TemplateBool(i == m_templateTypeNames.size() - 1);

            templateClass << item;

            ++i;
        }

        if (i > 0)
            data["templateClass"] = templateClass;
        else
            data["TemplateDefinationClass"] = utils::TemplateBool(false);
    }

    // don't do anything else if only registering
    if (m_metaData.GetFlag(native_attribute::Register))
        return data;

    // constructors
    if (!m_isAbstract)
    {
        Mustache::Data<std::string> constructors{ Mustache::Data<std::string>::Type::List };

        for (auto &ctor : m_constructors)
        {
            if (ctor->ShouldCompile())
                constructors << ctor->CompileTemplate(context);
        }

        data["constructor"] = constructors;
        data["dynamicConstructor"] = constructors;
    }

    // fields
    {
        Mustache::Data<std::string> fields{ Mustache::Data<std::string>::Type::List };

        for (auto &field : m_fields)
        {
            if (!field->IsProperty() && field->ShouldCompile())
                fields << field->CompileTemplate(context);
        }

        data["field"] = fields;
    }

    // Properties
    {
        Mustache::Data<std::string> fields{ Mustache::Data<std::string>::Type::List };

        for (auto &field : m_fields)
        {
            if (field->IsProperty() && field->ShouldCompile())
                fields << field->CompileTemplate(context);
        }

        data["property"] = fields;
    }

    // static fields
    {
        Mustache::Data<std::string> staticFields{ Mustache::Data<std::string>::Type::List };

        for (auto &staticField : m_staticFields)
        {
            if (staticField->ShouldCompile())
                staticFields << staticField->CompileTemplate(context);
        }

        data["staticField"] = staticFields;
    }

    // static fields
    {
        Mustache::Data<std::string> methods{ Mustache::Data<std::string>::Type::List };

        for (auto &method : m_methods)
        {
            if (method->ShouldCompile())
                methods << method->CompileTemplate(context);
        }

        data["method"] = methods;
    }

    // static fields
    {
        Mustache::Data<std::string> staticMethods{ Mustache::Data<std::string>::Type::List };

        for (auto &staticMethod : m_staticMethods)
        {
            if (staticMethod->ShouldCompile())
                staticMethods << staticMethod->CompileTemplate(context);
        }

        data["staticMethod"] = staticMethods;
    }

    return data;
}

void Class::AddBaseClass(std::shared_ptr<BaseClass> const & _base)
{
	m_baseClasses.push_back(_base);

	if (_base->classType)
	{
		for (auto const& abstractMethod : _base->classType->m_pureVirtualMethods)
		{
			m_unResolvedMethodOverrides.insert(abstractMethod);
		}
	}
}

void Class::ResolveOverrideMethod(std::shared_ptr<Method> const & _base, Method const* _override)
{
	auto iVirtual = m_unResolvedMethodOverrides.find(_base);
	if (iVirtual != m_unResolvedMethodOverrides.end())
	{
		m_unResolvedMethodOverrides.erase(iVirtual);
	}
}

void Class::ResolvePropertyMethod(std::string const & name, Field * property)
{
	bool isgetter = name[0] == 'G';
	for (auto& m : m_methods)
	{
		if (m->GetName() == name)
		{
			if (isgetter)
				property->_getter_method = m;
			else
				property->_setter_method = m;
			return;
		}
	}

	for (auto& m : m_staticMethods)
	{
		if (m->GetName() == name)
		{
			if (isgetter)
				property->_getter_method = m;
			else
				property->_setter_method = m;
			return;
		}
	}

	m_unResolvedPropertyMethods[name] = property;
}

void Class::DeclareTemplate(AstType const& defination, const Cursor* cursor)
{
    if (!m_isTemplate /*|| (!m_enabledSystemClass && m_isSystemClass)*/)
        return;

    AstType* tType = defination.GetElaboratedType();
    if (tType->GetCanonicalType() &&
		tType->GetCanonicalType()->GetKind() != CXTypeKind::CXType_Unexposed &&
		tType->GetCanonicalType()->GetDisplayName().find("type-parameter-") == std::string::npos)
        tType = tType->GetCanonicalType();

	if (!tType || tType->GetKind() == CXTypeKind::CXType_Unexposed)
		return;

	std::string qName = tType->GetQualifiedName();

    for (auto const& klass : m_definations)
    {
        if (klass->m_qualifiedName == qName)
            return;
    }

    m_astType = (AstType*)&defination;

    auto templateArgs(std::move(tType->GetTemplateArguments()));
    DeclareTemplate(qName, templateArgs, cursor);
}

void Class::DeclareTemplate(std::string const & DisplayName, std::vector<AstType*> const & templateArgs, const Cursor* cursor)
{
    if (!m_isTemplate)
        return;

    for (auto const& klass : m_definations)
    {
        if (klass->m_qualifiedName == DisplayName)
            return;
    }

    auto klass = std::make_shared<Class>(*this);
    m_definations.push_back(klass);

    if (ReflectionModulParser::Instance->GetActiveModul())
    {
        klass->m_modulFile = &ReflectionModulParser::Instance->GetActiveModul()->modulDescriptor;
        if (cursor)
        {
            auto fileDesc = cursor->GetFileDesc();
            auto IModulFile = ReflectionModulParser::Instance->GetModulFile(fileDesc);
            if (IModulFile)
                klass->m_modulFile = IModulFile;
        }

        // Hack for outside template declare definition
        if (!m_modulFile)
        {
            klass->m_modulFile->classes.emplace_back(shared_from_this());
            m_modulFile = klass->m_modulFile;
        }
    }

    klass->m_templateClass = this;
    klass->m_name = klass->m_spellingName;
    klass->m_displayName = klass->m_qualifiedName = DisplayName; //utils::GetQualifiedName(tType);
    klass->m_isTemplate = false;
    klass->m_isAbstract = m_isAbstract;
    klass->hasVirtualBase = hasVirtualBase;
    klass->isReflObject = isReflObject;
    klass->m_enabledSystemClass = m_enabledSystemClass;
    klass->m_isSystemClass = m_isSystemClass;
    klass->m_isCanCopy = m_isCanCopy;
    klass->m_metaData = m_metaData;

    bool hasCopyContructor = true;
    klass->m_templateTypeNames.clear();

    for (auto const& t : templateArgs)
    {
        if (t->FindClassLangType())
            hasCopyContructor = hasCopyContructor && t->FindClassLangType()->copyConstructor;
        if (t->GetCanonicalType() && t->GetCanonicalType()->GetDisplayName().find("type-parameter-") == std::string::npos)
        {
            klass->m_templateTypes.push_back(t->GetCanonicalType());
            klass->m_templateTypeNames.push_back(t->GetCanonicalType()->GetQualifiedName());
        }
        else
        {
            klass->m_templateTypes.push_back(t);
            klass->m_templateTypeNames.push_back(t->GetQualifiedName());
        }
    }

    klass->m_fields.clear();
    for (auto& field : m_fields)
    {
        klass->m_fields.emplace_back(std::make_shared<Field>(*field.get()));
    }

    klass->m_methods.clear();
    for (auto& method : m_methods)
    {
        klass->m_methods.emplace_back(std::make_shared<Method>(*method.get()));
    }

    klass->m_constructors.clear();
    for (auto& method : m_constructors)
    {
        klass->m_constructors.emplace_back(std::make_shared<Constructor>(*method.get()));

        if (method->isDefaultConstructor)
            klass->defaultConstructor = method.get();

        if (method->isCopyConstructor)
            klass->copyConstructor = method.get();

        if (method->isMoveConstructor)
            klass->moveConstructor = method.get();
    }

    klass->m_baseClasses.clear();
    for (auto& base : m_baseClasses)
    {
        klass->m_baseClasses.emplace_back(std::make_shared<BaseClass>(*base.get()));

        size_t pos = base->name.find('<');
        if (pos != std::string::npos)
        {
            if (Class* baseklass = ReflectionModulParser::Instance->GetClassDesc(base->astType->GetDeclarationHash()))
            {
                std::vector<AstType*> baseTemplateArgs;
                std::string baseName = base->name;

                AstType* declType = base->astType;
                while (true)
                {
                    switch (declType->GetKind())
                    {
                        case CXType_Pointer:
                        case CXType_LValueReference:
                        case CXType_RValueReference:
                        {
                            declType = declType->GetElaboratedType();
                            continue;
                        }
                    }

                    break;
                }

                switch (declType->GetKind())
                {
                    case CXType_Elaborated:
                    case CXType_Unexposed:
                    {
                        baseName = declType != declType->GetCanonicalType() ? declType->GetCanonicalType()->GetQualifiedName() : baseName;
                        /// "class_name<type-parameter-0-1>"

                        for (int i = 0; i != klass->m_templateTypeNames.size(); ++i)
                        {
                            if (baseName.find(System::String::format("type-parameter-%u-%u", 0, i)) != std::string::npos)
                            {
                                baseTemplateArgs.push_back(klass->m_templateTypes[i]);
                                System::String newType(baseName);
                                baseName = newType.Replace(System::String::format("type-parameter-%u-%u", 0, i), klass->m_templateTypeNames[i]);
                            }
                        }
                        break;
                    }
                }

                baseklass->DeclareTemplate(baseName, baseTemplateArgs, cursor);
            }
        }
    }
}

//void TemplateExpose(AstType* astType)
//{
//    std::string _typename = 
//
//    size_t pos = m_returnType.find('<');
//    if (pos != std::string::npos)
//    {
//        std::string returnType = declType.GetDisplayName();
//        size_t pos2 = returnType.find('<');
//        if (pos2 != std::string::npos)
//        {
//            size_t startBlock_base = pos2;
//            size_t startBlock_canonical = pos;
//
//            size_t finalBlock_canonical = m_returnType.find('>', startBlock_canonical);
//
//            while (startBlock_canonical < finalBlock_canonical)
//            {
//                size_t endBlock_base = returnType.find(',', startBlock_base + 1);
//                if (endBlock_base == std::string::npos) endBlock_base = returnType.find('>', startBlock_base + 1);
//                size_t endBlock_canonical = m_returnType.find(',', startBlock_canonical + 1);
//                if (endBlock_canonical == std::string::npos) endBlock_canonical = m_returnType.find('>', startBlock_canonical + 1);
//
//                returnType.erase(startBlock_base, endBlock_base - startBlock_base);
//                returnType.insert(startBlock_base, m_returnType.substr(startBlock_canonical, endBlock_canonical - startBlock_canonical));
//
//                endBlock_base = returnType.find(',', startBlock_base);
//                if (endBlock_base == std::string::npos) break; // no more template from base type (auto declare)
//
//                startBlock_base = endBlock_base;
//                startBlock_canonical = endBlock_canonical;
//            }
//
//            m_returnType = returnType;
//        }
//    }
//}

void Class::BuildTemplateDefinition()
{
    if (!m_isAbstract)
        m_isAbstract = m_templateClass->m_isAbstract;

    if (!hasVirtualBase)
        hasVirtualBase = m_templateClass->hasVirtualBase;

    if (!isReflObject)
        isReflObject = m_templateClass->isReflObject;

	m_unResolvedMethodOverrides = m_templateClass->m_unResolvedMethodOverrides;

    for (auto& field : m_fields)
    {
        field->m_parent = this;

        //bool showDebug = false;
        //if (field->m_name.find("Model") != std::string::npos)
        //    showDebug = true;

        AstType* declType = field->m_fieldAstType;
        while (true)
        {
            switch (declType->GetKind())
            {
                case CXType_Pointer:
                case CXType_LValueReference:
                case CXType_RValueReference:
                {
                    declType = declType->GetElaboratedType();
                    continue;
                }
            }

            break;
        }

        switch (declType->GetKind())
        {
            case CXType_Elaborated:
            case CXType_Unexposed:
            {
                //field->m_type = field->m_fieldAstType->GetCanonicalType()->GetQualifiedName();
                for (int i = 0; i != m_templateClass->m_templateTypeNames.size(); ++i)
                {
                    System::String newType(field->m_type);
                    field->m_type = newType.Replace(System::String::format("type-parameter-%u-%u", 0, i), m_templateTypeNames[i]);
                }
                break;
            }
        }

        if (!field->m_NoDefaultClassCopyMethod)
        {
            auto hash = field->GetFieldAstType()->GetDeclarationHash();
            if (field->GetFieldAstType()->GetElaboratedType())
                hash = field->GetFieldAstType()->GetElaboratedType()->GetDeclarationHash();

            Class* typeClass = ReflectionModulParser::Instance->GetClassDesc(hash);
            if (typeClass)
            {
                if (!typeClass->m_isCanCopy)
                    field->m_NoDefaultClassCopyMethod = true;
            }

            for (auto ta : declType->GetTemplateArguments())
            {
                auto hash = ta->GetDeclarationHash();
                if (ta->GetElaboratedType())
                    hash = ta->GetElaboratedType()->GetDeclarationHash();

                //if (showDebug)
                //    std::cout << ta->GetDisplayName() << std::endl;

                Class* typeClass = ReflectionModulParser::Instance->GetClassDesc(hash);
                if (typeClass)
                {
                    //if (showDebug)
                    //    std::cout << "has copy " << typeClass->m_isCanCopy << std::endl;

                    if (!typeClass->m_isCanCopy)
                        field->m_NoDefaultClassCopyMethod = true;
                }
                //else
                //{
                //    if (showDebug)
                //        std::cout << "not found class hash " << hash << std::endl;
                //}
            }
        }
    }

    for (auto& method : m_methods)
    {
        method->m_parent = this;

        if (method->m_returnType.find("type-parameter-") != std::string::npos)
        {
            for (int i = 0; i != m_templateClass->m_templateTypeNames.size(); ++i)
            {
                System::String newType(method->m_returnType);
                method->m_returnType = newType.Replace(System::String::format("type-parameter-%u-%u", 0, i), m_templateTypeNames[i]);
            }
        }
        else if (m_templateClass && method->m_returnType.find("<") != std::string::npos)
        {
            AstType* declType = method->m_returnAstTypeInfo;
            while (true)
            {
                switch (declType->GetKind())
                {
                    case CXType_Pointer:
                    case CXType_LValueReference:
                    case CXType_RValueReference:
                    {
                        declType = declType->GetElaboratedType();
                        continue;
                    }
                }

                break;
            }

            if (declType && declType->GetDisplayName() == m_templateClass->m_name)
                method->m_returnType = m_displayName;
        }

        for (int i = 0; i < method->m_signature.size(); ++i)
        {
            std::string& arg = method->m_signature[i];
            if (arg.find("type-parameter-") == std::string::npos)
            {
                if (m_templateClass && arg.find("<") != std::string::npos)
                {
                    AstType* declType = method->m_arguments[i];
                    while (true)
                    {
                        switch (declType->GetKind())
                        {
                            case CXType_Pointer:
                            case CXType_LValueReference:
                            case CXType_RValueReference:
                            {
                                declType = declType->GetElaboratedType();
                                continue;
                            }
                        }

                        break;
                    }

                    if (declType && declType->GetDisplayName() == m_templateClass->m_name)
                        arg = m_displayName;
                }
                continue;
            }

            for (int i = 0; i != m_templateClass->m_templateTypeNames.size(); ++i)
            {
                System::String newType(arg);
                arg = newType.Replace(System::String::format("type-parameter-%u-%u", 0, i), m_templateTypeNames[i]);
            }
        }
    }

    for (auto& method : m_constructors)
    {
        method->m_parent = this;

        for (int i = 0; i < method->m_signature.size(); ++i)
        {
            std::string& arg = method->m_signature[i];
            if (arg.find("type-parameter-") == std::string::npos)
            {
                if (m_templateClass && arg.find("<") != std::string::npos)
                {
                    AstType* declType = method->m_arguments[i];
                    while (true)
                    {
                        switch (declType->GetKind())
                        {
                            case CXType_Pointer:
                            case CXType_LValueReference:
                            case CXType_RValueReference:
                            {
                                declType = declType->GetElaboratedType();
                                continue;
                            }
                        }

                        break;
                    }

                    if (declType && declType->GetDisplayName() == m_templateClass->m_name)
                        arg = m_displayName;
                }
                continue;
            }

            for (int i = 0; i != m_templateClass->m_templateTypeNames.size(); ++i)
            {
                System::String newType(arg);
                arg = newType.Replace(System::String::format("type-parameter-%u-%u", 0, i), m_templateTypeNames[i]);
            }
        }
    }

    // TODO: Template installation by base decl
    for (auto& base : m_baseClasses)
    {
        AstType* declType = base->astType;
        while (true)
        {
            switch (declType->GetKind())
            {
                case CXType_Pointer:
                case CXType_LValueReference:
                case CXType_RValueReference:
                {
                    declType = declType->GetElaboratedType();
                    continue;
                }
            }

            break;
        }

        switch (declType->GetKind())
        {
            case CXType_Elaborated:
            case CXType_Unexposed:
            {
                base->name = declType->GetCanonicalType()->GetQualifiedName();
                /// "class_name<type-parameter-0-1>"

                for (int i = 0; i != m_templateClass->m_templateTypeNames.size(); ++i)
                {
                    System::String newType(base->name);
                    base->name = newType.Replace(System::String::format("type-parameter-%u-%u", 0, i), m_templateTypeNames[i]);
                }
                break;
            }
        }
    }
}

void Class::PrepareForAssembly()
{
    extern std::string standardMSVCIncludePath;
    m_isPreparedForAssembly = true;

    for (auto& field : m_fields)
    {
        if (m_modulFile)
        {
            auto hash = field->GetFieldAstType()->GetDeclarationHash();
            if (field->GetFieldAstType()->GetElaboratedType())
                hash = field->GetFieldAstType()->GetElaboratedType()->GetDeclarationHash();

            Class* typeClass = ReflectionModulParser::Instance->GetClassDesc(hash);
            if (typeClass)
            {
                if (!typeClass->m_isCanCopy)
                    field->m_NoDefaultClassCopyMethod = true;

                if (!typeClass->m_definitionHeader.empty())
                {
                    System::String path = typeClass->m_definitionHeader;
                    path = path.Replace(standardMSVCIncludePath + "\\", "");
                    bool banned = false;
                    for (auto const& binc : _bannedIncludes)
                        if (strstr(path.c_str(), binc.c_str()))
                        {
                            banned = true;
                            break;
                        }

                    if (banned)
                        continue;

                    if (path.find('.') == std::string::npos)
                        m_modulFile->includes.insert(System::String::format("<%s>", path.c_str()));
                    else
                        m_modulFile->includes.insert(System::String::format("\"%s\"", path.c_str()));
                }
            }

            if (!field->m_NoDefaultClassCopyMethod)
            {
                for (auto ta : field->GetFieldAstType()->GetTemplateArguments())
                {
                    auto hash = ta->GetDeclarationHash();
                    if (ta->GetElaboratedType())
                        hash = ta->GetElaboratedType()->GetDeclarationHash();

                    Class* typeClass = ReflectionModulParser::Instance->GetClassDesc(hash);
                    if (typeClass)
                    {
                        if (!typeClass->m_isCanCopy)
                            field->m_NoDefaultClassCopyMethod = true;
                    }
                }
            }
        }
    }

    for (auto iMethod = m_methods.begin(); iMethod != m_methods.end();)
    {
        if (!m_isAbstract && (*iMethod)->IsPureVirtual())
            m_isAbstract = true;

        //bool propertyMeth = false;
        //for (auto iField = m_fields.begin(); iField != m_fields.end(); ++iField)
        //{
        //    if ((*iField)->m_Setter == (*iMethod)->m_name || (*iField)->m_Getter == (*iMethod)->m_name)
        //    {
        //        propertyMeth = true;
        //        break;
        //    }
        //}

        if (m_modulFile)
        {
            auto hash = (*iMethod)->GetReturnAstType()->GetDeclarationHash();
            if ((*iMethod)->GetReturnAstType()->GetElaboratedType())
                hash = (*iMethod)->GetReturnAstType()->GetElaboratedType()->GetDeclarationHash();

            Class* typeClass = ReflectionModulParser::Instance->GetClassDesc(hash);
            if (typeClass && !typeClass->m_definitionHeader.empty())
            {
                System::String path = typeClass->m_definitionHeader;
                path = path.Replace(standardMSVCIncludePath + "\\", "");

                path = path.Replace(standardMSVCIncludePath + "\\", "");
                bool banned = false;
                for (auto const& binc : _bannedIncludes)
                    if (strstr(path.c_str(), binc.c_str()))
                    {
                        banned = true;
                        break;
                    }

                if (banned)
                    continue;

                if (path.find('.') == std::string::npos)
                    m_modulFile->includes.insert(System::String::format("<%s>", path.c_str()));
                else
                    m_modulFile->includes.insert(System::String::format("\"%s\"", path.c_str()));
            }
        }

        //if (propertyMeth)
        //{
        //    m_methods.erase(iMethod++);
        //}
        //else
        {
            ++iMethod;
        }
    }

    if (!hasVirtualBase || !isReflObject)
    {
        VisitBaseClasses([this](BaseClass const* base) -> void
        {
            if (base->IsVirtualBase && !hasVirtualBase)
            {
                hasVirtualBase = true;
            }

            if (!isReflObject && base->class_name == kTypeObject)
            {
                isReflObject = true;
            }
        });
    }
}

std::shared_ptr<Method> Class::GetMethod(size_t guid) const
{
	Method * method = nullptr;
	for (auto const& m : m_methods)
	{
		if (m->m_guid == guid)
			return m;
	}
	return nullptr;
}

bool Class::isAccessible(void) const
{
    return m_enabled || m_metaData.GetFlag(native_attribute::Register);
}
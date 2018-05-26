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

#include "LanguageType.h"

#include "LanguageTypes/Constructor.h"
#include "LanguageTypes/Field.h"
#include "LanguageTypes/Global.h"
#include "LanguageTypes/Method.h"
#include "LanguageTypes/Function.h"

class Class;
struct ModuleFile;

struct BaseClass
{
    BaseClass(const Cursor &cursor);

    MetaDataManager m_metaData;
    std::string name;
    std::string class_name;
    CursorType type;
    AstType* astType;
	std::shared_ptr<Class> classType;

    bool IsVirtualBase;
    CX_CXXAccessSpecifier m_accessModifier;
};

class Class : public LanguageType, public std::enable_shared_from_this<Class>
{
    // to access m_qualifiedName
    friend class Global;
    friend class Function;
    friend class Method;
    friend class MethodBase;
    friend class Constructor;
    friend class Field;
    friend class FieldBase;
    friend class ReflectionModulParser;
    friend class ReflectionParser;

public:
    Class(std::string const& name);
    Class(const Cursor &cursor, const Namespace &currentNamespace);
    virtual ~Class();

    virtual bool ShouldCompile(void) const override;
    Class* ToClass() override final { return this; }
    void MetaInit(const Cursor &cursor, const Namespace &currentNamespace) override;

    Mustache::Data<std::string> CompileTemplate( const ReflectionParser *context ) const override;

    std::string const& GetName() const { return m_name; }
    std::string const& GetDisplayName() const { return m_displayName; }

    bool IsTemplate() const { return m_isTemplate; }
    bool IsReflObject() const { return isReflObject; }
    bool HasVirtualBase() const { return hasVirtualBase; }

	void AddBaseClass(std::shared_ptr<BaseClass> const& _base);
	void ResolveOverrideMethod(std::shared_ptr<Method> const& _base, Method const* _override);
	void ResolvePropertyMethod(std::string const& name, Field*);

    void DeclareTemplate(AstType const& defination, const Cursor* cursor);
    void DeclareTemplate(std::string const& DisplayName, std::vector<AstType*> const& templateArgs, const Cursor* cursor);
    void BuildTemplateDefinition();
    void PrepareForAssembly();

    template<typename T>
    inline void VisitBaseClasses(T visitor) const
    {
        auto klassStorage = ReflectionModulParser::Instance->sClassObjectStorage;
        for (auto base : m_baseClasses)
        {
            if (Class* klass = ReflectionModulParser::Instance->GetClassDesc(base->astType->GetDeclarationHash()))
                if (klass != this)
                    klass->VisitBaseClasses(visitor);

            visitor(base.get());
        }
    }

    template<typename T>
    inline void VisitBaseClassesEx(T visitor) const
    {
        auto klassStorage = ReflectionModulParser::Instance->sClassObjectStorage;
        for (auto base : m_baseClasses)
        {
            if (Class* klass = ReflectionModulParser::Instance->GetClassDesc(base->astType->GetDeclarationHash()))
                if (klass != this)
                    klass->VisitBaseClassesEx(visitor);

            visitor(const_cast<Class*>(this), base.get());
        }
    }

    bool isMetaInitialized = false;

    bool IsPreparedForAssembly() const { return m_isPreparedForAssembly; }

	std::shared_ptr<Method> GetMethod(size_t guid) const;

private:
    Class* m_templateClass;
    ModuleFile* m_modulFile = nullptr;

    CursorType m_type;

    std::string m_templateBaseModulFile;
    std::string m_name;
    std::string m_spellingName;
    std::string m_displayName;
    std::string m_qualifiedName;
    std::string m_definitionHeader;

    Namespace m_namespace;

    bool m_isTemplate;
    mutable bool m_isAbstract;
    bool m_noDestructor;
    bool isReflObject;
    bool m_isCanCopy = true;
    bool hasVirtualBase;
    bool m_enabledSystemClass = false;
    bool m_isSystemClass = false;
    bool m_isPreparedForAssembly = false;

    Constructor* defaultConstructor = nullptr;
    Constructor* copyConstructor = nullptr;
    Constructor* moveConstructor = nullptr;

    template<typename T>
    using SharedPtrVector = std::list<std::shared_ptr<T>>;

    std::vector<AstType*> m_templateTypes;
    std::vector<std::string> m_templateTypeNames;
    std::set<std::string> extra_includes;

    SharedPtrVector<BaseClass> m_baseClasses;

    SharedPtrVector<Constructor> m_constructors;

    SharedPtrVector<Field> m_unexposedMember;
    SharedPtrVector<Field> m_fields;
    SharedPtrVector<Global> m_staticFields;

    SharedPtrVector<Method> m_methods;
    SharedPtrVector<Method> m_pureVirtualMethods;
    SharedPtrVector<Function> m_staticMethods;

    SharedPtrVector<Class> m_definations;

	std::set<std::shared_ptr<Method> /* base */> m_unResolvedMethodOverrides;
	std::map<std::string, Field* /* property */> m_unResolvedPropertyMethods;

    bool isAccessible(void) const;
};

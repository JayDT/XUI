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

#include "TypeData.h"
#include "Type.h"

#include "Constructor.h"
#include "Destructor.h"

#include "Field.h"
#include "Global.h"

#include "Method.h"
#include "Function.h"

#include "Enum.h"
#include "EnumContainer.h"

#include "ReflectionModule.h"
#include "ReflectionDatabase.h"

namespace System::Reflection
{
    inline namespace meta
    {
        std::map<intptr_t, FieldSetterBase*> singletonFieldSetter;
        std::map<intptr_t, FieldGetterBase*> singletonFieldGetter;
		std::map<intptr_t, FieldSetterBase*> singletonStaticFieldSetter;
		std::map<intptr_t, FieldGetterBase*> singletonStaticFieldGetter;
		std::map<intptr_t, FunctionInvokerBase*> singletonFunctionInvoker;
        std::map<intptr_t, MethodInvokerBase*> singletonMethodInvoker;

        intptr_t TypeData::GetPointerAdjustment(Variant const & instance)
        {
            return intptr_t(__class_object_adjuter_impl(internalId, (Variant&)instance));
        }

        TypeData::TypeData(void)
            : __class_object_adjuter_impl(nullptr)
            , isEnum( false )
            , isPrimitive( false )
            , isPointer( false )
            , isClass( false )
            , isDefined( false )
            , enumeration { nullptr }  { }

        TypeData::~TypeData()
        {
            if (TypeDescriptor)
                delete TypeDescriptor;
            TypeDescriptor = nullptr;
        }

        ///////////////////////////////////////////////////////////////////////

        std::vector<TypeData::AttachedClass>* TypeData::GetTemplateTypes() const
        {
            return TemplateClasses;
        }

        void TypeData::AddBaseClasses(
            ReflectionDatabase & db, 
            TypeID thisType, 
            AttachedClass const & classes)
        {
            if (!BaseClasses)
                BaseClasses = new std::vector<AttachedClass>;
            BaseClasses->push_back(classes);
            
            if (classes.m_type && classes->IsValid())
            {
                // add this type to the base type's derived classes
                ReflectionModulManager::RefDatabase rdb = ReflectionModulManager::Instance().GetDatabase(classes->m_id.ModulID);
                if (!rdb->types[classes->m_id]->DerivedClasses)
                    rdb->types[classes->m_id]->DerivedClasses = new std::vector<Type const*>;
                rdb->types[classes->m_id]->DerivedClasses->push_back(db.types[thisType]);
            }
        }

        void TypeData::InitializeTemplateTypes(
            ReflectionDatabase & db, 
            TypeID thisType, 
            const std::initializer_list<AttachedClass>& classes)
        {
            if (!TemplateClasses)
                TemplateClasses = new std::vector<AttachedClass>;
            TemplateClasses->insert(TemplateClasses->end(), classes.begin(), classes.end());
        }

        const std::vector<TypeData::AttachedClass>* TypeData::GetBaseClasses(void) const
        {
            return BaseClasses;
        }

        ///////////////////////////////////////////////////////////////////////

        const std::vector<Type const*>* TypeData::GetDerivedClasses(void) const
        {
            return DerivedClasses;
        }

        ///////////////////////////////////////////////////////////////////////

        std::vector<Constructor*>* TypeData::GetConstructors(void) const
        {
            return constructors;
        }

        ///////////////////////////////////////////////////////////////////////

        std::vector<Constructor*>* TypeData::GetDynamicConstructors(void) const
        {
            return GetConstructors();
        }

        ///////////////////////////////////////////////////////////////////////

        std::vector<Method*> const* TypeData::GetMethods(void) const
        {
            return methods;
        }

        ///////////////////////////////////////////////////////////////////////

        std::vector<Function*> const* TypeData::GetStaticMethods(void) const
        {
            return staticMethods;
        }

        ///////////////////////////////////////////////////////////////////////

        const std::vector<Field*>* TypeData::GetFields(void) const
        {
            // @@@TODO: recursively get base class fields

            return fields;
        }

        const std::vector<Field*>* TypeData::GetProperties(void) const
        {
            return Properties;
        }

        ///////////////////////////////////////////////////////////////////////

        std::vector<Global*>* TypeData::GetStaticFields(void) const
        {
            return staticFields;
        }

        ///////////////////////////////////////////////////////////////////////

        const Constructor &TypeData::GetConstructor(
            const ArgHashStorage & signature
        ) const
        {
            if (constructors)
            {
                for (Constructor* m : *constructors)
                    if (m->CanCall(signature))
                        return *m;
            }
            return Constructor::Invalid();
        }

	    const Constructor& TypeData::GetConstructor(
			const InvokableSignature& signature
		) const
        {
			if (constructors)
			{
				for (Constructor* m : *constructors)
					if (m->CanCall(signature))
						return *m;
			}
			return Constructor::Invalid();
        }

	    const Constructor &TypeData::GetDynamicConstructor(
            const ArgHashStorage & signature
        ) const
        {
            return GetConstructor(signature);
        }

        ///////////////////////////////////////////////////////////////////////

        void TypeData::AddField(
            const char* name,
            size_t NativeFieldTypeHashCode,
            size_t internalId,
            Type const& FieldType,
            Type const& ClassType,
            Variant methodGetter(size_t methodId, Variant &&obj),
            void methodSetter(size_t methodId, Variant &&obj, Variant &&value),
            FieldDesc const& fieldinfo_impl(size_t fieldId),
            const MetaManager::Initializer & meta,
            CXXAccessSpecifier accessLevel)
        {
            FieldGetterBase* Getter = nullptr;
            if (methodGetter)
            {
                FieldGetterBase*& helper = singletonFieldGetter[reinterpret_cast<intptr_t>(methodGetter)];
                if (!helper)
                    helper = new PropertyGetter(methodGetter);
                Getter = helper;
            }
            
            FieldSetterBase* Setter = nullptr;
            if (methodSetter)
            {
                FieldSetterBase*& helper = singletonFieldSetter[reinterpret_cast<intptr_t>(methodGetter)];
                if (!helper)
                    helper = new PropertySetter(methodSetter);
                Setter = helper;
            }
            
            Field* field = new Field(
                name,
                FieldType,
                ClassType,
                Getter,
                Setter,
                accessLevel);
            
            if (!fields)
                fields = new std::vector<Field*>;

            if (fields->capacity() <= fields->size())
                fields->reserve(fields->size() + 1);
            fields->push_back( field );
            
            if (fieldinfo_impl)
                field->m_fieldInfo = &fieldinfo_impl(internalId);
            field->m_fieldTypeHashCode = NativeFieldTypeHashCode;
            field->m_isProperty = false;
            field->m_UID = internalId;
            field->m_metaData = meta;
        }

        void TypeData::AddStaticField(
            const char* name,
            size_t NativeFieldTypeHashCode,
            size_t internalId,
            Type const& FieldType,
            Type const& ClassType,
            Variant methodGetter(size_t methodId, Variant &&obj),
            void methodSetter(size_t methodId, Variant &&obj, Variant &&value),
            FieldDesc const& fieldinfo_impl(size_t fieldId),
            const MetaManager::Initializer & meta,
            CXXAccessSpecifier accessLevel)
        {
            FieldGetterBase* Getter = nullptr;
            if (methodGetter)
            {
                FieldGetterBase*& helper = singletonFieldGetter[reinterpret_cast<intptr_t>(methodGetter)];
                if (!helper)
                    helper = new PropertyGetter(methodGetter);
                Getter = helper;
            }

            FieldSetterBase* Setter = nullptr;
            if (methodSetter)
            {
                FieldSetterBase*& helper = singletonFieldSetter[reinterpret_cast<intptr_t>(methodGetter)];
                if (!helper)
                    helper = new PropertySetter(methodSetter);
                Setter = helper;
            }

            Global* field = new Global(
                name,
                FieldType,
                Getter,
                Setter,
                ClassType,
                accessLevel);

            if (!staticFields)
                staticFields = new std::vector<Global*>;
            if (staticFields->capacity() <= staticFields->size())
                staticFields->reserve(staticFields->size() + 1);
            staticFields->push_back(field);
            
            if (fieldinfo_impl)
                field->m_fieldInfo = &fieldinfo_impl(internalId);
            field->m_fieldTypeHashCode = NativeFieldTypeHashCode;
            field->m_UID = internalId;
            field->m_metaData = meta;
        }

        const Field &TypeData::GetField(const std::string &name) const
        {
            if (fields)
            {
                for (auto &field : *fields)
                    if (field->GetName() == name)
                        return *field;
            }
            return Field::Invalid( );
        }

        void TypeData::AddProperty(
            const char* name,
            size_t NativeFieldTypeHashCode,
            size_t internalId,
            Type const& FieldType,
            Type const& ClassType,
            Variant methodGetter(size_t methodId, Variant &&obj),
            void methodSetter(size_t methodId, Variant &&obj, Variant &&value), 
            FieldDesc const& fieldinfo_impl(size_t fieldId),
            const MetaManager::Initializer & meta,
            CXXAccessSpecifier accessLevel,
			bool getterStatic,
			bool setterStatic)
        {
            FieldGetterBase* Getter = nullptr;
            if (methodGetter)
            {
				if (getterStatic)
				{
					FieldGetterBase*& helper = singletonStaticFieldGetter[reinterpret_cast<intptr_t>(methodGetter)];
					if (!helper)
					{
						helper = new PropertyGetter(methodGetter);
						static_cast<PropertyGetter*>(helper)->isStatic = true;
					}
					Getter = helper;
				}
				else
				{
					FieldGetterBase*& helper = singletonFieldGetter[reinterpret_cast<intptr_t>(methodGetter)];
					if (!helper)
						helper = new PropertyGetter(methodGetter);
					Getter = helper;
				}
            }
            
            FieldSetterBase* Setter = nullptr;
            if (methodSetter)
            {
				if (setterStatic)
				{
					FieldSetterBase*& helper = singletonFieldSetter[reinterpret_cast<intptr_t>(methodGetter)];
					if (!helper)
					{
						helper = new PropertySetter(methodSetter);
						static_cast<PropertySetter*>(helper)->isStatic = true;
					}
					Setter = helper;
				}
				else
				{
					FieldSetterBase*& helper = singletonFieldSetter[reinterpret_cast<intptr_t>(methodGetter)];
					if (!helper)
						helper = new PropertySetter(methodSetter);
					Setter = helper;
				}
            }
            
            Field* prop = new Field(
                name,
                FieldType,
                ClassType,
                Getter,
                Setter,
                accessLevel);

            if (!Properties)
                Properties = new std::vector<Field*>;

            if (Properties->capacity() <= Properties->size())
                Properties->reserve(Properties->size() + 1);
            Properties->push_back( prop );
            
            if (fieldinfo_impl)
                prop->m_fieldInfo = &fieldinfo_impl(internalId);
            prop->m_fieldTypeHashCode = NativeFieldTypeHashCode;
            prop->m_isProperty = true;
            prop->m_UID = internalId;
            prop->m_metaData = meta;
        }

        ///////////////////////////////////////////////////////////////////////

        const Field &TypeData::GetProperty(const std::string &name) const
        {
            if (Properties)
            {
                for (auto &field : *Properties)
                    if (name == field->GetName())
                        return *field;
            }
            return Field::Invalid( );
        }

        ///////////////////////////////////////////////////////////////////////

        const Global &TypeData::GetStaticField(const std::string &name) const
        {
            if (staticFields)
            {
                for (auto &field : *staticFields)
                    if (field->GetName() == name)
                        return *field;
            }
            return Global::Invalid( );
        }

        Method const* TypeData::GetMethod(const std::string &name) const
        {
            if (methods)
            {
                for (Method* method : *methods)
                {
                    if (method->GetName() == name)
                        return method;
                }
            }
            return nullptr;
        }

        ///////////////////////////////////////////////////////////////////////

        Method const* TypeData::GetMethod(
            const std::string &name, 
            const ArgHashStorage &signature
        ) const
        {
            auto base = GetMethod(name);
            if (!base)
                return nullptr;

            if (const_cast<Method*>(base)->CanCall(signature))
                return base;

            return nullptr;
        }

        void TypeData::AddStaticMethod(const char* name, Variant invoker_impl(size_t methodId, Variant &&obj, const ArgumentList && arguments), MethodDesc const & methodinfo_impl(size_t methodId), size_t methodid, const MetaManager::Initializer & meta,
            CXXAccessSpecifier accessLevel)
        {
            MethodDesc const& mInfo = methodinfo_impl(methodid);
            if (mInfo.m_classType->GetID() == InvalidTypeID)
                return;

            FunctionInvokerBase*& invoker = singletonFunctionInvoker[reinterpret_cast<intptr_t>(invoker_impl)];
            if (!invoker)
            {
                invoker = new FunctionWrapper;
                FunctionWrapper* methodWrapper = (FunctionWrapper*)invoker;
                methodWrapper->invoker = invoker_impl;
                methodWrapper->methodinfo = methodinfo_impl;
            }

            Function* meth = new Function(name, *mInfo.m_classType, invoker, accessLevel);
            meth->m_metaData = meta;

            if (!staticMethods)
                staticMethods = new std::vector<Function*>;

            if (staticMethods->capacity() <= staticMethods->size())
                staticMethods->reserve(staticMethods->size() + 1);

            meth->m_UID = methodid;
            staticMethods->push_back(meth);
        }

        ///////////////////////////////////////////////////////////////////////

        Function const* TypeData::GetStaticMethod(const std::string &name) const
        {
            if (staticMethods)
            {
                for (Function* method : *staticMethods)
                {
                    if (method->GetName() == name)
                        return method;
                }
            }
            return nullptr;
        }

        ///////////////////////////////////////////////////////////////////////

        Function const* TypeData::GetStaticMethod(
            const std::string &name, 
            const ArgHashStorage & signature
        ) const
        {
            auto base = GetStaticMethod(name);
            if (!base)
                return nullptr;

            if (const_cast<Function*>(base)->CanCall(signature))
                return base;

            return nullptr;
        }

        void TypeData::AddMethod(
            const char* name,
            Variant invoker_impl(size_t methodId, Variant &&obj, const ArgumentList && arguments),
            MethodDesc const& methodinfo_impl(size_t methodId),
            size_t methodid,
            const MetaManager::Initializer &meta,
            CXXAccessSpecifier accessLevel
        )
        {
            MethodDesc const& mInfo = methodinfo_impl(methodid);
            if (mInfo.m_classType->GetID() == InvalidTypeID)
                return;

            MethodInvokerBase*& invoker = singletonMethodInvoker[reinterpret_cast<intptr_t>(invoker_impl)];
            if (!invoker)
            {
                invoker = new MethodWrapper;
                MethodWrapper* methodWrapper = (MethodWrapper*)invoker;
                methodWrapper->invoker = invoker_impl;
                methodWrapper->methodinfo = methodinfo_impl;
            }

            Method* meth = new Method(name, *mInfo.m_classType, invoker, true, accessLevel);
            meth->m_metaData = meta;

            if (!methods)
                methods = new std::vector<Method*>;

            if (methods->capacity() <= methods->size())
                methods->reserve(methods->size() + 1);

            meth->m_UID = methodid;
            methods->push_back(meth);
        }

        void TypeData::AddConstructor(
            Variant invoker_impl(size_t methodId, Variant &&obj, const ArgumentList && arguments),
            MethodDesc const& methodinfo_impl(size_t methodId),
            size_t methodid,
            const MetaManager::Initializer &meta,
            CXXAccessSpecifier accessLevel
        )
        {
            MethodDesc const& mInfo = methodinfo_impl(methodid);
            if (mInfo.m_classType->GetID() == InvalidTypeID)
                return;

            FunctionInvokerBase*& invoker = singletonFunctionInvoker[reinterpret_cast<intptr_t>(invoker_impl)];
            if (!invoker)
            {
                invoker = new FunctionWrapper;
                FunctionWrapper* methodWrapper = (FunctionWrapper*)invoker;
                methodWrapper->invoker = invoker_impl;
                methodWrapper->methodinfo = methodinfo_impl;
            }
            
            Constructor* meth = new Constructor(*mInfo.m_classType, invoker, true, accessLevel);
            meth->m_UID = methodid;
            meth->m_metaData = meta;

            if (!constructors)
                constructors = new std::vector<Constructor*>;
            
            constructors->push_back(meth);
        }

        ///////////////////////////////////////////////////////////////////////

        void TypeData::SetArrayConstructor(
            Variant invoker_impl(size_t methodId, Variant &&obj, const ArgumentList && arguments),
            MethodDesc const& methodinfo_impl(size_t methodId),
            size_t methodid,
            CXXAccessSpecifier accessLevel)
        {
            MethodDesc const& mInfo = methodinfo_impl(methodid);
            if (mInfo.m_classType->GetID() == InvalidTypeID)
                return;
            
            FunctionInvokerBase*& invoker = singletonFunctionInvoker[reinterpret_cast<intptr_t>(invoker_impl)];
            if (!invoker)
            {
                invoker = new FunctionWrapper;
                FunctionWrapper* methodWrapper = (FunctionWrapper*)invoker;
                methodWrapper->invoker = invoker_impl;
                methodWrapper->methodinfo = methodinfo_impl;
            }

            arrayConstructor = new Constructor(*mInfo.m_classType, invoker, true, accessLevel);
            arrayConstructor->m_UID = methodid;
        }

        ///////////////////////////////////////////////////////////////////////

        void TypeData::SetDestructor(
            Variant invoker_impl(size_t methodId, Variant &&obj, const ArgumentList && arguments),
            MethodDesc const& methodinfo_impl(size_t methodId),
            size_t methodid)
        {
            MethodDesc const& mInfo = methodinfo_impl(methodid);
            if (mInfo.m_classType->GetID() == InvalidTypeID)
                return;
            
            MethodInvokerBase*& invoker = singletonMethodInvoker[reinterpret_cast<intptr_t>(invoker_impl)];
            if (!invoker)
            {
                invoker = new MethodWrapper;
                MethodWrapper* methodWrapper = (MethodWrapper*)invoker;
                methodWrapper->invoker = invoker_impl;
                methodWrapper->methodinfo = methodinfo_impl;
            }

            destructor = new Destructor(*mInfo.m_classType, invoker);
            destructor->m_UID = methodid;
        }

        void TypeData::SetEnum(
            const std::string &name,
            Type const* type,
            Type const* underlyingType,
            const std::initializer_list<std::pair<std::string, unsigned long long>> &initializer
        )
        {
            enumeration = new Enum(new EnumContainer(name, type, underlyingType, initializer));
        }

    }
}

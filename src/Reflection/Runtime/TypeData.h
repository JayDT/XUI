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

#include "standard/Platform/Define.h"

#include "TypeID.h"
#include "Variant.h"
#include "MetaManager.h"

#include "ArgumentConfig.h"
#include "InvokableConfig.h"

#include <typeindex>

#define InitializeTypeData(Data, ...) \
    Data->isTrivialConstruct = std::is_trivially_default_constructible<__VA_ARGS__>::value;                               \
    Data->isClass = std::is_class< std::remove_pointer< std::decay<__VA_ARGS__>::type >::type >::value;                   \
    Data->isEnum = std::is_enum< std::remove_pointer< std::decay<__VA_ARGS__>::type >::type >::value;                     \
    Data->isPointer = std::is_pointer< __VA_ARGS__>::value;                                                               \
    Data->isPrimitive = std::is_arithmetic< std::remove_pointer< std::decay<__VA_ARGS__>::type >::type >::value;          \
    Data->isFloatingPoint = std::is_floating_point< std::remove_pointer< std::decay<__VA_ARGS__>::type >::type >::value;  \
    Data->isSigned = std::is_signed< std::remove_pointer< std::decay<__VA_ARGS__>::type >::type >::value;                 \
    Data->isString = typeid(std::remove_cv<std::remove_reference< std::decay<__VA_ARGS__>::type >::type>::type) == typeid(System::String); \
    Data->size = sizeof(_Get_size_type<__VA_ARGS__>::type);                                                               \
    Data->native_hash_code = typeid(__VA_ARGS__).hash_code();                                                             \

#define DeclareReflTypeData(DisplayName, ...)                                                                               \
    {                                                                                                                       \
        {                                                                                                                   \
            m::TypeID id = db.AllocateType( STRINGIFY(DisplayName), typeid(__VA_ARGS__) );                                  \
            auto &type = db.types[ id ];                                                                                    \
                                                                                                                            \
            if (id != System::Reflection::meta::InvalidTypeID) {                                                            \
                const_cast<System::Reflection::meta::Type*>(type)->ElaborateType = nullptr;                                 \
                System::Reflection::meta::TypeData* Data = &type->GetTypeData();                                            \
                InitializeTypeData(Data, __VA_ARGS__)                                                                       \
            } {                                                                                                             \
                m::TypeID uid = db.AllocateType( STRINGIFY(DisplayName*), typeid(__VA_ARGS__*) );                           \
                auto &_type = db.types[ uid ];                                                                              \
                if (uid != System::Reflection::meta::InvalidTypeID) {                                                       \
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];                     \
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                       \
                    InitializeTypeData(Data, __VA_ARGS__*)                                                                  \
                }                                                                                                           \
            } {                                                                                                             \
                m::TypeID uid = db.AllocateType( STRINGIFY(const DisplayName*), typeid(const __VA_ARGS__*) );               \
                auto &_type = db.types[ uid ];                                                                              \
                if (uid != System::Reflection::meta::InvalidTypeID) {                                                       \
                    const_cast<System::Reflection::meta::Type*>(_type)->ElaborateType = db.types[ id ];                     \
                    System::Reflection::meta::TypeData* Data = &_type->GetTypeData();                                       \
                    InitializeTypeData(Data, const __VA_ARGS__*)                                                            \
                }                                                                                                           \
            }                                                                                                               \
        }                                                                                                                   \
    }


namespace System::Reflection
{
    inline namespace meta
    {
        class Variant;
        class Enum;
        class Constructor;
        class Destructor;
        class Field;
        class Global;
        class Method;
        class Function;
        class Object;
        class Argument;
        class Enum;

        struct MethodDesc;

        class MetaManager;
        class ReflectionDatabase;

        // Strong type helpers
        struct ITypeDescriptor
        {
            virtual Variant DefaultValue() const = 0;
            virtual Variant EnumValue(long long value) const = 0;
            //virtual bool Equal(const Variant& rhs) const = 0; // Todo: SFINAE
            virtual int ToInt(const Variant& lhs) const = 0;
            virtual bool ToBool(const Variant& lhs) const = 0;
            virtual float ToFloat(const Variant& lhs) const = 0;
            virtual double ToDouble(const Variant& lhs) const = 0;
            virtual long long ToLong(const Variant& lhs) const = 0;
            //virtual std::string ToString(const Variant& lhs) const = 0;
            virtual int ToInt(void* ptr) const = 0;
            virtual bool ToBool(void* ptr) const = 0;
            virtual float ToFloat(void* ptr) const = 0;
            virtual double ToDouble(void* ptr) const = 0;
            virtual long long ToLong(void* ptr) const = 0;
        };

        template<typename TType>
        class TypeDescriptor : public ITypeDescriptor
        {
            Variant DefaultValue() const override
            {
                return Variant::Empty; //TType();
            }

            virtual Variant EnumValue(long long value) const
            {
                return TType(value);
            }

            int ToInt(const Variant& lhs) const override { return (int)lhs.GetValue<TType>(); }
            bool ToBool(const Variant& lhs) const override { return (bool)lhs.GetValue<TType>(); }
            float ToFloat(const Variant& lhs) const override { return (float)lhs.GetValue<TType>(); }
            double ToDouble(const Variant& lhs) const override { return (double)lhs.GetValue<TType>(); }
            long long ToLong(const Variant& lhs) const override { return (long long)lhs.GetValue<TType>(); }
            //std::string ToString(const Variant& lhs) const override { return std::to_string(lhs.GetValue<TType>()); }
            virtual int ToInt(void* ptr) const override { return (int)(*(TType*)ptr); }
            virtual bool ToBool(void* ptr) const override { return (bool)(*(TType*)ptr); }
            virtual float ToFloat(void* ptr) const override { return (float)(*(TType*)ptr); }
            virtual double ToDouble(void* ptr) const override { return (double)(*(TType*)ptr); }
            virtual long long ToLong(void* ptr) const override { return (long long)(*(TType*)ptr); }
        };

        template<typename TType>
        class TypeDescriptor<TType*> : public ITypeDescriptor
        {
            Variant DefaultValue() const override
            {
                return (TType*)nullptr;
            }

            virtual Variant EnumValue(long long value) const
            {
                return TType(value);
            }
  
            int ToInt(const Variant& lhs) const override { return (int)*lhs.GetValue<TType*>(); }
            bool ToBool(const Variant& lhs) const override { return (bool)*lhs.GetValue<TType*>(); }
            float ToFloat(const Variant& lhs) const override { return (float)*lhs.GetValue<TType*>(); }
            double ToDouble(const Variant& lhs) const override { return (double)*lhs.GetValue<TType*>(); }
            long long ToLong(const Variant& lhs) const override { return (long long)*lhs.GetValue<TType*>(); }
            //std::string ToString(const Variant& lhs) const override { return std::to_string(lhs.GetValue<TType*>()); }
            virtual int ToInt(void* ptr) const override { return (int)(**(TType**)ptr); }
            virtual bool ToBool(void* ptr) const override { return (bool)(**(TType**)ptr); }
            virtual float ToFloat(void* ptr) const override { return (float)(**(TType**)ptr); }
            virtual double ToDouble(void* ptr) const override { return (double)(**(TType**)ptr); }
            virtual long long ToLong(void* ptr) const override { return (long long)(**(TType**)ptr); }
        };

        struct TC_METARUNTIME_API TypeData
        {
            typedef intptr_t class_object_adjuter_impl(size_t, Variant& from);
            typedef std::shared_ptr<Object> class_shared_contructor_impl(size_t, const ArgumentList&& arguments);

            bool isTrivialConstruct : 1;
            bool isEnum : 1;
            bool isPrimitive : 1;
            bool isSigned : 1;
            bool isFloatingPoint : 1;
            bool isPointer : 1;
            bool isClass : 1;
            bool isDefined : 1;
            bool isString : 1; // Cache for freqvent test

            size_t internalId = 0;
            size_t size = 0;
            size_t native_hash_code = 0;
            CXVisibilityKind linkerVisibility;

            const char* fullname = "";
            const char* name = "";
            const char* nameSpace = "";

            class_object_adjuter_impl* __class_object_adjuter_impl = nullptr;
            class_shared_contructor_impl* __class_shared_contructor_impl = nullptr;
            Constructor* arrayConstructor = nullptr;
            Destructor* destructor = nullptr;

            Enum* enumeration = nullptr;  // enum type
            ITypeDescriptor* TypeDescriptor = nullptr;

            MetaManager metaData;

            intptr_t GetPointerAdjustment(Variant const& instance);

            // class type
            struct AttachedClass
            {
                AttachedClass()
                    : m_nativeDescriptor(typeid(void))
                {

                }

                AttachedClass(Type const* type,
                    std::type_index const& nativeDescriptor,
                    size_t alignment,
                    bool isVirtual,
                    CXXAccessSpecifier accessLevel)
                    : m_type(type)
                    , m_nativeDescriptor(nativeDescriptor)
                    , m_alignment(alignment)
                    , m_isVirtual(isVirtual)
                    , m_accessLevel(accessLevel)
                {

                }

                AttachedClass(const AttachedClass& rhs)
                    : m_nativeDescriptor(typeid(void))
                {
                    m_type = rhs.m_type;
                    m_nativeDescriptor = rhs.m_nativeDescriptor;
                    m_alignment = rhs.m_alignment;
                    m_isVirtual = rhs.m_isVirtual;
                    m_accessLevel = rhs.m_accessLevel;
                }

                Type const* m_type = nullptr;
                std::type_index m_nativeDescriptor;
                size_t m_alignment = 0;
                bool m_isVirtual = false;
                CXXAccessSpecifier m_accessLevel = CXXAccessSpecifier::Public;

                Type const* operator->() const { return m_type; }
            };

            std::vector<AttachedClass>* BaseClasses = nullptr;
            std::vector<Type const*>* DerivedClasses = nullptr;
            std::vector<AttachedClass>* TemplateClasses = nullptr;
            std::vector<Constructor*>* constructors = nullptr;
            std::vector<Field*>* fields = nullptr;
            std::vector<Field*>* Properties = nullptr;
            std::vector<Global*>* staticFields = nullptr;
            std::vector<Method*>* methods = nullptr;
            std::vector<Function*>* staticMethods = nullptr;

            ///////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////

            TypeData(void);
            virtual ~TypeData();

            ///** @brief Determines if this type derives from the specified 
            // *         runtime type.
            // *  @return true if both types are class types and this type
            // *          derives from "other".
            // */
            //template<typename T>
            //bool DerivesFrom(void) const;

            /** @brief Gets all base classes for this type.
            *  @return Type set of all base classes for this type.
            */
            const std::vector<TypeData::AttachedClass>* GetBaseClasses(void) const;

            /** @brief Gets all classes that derive from this class type.
            *  @return Type set of all derived classes for this type.
            */
            const std::vector<Type const*>* GetDerivedClasses(void) const;


            /** @brief Gets all constructors for this type
            *         assuming it's a class type.
            *  @return Set of constructors for this type.
            */
            std::vector<Constructor*>* GetConstructors(void) const;

            /** @brief Gets all dynamic constructors for this type
            *         assuming it's a class type.
            *  @return Set of all dynamic constructors for this type.
            */
            std::vector<Constructor*>* GetDynamicConstructors(void) const;

            ///////////////////////////////////////////////////////////////////

            /** @brief Gets all methods for this type
            *         assuming it's a class type.
            *  @return Set of methods for this type.
            */
            std::vector<Method*> const* GetMethods(void) const;

            ///////////////////////////////////////////////////////////////////

            /** @brief Gets all fields for this type
            *         assuming it's a class type.
            *  @return Set of fields for this type.
            */
            const std::vector<Field*>* GetFields(void) const;

            ///////////////////////////////////////////////////////////////////

            /** @brief Gets all static methods for this type
            *         assuming it's a class type.
            *  @return Set of static methods for this type.
            */
            std::vector<Function*> const* GetStaticMethods(void) const;

            ///////////////////////////////////////////////////////////////////

            /** @brief Gets all fields for this type
            *         assuming it's a class type.
            *  @return Set of fields for this type.
            */
            const std::vector<Field*>* GetProperties(void) const;

            ///////////////////////////////////////////////////////////////////

            /** @brief Gets all static fields for this type
            *         assuming it's a class type.
            *  @return Set of static fields for this type.
            */
            std::vector<Global*>* GetStaticFields(void) const;

            ///////////////////////////////////////////////////////////////////

            std::vector<TypeData::AttachedClass>* GetTemplateTypes() const;

            void AddBaseClasses(
                ReflectionDatabase &db,
                TypeID thisType,
                AttachedClass const& classes
            );

            void InitializeTemplateTypes(
                ReflectionDatabase &db,
                TypeID thisType,
                const std::initializer_list<AttachedClass> &classes
            );

            ///////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////

            void AddConstructor(
                Variant invoker_impl(size_t methodId, Variant&& obj, const ArgumentList&& arguments),
                MethodDesc const& methodinfo_impl(size_t methodId),
                size_t methodid,
                const MetaManager::Initializer &meta,
                CXXAccessSpecifier accessLevel = CXXAccessSpecifier::Public
            );

            void SetArrayConstructor(
                Variant invoker_impl(size_t methodId, Variant&& obj, const ArgumentList&& arguments),
                MethodDesc const& methodinfo_impl(size_t methodId),
                size_t methodid,
                CXXAccessSpecifier accessLevel = CXXAccessSpecifier::Public);

            const Constructor &GetConstructor(
                const ArgHashStorage & signature
            ) const;


			const Constructor &GetConstructor(
				const InvokableSignature & signature
			) const;

            const Constructor &GetDynamicConstructor(
                const ArgHashStorage & signature
            ) const;

            ///////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////

            void SetDestructor(
                Variant invoker_impl(size_t methodId, Variant&& obj, const ArgumentList&& arguments),
                MethodDesc const& methodinfo_impl(size_t methodId),
                size_t methodid);

            ///////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////

            // Field Getter, Field Setter
            void AddField(
                const char* name,
                size_t NativeFieldTypeHashCode,
                size_t internalId,
                Type const& FieldType,
                Type const& ClassType,
                Variant methodGetter(size_t methodId, Variant&& obj),
                void methodSetter(size_t methodId, Variant&& obj, Variant&& value),
                FieldDesc const& methodinfo_impl(size_t fieldId),
                const MetaManager::Initializer &meta,
                CXXAccessSpecifier accessLevel = CXXAccessSpecifier::Public
            );

            const Field &GetField(const std::string &name) const;

            // Const Method Getter, Method Setter
			void AddProperty(
				const char* name,
				size_t NativeFieldTypeHashCode,
				size_t internalId,
				Type const& FieldType,
				Type const& ClassType,
				Variant methodGetter(size_t methodId, Variant&& obj),
				void methodSetter(size_t methodId, Variant&& obj, Variant&& value),
				FieldDesc const& methodinfo_impl(size_t fieldId),
				const MetaManager::Initializer &meta,
				CXXAccessSpecifier accessLevel = CXXAccessSpecifier::Public,
				bool getterStatic = false,
				bool setterStatic = false
            );

            const Field &GetProperty(const std::string &name) const;

            ///////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////

            // Field Getter, Field Setter
            void AddStaticField(
                const char* name,
                size_t NativeFieldTypeHashCode,
                size_t internalId,
                Type const& FieldType,
                Type const& ClassType,
                Variant methodGetter(size_t methodId, Variant&& obj),
                void methodSetter(size_t methodId, Variant&& obj, Variant&& value),
                FieldDesc const& methodinfo_impl(size_t fieldId),
                const MetaManager::Initializer &meta,
                CXXAccessSpecifier accessLevel = CXXAccessSpecifier::Public
            );

            const Global &GetStaticField(const std::string &name) const;

            ///////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////

            void AddMethod(
                const char* name,
                Variant invoker_impl(size_t methodId, Variant&& obj, const ArgumentList&& arguments),
                MethodDesc const& methodinfo_impl(size_t methodId),
                size_t methodid,
                const MetaManager::Initializer &meta,
                CXXAccessSpecifier accessLevel = CXXAccessSpecifier::Public
            );

            Method const* GetMethod(
                const std::string &name
            ) const;

            Method const* GetMethod(
                const std::string &name, 
                const ArgHashStorage &signature
            ) const;

            ///////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////

            void AddStaticMethod(
                const char* name,
                Variant invoker_impl(size_t methodId, Variant && obj, const ArgumentList&& arguments),
                MethodDesc const& methodinfo_impl(size_t methodId),
                size_t methodid,
                const MetaManager::Initializer &meta,
                CXXAccessSpecifier accessLevel = CXXAccessSpecifier::Public
            );

            Function const* GetStaticMethod(
                const std::string &name
            ) const;

            Function const* GetStaticMethod(
                const std::string &name, 
                const ArgHashStorage & signature
            ) const;

            ///////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////

            void SetEnum(
                const std::string &name, 
                Type const* type,
                Type const* underlyingType,
                const std::initializer_list<std::pair<std::string, unsigned long long>> &initalizer
            );
        };
    }
}

#include "Impl/TypeData.hpp"

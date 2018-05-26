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

#include "Type.h"
#include "TypeCreator.h"

#include "Constructor.h"
#include "Destructor.h"
#include "Field.h"
#include "Method.h"
#include "Function.h"
#include "Enum.h"

#include "ArrayWrapper.h"

#include "RuntimeMetaProperties.h"

#include "ReflectionDatabase.h"
#include "ReflectionModule.h"

#include "Common/Logging.h"

namespace System::Reflection
{
    using Json = json11::Json;
}

namespace System::Reflection
{
    inline namespace meta
    {
        Type::Type(ReflectionDatabase* db)
            : m_id( InvalidTypeID )
            , m_reflectionDatabase(db)
            , ElaborateType(nullptr)
        {
            ++ReflectionModule::sTypeCounter;
        }

        ///////////////////////////////////////////////////////////////////////

        Type::Type(ReflectionDatabase* db, TypeID id, const char* name)
            : m_id( id )
            , m_reflectionDatabase(db)
            , ElaborateType(nullptr)
        {
            ++ReflectionModule::sTypeCounter;
            fullname = name;
        }

        Type::~Type()
        {
            --ReflectionModule::sTypeCounter;
        }

        ///////////////////////////////////////////////////////////////////////

        const Type &Type::Invalid(void)
        {
            static const Type& invalid = ReflectionModulManager::Instance().GetType(InvalidTypeID);

            return invalid;
        }

        ///////////////////////////////////////////////////////////////////////

        TypeID const& Type::GetID(void) const
        {
            return m_id;
        }

        ///////////////////////////////////////////////////////////////////////

        bool Type::ListsEqual(const List &a, const List &b)
        {
            if (a.size( ) != b.size( ))
                return false;

            auto equal = true;

            for (size_t i = 0u, size = a.size( ); equal && i < size; ++i)
                equal = (a[ i ] == b[ i ]);

            return equal;
        }

        ///////////////////////////////////////////////////////////////////////

        bool Type::IsValid(void) const
        {
            return m_id != InvalidTypeID;
        }

        ///////////////////////////////////////////////////////////////////////

        bool Type::IsPrimitive(void) const
        {
            return isPrimitive;
        }

        ///////////////////////////////////////////////////////////////////////

        bool Type::IsFloatingPoint(void) const
        {
            return isFloatingPoint;
        }

        ///////////////////////////////////////////////////////////////////////

        bool Type::IsSigned(void) const
        {
            return isSigned;
        }

        ///////////////////////////////////////////////////////////////////////

        bool Type::IsEnum(void) const
        {
            return isEnum;
        }

        ///////////////////////////////////////////////////////////////////////

        bool Type::IsPointer(void) const
        {
            return isPointer;
        }

        ///////////////////////////////////////////////////////////////////////

        bool Type::IsClass(void) const
        {
            return isClass;
        }

        ///////////////////////////////////////////////////////////////////////

        bool Type::IsArray(void) const
        {
            return false;
        }

        ///////////////////////////////////////////////////////////////////////

        size_t Type::SizeOf() const
        {
            return size;
        }

        ///////////////////////////////////////////////////////////////////////

        size_t Type::GetHashCode() const
        {
            return native_hash_code;
        }

        ///////////////////////////////////////////////////////////////////////

        const char* Type::GetFullName(void) const
        {
            auto &name = fullname;

            //if (IsArray( ))
            //    return "Array<" + name + ">";

            return name;
        }

        const char* Type::GetName(void) const
        {
            return name;
        }

        ///////////////////////////////////////////////////////////////////////

        const char* Type::GetNameSpace(void) const
        {
            return nameSpace;
        }

        const MetaManager &Type::GetMeta(void) const
        {
            return metaData;
        }

        ///////////////////////////////////////////////////////////////////////

        void Type::Destroy(Variant &instance) const
        {
            if (destructor->IsValid( ))
                destructor->Invoke( instance );
        }

        ///////////////////////////////////////////////////////////////////////

        Type const& Type::GetDecayedType(void) const
        {
            UAssert( false, "Type::GetDecayedType() not implemented." );

            // @@@TODO: convert to non pointer/const pointer type
            return Type::Invalid( );
        }


        ///////////////////////////////////////////////////////////////////////

        const Enum* Type::GetEnum(void) const
        {
            return enumeration;
        }

        ///////////////////////////////////////////////////////////////////////

        bool Type::DerivesFrom(const Type &other) const
        {
            auto baseClasses = GetBaseClasses();
            if (baseClasses)
            {
                for (auto const& base : *baseClasses)
                {
                    if (base.m_type->IsValid() && *base.m_type == other)
                        return true;

					if (other.IsPointer() && !base.m_type->IsPointer() && other.ElaborateType && base.m_type->IsValid() && *base.m_type == *other.ElaborateType)
						return true;

                    if (base->DerivesFrom(other))
                        return true;
                }
            }
            return false;
        }

        bool Type::IsAssignableFrom(const Type & currentType) const
        {
            if (this == &currentType)
                return true;

            if (currentType.DerivesFrom(*this))
                return true;

            return false;
        }

        ///////////////////////////////////////////////////////////////////////

        //const Constructor &Type::GetConstructor(
        //    const ArgHashStorage & signature
        //) const
        //{
        //    return GetTypeData().GetConstructor(signature);
        //}
        //
        /////////////////////////////////////////////////////////////////////////
        //
        //const Constructor &Type::GetDynamicConstructor(
        //    const ArgHashStorage & signature
        //) const
        //{
        //    return GetTypeData().GetDynamicConstructor(signature);
        //}

        ///////////////////////////////////////////////////////////////////////

        const Constructor &Type::GetArrayConstructor(void) const
        {
            return *arrayConstructor;
        }

        ///////////////////////////////////////////////////////////////////////

        const Destructor &Type::GetDestructor(void) const
        {
            return *destructor;
        }

        ///////////////////////////////////////////////////////////////////////

        //const Method &Type::GetMethod(const std::string &name) const
        //{
        //    return GetTypeData().GetMethod( name );
        //}

        ///////////////////////////////////////////////////////////////////////

        //const Method &Type::GetMethod(
        //    const std::string &name, 
        //    const ArgHashStorage & signature
        //) const
        //{
        //    return GetTypeData().GetMethod( name, signature );
        //}


        ///////////////////////////////////////////////////////////////////////

        //const Function &Type::GetStaticMethod(const std::string &name) const
        //{
        //    return GetTypeData().GetStaticMethod( name );
        //}

        ///////////////////////////////////////////////////////////////////////

        //const Function &Type::GetStaticMethod(
        //    const std::string &name, 
        //    const ArgHashStorage & signature
        //) const
        //{
        //    return GetTypeData().GetStaticMethod( name, signature );
        //}



        ///////////////////////////////////////////////////////////////////////

        //const Field &Type::GetField(const std::string &name) const
        //{
        //    return GetTypeData().GetField( name );
        //}

        const Field & Type::GetProperty(const std::string & name) const
        {
            return GetTypeData().GetProperty(name);
        }

        ///////////////////////////////////////////////////////////////////////

        //const Global &Type::GetStaticField(const std::string &name) const
        //{
        //    return GetTypeData().GetStaticField( name );
        //}

        const Field &Type::GetRuntimeProperty(const std::string &name) const
        {
            const Field* result = &GetProperty(name);

            if ((!result || !result->IsValid()) && BaseClasses)
                for (auto const& base : *BaseClasses)
                {
                    result = &base->GetRuntimeProperty(name);
                    if (result->IsValid())
                        break;
                }

            if (!result)
                return Field::Invalid();
            return *result;
        }


        const Field &Type::GetRuntimeField(const std::string &name) const
        {
            const Field* result = &GetField(name);

            if ((!result || !result->IsValid()) && BaseClasses)
                for (auto const& base : *BaseClasses)
                {
                    result = &base->GetRuntimeField(name);
                    if (result->IsValid())
                        break;
                }

            if (!result)
                return Field::Invalid();
            return *result;
        }

        const Method &Type::GetRuntimeMethod(const std::string &name) const
        {
            const Method* result = GetMethod(name);

            if ((!result || !result->IsValid()) && BaseClasses)
                for (auto const& base : *BaseClasses)
                {
                    result = &base->GetRuntimeMethod(name);
                    if (result->IsValid())
                        break;
                }

            if (!result)
                return Method::Invalid();
            return *result;
        }

        const Method &Type::GetRuntimeMethod(const std::string &name, const ArgHashStorage & signature) const
        {
            const Method* result = GetMethod(name);

            if ((!result || !result->IsValid()) && BaseClasses)
                for (auto const& base : *BaseClasses)
                {
                    result = &base->GetRuntimeMethod(name);
                    if (result->IsValid() && result->CanCall(signature))
                        break;
                }

            if (!result)
                return Method::Invalid();
            return *result;
        }

        const Method & Type::GetRuntimeMethod(const std::string & name, const InvokableSignature & signature) const
        {
            const Method* result = GetMethod(name);

            if ((!result || !result->IsValid()) && BaseClasses)
                for (auto const& base : *BaseClasses)
                {
                    result = &base->GetRuntimeMethod(name);
                    if (result->IsValid() && result->CanCall(signature))
                        break;
                }

            if (!result)
                return Method::Invalid();
            return *result;
        }

        ///////////////////////////////////////////////////////////////////////

        Json Type::SerializeJson(const Variant &instance, bool invokeHook) const
        {
            UAssert(
                *this == *instance.GetType( ),
                "Serializing incompatible variant instance.\n"
                "Got '%s', expected '%s'",
                instance.GetType( )->GetName( ),
                GetName( )
            );

            if (IsArray())
            {
                Json::array array;
            
                auto wrapper = instance.GetArray( );
                auto size = wrapper->Size( );
            
                for (size_t i = 0; i < size; ++i)
                {
                    auto value = wrapper->GetValue( i );
            
                    array.emplace_back( 
                        value.GetType( )->SerializeJson( value, invokeHook ) 
                    );
                }
            
                return array;
            }

            if (IsPrimitive() && *this == typeid(bool))
            {
                return { instance.ToBool( ) };
            }

            auto &meta = GetMeta( );
            auto isEnum = IsEnum( );

            // number, or non-associative enum
            if (IsPrimitive( ) /*|| meta.GetProperty<SerializeAsNumber>( )*/)
            {
                if (IsFloatingPoint( ) || !IsSigned( ))
                    return { instance.ToDouble( ) };
 
                return { instance.ToInt( ) };
            }

            // associative enum value
            if (isEnum)
            {
                return GetEnum( )->GetKey( instance.ToLong() );
            }

            if (*this == typeid(std::string))
            {
                return { instance.ToString( ) };
            }
            
            Json::object object { };

            for (auto const& field : *GetFields())
            {
                auto value = field->GetInternalValue( instance );

                auto json = value.SerializeJson( );

                value.m_base->OnSerialize( const_cast<Json::object&>( json.object_items( ) ) );

                object[ field->GetName( ) ] = json;
            }

            if (invokeHook)
                instance.m_base->OnSerialize( object );

            return object;
        }

        ///////////////////////////////////////////////////////////////////////

        Json Type::SerializeJson(const Variant &instance, SerializationGetterOverride getterOverride, bool invokeHook) const
        {
            UAssert(
                *this == *instance.GetType( ),
                "Serializing incompatible variant instance.\n"
                "Got '%s', expected '%s'",
                instance.GetType( )->GetName( ),
                GetName( )
            );

            if (IsArray( ))
            {
                Json::array array;
            
                auto wrapper = instance.GetArray( );
                auto size = wrapper->Size( );
            
                for (size_t i = 0; i < size; ++i)
                {
                    auto value = wrapper->GetValue( i );
            
                    array.emplace_back( 
                        value.GetType( )->SerializeJson( value, invokeHook )
                    );
                }
            
                return array;
            }

            if (IsPrimitive() && *this == typeid(bool))
            {
                return { instance.ToBool( ) };
            }

            auto &meta = GetMeta( );
            auto isEnum = IsEnum( );

            // number, or non-associative enum
            if (IsPrimitive( ) /*|| meta.GetProperty<SerializeAsNumber>( )*/)
            {
                if (IsFloatingPoint( ) || !IsSigned( ))
                    return { instance.ToDouble( ) };
 
                return { instance.ToInt( ) };
            }

            // associative enum value
            if (isEnum)
            {
                return GetEnum( )->GetKey( instance.ToLong());
            }

            if (*this == typeid(std::string))
            {
                return { instance.ToString( ) };
            }
            
            Json::object object { };

            for (auto const&field : *GetFields())
            {
                auto const& value = getterOverride( instance, *field );

                auto json = value.SerializeJson( );

                value.m_base->OnSerialize( const_cast<Json::object&>( json.object_items( ) ) );

                object[ field->GetName( ) ] = json;
            }

            if (invokeHook)
                instance.m_base->OnSerialize( object );

            return object;
        }

        ///////////////////////////////////////////////////////////////////////

        Variant Type::DeserializeJson(const Json &value) const
        {
            static ArgHashStorage null_signatures;
            auto &ctor = GetTypeData().GetConstructor( null_signatures );

            return DeserializeJson( value, ctor );
        }

        ///////////////////////////////////////////////////////////////////////

        Variant Type::DeserializeJson(const Json &value, const Constructor &ctor) const
        {
            // array types get special care
            if (IsArray( ))
            {
                auto const& nonArrayType = *this;
                auto arrayCtor = GetArrayConstructor( );
            
                UAssert(ctor.IsValid(),
                    "Serialization requires a default constructor.\nWith type '%s'.",
                    GetName()
                );
            
                UAssert( arrayCtor.IsValid( ),
                    "Type '%s' does not have an array constructor.\n"
                    "Makes sure it is enabled with the meta property 'EnableArrayType'.",
                    nonArrayType.GetName( )
                );
            
                auto instance = arrayCtor.Invoke( );
                auto wrapper = instance.GetArray( );
            
                size_t i = 0;
            
                for (auto const&item : value.array_items( ))
                {
                    wrapper->Insert( 
                        i++, 
                        nonArrayType.DeserializeJson( item, ctor ) 
                    );
                }
            
                return instance;
            }
            // we have to handle all primitive types explicitly
            if (IsPrimitive( ))
            {
                if (*this == typeid( int ))
                    return value.int_value( );
                else if (*this == typeid( unsigned int ))
                    return (static_cast<unsigned int>( value.number_value( ) ));
                if (*this == typeid(long long))
                    return (static_cast<long long>(value.number_value()));
                else if (*this == typeid(unsigned long long))
                    return (static_cast<unsigned long long>(value.number_value()));
                else if (*this == typeid( bool ))
                    return (value.bool_value( ));
                else if (*this == typeid( float ))
                    return (static_cast<float>( value.number_value( ) ));
                else if (*this == typeid( double ))
                    return (value.number_value( ));
            }
            else if (IsEnum( ))
            {
                // number literal
                if (value.is_number( ))
                    return (value.int_value( ));

                // associative value
                auto enumValue = GetEnum( )->GetValue( value.string_value( ) );

                // make sure we can find the key
                if (enumValue.IsValid( ))
                    return enumValue;
                
                // use the default value as we couldn't find the key
                return TypeCreator::Create( *this );
            }
            else if (*this == typeid( std::string ))
            {
                return Variant(value.string_value( ));
            }

            UAssert(ctor.IsValid(),
                "Serialization requires a default constructor.\nWith type '%s'.",
                GetName()
            );

            // @@@TODO: forward arguments to constructor
            auto instance = ctor.Invoke( );

            DeserializeJson( instance, value );

            return instance;
        }

        ///////////////////////////////////////////////////////////////////////

        void Type::DeserializeJson(Variant &instance, const Json &value) const
        {
            for (auto const&field : *GetFields())
            {
                auto const& fieldType = field->GetFieldType( );

                UAssert( fieldType->IsValid( ),
                    "Unknown type for field '%s' in base type '%s'. Is this type reflected?",
                    fieldType->GetName( ),
                    GetName( )
                );

                auto &fieldData = value[ field->GetName( ) ];

                if (!fieldData.is_null( ))
                {
                    auto fieldValue = fieldType->DeserializeJson( fieldData );

                    fieldValue.m_base->OnDeserialize( fieldData );

                    field->InternalSetValue( instance, fieldValue );
                }
            }

            instance.m_base->OnDeserialize( value );
        }

        intptr_t Type::GetPointerAdjustment(Object * instance) const
        {
            return intptr_t(GetTypeData().GetPointerAdjustment(Variant(instance)));
        }

        intptr_t Type::GetToP(Object * instance) const
        {
            intptr_t adjuster = GetPointerAdjustment(instance);
            return reinterpret_cast<intptr_t>(instance) - adjuster;
        }

        ReflectionModule* Type::Assembly() const { return &m_reflectionDatabase->GetReflectionModul(); }
        TypeData & Type::GetTypeData() const 
        { 
            return *((TypeData*)(this));
        }
    }
}

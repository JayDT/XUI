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

#include "Macros.h"

#include "TypeID.h"
#include "TypeData.h"
#include "TypeConfig.h"
#include "InvokableConfig.h"
#include "ArgumentConfig.h"
#include "JsonConfig.h"

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

        class MetaManager;

        class TC_METARUNTIME_API ReflectionDatabase;
        class TC_METARUNTIME_API ReflectionModule;

        class TC_METARUNTIME_API Type : public TypeData
        {
            Type(const Type &rhs) = delete;
            Type &operator=(const Type &rhs) = delete;

        public:
            typedef std::vector<Type const*> List;
            typedef std::function<Variant(const Variant &, const Field &)> SerializationGetterOverride;

            Type(ReflectionDatabase* db);
            Type(ReflectionDatabase* db, TypeID id, const char* name);
            virtual ~Type();

#pragma region Type Operators
            // Move to header because compiler can be inline cross library
            ///////////////////////////////////////////////////////////////////////

            inline operator bool(void) const
            {
                return m_id != InvalidTypeID ? true : false;
            }

            ///////////////////////////////////////////////////////////////////////

            inline bool operator<(const Type &rhs) const
            {
                return m_id < rhs.m_id ? true : false;
            }

            ///////////////////////////////////////////////////////////////////////

            inline bool operator>(const Type &rhs) const
            {
                return m_id > rhs.m_id ? true : false;
            }

            ///////////////////////////////////////////////////////////////////////

            inline bool operator<=(const Type &rhs) const
            {
                return m_id <= rhs.m_id ? true : false;
            }

            ///////////////////////////////////////////////////////////////////////

            inline bool operator>=(const Type &rhs) const
            {
                return m_id >= rhs.m_id ? true : false;
            }

            ///////////////////////////////////////////////////////////////////////

            inline bool operator==(const Type &rhs) const
            {
                return m_id == rhs.m_id ? true : false;
            }

            ///////////////////////////////////////////////////////////////////////

            inline bool operator!=(const Type &rhs) const
            {
                return m_id != rhs.m_id ? true : false;
            }

            ///////////////////////////////////////////////////////////////////////

            inline bool operator==(const std::type_info &rhs) const
            {
                return GetHashCode() == rhs.hash_code() ? true : false;
            }

            ///////////////////////////////////////////////////////////////////////

            inline bool operator!=(const std::type_info &rhs) const
            {
                return GetHashCode() != rhs.hash_code() ? true : false;
            }

#pragma endregion Type Operators

            /** @brief Gets an instance of an invalid type.
             */
            static const Type &Invalid(void);

            ///////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////

            /** @brief Gets the internal id of the type.
             */
            TypeID const& GetID(void) const;

            ///////////////////////////////////////////////////////////////////

            ///** @brief Gets a type by deducing the type of an object.
            // *  @param obj Object to deduce type from.
            // */
            //template<typename T>
            //static Type Get(T &&obj);

            ///////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////

            /** @brief Compares two type lists.
             *  @param a First list
             *  @param b Second list
             *  @return true if "a" is identical to "b"
             */
            static bool ListsEqual(const List &a, const List &b);

            ///////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////

            /** @brief Determines if this type is valid.
             *  @return true if the internal id is not Type::INVALID
             */
            bool IsValid(void) const;

            /** @brief Determines if this type is primitive.
             *  @return true if the type is among (int, bool, char, etc).
             */
            bool IsPrimitive(void) const;

            /** @brief Determines if this type is a floating point
             *          float, double, long double, etc.
             *  @return true if the type is among (float, double, long double, etc).
             */
            bool IsFloatingPoint(void) const;

            /** @brief Determines if this type is signed (unsigned int, etc).
             *  @return true if the type is signed.
             */
            bool IsSigned(void) const;

            /** @brief Determines if this type is an enumeration.
             *  @return true if the type is either an enum or enum class.
             */
            bool IsEnum(void) const;

            /** @brief Determines if this type is a pointer.
             *  @return true if the type has any level of indirection. 
             *          ie - (int *), (int **), etc.
             */
            bool IsPointer(void) const;

            /** @brief Determines if this type is a class.
             *  @return true if the type is a class or struct.
             */
            bool IsClass(void) const;

            /** @brief Determines if this type is an array type.
             *  @return true if the type is an array type.
             */
            bool IsArray(void) const;

            bool IsStringType(void) const { return isString; }

            size_t SizeOf() const;

            size_t GetHashCode() const;

            CXVisibilityKind GetVisibility() const
            {
                return linkerVisibility;
            }

            ///////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////

            /** @brief Gets the human readable name for this type.
             *  @return Qualified name of the type as it is declared.
             *          ie - "boost::regex"
             */
            const char* GetFullName(void) const;
            const char* GetName(void) const;
            const char* GetNameSpace(void) const;

            /** @brief Gets meta data for this type.
             *  @return Meta Data Manager for this type.
             */
            const MetaManager &GetMeta(void) const;

            /** @brief Deconstructs the given object instance.
             *  @param instance Variant object instance to destruct.
             */
            void Destroy(Variant &instance) const;

            ///////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////

            /** @brief Gets the decayed version of this type.
             *  @return Type with no qualifiers like const, and no pointer.
             *          ie - const int * -> int
             */
            Type const& GetDecayedType(void) const;

            /** @brief Gets the enumeration representing this type, 
             *         assuming it's an enum type.
             *  @return Reference to the enum type in the reflection database.
             */
            const Enum* GetEnum(void) const;

            ///////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////

            /** @brief Determines if this type derives from the specified 
             *         runtime type.
             *  @param other Other class type.
             *  @return true if both types are class types and this type
             *          derives from "other".
             */
            bool DerivesFrom(const Type &other) const;

            bool IsAssignableFrom(const Type& currentType) const;

            template<typename TType>
            bool IsAssignableFrom() const
            {
                static const System::Reflection::Type& type = typeof(TType);
                return type.IsAssignableFrom(*this);
            }

            ///////////////////////////////////////////////////////////////////
            ///////////////////////////////////////////////////////////////////

            /** @brief Gets a constructor for this type.
             *  @param signature Signature of the constructor.
             *  @return Reference to the constructor with the given signature
             *          in the reflection database.
             */
            //const Constructor &GetConstructor(
            //    const ArgHashStorage & signature
            //) const;

            /** @brief Gets a dynamic constructor for this type with the 
             *         specified argument signature.
             *  @param signature Signature of the dynamic constructor.
             *  @return Reference to the dynamic constructor with the given 
             *          signature in the reflection database.
             */
            //const Constructor &GetDynamicConstructor(
            //    const ArgHashStorage & signature
            //) const;

            /** @brief Gets the constructor for this array type.
             *  @return Reference to the array constructor in the reflection database.
             */
            const Constructor &GetArrayConstructor(void) const;

            /** @brief Gets the destructor for this type assuming it's a 
             *         class type.
             *  @return Reference to the destructor in the reflection database.
             */
            const Destructor &GetDestructor(void) const;

            ///////////////////////////////////////////////////////////////////

            /** @brief Gets a method for this type.
             *  @param name Name of the method.
             *  @return Reference to the method in the reflection database.
             *     If the method doesn't exist, an invalid method.
             *     If the method exists, but has overloads, the first declared.
             */
            //const Method &GetMethod(const std::string &name) const;

            /** @brief Gets a method for this type.
             *  @param name Name of the method.
             *  @param signature Specific overload for this method.
             *  @return Reference to the method in the reflection database
             *          with the specified overload. If the specific overload
             *          doesn't exist, an invalid method is returned.
             */
            //const Method &GetMethod(
            //      const std::string &name, 
            //      const ArgHashStorage & signature
            //) const;

            ///////////////////////////////////////////////////////////////////

            /** @brief Gets a static method for this type.
             *  @param name Name of the static method.
             *  @return Reference to the method in the reflection database.
             *     If the method doesn't exist, an invalid function.
             *     If the method exists, but has overloads, the first declared.
             */
            //const Function &GetStaticMethod(const std::string &name) const;

            /** @brief Gets a static method for this type.
             *  @param name Name of the static method.
             *  @param signature Specific overload for this method.
             *  @return Reference to the method in the reflection database
             *          with the specified overload. If the specific overload
             *          doesn't exist, an invalid function is returned.
             */
            //const Function &GetStaticMethod(
            //      const std::string &name, 
            //      const ArgHashStorage & signature
            //) const;

            ///////////////////////////////////////////////////////////////////

            /** @brief Gets a specific field for this type.
             *  @param name Name of the field.
             *  @return Reference to the field in the reflection database.
             *          If the field doesn't exist, an invalid field.
             */
            //const Field &GetField(const std::string &name) const;

            ///////////////////////////////////////////////////////////////////

            /** @brief Gets a specific field for this type.
            *  @param name Name of the field.
            *  @return Reference to the field in the reflection database.
            *          If the field doesn't exist, an invalid field.
            */
            const Field &GetProperty(const std::string &name) const;

            ///////////////////////////////////////////////////////////////////

            const Field &GetRuntimeProperty(const std::string &name) const;
            const Field &GetRuntimeField(const std::string &name) const;
            const Method &GetRuntimeMethod(const std::string &name) const;
            const Method &GetRuntimeMethod(const std::string &name, const ArgHashStorage & signature) const;
            const Method &GetRuntimeMethod(const std::string &name, const InvokableSignature & signature) const;

            template<typename ...Args>
            const Method &GetRuntimeMethod(const std::string &name, Args && ...args) const
            {
                const InvokableSignature& signatureTypes = GetInvokableSignature(std::forward<Args>(args)...);
                return GetRuntimeMethod(name, signatureTypes);
            }

            //template<typename ClassType>
            //static Json SerializeJson(const ClassType &instance, bool invokeHook = true);

            json11::Json SerializeJson(const Variant &instance, bool invokeHook = true) const;
            json11::Json SerializeJson(const Variant &instance, SerializationGetterOverride getterOverride, bool invokeHook = true) const;

            //template<typename ClassType>
            //static ClassType DeserializeJson(const Json &value);

            Variant DeserializeJson(const json11::Json &value) const;
            Variant DeserializeJson(const json11::Json &value, const Constructor &ctor) const;
            void DeserializeJson(Variant &instance, const json11::Json &value) const;
            intptr_t GetPointerAdjustment(Object* instance) const;
            intptr_t GetToP(Object * instance) const;

            ReflectionDatabase* AssemblyDB() const { return m_reflectionDatabase; }
            ReflectionModule* Assembly() const;

            /// Templates

            inline bool VisitInheritClasses(std::function<bool(const Type*)> const& visitor) const
            {
                if (BaseClasses)
                    for (auto const& base : *BaseClasses)
                        if (!base->VisitInheritClasses(visitor))
                            return false;

                return visitor(this);
            }

            inline bool VisitDescendInheritClasses(std::function<bool(const Type*)> const& visitor) const
            {
                if (BaseClasses)
                {
                    for (auto const& base : *BaseClasses)
                        if (!visitor(base.m_type))
                            return false;
                    for (auto const& base : *BaseClasses)
                        if (!base->VisitDescendInheritClasses(visitor))
                            return false;
                }

                return true;
            }


        private:
            friend class std::allocator<Type>;

            friend struct TypeData;

            friend class Variant;
            friend class Argument;
            friend class Enum;
            friend class EnumBase;
            friend class Constructor;
            friend class Destructor;
            friend class Method;
            friend class Field;
            friend class Function;
            friend class Global;

            TypeID m_id;
            ReflectionDatabase* m_reflectionDatabase;
        public:
            Type const* ElaborateType;

            TypeData& GetTypeData() const;
        };
    }

    //typedef meta::Type Type;
}

namespace std
{
    template<>
    struct hash<System::Reflection::meta::Type>
    {
        size_t operator()(const System::Reflection::meta::Type &__type) const
        {
            return hash<System::Reflection::meta::TypeID>()(__type.GetID());
        }
    };
}

#include "Impl/Type.hpp"
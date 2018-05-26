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

#ifndef __SYSTEM_BUILDIN_ATTRIBUTES_H__
#define __SYSTEM_BUILDIN_ATTRIBUTES_H__

#include "Runtime/MetaProperty.h"
#include "Runtime/Type.h"

#include <typeindex>

#pragma region Built-In Parser Properties
namespace System
{
    //#if (!defined(__REFLECTION_PARSER__)) //  && defined(__INTELLISENSE__)

    /** @brief Enables introspection of this type.
     *         You can also enable a class type by inheriting from Reflection::meta::Object.
     */
    class Enable : public System::Reflection::meta::MetaProperty { };

    ///////////////////////////////////////////////////////////////////////////////

    /** @brief Only registers this type. Does not generate class information.
     */
    class Register : public System::Reflection::meta::MetaProperty { };

    ///////////////////////////////////////////////////////////////////////////////

    /** @brief Explicitly disables introspection of this type.
     */
    class Disable : public System::Reflection::meta::MetaProperty { };

    ///////////////////////////////////////////////////////////////////////////////

    /** @brief Requires all methods in a class to be whitelisted by default.
     */
    class WhiteListMethods : public System::Reflection::meta::MetaProperty { };

    ///////////////////////////////////////////////////////////////////////////////

    /** @brief Explicitly disables generation of non dynamic constructors.
     *         Only applies to class constructors.
     */
    class DisableNonDynamic : public System::Reflection::meta::MetaProperty { };

    ///////////////////////////////////////////////////////////////////////////////

    /** @brief Uses the "WrapObject" Variant policy in dynamic constructors.
     */
    class WrapObject : public System::Reflection::meta::MetaProperty { };

    ///////////////////////////////////////////////////////////////////////////////

    /** @brief Explicitly enables generation of pointers to class types.
     *         Applies to class types.
     */
    class EnablePtrType : public System::Reflection::meta::MetaProperty { };

    ///////////////////////////////////////////////////////////////////////////////

    /** @brief Explicitly enables generation of constant pointers to class types.
     *         Applies to class types.
     */
    class EnableConstPtrType : public System::Reflection::meta::MetaProperty { };

    ///////////////////////////////////////////////////////////////////////////////

    class AbstractType : public System::Reflection::meta::MetaProperty { };

    /** @brief Explicitly enables generation of the array type for this type.
     */
    class EnableArrayType : public System::Reflection::meta::MetaProperty { };

    class PropertyType : public System::Reflection::meta::MetaProperty { };

    /** @brief Define event hook type.
    */
    class PropertyEventType : public System::Reflection::meta::MetaProperty
    {
    public:
        const std::type_index m_type;

    public:
        PropertyEventType(std::type_index const& type) : m_type(type) {}

        std::type_index const& GetTypeIndex() const { return m_type; }
    };

    class ConstArrayType : public System::Reflection::meta::MetaProperty
    {
    public:
        size_t m_unArrayType;
        size_t m_size;

        System::Reflection::meta::Type const * GetUnArrayType() const
        {
            return &System::Reflection::type_of(m_unArrayType);
        }

        ConstArrayType() : m_unArrayType(0), m_size(0) {}
        ConstArrayType(size_t unArrayType, size_t size)
            : m_unArrayType(unArrayType)
            , m_size(size)
        {

        }
    };

    ///////////////////////////////////////////////////////////////////////////////

    class ReflInclude : public System::Reflection::meta::MetaProperty
    {
    public:
        /** @param getterName Name of the getter function.
        *         The name is relative to the context.
        *         For example, in a class the name is a public member function.
        *         In a Global or Static Field, it's a globally accessible function.
        */
        ReflInclude() {}
        ReflInclude(const char *include) {}
    };

    /** @brief Defines an explicit getter for this type.
     *         Applies to Globals, Fields and Static Fields.
     */
    class Getter : public System::Reflection::meta::MetaProperty
    {
    public:
        /** @param getterName Name of the getter function.
         *         The name is relative to the context.
         *         For example, in a class the name is a public member function.
         *         In a Global or Static Field, it's a globally accessible function.
         */
        Getter() {}
        Getter(const char *getterName) {}
    };

    ///////////////////////////////////////////////////////////////////////////////

    /** @brief Defines an explicit setter for this type.
     *         Applies to Globals, Fields and Static Fields.
     */
    class Setter : public System::Reflection::meta::MetaProperty
    {
    public:
        /** @param setterName Name of the setter function.
         *         The name is relative to the context.
         *         For example, in a class the name is a public member function.
         *         In a Global or Static Field, it's a globally accessible function.
         */
        Setter() {}
        Setter(const char *setterName) {}
    };

    /** @brief Defines an explicit getter for this type. Parent type is not assumed
     *         Applies to Globals, Fields and Static Fields.
     */
    class ExplicitGetter : public System::Reflection::meta::MetaProperty
    {
    public:
        /** @param getterName Name of the getter function.
         */
        ExplicitGetter() {}
        ExplicitGetter(const char *getterName) {}
    };

    ///////////////////////////////////////////////////////////////////////////////

    /** @brief Defines an explicit setter for this type. Parent type is not assumed
     *         Applies to Globals, Fields and Static Fields.
     */
    class ExplicitSetter : public System::Reflection::meta::MetaProperty
    {
    public:
        /** @param setterName Name of the setter function.
         */
        ExplicitSetter() {}
        ExplicitSetter(const char *setterName) {}
    };

    ///////////////////////////////////////////////////////////////////////////////

    /** @brief Overrides the display name of a type. Only affects GetX( ) Type functions.
     *         Applies to Enums, Classes, Fields, Functions, Globals and Methods
     */
    class DisplayName : public System::Reflection::meta::MetaProperty
    {
    public:
        /** @param displayName Name of the setter function.
         *         The name is relative to the context.
         *         For example, in a class the name is a public member function.
         *         In a Global or Static Field, it's a globally accessible function.
         */
        DisplayName() {}
        DisplayName(const char *displayName) {}
    };

    ///////////////////////////////////////////////////////////////////////////////

    //#endif // if (!defined(__REFLECTION_PARSER__) && defined(__INTELLISENSE__))
    namespace Reflection
    {
        struct AssemblyTitle : Reflection::meta::MetaProperty
        {
            AssemblyTitle() {}
            AssemblyTitle(std::string const& title) { }

            std::string Title;
        };

        struct AssemblyDescription : Reflection::meta::MetaProperty
        {
            AssemblyDescription() {}
            AssemblyDescription(std::string description) { }

            std::string Description;
        };

        struct AssemblyConfiguration : Reflection::meta::MetaProperty
        {
            AssemblyConfiguration() {}
            AssemblyConfiguration(std::string description) { }

            std::string Configuration;
        };

        struct AssemblyProduct : Reflection::meta::MetaProperty
        {
            AssemblyProduct() {}
            AssemblyProduct(std::string description) { }

            std::string Product;
        };

        struct AssemblyCopyright : Reflection::meta::MetaProperty
        {
            AssemblyCopyright() {}
            AssemblyCopyright(std::string description) { }

            std::string Copyright;
        };

        struct AssemblyTrademark : Reflection::meta::MetaProperty
        {
            AssemblyTrademark() {}
            AssemblyTrademark(std::string description) { }

            std::string Trademark;
        };

        struct Guid : Reflection::meta::MetaProperty
        {
            Guid() {}
            Guid(std::string description) { }

            std::string Value;
        };

        struct AssemblyVersion : Reflection::meta::MetaProperty
        {
            AssemblyVersion() {}
            AssemblyVersion(std::string description) { }

            std::string Version;
        };

        struct AssemblyFileVersion : Reflection::meta::MetaProperty
        {
            AssemblyFileVersion() {}
            AssemblyFileVersion(std::string description) { }

            std::string FileVersion;
        };
    }

#pragma endregion  
}

#endif // __SYSTEM_BUILDIN_ATTRIBUTES_H__

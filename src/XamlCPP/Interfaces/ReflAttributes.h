/*
 * Copyright (C) 2017-2018 Tauri JayD <https://www.>
 * Copyright (c) 2016 OmniGUI Platform
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

#ifndef __XAMLCPP_REFLECTION_ATTRIBUTE_EX_H__
#define __XAMLCPP_REFLECTION_ATTRIBUTE_EX_H__

#include "Reflection/Runtime/MetaProperty.h"

#include <string>
#include <typeindex>

namespace System::Reflection::Assembly
{
    class AssemblyModulXamlCPP;
}

namespace XamlCPP
{
    class NameAttribute : public System::Reflection::meta::MetaProperty
    {
    public:
        const char* m_name;

        NameAttribute() : m_name(0) {}
        NameAttribute(const char* name)
            : m_name(name)
        {}
    };

    class DependsOnAttribute : public System::Reflection::meta::MetaProperty
    {
        friend class System::Reflection::Assembly::AssemblyModulXamlCPP;
    private:
        std::string privatePropertyName;

    public:
        DependsOnAttribute() {}
        DependsOnAttribute(const std::string &propertyName)
        {
            this->privatePropertyName = propertyName;
        }

        std::string getPropertyName() const
        {
            return privatePropertyName;
        }
    };

    struct ContentAttribute : public System::Reflection::meta::MetaProperty
    {
    public:
        ContentAttribute() {}
    };

    struct EventAttribute : public System::Reflection::meta::MetaProperty
    {
    public:
        EventAttribute() {}
    };

    struct TypeConverterAttribute : public System::Reflection::meta::MetaProperty
    {
        const std::type_index m_type;
    public:
        TypeConverterAttribute(std::type_index const& type) : m_type(type) {}

        std::type_index const& GetConverterTypeIndex() const { return m_type; }
    };

    struct MapKeyPropertyAttribute : public System::Reflection::meta::MetaProperty
    {
        const std::string m_propertyName;
    public:
        MapKeyPropertyAttribute(std::string const& propertyName) : m_propertyName(propertyName) {}

        std::string const& GetName() const { return m_propertyName; }
    };
    /// <summary>
    /// Signifies that a binding can be assigned to a property.
    /// </summary>
    /// <remarks>
    /// Usually in markup, when a binding is set for a property that property will be bound. 
    /// Applying this attribute to a property indicates that the binding should be assigned to 
    /// the property rather than bound.
    /// </remarks>
    struct AssignBindingAttribute : public System::Reflection::meta::MetaProperty
    {
    public:
        AssignBindingAttribute() {}
    };

    class VisualContentAttribute : public System::Reflection::meta::MetaProperty
    {
    public:
        size_t m_contentType;

        VisualContentAttribute() : m_contentType(0) {}
        VisualContentAttribute(size_t contentType)
            : m_contentType(contentType)
        {}
    };

    class LogicalContentAttribute : public System::Reflection::meta::MetaProperty
    {
    public:
        size_t m_contentType;

        LogicalContentAttribute() : m_contentType(0) {}
        LogicalContentAttribute(size_t contentType)
            : m_contentType(contentType)
        {}
    };


    struct TemplatePartAttribute : public System::Reflection::meta::MetaProperty
    {
        const std::type_index m_type;
        const std::string m_name;
    public:
        TemplatePartAttribute(std::string name, std::type_index const& type) : m_type(type), m_name(name) {}

        std::type_index const& GetTypeIndex() const { return m_type; }
        std::string const& GetName() const { return m_name; }
    };
}

#endif //!__XAMLCPP_REFLECTION_ATTRIBUTE_EX_H__

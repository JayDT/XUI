/*
 * Copyright (C) 2017-2018 Tauri JayD <https://www.>
 * Copyright (c) 2014 Steven Kirk <The Avalonia Project>
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

#ifndef __DEPENDENCY_PROPERTY_MGR_H__
#define __DEPENDENCY_PROPERTY_MGR_H__

#include "standard/BasicPrimities.h"
#include "standard/Errors.h"
#include "Reflection/Runtime/Variant.h"
#include "Reflection/Runtime/Object.h"
#include "XamlCPP/Core/Dependencies/IDependencyObject.h"

#include <set>

namespace XamlCPP::Core::Dependency
{
    struct IPropertyObject;
}

namespace XUI::Core::Dependency
{
    class DependencyObject;

    /// <summary>
    /// Class representing the <see cref="UnsetValue"/>.
    /// </summary>
    struct Unset : System::Reflection::meta::Object
    {
        /// <summary>
        /// Returns the string representation of the <see cref="UnsetValue"/>.
        /// </summary>
        /// <returns>The string "(unset)".</returns>
        std::string ToString() const override { return "(unset)"; }
    };

    template<typename TProperty>
    typename std::shared_ptr<TProperty> InitializeStaticProperty(typename std::shared_ptr<TProperty> _property, void(*init)(TProperty*))
    {
        init(_property.get());
        return _property;
    }

    class TC_XUI_API DependencyPropertyMgr : public XamlCPP::Core::Dependency::IDependencyPropertyMgr
    {
        DependencyPropertyMgr();
        virtual ~DependencyPropertyMgr();

        void GetRegistered(std::vector<XamlCPP::Core::Dependency::IPropertyObject*>& registered, System::Reflection::meta::Type const* o);
        void GetAttached(std::vector<XamlCPP::Core::Dependency::IPropertyObject*>& registered, System::Reflection::meta::Type const* o);

    public:
        static System::Reflection::meta::Variant UnsetValue;
        static uint GeneratePropertyUID();
        static DependencyPropertyMgr* Instance();

        typedef std::unordered_map<uint, XamlCPP::Core::Dependency::IPropertyObject*> PropertyObjectMap;
        typedef google::dense_hash_map<System::Reflection::meta::Type const*, PropertyObjectMap> TypedPropertyObjectMap;
        typedef std::unordered_map<std::string, XamlCPP::Core::Dependency::IPropertyObject*> PropertyObjectNamedMap;
        typedef google::dense_hash_map<System::Reflection::meta::Type const*, PropertyObjectNamedMap> TypedPropertyObjectNamedMap;

        TypedPropertyObjectMap _registered;
        TypedPropertyObjectMap _registeredCache;
        TypedPropertyObjectNamedMap _inheritCache;
        TypedPropertyObjectMap _attached;

        struct LogNamedCategories
        {
            static constexpr const char* Property = "Property";
            static constexpr const char* Binding = "Binding";
            static constexpr const char* Visual = "Visual";
            static constexpr const char* Layout = "Layout";
            static constexpr const char* Control = "Control";
        };

        PropertyObjectNamedMap const& GetInheritRegistered(System::Reflection::meta::Type const* o);
        std::vector<XamlCPP::Core::Dependency::IPropertyObject*> GetRegistered(System::Reflection::meta::Type const* o) override;
        std::vector<XamlCPP::Core::Dependency::IPropertyObject*> GetAttached(System::Reflection::meta::Type const* o);
        std::vector<XamlCPP::Core::Dependency::IPropertyObject*> GetRegistered(DependencyObject* o);
        XamlCPP::Core::Dependency::IPropertyObject* FindRegistered(System::Reflection::meta::Type const* type, XamlCPP::Core::Dependency::IPropertyObject* property);
        XamlCPP::Core::Dependency::IPropertyObject* FindRegistered(System::Reflection::meta::Variant* o, XamlCPP::Core::Dependency::IPropertyObject* property);

        XamlCPP::Core::Dependency::IPropertyObject* FindRegistered(System::Reflection::meta::Type const* type, std::string const& name) override;

        XamlCPP::Core::Dependency::IPropertyObject* FindRegistered(DependencyObject* o, std::string const& name);

        std::set<std::string> GetImplementedTypes(System::Reflection::meta::Type const* type);

        bool IsRegistered(System::Reflection::meta::Type const* type, XamlCPP::Core::Dependency::IPropertyObject* property) override;
        bool IsRegistered(System::Reflection::meta::Object const* o, XamlCPP::Core::Dependency::IPropertyObject* property);

        void Register(System::Reflection::meta::Type const* type, std::shared_ptr<XamlCPP::Core::Dependency::IPropertyObject> const& property);

        System::Reflection::Variant const& GetUnSetValue() const override;
        System::Reflection::Variant const& GetNullValue() const override; // not implement yet
    };
}

#endif // !__DEPENDENCY_PROPERTY_MGR_H__

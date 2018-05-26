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

#include "DependencyPropertyMgr.h"

#include "Core/Dependencies/Models/DataProperty.h"
#include "Core/Media/MediaTypes.h"

#include "Core/Dependencies/DependencyObject.h"
#include "XamlCPP/Core/Dependencies/IPropertyObject.h"
#include "XUI/Platform/UIService.h"
#include "Reflection/Runtime/Type.h"

#include "standard/collection.h"
#include "cpplinq/linq.hpp"

/// https://github.com/WojciechMula/simd-string

using namespace XUI::Core::Dependency;
using namespace XamlCPP::Core::Dependency;

uint s_nextId = 0;
System::Reflection::meta::Variant XUI::Core::Dependency::DependencyPropertyMgr::UnsetValue;

DependencyPropertyMgr * XUI::Core::Dependency::DependencyPropertyMgr::Instance()
{
    static DependencyPropertyMgr _instance;
    return &_instance;
}

XUI::Core::Dependency::DependencyPropertyMgr::DependencyPropertyMgr()
{
    _registered.set_empty_key(nullptr);
    _registeredCache.set_empty_key(nullptr);
    _inheritCache.set_empty_key(nullptr);
    _attached.set_empty_key(nullptr);

    UnsetValue = System::Reflection::meta::Variant(new Unset, static_cast<System::Reflection::meta::variant_policy::Copy*>(nullptr));
    XUI::Platform::UIService::Instance()->Add<XamlCPP::Core::Dependency::IDependencyPropertyMgr>(static_cast<XamlCPP::Core::Dependency::IDependencyPropertyMgr*>(this));
}

XUI::Core::Dependency::DependencyPropertyMgr::~DependencyPropertyMgr()
{
    XUI::Platform::UIService::Instance()->Remove<XamlCPP::Core::Dependency::IDependencyPropertyMgr>();
}

void XUI::Core::Dependency::DependencyPropertyMgr::GetRegistered(std::vector<IPropertyObject*>& registered, System::Reflection::meta::Type const * type)
{
    auto inner = _registered.find(type);
    if (inner != _registered.end())
    {
        for (auto const& p : inner->second)
            registered.push_back(p.second);
    }
}

void XUI::Core::Dependency::DependencyPropertyMgr::GetAttached(std::vector<XamlCPP::Core::Dependency::IPropertyObject*>& registered, System::Reflection::meta::Type const* type)
{
    auto inner = _attached.find(type);
    if (inner != _attached.end())
    {
        for (auto const& p : inner->second)
            registered.push_back(p.second);
    }
}

uint XUI::Core::Dependency::DependencyPropertyMgr::GeneratePropertyUID()
{
    return ++s_nextId;
}

void BuildInheritPropertyCacheFrom(System::Reflection::meta::Type const* _type, DependencyPropertyMgr::TypedPropertyObjectMap const& _registered, XUI::Core::Dependency::DependencyPropertyMgr::PropertyObjectNamedMap& cache)
{
    auto inner = _registered.find(_type);
    if (inner != _registered.end())
    {
        for (auto const& p : inner->second)
            cache[p.second->GetName()] = p.second;
    }

    if (_type->GetBaseClasses())
    {
        for (auto const& base : *_type->GetBaseClasses())
            BuildInheritPropertyCacheFrom(base.m_type, _registered, cache);
    }
}

XUI::Core::Dependency::DependencyPropertyMgr::PropertyObjectNamedMap const & XUI::Core::Dependency::DependencyPropertyMgr::GetInheritRegistered(System::Reflection::meta::Type const * o)
{
    auto iCache = _inheritCache.find(o);
    if (iCache != _inheritCache.end())
        return iCache->second;

    XUI::Core::Dependency::DependencyPropertyMgr::PropertyObjectNamedMap& cache = _inheritCache[o];
    BuildInheritPropertyCacheFrom(o, _registered, cache);
    return cache;
}

std::vector<IPropertyObject*> XUI::Core::Dependency::DependencyPropertyMgr::GetRegistered(System::Reflection::meta::Type const* type)
{
    System::ThrowIfFailed<System::ArgumentNullException>(type != nullptr);

    std::vector<IPropertyObject*> _reg_objects;
    _reg_objects.reserve(50);
    type->VisitInheritClasses([&](System::Reflection::meta::Type const* type)
    {
        GetRegistered(_reg_objects, type);
        return true;
    });

    return std::move(_reg_objects);
}

std::vector<XamlCPP::Core::Dependency::IPropertyObject*> XUI::Core::Dependency::DependencyPropertyMgr::GetAttached(System::Reflection::meta::Type const* type)
{
    System::ThrowIfFailed<System::ArgumentNullException>(type != nullptr);

    std::vector<IPropertyObject*> _reg_objects;
    _reg_objects.reserve(50);
    type->VisitInheritClasses([&](System::Reflection::meta::Type const* type)
    {
        GetAttached(_reg_objects, type);
        return true;
    });

    return std::move(_reg_objects);
}

std::vector<IPropertyObject*> XUI::Core::Dependency::DependencyPropertyMgr::GetRegistered(DependencyObject* o)
{
    System::ThrowIfFailed<System::ArgumentNullException>(o != nullptr);

    return std::move(GetRegistered(o->GetType()));
}

IPropertyObject* XUI::Core::Dependency::DependencyPropertyMgr::FindRegistered(System::Reflection::meta::Type const* type, IPropertyObject* property)
{
    TypedPropertyObjectMap::iterator cache;
    IPropertyObject* result = nullptr;

    if (System::Collection::GetContains(_registeredCache, type, cache))
    {
        if (System::Collection::GetValue(cache->second, property->Id, result))
        {
            return result;
        }
    }

    // Build Cache
    type->VisitInheritClasses([&](System::Reflection::meta::Type const* basetype)
    {
        TypedPropertyObjectMap::iterator inner;
        if (System::Collection::GetContains(_registered, basetype, inner))
        {
            if (System::Collection::GetValue(inner->second, property->Id, result))
            {
                _registeredCache[type][property->Id] = result;
            }
        }

        return true;
    });

    type->VisitInheritClasses([&](System::Reflection::meta::Type const* basetype)
    {
        TypedPropertyObjectMap::iterator inner;
        if (System::Collection::GetContains(_attached, basetype, inner))
        {
            if (System::Collection::GetValue(inner->second, property->Id, result))
            {
                _registeredCache[type][property->Id] = result;
            }
        }

        return true;
    });

    return result;
}

IPropertyObject * XUI::Core::Dependency::DependencyPropertyMgr::FindRegistered(System::Reflection::meta::Variant * o, IPropertyObject * property)
{
    return FindRegistered(o->GetType(), property);
}

IPropertyObject* FindPropertyFrom(System::Reflection::meta::Type const* _type, DependencyPropertyMgr::TypedPropertyObjectMap const& _registered, std::string const& name)
{
    IPropertyObject* propertyObj = nullptr;

    auto inner = _registered.find(_type);
    if (inner != _registered.end())
    {
        for (auto const& p : inner->second)
            if (name.size() == p.second->GetName().size() && memcmp(p.second->GetName().c_str(), name.c_str(), name.size()) == 0)
            {
                propertyObj = p.second;
                break;
            }
    }

    if (!propertyObj && _type->GetBaseClasses())
    {
        for (auto const& base : *_type->GetBaseClasses())
        {
            propertyObj = FindPropertyFrom(base.m_type, _registered, name);
            if (propertyObj)
                return propertyObj;
        }
    }

    return propertyObj;
}

IPropertyObject* XUI::Core::Dependency::DependencyPropertyMgr::FindRegistered(System::Reflection::meta::Type const* type, std::string const& name)
{
    System::ThrowIfFailed<System::ArgumentNullException>(type != nullptr);
    System::ThrowIfFailed<System::ArgumentNullException>(!name.empty());


    auto const& cache = GetInheritRegistered(type);
    auto iProperty = cache.find(name);
    if (iProperty != cache.end())
        return iProperty->second;

    //IPropertyObject* propertyObj = FindPropertyFrom(type, _registered, name);

    //type->VisitInheritClasses([&name, &propertyObj, this](System::Reflection::meta::Type const* _type)
    //{
    //    if (!_type->IsValid())
    //        return true;
    //
    //    auto inner = _registered.find(_type);
    //    if (inner != _registered.end())
    //    {
    //        for (auto const& p : inner->second)
    //            if (name.size() == p.second->GetName().size() && memcmp(p.second->GetName().c_str(), name.c_str(), name.size()) == 0)
    //            {
    //                propertyObj = p.second;
    //                break;
    //            }
    //    }
    //    return !propertyObj;
    //});

    //if (!propertyObj)
    {
        return FindPropertyFrom(type, _attached, name);

        //type->VisitInheritClasses([&name, &propertyObj, this](System::Reflection::meta::Type const* _type)
        //{
        //    if (!_type->IsValid())
        //        return true;
        //
        //    auto inner = _attached.find(_type);
        //    if (inner != _attached.end())
        //    {
        //        for (auto const& p : inner->second)
        //            if (name.size() == p.second->GetName().size() && memcmp(p.second->GetName().c_str(), name.c_str(), name.size()) == 0)
        //            {
        //                propertyObj = p.second;
        //                break;
        //            }
        //    }
        //    return !propertyObj;
        //});
    }

    //return propertyObj;
}

IPropertyObject* XUI::Core::Dependency::DependencyPropertyMgr::FindRegistered(DependencyObject* o, std::string const& name)
{
    return FindRegistered(o->GetType(), name);
}

std::set<std::string> XUI::Core::Dependency::DependencyPropertyMgr::GetImplementedTypes(System::Reflection::meta::Type const* type)
{
    std::set<std::string> type_names;
    type->VisitInheritClasses([&](System::Reflection::meta::Type const* type)
    {
        type_names.insert(type->GetFullName());
        return true;
    });
    return type_names;
}

bool XUI::Core::Dependency::DependencyPropertyMgr::IsRegistered(System::Reflection::meta::Type const* type, IPropertyObject* property)
{
    return FindRegistered(type, property) != nullptr;
}

bool XUI::Core::Dependency::DependencyPropertyMgr::IsRegistered(System::Reflection::meta::Object const* o, IPropertyObject* property)
{
    return IsRegistered(o->GetType(), property);
}

void XUI::Core::Dependency::DependencyPropertyMgr::Register(System::Reflection::meta::Type const* type, std::shared_ptr<IPropertyObject> const& property)
{
    System::ThrowIfFailed<System::ArgumentNullException>(type != nullptr);
    System::ThrowIfFailed<System::ArgumentNullException>(property != nullptr);

    _registered[type][property->Id] = property.get();
    if (property->IsAttached)
        _attached[property->OwnerType][property->Id] = property.get();

    _registeredCache.clear();
}

System::Reflection::Variant const & XUI::Core::Dependency::DependencyPropertyMgr::GetUnSetValue() const
{
    return DependencyPropertyMgr::UnsetValue;
}

System::Reflection::Variant const & XUI::Core::Dependency::DependencyPropertyMgr::GetNullValue() const
{
    return DependencyPropertyMgr::UnsetValue;
}

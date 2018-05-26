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

#include "Cursor.h"

#include "standard/Platform/Define.h"
#include "Wrappers/Mustache.h"

class Class;
class ReflectionParser;

class MetaDataManager
{
    typedef std::pair<std::string, std::string> Property;
    typedef std::unordered_map<std::string, std::string> PropertyMap;
    typedef std::unordered_multimap<std::string, std::string> PropertyMultiMap;
    typedef std::unordered_multimap<CXCursorKind, Property> AttributeMap;
    typedef std::unordered_map<std::string, PropertyMultiMap> NamespacedPropertyMap;
    typedef std::pair<AttributeMap::const_iterator, AttributeMap::const_iterator> RangeAttributeMap;
    typedef std::unordered_multimap<CXCursorKind, Cursor> SpecificationMap;
    typedef std::pair<SpecificationMap::const_iterator, SpecificationMap::const_iterator> RangeSpecMap;

    friend class ReflectionParser;

public:
    MetaDataManager() {}
    //MetaDataManager(const Cursor &cursor);

    void Parse(const Cursor &cursor);

    std::string GetProperty(const std::string &key) const;
    bool GetFlag(const std::string &key) const;

    std::string GetNativeString(const std::string &key) const;

    RangeAttributeMap GetAttribute(CXCursorKind) const;

    RangeSpecMap GetSpecCursor(CXCursorKind) const;

    PropertyMultiMap const* GetNsAttributes(std::string const& ns) const
    {
        auto iAttr = m_namespacedProperties.find(ns);
        if (iAttr != m_namespacedProperties.end())
            return &iAttr->second;
        return nullptr;
    }

    void AddProperty(std::string const& type, std::string const& arguments);

    void CompileTemplateData(
        Mustache::Data<std::string> &data, 
        const ReflectionParser *context,
        std::string const& ns = ""
    ) const;

private:
    PropertyMap m_properties;
    NamespacedPropertyMap m_namespacedProperties;
    AttributeMap m_attributes;
    SpecificationMap m_specifications;

    std::vector<Property> extractProperties(const Cursor &cursor, std::string& attrNs) const;
};
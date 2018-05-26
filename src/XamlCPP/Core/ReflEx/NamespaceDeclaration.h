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

#pragma once
#ifndef __XAMLCPP_NamespaceDeclaration_H__
#define __XAMLCPP_NamespaceDeclaration_H__

#include <System/System.h>
#include "XamlType.h"

namespace XamlCPP::Core::ReflEx
{
    class NamespaceDeclaration
    {
        std::string ns;
        std::string prefix;

    public:
        NamespaceDeclaration(std::string const& ns, std::string const& prefix)
            : ns(ns)
            , prefix(prefix)
        {
        }

        void SetNamespace(std::string const& ns)
        {
            this->ns = ns;
        }

        std::string const& GetNamespace() const
        {
            return ns;
        }

        void SetPrefix(std::string const& prefix)
        {
            this->prefix = prefix;
        }

        std::string const& GetPrefix() const
        {
            return prefix;
        }

        int GetHashCode()
        {
            std::hash<std::string> hasher;
            return (hasher(ns) * 397) ^ (hasher(prefix));
        }

        std::string ToString() const
        {
            std::string finalPrefix;
            if (prefix.empty())
            {
                finalPrefix = "{Default}";
            }
            else
            {
                finalPrefix = prefix;
            }

            return finalPrefix + " => " + ns;
        }

        bool operator==(NamespaceDeclaration const& rhs)
        {
            return ns == rhs.ns && prefix == rhs.prefix;
        }
    };
}

#endif // !__XAMLCPP_NamespaceDeclaration_H__

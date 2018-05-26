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
#ifndef __XAMLCPP_CLRNAMESPACE_H__
#define __XAMLCPP_CLRNAMESPACE_H__

#include <System/System.h>
#include "Interfaces/INamespaceRegistry.h"

namespace XamlCPP::Core::Proto
{
    class PrefixRegistration;
    class XamlNamespace;
};


namespace XamlCPP::Core::ReflEx
{
    class ClrNamespace : public Interfaces::Namespace
    {
    private:
        System::Reflection::meta::ReflectionModule *const assembly;
        const std::string ns;
        std::string _name;
    public:
        virtual ~ClrNamespace()
        {
        }

        ClrNamespace(System::Reflection::meta::ReflectionModule * assembly, const std::string &ns);

        System::Reflection::meta::ReflectionModule *getAssembly() const;

        std::string getNamespace() const;

        System::Reflection::Type const* Get(const std::string &typeName) override;

        int GetHashCode() ;

        // Inherited via Namespace
        PROPERTY_T(std::string, Name)
        virtual PROPERTY_T_GET(Name, ) { return _name; }
        virtual PROPERTY_T_SET(Name) { _name = value; }
    };
}
#endif //!__XAMLCPP_CLRNAMESPACE_H__
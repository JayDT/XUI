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

#include "Reflection/Runtime/MetaProperty.h"

namespace XamlCPP
{
    class ParseException : public std::exception
    {
    private:
        std::string msg;

    public:
        ParseException(const std::string& message = "") : msg(message)
        {
        }

        const char * what() const throw()
        {
            return msg.c_str();
        }
    };

    class TypeNotFoundException : public std::exception
    {
    private:
        std::string msg;

    public:
        TypeNotFoundException(const std::string& message = "") : msg(message)
        {
        }

        const char * what() const throw()
        {
            return msg.c_str();
        }
    };

    //TC_XAMLCPP_API
    class XmlnsDefinition : public System::Reflection::meta::MetaProperty
    {
    public:
        /// <summary>
        /// Gets or sets the URL of the XML namespace.
        /// </summary>
        std::string XmlNamespace;

        /// <summary>
        /// Gets or sets the CLR namespace.
        /// </summary>
        std::string ClrNamespace;

        XmlnsDefinition() {}
        XmlnsDefinition(std::string const& xmlNamespace, std::string const& clrNamespace)
            : XmlNamespace(xmlNamespace)
            , ClrNamespace(clrNamespace)
        {}
    };
}
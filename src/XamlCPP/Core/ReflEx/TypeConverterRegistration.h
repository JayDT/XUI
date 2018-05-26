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
#ifndef __XAMLCPP_TYPECONVERTER_REGISTRATION_H__
#define __XAMLCPP_TYPECONVERTER_REGISTRATION_H__

#include <System/System.h>
#include "XamlCPP/Interfaces/ITypeConverter.h"

namespace XamlCPP::Core
{
    class TypeConverterRegistration
    {
    private:
        const System::Reflection::Type *const type;
        std::shared_ptr<Interfaces::ITypeConverter> typeConverter;

    public:

        std::size_t Typehash = 0;

        ~TypeConverterRegistration()
        {
        }

        TypeConverterRegistration(const System::Reflection::Type *type, std::shared_ptr<Interfaces::ITypeConverter> const& typeConverter, std::size_t typehash = 0)
            : type(type), typeConverter(typeConverter)
        {
            if (type)
            {
                typehash = type->GetHashCode();
            }

            Typehash = typehash;
        }

        const System::Reflection::Type *getTargetType() const { return type; }

        std::shared_ptr<Interfaces::ITypeConverter> const& getTypeConverter() const { return typeConverter; }
    };

}
#endif //!__XAMLCPP_TYPECONVERTER_REGISTRATION_H__
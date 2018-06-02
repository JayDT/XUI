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

#ifndef __XAMLCPP_TYPEEXTENSION_H__
#define __XAMLCPP_TYPEEXTENSION_H__

#include <System/System.h>
#include <standard/misc.h>
#include "Interfaces/IMarkupExtension.h"
#include "Interfaces/IValueConverter.h"
#include "Interfaces/ITypeRepository.h"

namespace XamlCPP::Markup
{
    class TypeExtension : public Interfaces::IMarkupExtension
    {
    private:
        const System::Reflection::Type* ResolveFromString(const std::string &type, Interfaces::ITypeRepository *typeRepository);

    public:
        PROPERTY_AUTO_QT(const System::Reflection::Type*, TypeType)
        PROPERTY_AUTO_QT(std::string, TypeName)

        TypeExtension();

        TypeExtension(const System::Reflection::Type* type);

        System::Reflection::Variant ProvideValue(Core::MarkupExtensionContext *markupExtensionContext) override;
    };
}


#endif	//#ifndef __XAMLCPP_TYPEEXTENSION_H__

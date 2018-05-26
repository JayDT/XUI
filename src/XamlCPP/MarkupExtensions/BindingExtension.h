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

#ifndef __XAMLCPP_BINDINGEXTENSION_H__
#define __XAMLCPP_BINDINGEXTENSION_H__

#include <System/System.h>
#include <standard/misc.h>
#include "Interfaces/IBinding.h"
#include "Interfaces/IMarkupExtension.h"
#include "Interfaces/IValueConverter.h"
#include "XamlCPP/DataBind/RelativeSource.h"

namespace XamlCPP::Markup
{
    struct TC_XAMLCPP_API BindingExtension : Interfaces::IMarkupExtension
    {
        BindingExtension();

        explicit BindingExtension(const std::string &path);

		virtual ~BindingExtension() {}

        PROPERTY_AUTO_QT(Interfaces::IValueConverter*, Converter)
        PROPERTY_AUTO_QT(System::Reflection::Variant, ConverterParameter)
        PROPERTY_AUTO_QT(std::string, ElementName)
        PROPERTY_AUTO_QT(System::Reflection::Variant, FallbackValue)
        PROPERTY_AUTO_QT(BindingMode, Mode)
        PROPERTY_AUTO_QT(std::string, Path)
        PROPERTY_AUTO_QT(BindingLevel, Priority)
        PROPERTY_AUTO_QT(System::Reflection::RefObject, Source)
        PROPERTY_AUTO_QT(std::shared_ptr<Markup::RelativeSource>, RelativeSource)

        // Inherited via IMarkupExtension
        System::Reflection::Variant ProvideValue(Core::MarkupExtensionContext * markupExtensionContext) override;
    };
}


#endif	//#ifndef __XAMLCPP_BINDINGEXTENSION_H__

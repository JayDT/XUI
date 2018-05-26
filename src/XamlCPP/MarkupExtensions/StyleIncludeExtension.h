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

#ifndef __XAMLCPP_STYLEINCLUDEEXTENSION_H__
#define __XAMLCPP_STYLEINCLUDEEXTENSION_H__

#include <System/System.h>
#include <standard/misc.h>
#include "Interfaces/IMarkupExtension.h"
#include "Interfaces/IValueConverter.h"
#include "Interfaces/IBinding.h"
#include "Interfaces/IResourceMap.h"

namespace XamlCPP::Markup
{
    // TODO: implement this
    struct StyleIncludeExtension : public Interfaces::IMarkupExtension
    {
        StyleIncludeExtension()
        {
        }

        System::Reflection::Variant ProvideValue(Core::MarkupExtensionContext * markupExtensionContext) override
        {
            return System::Reflection::Variant::Empty;
        }

        PROPERTY_AUTO_QT(std::string, Source)
    };
}


#endif	//#ifndef __XAMLCPP_STYLEINCLUDEEXTENSION_H__

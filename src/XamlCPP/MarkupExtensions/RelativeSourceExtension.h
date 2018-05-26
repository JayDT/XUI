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

#ifndef __XAMLCPP_RELATIVESOURCEEXTENSION_H__
#define __XAMLCPP_RELATIVESOURCEEXTENSION_H__

#include <System/System.h>
#include <standard/misc.h>
#include "Interfaces/IMarkupExtension.h"
#include "Interfaces/IValueConverter.h"

namespace XamlCPP::Markup
{
    class TC_XAMLCPP_API RelativeSourceExtension : public Interfaces::IMarkupExtension
    {
    public:
        RelativeSourceExtension();

        RelativeSourceExtension(RelativeSourceMode mode);

		virtual ~RelativeSourceExtension() {}

        PROPERTY_AUTO_QT(RelativeSourceMode, Mode)
        PROPERTY_AUTO_QT(const System::Reflection::Type*, AncestorType)
        PROPERTY_AUTO_QT(int, AncestorLevel)

        // Inherited via IMarkupExtension
        System::Reflection::Variant ProvideValue(Core::MarkupExtensionContext * markupExtensionContext) override;
    };
}


#endif	//#ifndef __XAMLCPP_RELATIVESOURCEEXTENSION_H__

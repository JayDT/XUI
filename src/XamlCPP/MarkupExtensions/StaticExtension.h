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

#ifndef __XAMLCPP_STATICEXTENSION_H__
#define __XAMLCPP_STATICEXTENSION_H__

#include <System/System.h>
#include <standard/misc.h>
#include "Interfaces/IMarkupExtension.h"
#include "Interfaces/IValueConverter.h"

namespace XamlCPP::Markup
{
    class StaticExtension : public Interfaces::IMarkupExtension
    {
    public:
        StaticExtension();

        StaticExtension(const std::string &identifier);

        PROPERTY_AUTO_QT(std::string, Identifier)

        // Inherited via IMarkupExtension
        System::Reflection::Variant ProvideValue(Core::MarkupExtensionContext * markupExtensionContext) override;

    private:
        static std::tuple<std::string, std::string> GetTypeAndMember(const std::string &s);

        static std::tuple<std::string, std::string> GetPrefixAndType(const std::string &s);

        System::Reflection::Variant GetValue(const System::Reflection::Type* type, const std::string &name);
    };
}


#endif	//#ifndef __XAMLCPP_STATICEXTENSION_H__

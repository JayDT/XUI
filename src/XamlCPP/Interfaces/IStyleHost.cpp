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

#include "StdAfx.h"
#include "IStyleHost.h"
#include "IStyle.h"

/// <summary>
/// Tries to find a named resource within the style.
/// </summary>
/// <param name="name">The resource name.</param>
/// <returns>
/// The resource if found, otherwise <see cref="AvaloniaProperty.UnsetValue"/>.
/// </returns>
System::Reflection::Variant FindResource(XamlCPP::Interfaces::StylesList& styles, std::string const& name)
{
    for(auto iStyle = styles.GetNativeEnumerator().rbegin(); iStyle != styles.GetNativeEnumerator().rend(); ++iStyle)
    {
        auto result = (*iStyle)->FindResource(name);

        //if (result)
        {
            return result;
        }
    }

    return System::Reflection::Variant::Empty;
}

System::Reflection::Variant XamlCPP::Interfaces::IStyleHost::FindStyleResource(std::string name)
{
    System::ThrowIfFailed<System::ArgumentException>(!name.empty());

    XamlCPP::Interfaces::IStyleHost* control = this;
    while (control)
    {
        auto result = FindResource(control->GetStyles(), name);
        if (result.GetBase())
            return result;

        control = control->GetStylingParent();
    }

    return { };
}

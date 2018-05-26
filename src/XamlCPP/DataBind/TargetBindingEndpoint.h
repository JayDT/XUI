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

#ifndef __XAMLCPP_TARGETBINDINGENDPOINT_H__
#define __XAMLCPP_TARGETBINDINGENDPOINT_H__

#include <System/System.h>
#include <standard/misc.h>
#include "Interfaces/IMarkupExtension.h"
#include "Interfaces/IValueConverter.h"
#include "XamlCPP/Core/Dependencies/IDependencyObject.h"

namespace XamlCPP::Markup
{
    struct TargetBindingEndpoint : public System::Reflection::Object
    {
        std::shared_ptr<Core::Dependency::IDependencyObject> Object;
        Core::Dependency::IPropertyObject* Property = nullptr;

        TargetBindingEndpoint(Core::Dependency::IDependencyObject *obj, Core::Dependency::IPropertyObject* _property)
        {
            Object = obj->shared_from_this();
            Property = _property;
        }
    };
}


#endif	//#ifndef __XAMLCPP_TARGETBINDINGENDPOINT_H__

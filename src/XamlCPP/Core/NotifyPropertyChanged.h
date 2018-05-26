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

#ifndef __NOTIFY_PROPERTY_CHANGED_H__
#define __NOTIFY_PROPERTY_CHANGED_H__

#include <standard/Platform/Define.h>
#include "standard/events.h"
#include <rxcpp/rx.hpp>
#include <System/BuildInMetaAttributes.h>

namespace System::Reflection
{
    inline namespace meta
    {
        class Variant;
    }
}

namespace XamlCPP::Core
{
    struct PropertyChangedEventArgs : System::Events::EventArg
    {
        PropertyChangedEventArgs(const char* propertyName)
            : PropertyName(propertyName) {}

        const char* PropertyName;
    };

    struct TC_XAMLCPP_API NotifyPropertyChanged : public System::Reflection::meta::Object
    {
        //explicit NotifyPropertyChanged() {}
        virtual ~NotifyPropertyChanged();

        System::Events::EventHandler<PropertyChangedEventArgs> PropertyChanged;

        void RaisePropertyChanged(const char* propName);

        std::shared_ptr<rxcpp::observable<System::Reflection::meta::Variant>> GetObservable(std::string const& propName);
    };
};
#endif // !__NOTIFY_PROPERTY_CHANGED_H__

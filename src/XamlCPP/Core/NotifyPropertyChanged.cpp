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
#include "NotifyPropertyChanged.h"
#include "Log/Log.h"

#include "Reflection/Runtime/CXXReflection.h"

XamlCPP::Core::NotifyPropertyChanged::~NotifyPropertyChanged()
{
}

void XamlCPP::Core::NotifyPropertyChanged::RaisePropertyChanged(const char * propName)
{
    if (PropertyChanged)
    {
        PropertyChangedEventArgs e(propName);
        PropertyChanged(this, e);
    }
}

std::shared_ptr<rxcpp::observable<System::Reflection::meta::Variant>> XamlCPP::Core::NotifyPropertyChanged::GetObservable(std::string const & propName)
{
    std::string propertyName = propName;
    System::Reflection::meta::Object* instance = dynamic_cast<System::Reflection::meta::Object*>(this); // @ToDo:: this is unsafe
    System::ThrowIfFailed<System::NotSupportedException>(instance, "Required Reflection Object");

    System::Reflection::meta::Type const* instanceType = &typeinstanceof(this);
    XamlCPP::Core::NotifyPropertyChanged* notifier = this;
    //void* instanceTopLevel = reinterpret_cast<void*>(instanceType->GetToP(instance));

    return std::make_shared<rxcpp::observable<System::Reflection::meta::Variant>>(rxcpp::dynamic_observable<System::Reflection::meta::Variant>(
        [&, instance, notifier, propertyName, instanceType](rxcpp::subscriber<System::Reflection::meta::Variant> const& observer)
    {
        System::Reflection::meta::Field const* prop = &instanceType->GetRuntimeProperty(propertyName);
        if (!prop->IsValid())
        {
            System::Console()->WriteErrorLine("invalid binding ToDo: improve this error message");
            return;
        }

        System::Reflection::meta::Variant _instance(instance);
        auto value = prop->GetInternalValue(_instance);
        observer.on_next(value);

        auto handler = PropertyChanged += [_instance, prop, propertyName, lobserver = observer](void* sender, PropertyChangedEventArgs const& e)
        {
            if (propertyName == e.PropertyName)
            {
                auto value = prop->GetInternalValue(_instance);
                lobserver.on_next(value);
            }
        };

        observer.add(rxcpp::make_subscription([lhandler = handler, notifier]()
        {
            notifier->PropertyChanged -= lhandler;
        }));
    }
    ));
}

/*
 * Copyright (C) 2017-2018 Tauri JayD <https://www.>
 * Copyright (c) 2014 Steven Kirk <The Avalonia Project>
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

#include "rxcpp/rx.hpp"

#include "Interfaces/IDescription.h"
#include "Core/Dependencies/BindValue.h"
#include "Core/Dependencies/DependencyPropertyMgr.h"
#include "Core/Dependencies/DependencyObservable.h"
#include "Core/Dependencies/DependencyObject.h"
#include "XamlCPP/DataBind/BindingNotification.h"
#include "XamlCPP/Core/Dependencies/IPropertyObject.h"
#include "Core/Observer/IObserver.h"
#include "XamlCPP/Core/TypeExtensions/TypeConverter.h"
#include "XamlCPP/Core/LogInterface.h"
#include "Reflection/Runtime/CXXReflection.h"
#include "BindValueEntry.h"
#include "BindValueLevel.h"

using namespace XUI::Core;

void OnError(std::exception_ptr)
{

}

XUI::Core::Dependency::BindValueEntry::BindValueEntry(BindValueLevel* owner, int index)
{
    _owner = owner;
    _index = index;
}

XUI::Core::Dependency::BindValueEntry::~BindValueEntry()
{
    if (_subscription.is_subscribed())
        _subscription.unsubscribe();
}

void XUI::Core::Dependency::BindValueEntry::Start(XamlCPP::Core::Dependency::GenericObservable* binding)
{
    System::ThrowIfFailed<System::ArgumentNullException>(binding != nullptr);
    System::ThrowIfFailed<std::exception>(binding != nullptr, "PriorityValue.Entry.Start() called more than once.");

    Observable = binding;
    _value = DependencyPropertyMgr::UnsetValue;

    //if (auto iDesc = dynamic_cast<XUI::Interfaces::IDescription*>(binding))
    //{
    //    Description = iDesc->Description();
    //}

    _subscription.move(std::move(binding->subscribe(
        std::bind(&BindValueEntry::ValueChanged, this, std::placeholders::_1),
        OnError,
        std::bind(&BindValueEntry::Completed, this))));
}

void XUI::Core::Dependency::BindValueEntry::Start(XamlCPP::Core::Dependency::BindingSubject * binding)
{
    _subject = binding->shared_from_this();
    Start(&_subject->Subject.get_observable());
}

void XUI::Core::Dependency::BindValueEntry::ValueChanged(System::Reflection::meta::Variant const& value)
{
    static auto const& bindingNotificationType = typeof(XamlCPP::DataBind::BindingNotification);

    if (bindingNotificationType.IsAssignableFrom(*value.GetType()))
    {
        auto notification = value.GetValue<XamlCPP::DataBind::BindingNotification*>();

        if (notification->HasValue())
        {
            _value = notification->Value;
            _owner->Changed(this);
        }

        if (notification->ErrorType != BindingErrorType::None)
        {
            _owner->LevelError(notification);
        }
    }
    else
    {
        _value = value;
        _owner->Changed(this);
    }
}

void XUI::Core::Dependency::BindValueEntry::Completed()
{
    _owner->Completed(this);
}

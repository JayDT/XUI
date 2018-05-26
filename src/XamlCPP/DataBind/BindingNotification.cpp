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

#include "BindingNotification.h"

XamlCPP::DataBind::BindingNotification::BindingNotification()
    : _value()
    , Error(nullptr)
    , ErrorType(BindingErrorType::None) 
{
}

XamlCPP::DataBind::BindingNotification::BindingNotification(System::Reflection::meta::Variant const & value)
    : _value(value)
    , Error(nullptr)
    , ErrorType(BindingErrorType::None)
{
}

XamlCPP::DataBind::BindingNotification::BindingNotification(std::exception const * error, BindingErrorType errorType)
{
    if (errorType == BindingErrorType::None)
    {
        throw System::ArgumentException("'errorType' may not be None");
    }

    Error = error;
    ErrorType = errorType;
}

XamlCPP::DataBind::BindingNotification::BindingNotification(std::exception const & error, BindingErrorType errorType)
{
    if (errorType == BindingErrorType::None)
    {
        throw System::ArgumentException("'errorType' may not be None");
    }

    _error = std::make_shared<std::exception>(error);
    Error = _error.get();
    ErrorType = errorType;
}

XamlCPP::DataBind::BindingNotification::BindingNotification(std::exception const * error, BindingErrorType errorType, System::Reflection::meta::Variant const & fallbackValue)
    : BindingNotification(error, errorType)
{
    _value = fallbackValue;
}

bool XamlCPP::DataBind::BindingNotification::HasValue() const
{
    return _value.GetBase();
}

System::Reflection::meta::Variant XamlCPP::DataBind::BindingNotification::GetValue() const
{
    return _value;
    //if (_value)
    //{
    //    object result;
    //
    //    if (_value.TryGetTarget(out result))
    //    {
    //        return result == NullValue ? null : result;
    //    }
    //}
    //
    //// There's the possibility of a race condition in that HasValue can return true,
    //// and then the value is GC'd before Value is read. We should be ok though as
    //// we return UnsetValue which should be a safe alternative.
    //return DependencyPropertyMgr::UnsetValue;
}

void XamlCPP::DataBind::BindingNotification::AddError(std::exception const * e, BindingErrorType type)
{
    System::ThrowIfFailed<System::ArgumentNullException>(e != nullptr);
    System::ThrowIfFailed<System::ArgumentException>(type != BindingErrorType::None);

    Error = Error != nullptr ? new System::AggregateException("Error, e") : e;

    if (type == BindingErrorType::Error || ErrorType == BindingErrorType::Error)
    {
        ErrorType = BindingErrorType::Error;
    }
}

void XamlCPP::DataBind::BindingNotification::ClearValue()
{
    _value = System::Reflection::meta::Variant();
}

void XamlCPP::DataBind::BindingNotification::SetValue(System::Reflection::meta::Variant const & value)
{
    _value = value;
}

std::string XamlCPP::DataBind::BindingNotification::ToString() const
{
    switch (ErrorType)
    {
        case BindingErrorType::None:
            return "{{Value: {Value}}}";
        default:
            return HasValue() ?
                "{{{ErrorType}: {Error}, Fallback: {Value}}}" :
                "{{{ErrorType}: {Error}}}";
    }
}

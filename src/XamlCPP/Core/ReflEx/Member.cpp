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

#include "Member.h"

XamlCPP::Core::Proto::XamlType * XamlCPP::Core::ReflEx::Member::LookupType()
{
    auto underlyingType = DeclaringType->UnderlyingType();
    auto property = &underlyingType->GetRuntimeProperty(Name);
    if (!property->IsValid()) // for event handlers
    {
        property = &underlyingType->GetRuntimeField(Name);
        IsField = property->IsValid();
    }

    System::ThrowIfFailed<System::AggregateException>(property->IsValid(), "Cannot find a property named \"{Name}\" in the type {underlyingType}");

    // Small hack for ref pointer (need rework std_shared type)
    IsRefPointer = property->GetFieldTypeHashCode() != property->GetFieldType()->GetHashCode();

    auto xamlType = TypeRepository->GetByType(property->GetFieldType());
    xamlType->IsRefPointer = IsRefPointer;
    return xamlType;
}

System::Reflection::meta::Field const * XamlCPP::Core::ReflEx::Member::RuntimeProperty()
{
    return &DeclaringType->UnderlyingType()->GetRuntimeProperty(Name);
}

bool XamlCPP::Core::ReflEx::Member::IsDirective() const
{
    return false;
}

bool XamlCPP::Core::ReflEx::Member::IsAttachable() const
{
    return false;
}

System::Reflection::meta::Field const * XamlCPP::Core::ReflEx::Member::RuntimeField()
{
    return &DeclaringType->UnderlyingType()->GetRuntimeField(Name);
}

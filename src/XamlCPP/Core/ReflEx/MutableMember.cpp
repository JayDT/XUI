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

#include "MutableMember.h"
#include "XamlType.h"
#include "Metadata.h"
#include "MemberValuePlugin.h"

XamlCPP::Core::ReflEx::MutableMember::MutableMember(std::string const & name, Proto::XamlType * declaringType, Interfaces::ITypeRepository * typeRepository, Interfaces::ITypeFeatureProvider * typeFeatureProvider)
    : MemberBase(name)
    , _valuePlugin(this)
{
    FeatureProvider = typeFeatureProvider;
    TypeRepository = typeRepository;
    DeclaringType = declaringType;
}

std::vector<XamlCPP::Core::ReflEx::Member*> XamlCPP::Core::ReflEx::MutableMember::getDependencies()
{
    return LookupDependencies();
}

std::vector<XamlCPP::Core::ReflEx::Member*> XamlCPP::Core::ReflEx::MutableMember::LookupDependencies()
{
    std::vector<Member*> result;
    auto metadata = FeatureProvider->GetMetadata(DeclaringType->UnderlyingType());
    if (metadata != nullptr)
    {
        auto namesOfPropsWeDependOn = metadata->GetMemberDependencies(Name);
        for (auto const& name : namesOfPropsWeDependOn)
        {
            auto member = DeclaringType->GetMember(name);
            if (member)
                result.push_back(member);
        }
    }
    return result;
}

std::string XamlCPP::Core::ReflEx::MutableMember::ToString() const
{
    return "Member: " + Name + " Type: " + DeclaringType->UnderlyingType()->GetName();
}

XamlCPP::Interfaces::IMemberValuePlugin * XamlCPP::Core::ReflEx::MutableMember::LookupXamlMemberValueConnector()
{
    return &_valuePlugin;
}

void XamlCPP::Core::ReflEx::MutableMember::SetValue(System::Reflection::Object * instance, System::Reflection::Variant const& value, Interfaces::IValueContext * pipeline)
{
    if (instance)
        GetMemberValuePlugin()->SetValue(instance, value, pipeline);
}

System::Reflection::Variant XamlCPP::Core::ReflEx::MutableMember::GetValue(System::Reflection::Object * instance)
{
    if (instance)
        return GetMemberValuePlugin()->GetValue(instance);
    return System::Reflection::Variant::Empty;
}

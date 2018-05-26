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

#pragma once
#include <System/System.h>
#include "MemberBase.h"
#include "Reflection/Runtime/Method.h"
#include "Core/Plugin/MemberValuePlugin.h"

//Forward Declaration
namespace XamlCPP
{
    namespace Interfaces
    {
        struct IMemberValuePlugin;
        struct ITypeRepository;
        struct ITypeFeatureProvider;
        struct IValueContext;
    }
}

namespace XamlCPP::Core::ReflEx
{
    class MutableMember : public MemberBase
    {
        friend class System::Reflection::Assembly::AssemblyModulXamlCPP;

        Plugin::MemberValuePlugin _valuePlugin;

    public:
        const System::Reflection::IMethod* Getter = nullptr;
        const System::Reflection::IMethod* Setter = nullptr;
        bool IsField = false;

        virtual System::Reflection::meta::Field const* RuntimeProperty() = 0;
        virtual System::Reflection::meta::Field const* RuntimeField() { return nullptr; }
        MutableMember* ToMutableMember() override { return this; }

        Interfaces::ITypeRepository* TypeRepository = nullptr;
        Proto::XamlType* DeclaringType = nullptr;
        bool IsRefPointer = false;
        Interfaces::IMemberValuePlugin* GetMemberValuePlugin() { return LookupXamlMemberValueConnector(); }
        Interfaces::ITypeFeatureProvider* FeatureProvider = nullptr;

        MutableMember(std::string const& name, Proto::XamlType* declaringType, Interfaces::ITypeRepository* typeRepository, Interfaces::ITypeFeatureProvider* typeFeatureProvider);

        std::vector<Member*> getDependencies();
        virtual std::vector<Member*> LookupDependencies();
        virtual std::string ToString() const override;
        virtual Interfaces::IMemberValuePlugin* LookupXamlMemberValueConnector();
        void SetValue(System::Reflection::Object* instance, System::Reflection::Variant const& value, Interfaces::IValueContext* pipeline);
        System::Reflection::Variant GetValue(System::Reflection::Object* instance);

        bool operator==(MutableMember const& rhs) const
        {
            return DeclaringType == rhs.DeclaringType && XamlType == rhs.XamlType && IsRefPointer == rhs.IsRefPointer;
        }
    };
}

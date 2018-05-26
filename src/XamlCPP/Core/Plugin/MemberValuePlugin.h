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
#include "Interfaces/IMemberValuePlugin.h"
#include "XamlCPP/Core/Dependencies/IDependencyObject.h"

namespace XamlCPP::Core
{
    namespace ReflEx
    {
        class MutableMember;
    }
}

namespace XamlCPP::Core::Plugin
{
    class MemberValuePlugin : public Interfaces::IMemberValuePlugin
    {
    private:
        ReflEx::MutableMember* member = nullptr;

        Core::Dependency::IPropertyObject* GetDependencyProperty(System::Reflection::Object * instance, Core::ReflEx::MutableMember* member);
        bool ApplyBinding(System::Reflection::Object * instance, Core::Dependency::IPropertyObject* property, Interfaces::IValueContext * valueContext, std::shared_ptr<Interfaces::IBinding> binding);

    public:
        MemberValuePlugin(ReflEx::MutableMember* member);

        // Inherited via IMemberValuePlugin
        virtual void SetValue(System::Reflection::Object * instance, System::Reflection::Variant const& value, Interfaces::IValueContext * valueContext) override;
        virtual System::Reflection::Variant GetValue(System::Reflection::Object * instance) override;
    };
}

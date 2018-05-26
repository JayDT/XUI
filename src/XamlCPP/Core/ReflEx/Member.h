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


//File: Member.h

#pragma once
#include <System/System.h>
#include "Core/ReflEx/MutableMember.h"

namespace XamlCPP::Core::ReflEx
{
    class Meta(AbstractType) Member : public MutableMember
    {
        friend class System::Reflection::Assembly::AssemblyModulXamlCPP;

        Proto::XamlType* LookupType();
    public:
        virtual System::Reflection::meta::Field const* RuntimeProperty() override;
        virtual System::Reflection::meta::Field const* RuntimeField() override;

        Member(std::string const& name, Proto::XamlType* declaringType, Interfaces::ITypeRepository* typeRepository, Interfaces::ITypeFeatureProvider* featureProvider)
            : MutableMember(name, declaringType, typeRepository, featureProvider)
        {
            XamlType = LookupType();
        }

        // Inherited via MutableMember
        virtual bool IsDirective() const override;
        virtual bool IsAttachable() const override;
    };
}

//End of file


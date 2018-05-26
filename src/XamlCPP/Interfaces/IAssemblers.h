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

#ifndef __XAMLCPP_XML_IASSEMBLERS_H__
#define __XAMLCPP_XML_IASSEMBLERS_H__

#include "Reflection/Runtime/CXXReflection.h"
#include "Interfaces/IRuntimeTypeSource.h"
#include "Interfaces/IInstanceLifeCycleListener.h"
#include "Interfaces/IParser.h"
#include "standard/events.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("Core/ReflEx/MutableMember.h"))
MetaAssemblyEnd

namespace XamlCPP::Core
{
    class Instruction;

    namespace ReflEx
    {
        struct MutableMember;
    }
};

namespace XamlCPP::Interfaces
{
    struct ITopDownValueContext;
    struct IInstanceLifeCycleListener;

    struct XamlSetValueEventArgs : public System::Events::EventArg
    {
        XamlSetValueEventArgs(Core::ReflEx::MutableMember* member, System::Reflection::Object* value)
        {
            Value = value;
            Member = member;
        }

        virtual ~XamlSetValueEventArgs() {}

        Core::ReflEx::MutableMember* Member = nullptr;
        System::Reflection::Object* Value = nullptr;
        bool Handled = false;
    };

    struct IObjectAssembler
    {
        virtual ~IObjectAssembler() {}

        virtual Interfaces::IRuntimeTypeSource *getTypeSource() const = 0;
        virtual Interfaces::IInstanceLifeCycleListener *GetLifecycleListener() const = 0;
        virtual Interfaces::ITopDownValueContext *getTopDownValueContext() const = 0;

        virtual System::Reflection::RefObject getResult() const = 0;
        virtual std::string ToString(std::vector<std::shared_ptr<Core::Instruction>> const& nodelist) const = 0;

        System::Events::EventHandler<XamlSetValueEventArgs> XamlSetValueHandler;

        virtual void Process(Core::Instruction* instruction) = 0;
        virtual void OverrideInstance(System::Reflection::Object* instance) = 0;
    };
}

#endif // !__XAMLCPP_XML_IASSEMBLERS_H__
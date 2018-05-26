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

#ifndef __XAMLCPP_OBJECTASSEMBLER_H__
#define __XAMLCPP_OBJECTASSEMBLER_H__

#include "Interfaces/IParser.h"
#include "Interfaces/IAssemblers.h"
#include "Interfaces/IValueContext.h"
#include "Core/Assembler/Level/Level.h"
#include <string>
#include <list>

namespace XamlCPP::Core
{
    class Instruction;

    namespace Assembler
    {
        class CurrentLevelWrapper;
        class PreviousLevelWrapper;
        class Level;
        class InstanceProperties;
        class XamlSetValueEventArgs;
        class Settings;
        class StateCommuter;
    };

    namespace Proto
    {
        class XamlType;
    }

    namespace ReflEx
    {
        class MemberBase;
        class MutableMember;
    };
};

namespace XamlCPP::Core::Assembler
{
    class ObjectAssembler : public Interfaces::IObjectAssembler
    {
    private:
        std::shared_ptr<StateCommuter> m_StateCommuter;
        std::vector<Level*> m_state;
        std::shared_ptr<Interfaces::IInstanceLifeCycleListener> m_LifecycleListener;
        System::Reflection::RefObject m_Result = nullptr;
        Interfaces::IRuntimeTypeSource *m_TypeSource = nullptr;

        System::Reflection::Object * rootInstance = nullptr;
        Proto::XamlType * rootInstanceXamlType = nullptr;
        std::shared_ptr<Interfaces::IValueContext> valueContext = nullptr;

        bool getIsLevelOneAndThereIsRootInstance() const;

        static std::shared_ptr<Interfaces::IInstanceLifeCycleListener> GetLifecycleListener(Settings *settings);

        ReflEx::MemberBase *GetActualMemberFromMemberSpecifiedInInstruction(ReflEx::MemberBase *specifiedMember);

    public:
        virtual ~ObjectAssembler()
        {
        }

        ObjectAssembler(Interfaces::IRuntimeTypeSource *typeSource, std::shared_ptr<Interfaces::IValueContext> const& valueContext, XamlCPP::Core::Assembler::Settings *settings = nullptr);

        ObjectAssembler(Interfaces::IRuntimeTypeSource *typeSource, std::shared_ptr<Interfaces::IInstanceLifeCycleListener> listener, Interfaces::IValueContext *context);

        StateCommuter *getStateCommuter() const;

        Interfaces::IInstanceLifeCycleListener *GetLifecycleListener() const;
        void setLifecycleListener(std::shared_ptr<Interfaces::IInstanceLifeCycleListener> value);
        Interfaces::ITopDownValueContext *getTopDownValueContext() const;

        System::Reflection::RefObject getResult() const override;
        void setResult(System::Reflection::RefObject value);
        //EventHandler<XamlSetValueEventArgs*> getXamlSetValueHandler() const;
        //void setXamlSetValueHandler(EventHandler<XamlSetValueEventArgs*> value);

        Interfaces::IRuntimeTypeSource *getTypeSource() const;

        void Process(Instruction *instruction);

        void OverrideInstance(System::Reflection::Object *instance);

        std::string ToString(std::vector<std::shared_ptr<Instruction>> const& nodelist) const override;

    };
}

#endif	//#ifndef __XAMLCPP_OBJECTASSEMBLER_H__

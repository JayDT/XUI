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

#include "ObjectAssembler.h"
#include "StateCommuter.h"
//#include "XamlSetValueEventArgs.h"
#include "Settings.h"
#include "Assembler/Level/Level.h"
#include "Command.h"
#include "NullLifecycleListener.h"
#include "Assembler/StateCommuter.h"
#include "Core/DeferredXamlLoader.h"
#include "Core/Instruction.h"
#include "Core/ReflEx/AttachableMember.h"
#include "Interfaces/IXamlCPP.h"

#include "Level/CurrentLevelWrapper.h"
#include "Command/EndMemberCommand.h"
#include "Command/EndObjectCommand.h"
#include "Command/GetObjectCommand.h"
#include "Command/ITopDownValueContext.h"
#include "Command/NamespaceDeclarationCommand.h"
#include "Command/StartMemberCommand.h"
#include "Command/StartObjectCommand.h"
#include "Command/ValueCommand.h"

#include "Assembler/Command/EndMemberCommand.h"

using namespace XamlCPP;
using namespace XamlCPP::Core;
using namespace XamlCPP::Core::Assembler;

ObjectAssembler::ObjectAssembler(Interfaces::IRuntimeTypeSource *typeSource, std::shared_ptr<Interfaces::IValueContext> const& valueContext, XamlCPP::Core::Assembler::Settings *settings)
    : ObjectAssembler(typeSource, ObjectAssembler::GetLifecycleListener(settings), valueContext.get())
{
    this->valueContext = valueContext;
    System::ThrowIfFailed<System::ArgumentException>(typeSource, "typeSource");
    System::ThrowIfFailed<System::ArgumentException>(valueContext.get(), "valueContext");

    m_TypeSource = typeSource;
    getStateCommuter()->RaiseLevel();

    if (settings && settings->GetDefaultAssembly())
        m_TypeSource->SetDefaultAssembly(settings->GetDefaultAssembly());

    if (settings && settings->IsTemplateLoader)
        getStateCommuter()->SetTemplateLoader(settings->IsTemplateLoader);

    auto rootInstanceType = rootInstance == nullptr ? nullptr : rootInstance->GetType();
}

ObjectAssembler::ObjectAssembler(Interfaces::IRuntimeTypeSource *typeSource, std::shared_ptr<Interfaces::IInstanceLifeCycleListener> listener, Interfaces::IValueContext *context)
{
    System::ThrowIfFailed<System::ArgumentException>(listener.get(), "listener");
    m_StateCommuter = std::make_shared<StateCommuter>(&m_state, typeSource, listener.get(), context);
    for (auto lvl : m_state)
        delete lvl;
    m_state.clear();
    setLifecycleListener(listener);
}

StateCommuter *ObjectAssembler::getStateCommuter() const
{
    return m_StateCommuter.get();
}

bool ObjectAssembler::getIsLevelOneAndThereIsRootInstance() const
{
    return getStateCommuter()->getLevel() == 1 && rootInstance != nullptr;
}

Interfaces::IInstanceLifeCycleListener *ObjectAssembler::GetLifecycleListener() const
{
    return m_LifecycleListener.get();
}

void ObjectAssembler::setLifecycleListener(std::shared_ptr<Interfaces::IInstanceLifeCycleListener> value)
{
    m_LifecycleListener = value;
}

Interfaces::ITopDownValueContext *ObjectAssembler::getTopDownValueContext() const
{
    return valueContext->getTopDownValueContext();
}

System::Reflection::RefObject ObjectAssembler::getResult() const
{
    return m_Result;
}

void ObjectAssembler::setResult(System::Reflection::RefObject value)
{
    m_Result = value;
}

//EventHandler<XamlSetValueEventArgs*> ObjectAssembler::getXamlSetValueHandler() const
//{
//    return m_XamlSetValueHandler;
//}
//
//void ObjectAssembler::setXamlSetValueHandler(EventHandler<XamlSetValueEventArgs*> value)
//{
//    m_XamlSetValueHandler = value;
//}

Interfaces::IRuntimeTypeSource *ObjectAssembler::getTypeSource() const
{
    return m_TypeSource;
}

void ObjectAssembler::Process(Instruction *instruction)
{
    Command *command;

    if (m_StateCommuter->GetDeferLoader() && m_StateCommuter->GetDeferLoader()->GetDeferCount())
    {
        // Add first refcount until end of template
        if (m_StateCommuter->GetDeferLoader()->GetDeferCount() == 0)
            m_StateCommuter->GetDeferLoader()->IncDeferCount();

        bool skip = true;
        switch (instruction->GetInstructionType())
        {
            case InstructionType::NamespaceDeclaration:
            case InstructionType::Value:
                break;
            case InstructionType::EndObject:
                m_StateCommuter->GetDeferLoader()->DecDeferCount();
                break;
            case InstructionType::EndMember:
                m_StateCommuter->GetDeferLoader()->DecDeferCount();
                if (m_StateCommuter->GetDeferLoader()->GetDeferCount() == 0)
                    skip = false;
                break;
            case InstructionType::StartMember:
                m_StateCommuter->GetDeferLoader()->IncDeferCount();
                break;
            case InstructionType::GetObject:
            case InstructionType::StartObject:
                m_StateCommuter->GetDeferLoader()->IncDeferCount();
                break;
            default:
                throw ParseException(System::String::format("The XamlInstructionType %d has an unexpected value", instruction->GetInstructionType()).c_str());
        }

        if (m_StateCommuter->GetDeferLoader()->GetDeferCount() < 0)
            throw ParseException(System::String::format("The XamlInstructionType %d has an unexpected value", instruction->GetInstructionType()).c_str());

        if (m_StateCommuter->GetDeferLoader()->GetDeferCount() > 0)
            m_StateCommuter->GetDeferLoader()->PushInstruction(instruction);

        if (skip)
            return;

        if (m_StateCommuter->GetDeferLoader()->GetDeferCount() == 0)
        {
            auto member = m_StateCommuter->getCurrent()->getMember()->ToMutableMember();
            auto xamlType = member->XamlType;

            member->SetValue(m_StateCommuter->getCurrent()->getInstance().ToObject(), m_StateCommuter->GetDeferLoader(), m_StateCommuter->getValueContext());
            m_StateCommuter->getCurrent()->GetLevel()->SetDeferLoader(nullptr);
            m_StateCommuter->SetDeferLoader(nullptr);
            //m_StateCommuter->getCurrent()->setWasAssociatedRightAfterCreation(true);
        }
    }

    switch (instruction->GetInstructionType())
    {
        case InstructionType::NamespaceDeclaration:
            command = new Commands::NamespaceDeclarationCommand(instruction->NamespaceDeclaration, m_StateCommuter);
            break;
        case InstructionType::StartObject:
            command = new Commands::StartObjectCommand(m_StateCommuter, getTypeSource(), instruction->XamlType, rootInstance);
            break;
        case InstructionType::StartMember:
            command = new Commands::StartMemberCommand(m_StateCommuter, GetActualMemberFromMemberSpecifiedInInstruction(instruction->Member));
            break;
        case InstructionType::Value:
            command = new Commands::ValueCommand(m_StateCommuter, valueContext.get(), instruction->Value.GetValue<std::string>());
            break;
        case InstructionType::EndObject:
            command = new Commands::EndObjectCommand(m_StateCommuter, [&](StateCommuter *stateCommuter)
            {
                if (stateCommuter->getCurrent()->getInstance().IsPointer() && stateCommuter->getCurrent()->getInstance().IsObject())
                    setResult(stateCommuter->getCurrent()->getInstance().GetValue<System::Reflection::RefObject>());
            }, GetLifecycleListener());
            break;
        case InstructionType::EndMember:
            command = new Commands::EndMemberCommand(getTypeSource(), m_StateCommuter, valueContext.get());
            break;
        case InstructionType::GetObject:
            command = new Commands::GetObjectCommand(m_StateCommuter);
            break;
        default:
            throw ParseException(System::String::format("The XamlInstructionType %d has an unexpected value", instruction->GetInstructionType()).c_str());
    }

    command->Execute();
    delete command;
}

void ObjectAssembler::OverrideInstance(System::Reflection::Object *instance)
{
    getStateCommuter()->RaiseLevel();
    getStateCommuter()->getCurrent()->setInstance(instance);

    auto collection = instance->As<System::Collection::Generic::IContainer>();
    if (collection != nullptr)
    {
        getStateCommuter()->getCurrent()->setCollection(collection);
    }
}

std::string XamlCPP::Core::Assembler::ObjectAssembler::ToString(std::vector<std::shared_ptr<Instruction>> const & nodelist) const
{
    std::ostringstream debugstr;
    int deep = 0;
    for (int i = 0; i != nodelist.size(); ++i)
    {
        if (nodelist[i]->GetInstructionType() == Core::InstructionType::EndMember || nodelist[i]->GetInstructionType() == Core::InstructionType::EndObject)
            --deep;

        debugstr << std::setfill('0') << std::setw(4) << i << "): [" << std::setfill('0') << std::setw(4) << deep << "] ";
        for (int d = 0; d < deep; ++d)
            debugstr << "  ";

        if (nodelist[i]->GetInstructionType() == Core::InstructionType::StartMember || nodelist[i]->GetInstructionType() == Core::InstructionType::StartObject || nodelist[i]->GetInstructionType() == Core::InstructionType::GetObject)
            ++deep;

        debugstr << nodelist[i]->ToString() << std::endl;
    }

    return debugstr.str();
}

std::shared_ptr<Interfaces::IInstanceLifeCycleListener> ObjectAssembler::GetLifecycleListener(Settings *settings)
{
    if (settings == nullptr ? nullptr : settings->getInstanceLifeCycleListener() != nullptr)
    {
        return settings->getInstanceLifeCycleListener();
    }

    return std::make_shared<NullLifecycleListener>();
}

ReflEx::MemberBase *ObjectAssembler::GetActualMemberFromMemberSpecifiedInInstruction(ReflEx::MemberBase *specifiedMember)
{
    if (getIsLevelOneAndThereIsRootInstance() && !specifiedMember->IsDirective() && rootInstanceXamlType != nullptr)
    {
        auto attachable = specifiedMember->ToAttachableMember();

        ReflEx::MemberBase *actualMember;

        if (attachable != nullptr)
        {
            actualMember = attachable->DeclaringType->GetAttachableMember(specifiedMember->Name);
        }
        else
        {
            actualMember = rootInstanceXamlType->GetMember(specifiedMember->Name);
        }

        return actualMember;
    }

    return specifiedMember;
}

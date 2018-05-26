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

#include "StartMemberCommand.h"
#include "ReflEx/Member.h"
#include "CoreTypes.h"
#include "Interfaces/IResourceMap.h"
#include "Core/DeferredXamlLoader.h"
#include "Core/Assembler/StateCommuter.h"
#include "Core/Assembler/Level/CurrentLevelWrapper.h"
#include "Interfaces/IXamlCPP.h"
#include "Reflection/Runtime/ReflectionModule.h"

using namespace XamlCPP::Core;
using namespace XamlCPP::Core::Assembler::Commands;

StartMemberCommand::StartMemberCommand(std::shared_ptr<XamlCPP::Core::Assembler::StateCommuter> const& stateCommuter, ReflEx::MemberBase *member)
    : Command(stateCommuter)
    , member(member)
{
}

DirectiveKind StartMemberCommand::GetDirectiveKind(ReflEx::MemberBase *member)
{
    if (member->IsDirective())
        return CoreTypes::GetDirectiveKind(static_cast<ReflEx::Directive*>(member));

    throw System::InvalidOperationException(System::String::format("Unexpected XAML directive. The directive {0} has been found and we don't know how to handle it.", member->ToString().c_str()).c_str());
}

void StartMemberCommand::Execute()
{
    auto realMember = member;

    auto mutable_ = dynamic_cast<ReflEx::MutableMember*>(member);

    if (mutable_ != nullptr && IsMemberEquivalentToNameDirective(mutable_))
    {
        realMember = CoreTypes::getName();
    }

    m_StateCommuter->getCurrent()->setMember(realMember);

    if (realMember->IsDirective())
    {
        SetCommuterStateAccordingToDirective(realMember);
    }
    else
    {
        CreateInstanceOfCurrentTypeAndAssociateIfPossible();
    }
}

void StartMemberCommand::CreateInstanceOfCurrentTypeAndAssociateIfPossible()
{
    static const System::Reflection::Type* FrameworkTemplateType = &System::Reflection::ReflectionModulManager::Instance().GetType("XUI::UI::DataTemplates::FrameworkTemplate");

    System::ThrowIfFailed<System::AggregateException>(FrameworkTemplateType->IsValid(), "Required XUI framework");

    m_StateCommuter->CreateInstanceOfCurrentXamlTypeIfNotCreatedBefore();

    if (m_StateCommuter->getCurrent()->getInstance().GetBase())
    {
        if (m_StateCommuter->getCurrent()->getInstance().IsObject() &&
            m_StateCommuter->getCurrent()->getMember() &&
            m_StateCommuter->getCurrent()->getInstance().ToObject()->Is(*FrameworkTemplateType) &&
            m_StateCommuter->getCurrent()->getMember()->Name == "XamlTemplate")
        {
            Interfaces::IResourceNode* resourceNode = nullptr;
            for (auto level : *m_StateCommuter->getStack())
            {
                if (!level->GetInstance().ToObject())
                    continue;

                auto control = level->GetInstance().ToObject()->Dynamic_As<Interfaces::IResourceNode>();
                if (control)
                {
                    resourceNode = control->ToIResourceNode();
                    break;
                }
            }
            m_StateCommuter->getCurrent()->GetLevel()->SetDeferLoader(std::make_shared<XamlCPP::Core::DeferredXamlLoader>(resourceNode, m_StateCommuter->getValueContext()));
            m_StateCommuter->SetDeferLoader(m_StateCommuter->getCurrent()->GetLevel()->GetDeferLoader());
            m_StateCommuter->GetDeferLoader()->IncDeferCount();
        }
    }

    if (!m_StateCommuter->getWasAssociatedRightAfterCreation())
    {
        // delayed after set all property (todo dependOn order)
        if (m_StateCommuter->getCurrent()->getInstance().GetBase())
        {
            const XamlCPP::MapKeyPropertyAttribute* keyName = m_StateCommuter->getCurrent()->getInstance().GetType()->GetMeta().GetCustomAttribute<XamlCPP::MapKeyPropertyAttribute>();
            if (keyName)
                return;
        }

        m_StateCommuter->AssociateCurrentInstanceToParentForCreation();
    }
}

bool StartMemberCommand::IsMemberEquivalentToNameDirective(ReflEx::MutableMember *memberToCheck)
{
    return memberToCheck == memberToCheck->DeclaringType->getRuntimeNamePropertyMember();
}

void StartMemberCommand::ForceInstanceCreationOfCurrentType()
{
    m_StateCommuter->CreateInstanceOfCurrentXamlTypeIfNotCreatedBefore();
}

void StartMemberCommand::SetCommuterStateAccordingToDirective(ReflEx::MemberBase *memberBase)
{
    switch (GetDirectiveKind(memberBase))
    {
        case DirectiveKind::Items:
            if (m_StateCommuter->getHasParent() && !m_StateCommuter->getParentIsOneToMany())
            {
                throw XamlCPP::ParseException("Cannot assign a more than one item to a member that is not a collection.");
            }

            if (!m_StateCommuter->getCurrent()->getIsGetObject())
            {
                AccommodateLevelsForIncomingChildren();
            }
            break;

        case DirectiveKind::Initialization:
            m_StateCommuter->setValueProcessingMode(ValueProcessingMode::InitializationValue);
            break;

        case DirectiveKind::Key:
            m_StateCommuter->setValueProcessingMode(ValueProcessingMode::Key);
            break;

        case DirectiveKind::MarkupExtensionArguments:
            m_StateCommuter->setValueProcessingMode(ValueProcessingMode::ConstructionParameter);
			m_StateCommuter->getCurrent()->GetLevel()->SetMarkupArgument(true); // 
            break;

        case DirectiveKind::Name:
            m_StateCommuter->CreateInstanceOfCurrentXamlTypeIfNotCreatedBefore();
            m_StateCommuter->setValueProcessingMode(ValueProcessingMode::Name);
            break;

        case DirectiveKind::Class:
            m_StateCommuter->setValueProcessingMode(ValueProcessingMode::Class);
            break;

        case DirectiveKind::SubClass:
            m_StateCommuter->setValueProcessingMode(ValueProcessingMode::SubClass);
            break;

        case DirectiveKind::UnknownContent:
            AccommodateLevelsForIncomingChildren();
            break;
    }
}

void StartMemberCommand::AccommodateLevelsForIncomingChildren()
{
    m_StateCommuter->CreateInstanceOfCurrentXamlTypeIfNotCreatedBefore();
    auto instance = m_StateCommuter->getCurrent()->getInstance();

    if (instance.IsObject())
    {
        auto collection = instance.ToObject()->As<System::Collection::Generic::IContainer>();
        if (collection != nullptr)
        {
            m_StateCommuter->getCurrent()->setCollection(collection);
        }
    }
}

std::string StartMemberCommand::ToString() const
{
    return System::String::format("Start Of Member: {0}", member->ToString());
}


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

#include "EndObjectCommand.h"
#include "Core/CoreTypes.h"
#include "Core/Assembler/StateCommuter.h"
#include "Core/Assembler/Level/CurrentLevelWrapper.h"
#include "Interfaces/IMarkupExtension.h"
#include "Interfaces/ISupportInitialize.h"
#include "XUI/Interfaces/IComponentConnector.h"

#include <standard/collection.h>

using namespace XamlCPP;
using namespace XamlCPP::Core::Assembler::Commands;


EndObjectCommand::EndObjectCommand(std::shared_ptr<XamlCPP::Core::Assembler::StateCommuter> const& stateCommuter, std::function<void(XamlCPP::Core::Assembler::StateCommuter*)> setResult, Interfaces::IInstanceLifeCycleListener *lifyCycleListener)
    : Command(stateCommuter)
    , setResult(setResult)
    , lifyCycleListener(lifyCycleListener)
{
}

void EndObjectCommand::Execute()
{
    if (!m_StateCommuter->GetDeferLoader() || m_StateCommuter->GetDeferLoader()->GetDeferCount() == 0)
    {
        if (!m_StateCommuter->getCurrent()->getIsGetObject())
        {
            m_StateCommuter->CreateInstanceOfCurrentXamlTypeIfNotCreatedBefore();
            m_StateCommuter->PutNameToCurrentInstanceIfAny();

            auto instance = m_StateCommuter->getCurrent()->getInstance();

            if (m_StateCommuter->getCurrent()->getIsMarkupExtension())
            {
                ProcessCurrentInstanceValueWithMarkupExtension();
            }
            else if (!m_StateCommuter->getWasAssociatedRightAfterCreation())
            {
                m_StateCommuter->AssociateCurrentInstanceToParent();
            }

            m_StateCommuter->RegisterInstanceNameToNamescope();
            if (instance.IsObject())
            {
                System::Reflection::Object* object = instance.ToObject();
                if (m_StateCommuter->GetTopLevel() && object->Is<Core::Dependency::IDependencyObject>())
                {
                    XUI::Interfaces::IComponentConnector* connector = m_StateCommuter->GetTopLevel()->Dynamic_As<XUI::Interfaces::IComponentConnector>();
                    if (connector)
                    {
                        Core::Dependency::IDependencyObject* dep_object = object->Static_As<Core::Dependency::IDependencyObject>();
                        if (dep_object)
                            connector->Connect(dep_object->GetGID(), object);
                        else
                            connector->Connect(0, object);
                    }
                }

                lifyCycleListener->OnAfterProperties(object);
                lifyCycleListener->OnEnd(object);

                Interfaces::ISupportInitialize* initializer = object->Dynamic_As<Interfaces::ISupportInitialize>();
                if (initializer)
                    initializer->EndInit();
            }
        }

        setResult(m_StateCommuter.get());
    }

    m_StateCommuter->DecreaseLevel();
}

void EndObjectCommand::ProcessCurrentInstanceValueWithMarkupExtension()
{
	ReflEx::MemberBase* member = m_StateCommuter->getPrevious()->getMember();
	if (member->IsDirective())
	{
		Core::Assembler::Commands::DirectiveKind kind = CoreTypes::GetDirectiveKind(static_cast<ReflEx::Directive*>(member));
		if (kind == Core::Assembler::Commands::DirectiveKind::MarkupExtensionArguments)
		{
			/// Store markup for parent markup contructor
			const_cast<std::vector<ConstructionArgument>&>(m_StateCommuter->getPrevious()->GetLevel()->GetCtorArguments()).push_back(ConstructionArgument(std::static_pointer_cast<Interfaces::IMarkupExtension>(m_StateCommuter->getCurrent()->getInstance().ToSharedObject())));
		}
		return;
	}

    auto processedValue = m_StateCommuter->GetValueProvidedByMarkupExtension(static_cast<Interfaces::IMarkupExtension*>(m_StateCommuter->getCurrent()->getInstance().ToObject()));
    m_StateCommuter->getCurrent()->setInstance(processedValue);

    if (processedValue.GetBase() && processedValue.IsObject())
    {
        auto collection = processedValue.ToObject()->As<System::Collection::Generic::IContainer>();
        if (collection != nullptr)
        {
            m_StateCommuter->getCurrent()->setCollection(collection);
        }
    }

    m_StateCommuter->AssociateCurrentInstanceToParent();
}

std::string EndObjectCommand::ToString() const
{
    return "End of Object";
}
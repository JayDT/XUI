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

#include "StartObjectCommand.h"
#include "Core/Assembler/StateCommuter.h"
#include "Core/Assembler/Level/CurrentLevelWrapper.h"
#include "Interfaces/IXamlCPP.h"

using namespace XamlCPP;
using namespace XamlCPP::Core::Assembler::Commands;

StartObjectCommand::StartObjectCommand(std::shared_ptr<XamlCPP::Core::Assembler::StateCommuter> const& stateCommuter, Interfaces::ITypeRepository *typeRepository, Core::Proto::XamlType *xamlType, System::Reflection::Object *rootInstance)
    : Command(stateCommuter)
    , typeRepository(typeRepository)
    , xamlType(xamlType)
    , rootInstance(rootInstance)
{
	if (xamlType && xamlType->Name() == "DynamicResourceExtension")
		printf("");
}

void StartObjectCommand::Execute()
{
    if (m_StateCommuter->getLevel() == 0)
    {
        throw ParseException("An object cannot start after level zero has been reached. This condition may indicate that there are more than one object at the Root Level. Please, verify that there is ONLY one root object.");
    }

    if (getConflictsWithObjectBeingConfigured())
    {
        m_StateCommuter->RaiseLevel();
    }

    m_StateCommuter->getCurrent()->setXamlType(xamlType);
    OverrideInstanceAndTypeInLevel1();
}

bool StartObjectCommand::getConflictsWithObjectBeingConfigured() const
{
    return m_StateCommuter->getCurrent()->getHasInstance() || m_StateCommuter->getCurrent()->getIsGetObject() || m_StateCommuter->getCurrent()->GetLevel()->GetMarkupArgument();
}

void StartObjectCommand::OverrideInstanceAndTypeInLevel1()
{
    if (m_StateCommuter->getLevel() == 1 && rootInstance != nullptr)
    {
        std::shared_ptr<StateCommuter> _Qualifier = m_StateCommuter;
        _Qualifier->getCurrent()->setInstance(rootInstance);

        auto collection = rootInstance->As<System::Collection::Generic::IContainer>();
        if (collection != nullptr)
        {
            _Qualifier->getCurrent()->setCollection(collection);
        }

        auto xamlTypeOfInstance = typeRepository->GetByType(rootInstance->GetType());
        m_StateCommuter->getCurrent()->setXamlType(xamlTypeOfInstance);
    }
}

std::string StartObjectCommand::ToString() const
{
    auto instance = rootInstance != nullptr ? System::String::format(", using Root Instance: %s", rootInstance->ToString()) : "";
    return System::String::format("Start Of Object: %s %s", xamlType->Name().c_str(), instance.c_str());
}


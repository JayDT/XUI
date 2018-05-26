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

#include "GetObjectCommand.h"
#include "Core/Assembler/StateCommuter.h"
#include "Core/Assembler/Level/CurrentLevelWrapper.h"

using namespace XamlCPP;
using namespace XamlCPP::Core::Assembler::Commands;

GetObjectCommand::GetObjectCommand(std::shared_ptr<XamlCPP::Core::Assembler::StateCommuter> const& stateCommuter)
    : Command(stateCommuter)
{
}

void GetObjectCommand::Execute()
{
    auto previousMember = m_StateCommuter->getCurrent()->getMember();

    m_StateCommuter->RaiseLevel();
    m_StateCommuter->getCurrent()->setIsGetObject(true);
    System::Reflection::Variant instanceToGet = m_StateCommuter->getValueOfPreviousInstanceAndItsMember();
    m_StateCommuter->getCurrent()->setInstance(instanceToGet);
    m_StateCommuter->getCurrent()->setMember(previousMember);

    if (instanceToGet.IsObject())
    {
        assert(instanceToGet.GetBase()->StorageType() != System::Reflection::VariantStorageType::Copy || instanceToGet.IsPointer()); // object copy not supported
        auto collection = instanceToGet.ToObject()->Static_As<System::Collection::Generic::IContainer>();
        if (collection != nullptr)
            m_StateCommuter->getCurrent()->setCollection(collection);
    }
}

std::string GetObjectCommand::ToString() const
{
    return "Get Object";
}


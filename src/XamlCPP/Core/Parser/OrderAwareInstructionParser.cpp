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

#include "StdAfx.h"
#include "OrderAwareInstructionParser.h"
#include "Instruction.h"
#include "Proto/XamlType.h"
#include "ReflEx/Member.h"

bool HasSomeMemberHasDependencies(XamlCPP::Core::Proto::XamlType* xamlType)
{
    for (auto const& member : xamlType->GetAllMembers())
    {
        if (member->ToMutableMember() != nullptr)
        {
            if (!member->ToMutableMember()->getDependencies().empty())
                return true;
        }
    }

    return false;
}

std::shared_ptr<std::vector<std::shared_ptr<XamlCPP::Core::Instruction>>> XamlCPP::Core::OrderAwareInstructionParser::Parse(const std::vector<std::shared_ptr<XamlCPP::Core::ProtoInstruction>>& protoNodes)
{
    std::shared_ptr<std::vector<std::shared_ptr<XamlCPP::Core::Instruction>>> nodeSorter = parser->Parse(protoNodes);
    //std::sort(nodeSorter->begin(), nodeSorter->end(), [](std::shared_ptr<XamlCPP::Core::Instruction> const& lhs, std::shared_ptr<XamlCPP::Core::Instruction> const& rhs)
    //{
    //    if (lhs->XamlType && HasSomeMemberHasDependencies(lhs->XamlType))
    //        return false;
    //    return true;
    //});
    return nodeSorter;
}

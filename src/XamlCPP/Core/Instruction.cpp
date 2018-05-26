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
#include "Instruction.h"

#include "NamespaceDeclaration.h"
#include "XamlType.h"

XamlCPP::Core::Instruction::Instruction(const InstructionType instructionType)
    : instructionType(instructionType)
    , internalNodeType(InternalNodeType::None)
    , data(nullptr)
{
}

XamlCPP::Core::Instruction::Instruction(const InstructionType instructionType, System::Reflection::Variant const& data)
    : instructionType(instructionType)
    , internalNodeType(InternalNodeType::None)
    , data(data)
{
}

std::string XamlCPP::Core::Instruction::ToString() const
{
    std::string str;
    switch (instructionType)
    {
        case InstructionType::None:
        {
            str = "None: ";
            switch (internalNodeType)
            {
                case InternalNodeType::StartOfStream:
                {
                    str += "Start Of Stream";
                    return str;
                }
                case InternalNodeType::EndOfStream:
                {
                    str += "End Of Stream";
                    return str;
                }
                case InternalNodeType::EndOfAttributes:
                {
                    str += "End Of Attributes";
                    return str;
                }
                default:
                {
                    return str;
                }
            }
        }
        case InstructionType::StartObject:
        {
            str = "StartObject: ";
            str += const_cast<XamlCPP::Core::Instruction*>(this)->XamlType->Name();
            return str;
        }
        case InstructionType::GetObject:
        {
            str = "GetObject: ";
            return str;
        }
        case InstructionType::EndObject:
        {
            str = "EndObject: ";
            return str;
        }
        case InstructionType::EndMember:
        {
            str = "EndMember: ";
            return str;
        }
        case InstructionType::StartMember:
        {
            str = "StartMember: ";
            str += const_cast<XamlCPP::Core::Instruction*>(this)->Member->ToString();
            return str;
        }
        case InstructionType::Value:
        {
            str = "Value: ";
            if (const_cast<XamlCPP::Core::Instruction*>(this)->Value.GetType())
            {
                str += const_cast<XamlCPP::Core::Instruction*>(this)->Value.GetType()->GetName();
                str += " ";
                str += const_cast<XamlCPP::Core::Instruction*>(this)->Value.ToString();
            }
            else
                str += "Unmanaged type";
            return str;
        }
        case InstructionType::NamespaceDeclaration:
        {
            str = "NamespaceDeclaration: ";
            str += const_cast<XamlCPP::Core::Instruction*>(this)->NamespaceDeclaration->ToString();
            return str;
        }
        default:
        {
            return str;
        }
    }
}

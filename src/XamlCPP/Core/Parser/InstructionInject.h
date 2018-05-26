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

#ifndef __XAMLCPP_INSTRUCTIONINJECT_H__
#define __XAMLCPP_INSTRUCTIONINJECT_H__

#include "Interfaces/IParser.h"
#include "Reflection/Runtime/CXXReflection.h"
#include "Core/Proto/XamlType.h"
#include "Core/Instruction.h"
#include "Core/ProtoInstruction.h"
#include "Core/CoreTypes.h"
#include "Core/ReflEx/NamespaceDeclaration.h"
#include "standard/events.h"

namespace XamlCPP::Core
{
    class Inject final
    {
    public:
        static std::shared_ptr<Instruction> StartOfObject(Proto::XamlType *xamlType)
        {
            System::ThrowIfFailed<System::ArgumentException>(xamlType, "xamlType");

            return std::make_shared<Instruction>(InstructionType::StartObject, xamlType);
        }

        static std::shared_ptr<Instruction> EndOfObject()
        {
            return std::make_shared<Instruction>(InstructionType::EndObject);
        }

        static std::shared_ptr<Instruction> PrefixDefinitionOfNamespace(ProtoInstruction* protoInstruction)
        {
            auto namespaceDeclaration = new ReflEx::NamespaceDeclaration(protoInstruction->Namespace, protoInstruction->Prefix);
            return std::make_shared<Instruction>(InstructionType::NamespaceDeclaration, namespaceDeclaration);
        }

        static std::shared_ptr<Instruction> StartOfMember(ReflEx::MemberBase *member)
        {
            return std::make_shared<Instruction>(InstructionType::StartMember, member);
        }

        static std::shared_ptr<Instruction> Value(const std::string &value)
        {
            return std::make_shared<Instruction>(InstructionType::Value, value);
        }

        static std::shared_ptr<Instruction> EndOfMember()
        {
            return std::make_shared<Instruction>(InstructionType::EndMember);
        }

        static std::shared_ptr<Instruction> GetObject()
        {
            return std::make_shared<Instruction>(InstructionType::GetObject);
        }

        static std::shared_ptr<Instruction> Items()
        {
            return std::make_shared<Instruction>(InstructionType::StartMember, CoreTypes::getItems());
        }

        static std::shared_ptr<Instruction> MarkupExtensionArguments()
        {
            return std::make_shared<Instruction>(InstructionType::StartMember, CoreTypes::getMarkupExtensionArguments());
        }

        static std::shared_ptr<Instruction> Initialization()
        {
            return std::make_shared<Instruction>(InstructionType::StartMember, CoreTypes::getInitialization());
        }

        static std::shared_ptr<Instruction> UnknownContent()
        {
            return std::make_shared<Instruction>(InstructionType::StartMember, CoreTypes::getUnknownContent());
        }
    };

};

#endif // !__XAMLCPP_INSTRUCTIONINJECT_H__
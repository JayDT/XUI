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

#ifndef __XAMLCPP_XML_CORE_INSTRUCTION_H__
#define __XAMLCPP_XML_CORE_INSTRUCTION_H__

#include "standard/BasicPrimities.h"
#include "Interfaces/IParser.h"
#include "Reflection/Runtime/CXXReflection.h"
#include "Core/ReflEx/NamespaceDeclaration.h"
#include "Core/ReflEx/MemberBase.h"
#include "standard/events.h"

#include <any>

#undef GetObject

MetaAssemblyBegin
Meta(modul: ReflInclude("Core/Assembler/StoredInstance.h"))
MetaAssemblyEnd

namespace XamlCPP::Core
{
    namespace Proto
    {
        class XamlType;
    }

    namespace ReflEx
    {
        struct MemberBase;
        class NamespaceDeclaration;
    }

    enum class InternalNodeType
    {
        EndOfAttributes,
        StartOfStream,
        EndOfStream,
        None,
    };

    enum class InstructionType
    {
        None,
        StartObject,
        EndObject,
        StartMember,
        EndMember,
        Value,
        NamespaceDeclaration,
        GetObject
    };

    class 
        Meta(AbstractType)
        Meta(ReflInclude("Core/ReflEx/Member.h"))
        Instruction : public System::Reflection::Object, public System::EnableSharedFromThisEx<Instruction>
    {
        friend class System::Reflection::Assembly::AssemblyModulXamlCPP;

    private:
        InstructionType instructionType = InstructionType::None;
        System::Reflection::Variant data;
        InternalNodeType internalNodeType = InternalNodeType::None;
    public:
        Instruction(const InstructionType instructionType);
        Instruction(const InstructionType instructionType, System::Reflection::Variant const& data);

        //PROPERTY_TG(Interfaces::IProtoParser*, ProtoParser)
        //PROPERTY_T_GET(ProtoParser, ) {}

        PROPERTY_TG(Proto::XamlType*, XamlType)
        PROPERTY_T_GET(XamlType, ) 
        {
            if (instructionType == InstructionType::StartObject && data.GetNativeTypeHash() == typeid(Proto::XamlType*).hash_code())
                return data.GetValue<Proto::XamlType*>();
            return nullptr;
        }

        PROPERTY_TG(ReflEx::MemberBase*, Member)
        PROPERTY_T_GET(Member, )
        {
            if (instructionType == InstructionType::StartMember && data.IsObject() && typeof(ReflEx::MemberBase).IsAssignableFrom(*data.GetType()))
                return data.GetValue<ReflEx::MemberBase*>();
            return nullptr;
        }

        PROPERTY_TG(System::Reflection::Variant, Value)
        PROPERTY_T_GET(Value, const&)
        {
            if (instructionType == InstructionType::Value)
                return data;
            return System::Reflection::Variant::Empty;
        }

        PROPERTY_TG(ReflEx::NamespaceDeclaration*, NamespaceDeclaration)
        PROPERTY_T_GET(NamespaceDeclaration, )
        {
            if (instructionType == InstructionType::NamespaceDeclaration && data.GetNativeTypeHash() == typeid(ReflEx::NamespaceDeclaration*).hash_code())
                return data.GetValue<ReflEx::NamespaceDeclaration*>();
            return nullptr;
        }

        InstructionType GetInstructionType() const { return instructionType; }
        InternalNodeType GetInternalNodeType() const { return internalNodeType; }

        size_t GetHashCode() const override
        {
            auto hashCode = (int)instructionType;
            //hashCode = (hashCode * 397) ^ (data. ? .GetHashCode() ? ? 0);
            //hashCode = (hashCode * 397) ^ (int)internalNodeType;
            return hashCode;
        }

        std::string ToString() const override;
    };
}

#endif // !__XAMLCPP_XML_CORE_INSTRUCTION_H__
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

#ifndef __XAMLCPP_INSTRUCTIONPARSER_H__
#define __XAMLCPP_INSTRUCTIONPARSER_H__

#include "Interfaces/IParser.h"
#include "Reflection/Runtime/CXXReflection.h"
#include "Core/ProtoInstruction.h"
#include "standard/events.h"

namespace XamlCPP::Core
{
    namespace ReflEx
    {
        struct MutableMember;
        struct MemberBase;
    }

    class InstructionParser : public Interfaces::IInstructionParser
    {
    private:
        bool privateEndOfStream = false;

        Interfaces::IRuntimeTypeSource *const typeSource;
        std::vector<std::shared_ptr<XamlCPP::Core::ProtoInstruction>> instructionStream;
        std::vector<std::shared_ptr<XamlCPP::Core::ProtoInstruction>>::iterator instructionStreamIterator;
        std::shared_ptr<std::vector<std::shared_ptr<XamlCPP::Core::Instruction>>> instructionResult;
        int instructionIndex = -1;

    public:
        virtual ~InstructionParser()
        {
        }

        InstructionParser(Interfaces::IRuntimeTypeSource *typeSource);

    private:
        bool getEndOfStream() const;
        void setEndOfStream(const bool &value);

        bool getCurrentNodeIsElement() const;

        XamlCPP::Core::NodeType getCurrentNodeType() const;

        bool getIsNestedPropertyImplicit() const;

        bool getCurrentNodeIsText() const;

        ProtoInstruction *getCurrent() const;
        std::string getCurrentText() const;
        std::string getCurrentPropertyText() const;
        ReflEx::MemberBase *getCurrentMember() const;

    public:
        std::shared_ptr<std::vector<std::shared_ptr<XamlCPP::Core::Instruction>>> Parse(const std::vector<std::shared_ptr<XamlCPP::Core::ProtoInstruction>>& protoNodes) override;

        void AddInstruction(std::shared_ptr<XamlCPP::Core::Instruction> const& inst);
        Interfaces::IRuntimeTypeSource * GetTypeSource() const {return typeSource; }

    private:
        void ParseElements();

        void SkipTextNodes();

        void ParseEmptyElement();

        void ParseNonEmptyElement();

        void ParseItemsOfCollection();

        void InjectNodesForTypeThatRequiresInitialization();

        void ReadEndTag();

        void ParseNestedProperties(Proto::XamlType *parentType);

        void ParseContentPropertyIfAny(Proto::XamlType *parentType);

        void SetNextInstruction();

        void ParseCollectionInsideThisProperty(ReflEx::MemberBase *member);

        bool getIsBeginingOfImplicitCollection() const;

        void ParseNestedProperty(ReflEx::MemberBase *member);

        void ParseInnerContentOfNestedProperty();

        void ParseMembersOfObject();

        void ParseMarkupExtension(const std::string &valueOfMember);

        void ParsePrefixDefinitions();

        static bool IsMarkupExtension(const std::string &text);
    };
}

#endif	//#ifndef __XAMLCPP_INSTRUCTIONPARSER_H__

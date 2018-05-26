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

#pragma once

#include <System/System.h>
#include "Reflection/Runtime/Field.h"
#include <standard/misc.h>

namespace XamlCPP::Interfaces
{
    struct IValueContext;
}

namespace XamlCPP::Core
{
    struct MarkupExtensionNode;
    struct TreeNode;

    struct Option : System::EnableSharedFromThisEx<Option>
    {
        virtual ~Option() {}
    };

    struct PositionalOption : Option
    {
        std::string Identifier;

        PositionalOption(std::string identifier)
        {
            Identifier = identifier;
        }

        virtual ~PositionalOption() {}
    };

    struct PropertyOption : Option
    {
        std::string Property;
        std::shared_ptr<TreeNode> Value = nullptr;

        PropertyOption(std::string const& property, std::shared_ptr<TreeNode> const& value)
        {
            Property = property;
            Value = value;
        }

        virtual ~PropertyOption() {}
    };

    typedef std::vector<std::shared_ptr<Option>> OptionsCollection;

    struct AssignmentNode
    {
        std::string Property;
        std::shared_ptr<TreeNode> NodeValue = nullptr;

        AssignmentNode(std::string const& property, std::shared_ptr<TreeNode> const& node)
        {
            Property = property;
            NodeValue = node;
        }
    };

    struct IdentifierNode
    {
        std::string Prefix;
        std::string TypeName;

        IdentifierNode()
        {

        }

        IdentifierNode(std::string const& typeName) : IdentifierNode("", typeName)
        {
        }

        IdentifierNode(std::string const& prefix, std::string const& typeName)
        {
            TypeName = typeName;
            Prefix = prefix;
        }
    };

    struct MarkupExtensionNode;
    struct StringNode;

    struct TreeNode
    {
        virtual ~TreeNode() {}

        virtual MarkupExtensionNode* ToMarkupExtensionNode() {
            return nullptr;
        }
        virtual StringNode* ToStringNode() {
            return nullptr;
        }
    };

    struct StringNode : TreeNode
    {
        std::string Value;

        StringNode(std::string const& value)
        {
            Value = value;
        }

        virtual ~StringNode() {}

        StringNode* ToStringNode() override final {
            return this;
        }
    };

    struct MarkupExtensionNode : TreeNode
    {
        OptionsCollection Options;
        IdentifierNode Identifier;

        MarkupExtensionNode()
        {
        }

        MarkupExtensionNode(IdentifierNode identifier) : MarkupExtensionNode(identifier, std::move(OptionsCollection()))
        {
        }

        MarkupExtensionNode(IdentifierNode identifier, OptionsCollection const&& options)
            : Options(std::move(options))
            , Identifier(identifier)
        {
        }

        virtual ~MarkupExtensionNode() {}

        MarkupExtensionNode* ToMarkupExtensionNode() override final {
            return this;
        }
    };
}

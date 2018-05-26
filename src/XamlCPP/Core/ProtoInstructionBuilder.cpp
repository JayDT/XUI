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

#include "CoreTypes.h"
#include "ReflEx/Member.h"
#include "ReflEx/AttachableMember.h"
#include "Proto/XamlType.h"
#include "ProtoInstructionBuilder.h"
#include "Proto/PrefixRegistration.h"
#include "Core/ReflEx/NamespaceDeclaration.h"

using namespace XamlCPP;

XamlCPP::Core::ProtoInstructionBuilder::ProtoInstructionBuilder(Interfaces::IRuntimeTypeSource *typeSource)
    : typeSource(typeSource)
{
}


XamlCPP::Core::ProtoInstructionBuilder::~ProtoInstructionBuilder()
{
}

XamlCPP::Core::PrefixRegistrationMode XamlCPP::Core::ProtoInstructionBuilder::getPrefixRegistrationMode() const
{
    return privatePrefixRegistrationMode;
}

std::shared_ptr<XamlCPP::Core::ProtoInstruction> XamlCPP::Core::ProtoInstructionBuilder::None()
{
    std::shared_ptr<XamlCPP::Core::ProtoInstruction> proto = std::make_shared<XamlCPP::Core::ProtoInstruction>();
    proto->Namespace = nullptr;
    proto->XamlType = nullptr;
    proto->NodeType = NodeType::None;
    return proto;
}

std::shared_ptr<XamlCPP::Core::ProtoInstruction> XamlCPP::Core::ProtoInstructionBuilder::NamespacePrefixDeclaration(ReflEx::NamespaceDeclaration *ns)
{
    return NamespacePrefixDeclaration(ns->GetPrefix(), ns->GetNamespace());
}

std::shared_ptr<XamlCPP::Core::ProtoInstruction> XamlCPP::Core::ProtoInstructionBuilder::NamespacePrefixDeclaration(const std::string &prefix, const std::string &ns)
{
    if (getPrefixRegistrationMode() == PrefixRegistrationMode::Automatic)
    {
        auto prefixRegistration = Proto::PrefixRegistration(prefix, ns);
        if (typeSource->RegisteredPrefixes.find(prefixRegistration) == typeSource->RegisteredPrefixes.end())
        {
            typeSource->RegisterPrefix(prefixRegistration);
        }
    }

    std::shared_ptr<XamlCPP::Core::ProtoInstruction> proto = std::make_shared<XamlCPP::Core::ProtoInstruction>();
    proto->Namespace = ns;
    proto->XamlType = nullptr;
    proto->NodeType = NodeType::PrefixDefinition;
    proto->Prefix = prefix;
    return proto;
}

std::shared_ptr<XamlCPP::Core::ProtoInstruction> XamlCPP::Core::ProtoInstructionBuilder::Element(const System::Reflection::Type *type, ReflEx::NamespaceDeclaration *nsDecl, bool isEmpty)
{
    std::shared_ptr<XamlCPP::Core::ProtoInstruction> proto = std::make_shared<XamlCPP::Core::ProtoInstruction>();
    proto->Namespace = nsDecl->GetNamespace();
    proto->Prefix = nsDecl->GetPrefix();
    proto->XamlType = typeSource->GetByType(type);
    proto->NodeType = isEmpty ? NodeType::EmptyElement : NodeType::Element;
    return proto;
}

template<typename T>
std::shared_ptr<XamlCPP::Core::ProtoInstruction> XamlCPP::Core::ProtoInstructionBuilder::NonEmptyElement(ReflEx::NamespaceDeclaration *nsDecl)
{
    return Element(typeof(T), nsDecl, false);
}

std::shared_ptr<XamlCPP::Core::ProtoInstruction> XamlCPP::Core::ProtoInstructionBuilder::NonEmptyElement(const System::Reflection::Type *type, ReflEx::NamespaceDeclaration *nsDecl)
{
    return Element(type, nsDecl, false);
}

std::shared_ptr<XamlCPP::Core::ProtoInstruction> XamlCPP::Core::ProtoInstructionBuilder::EmptyElement(const System::Reflection::Type *type, ReflEx::NamespaceDeclaration *nsDecl)
{
    return Element(type, nsDecl, true);
}

std::shared_ptr<XamlCPP::Core::ProtoInstruction> XamlCPP::Core::ProtoInstructionBuilder::NonEmptyPropertyElement(const System::Reflection::Type *type, const std::string &memberName, ReflEx::NamespaceDeclaration *namespaceDeclaration)
{
    return PropertyElement(type, memberName, namespaceDeclaration, false);
}

std::shared_ptr<XamlCPP::Core::ProtoInstruction> XamlCPP::Core::ProtoInstructionBuilder::PropertyElement(const System::Reflection::Type *type, const std::string &memberName, ReflEx::NamespaceDeclaration *namespaceDeclaration, bool isCollapsed)
{
    auto property_ = typeSource->GetByType(type)->GetMember(memberName);
    std::shared_ptr<XamlCPP::Core::ProtoInstruction> proto = std::make_shared<XamlCPP::Core::ProtoInstruction>();
    proto->PropertyElement = property_;
    proto->Prefix = namespaceDeclaration->GetPrefix();
    proto->Namespace = namespaceDeclaration->GetNamespace();
    proto->XamlType = nullptr;
    proto->NodeType = isCollapsed ? NodeType::EmptyPropertyElement : NodeType::PropertyElement;
    return proto;
}

template<typename T>
std::shared_ptr<XamlCPP::Core::ProtoInstruction> XamlCPP::Core::ProtoInstructionBuilder::PropertyElement(std::function<void*(T)> const& selector, ReflEx::NamespaceDeclaration *namespaceDeclaration, bool isCollapsed)
{
    return PropertyElement(T::typeid, selector->GetFullPropertyName(), namespaceDeclaration, isCollapsed);
}

std::shared_ptr<XamlCPP::Core::ProtoInstruction> XamlCPP::Core::ProtoInstructionBuilder::EndTag()
{
    std::shared_ptr<XamlCPP::Core::ProtoInstruction> proto = std::make_shared<XamlCPP::Core::ProtoInstruction>();
    proto->Namespace = "";
    proto->XamlType = nullptr;
    proto->NodeType = NodeType::EndTag;
    return proto;
}

std::shared_ptr<XamlCPP::Core::ProtoInstruction> XamlCPP::Core::ProtoInstructionBuilder::Text(const std::string &text)
{
    std::shared_ptr<XamlCPP::Core::ProtoInstruction> proto = std::make_shared<XamlCPP::Core::ProtoInstruction>();
    proto->Namespace = "";
    proto->NodeType = NodeType::Text;
    proto->XamlType = nullptr;
    proto->Text = text;
    return proto;
}

std::shared_ptr<XamlCPP::Core::ProtoInstruction> XamlCPP::Core::ProtoInstructionBuilder::Attribute(ReflEx::MemberBase *member, const std::string &value, const std::string &prefix)
{
    std::shared_ptr<XamlCPP::Core::ProtoInstruction> proto = std::make_shared<XamlCPP::Core::ProtoInstruction>();
    proto->PropertyAttribute = member;
    proto->NodeType = NodeType::Attribute;
    proto->PropertyAttributeText = value;
    proto->Prefix = prefix;
    return proto;
}

template<typename T>
std::shared_ptr<XamlCPP::Core::ProtoInstruction> XamlCPP::Core::ProtoInstructionBuilder::Attribute(std::function<void*(T)> const& selector, const std::string &value, const std::string &prefix)
{
    auto xamlMember = typeSource::GetByType(T::typeid).GetMember(selector->GetFullPropertyName());
    return Attribute(xamlMember, value, prefix);
}

std::shared_ptr<XamlCPP::Core::ProtoInstruction> XamlCPP::Core::ProtoInstructionBuilder::Key(const std::string &value)
{
    return Attribute(CoreTypes::getKey(), value, "x");
}

std::shared_ptr<XamlCPP::Core::ProtoInstruction> XamlCPP::Core::ProtoInstructionBuilder::Directive(ReflEx::Directive *directive, const std::string &value)
{
    return Attribute(directive, value, "x");
}

std::shared_ptr<XamlCPP::Core::ProtoInstruction> XamlCPP::Core::ProtoInstructionBuilder::ExpandedAttachedProperty(const System::Reflection::Type *owner, const std::string &name, ReflEx::NamespaceDeclaration *namespaceDeclaration)
{
    auto xamlType = typeSource->GetByType(owner);
    auto member = xamlType->GetAttachableMember(name);
    std::shared_ptr<XamlCPP::Core::ProtoInstruction> proto = std::make_shared<XamlCPP::Core::ProtoInstruction>();
    proto->Namespace = namespaceDeclaration->GetNamespace();
    proto->NodeType = NodeType::PropertyElement;
    proto->XamlType = nullptr;
    proto->PropertyElement = member;
    proto->Prefix = namespaceDeclaration->GetPrefix();
    return proto;
}

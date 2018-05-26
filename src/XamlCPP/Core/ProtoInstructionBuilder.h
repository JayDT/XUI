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

#ifndef __XAMLCPP_PROTOINSTRUCTIONPARSER_H__
#define __XAMLCPP_PROTOINSTRUCTIONPARSER_H__

#include "Core/ProtoInstruction.h"

#include <string>
#include <functional>
#include <memory>

#include "Interfaces/IRuntimeTypeSource.h"

namespace XamlCPP::Core
{
    namespace Proto
    {
        class XamlType;
    };

    namespace ReflEx
    {
        struct Directive;
        struct MemberBase;
        class NamespaceDeclaration;
    };

    enum PrefixRegistrationMode
    {
        Automatic,
        DontRegister
    };

    class ProtoInstructionBuilder
    {
    private:
        XamlCPP::Core::PrefixRegistrationMode privatePrefixRegistrationMode = XamlCPP::Core::PrefixRegistrationMode::Automatic;

        Interfaces::IRuntimeTypeSource *const typeSource;

    public:

        ProtoInstructionBuilder(Interfaces::IRuntimeTypeSource *typeSource);
        ~ProtoInstructionBuilder();

        // ReSharper disable once MemberCanBePrivate.Global
        XamlCPP::Core::PrefixRegistrationMode getPrefixRegistrationMode() const;

        std::shared_ptr<XamlCPP::Core::ProtoInstruction> None();

        std::shared_ptr<XamlCPP::Core::ProtoInstruction> NamespacePrefixDeclaration(ReflEx::NamespaceDeclaration *ns);

        std::shared_ptr<XamlCPP::Core::ProtoInstruction> NamespacePrefixDeclaration(const std::string &prefix, const std::string &ns);

    private:
        std::shared_ptr<XamlCPP::Core::ProtoInstruction> Element(const System::Reflection::Type *type, ReflEx::NamespaceDeclaration *nsDecl, bool isEmpty);

    public:
        template<typename T>
        std::shared_ptr<XamlCPP::Core::ProtoInstruction> NonEmptyElement(ReflEx::NamespaceDeclaration *nsDecl = nullptr);

        std::shared_ptr<XamlCPP::Core::ProtoInstruction> NonEmptyElement(const System::Reflection::Type *type, ReflEx::NamespaceDeclaration *nsDecl = nullptr);

        std::shared_ptr<XamlCPP::Core::ProtoInstruction> EmptyElement(const System::Reflection::Type *type, ReflEx::NamespaceDeclaration *nsDecl);

        template<typename T>
        std::shared_ptr<XamlCPP::Core::ProtoInstruction>  EmptyElement()
        {
            return EmptyElement<T>(nullptr);
        }

        template<typename T>
        std::shared_ptr<XamlCPP::Core::ProtoInstruction>  EmptyElement(ReflEx::NamespaceDeclaration *namespaceDeclaration)
        {
            return EmptyElement(typeof(T), namespaceDeclaration);
        }


    public:

        template<typename TParent>
        std::shared_ptr<XamlCPP::Core::ProtoInstruction>  InlineAttachableProperty(const std::string &name, const std::string &value, ReflEx::NamespaceDeclaration *namespaceDeclaration)
        {
            auto type = TParent::typeid;
            auto xamlType = typeSource->GetByType(type);

            auto member = xamlType->GetAttachableMember(name);

            std::shared_ptr<XamlCPP::Core::ProtoInstruction> proto = std::make_shared<XamlCPP::Core::ProtoInstruction>();
            proto->Namespace = nullptr;
            proto->NodeType = NodeType::Attribute;
            proto->XamlType = nullptr;
            proto->PropertyAttribute = member;
            proto->PropertyAttributeText = value;
            proto->Prefix = namespaceDeclaration->Prefix;
            return proto;
        }

        template<typename T>
        std::shared_ptr<XamlCPP::Core::ProtoInstruction>  EmptyPropertyElement(std::function<void*(T)> const& selector, ReflEx::NamespaceDeclaration *namespaceDeclaration)
        {
            return PropertyElement(selector, namespaceDeclaration, true);
        }

        template<typename T>
        std::shared_ptr<XamlCPP::Core::ProtoInstruction>  NonEmptyPropertyElement(std::function<void*(T)> const& selector, ReflEx::NamespaceDeclaration *namespaceDeclaration)
        {
            return PropertyElement(selector, namespaceDeclaration, false);
        }

        std::shared_ptr<XamlCPP::Core::ProtoInstruction>  NonEmptyPropertyElement(const System::Reflection::Type *type, const std::string &memberName, ReflEx::NamespaceDeclaration *namespaceDeclaration);

    private:
        std::shared_ptr<XamlCPP::Core::ProtoInstruction> PropertyElement(const System::Reflection::Type *type, const std::string &memberName, ReflEx::NamespaceDeclaration *namespaceDeclaration, bool isCollapsed);
        template<typename T>
        std::shared_ptr<XamlCPP::Core::ProtoInstruction> PropertyElement(std::function<void*(T)> const& selector, ReflEx::NamespaceDeclaration *namespaceDeclaration, bool isCollapsed);
    public:
        std::shared_ptr<XamlCPP::Core::ProtoInstruction> EndTag();
        std::shared_ptr<XamlCPP::Core::ProtoInstruction> Text(const std::string &text = "");
        std::shared_ptr<XamlCPP::Core::ProtoInstruction> Attribute(ReflEx::MemberBase *member, const std::string &value, const std::string &prefix);
        template<typename T>
        std::shared_ptr<XamlCPP::Core::ProtoInstruction> Attribute(std::function<void*(T)> const& selector, const std::string &value, const std::string &prefix);
        std::shared_ptr<XamlCPP::Core::ProtoInstruction> Key(const std::string &value);
        std::shared_ptr<XamlCPP::Core::ProtoInstruction> Directive(ReflEx::Directive *directive, const std::string &value);
        template<typename TParent>
        std::shared_ptr<XamlCPP::Core::ProtoInstruction> ExpandedAttachedProperty(const std::string &name, ReflEx::NamespaceDeclaration *namespaceDeclaration)
        {
            return ExpandedAttachedProperty(typeof(TParent), name, namespaceDeclaration);
        }
        std::shared_ptr<XamlCPP::Core::ProtoInstruction> ExpandedAttachedProperty(const System::Reflection::Type *owner, const std::string &name, ReflEx::NamespaceDeclaration *namespaceDeclaration);

    };
}

#endif //!__XAMLCPP_PROTOINSTRUCTIONPARSER_H__

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

#include "AttributeFeed.h"
#include "AttributeAssignment.h"
#include "DirectiveAssignment.h"
#include "UnprocessedAttributeBase.h"
#include "CoreTypes.h"
#include "PropertyLocator.h"
#include "Interfaces/IXamlCPP.h"

using namespace XamlCPP::Core::ProtoParser;

AttributeFeed::AttributeFeed(std::vector<AttributeAssignment>& assignments, const std::vector<NsPrefix>& specialPrefix)
    : specialPrefix(specialPrefix)
{
    for (auto& assign : assignments)
    {
        if (IsNamespaceDefinition(&assign))
            ParseNsDefinition(&assign);
        else if (IsDirective(&assign))
            ParseDirectives(&assign);
        else
            attributes.emplace_back(assign);
    }
}

bool AttributeFeed::IsAttribute(AttributeAssignment *assignment)
{
    return !IsDirective(assignment) && !IsNamespaceDefinition(assignment);
}

void AttributeFeed::ParseNsDefinition(UnprocessedAttributeBase *assignment)
{
    if (assignment->GetLocator()->Prefix == "xmlns")
    {
        prefixDefinitions.emplace_back(assignment->GetLocator()->PropertyName, assignment->GetValue());
        return;
    }
    if (assignment->GetLocator()->PropertyName == "xmlns")
    {
        prefixDefinitions.emplace_back("", assignment->GetValue());
        return;
    }

    throw XamlCPP::ParseException(System::String::format("Cannot extract a Namespace Prefix Definition from this assignment: %s", "assignment.ToString()"));
}

bool AttributeFeed::IsNamespaceDefinition(AttributeAssignment *assignment)
{
    return assignment->GetLocator()->GetIsNsPrefixDefinition();
}

void AttributeFeed::ParseDirectives(AttributeAssignment *assignment)
{
    auto xamlDirective = CoreTypes::GetDirective(assignment->GetLocator()->PropertyName);
    directives.emplace_back(xamlDirective, assignment->GetValue());

    if (CoreTypes::GetDirectiveKind(xamlDirective) == XamlCPP::Core::Assembler::Commands::DirectiveKind::Class)
        classOverride = std::make_shared<DirectiveAssignment>(xamlDirective, assignment->GetValue());
}

bool AttributeFeed::IsDirective(AttributeAssignment *attributeAssignment)
{
    if (!specialPrefix.empty())
    {
        for (auto const& prens : specialPrefix)
            if (attributeAssignment->GetLocator()->Prefix == prens.Prefix)
                return true;
    }
    return false;
}

std::vector<AttributeAssignment> const& AttributeFeed::GetRawAttributes() const
{
    return attributes;
}

std::vector<NsPrefix> const& AttributeFeed::GetPrefixRegistrations() const
{
    return prefixDefinitions;
}

std::vector<DirectiveAssignment> const& AttributeFeed::GetDirectives() const
{
    return directives;
}
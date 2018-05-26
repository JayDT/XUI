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

#include "AttributeParser.h"
#include "IXmlReader.h"
#include "AttributeFeed.h"
#include "AttributeAssignment.h"
#include "PropertyLocator.h"
#include "CoreTypes.h"

using namespace XamlCPP;
using namespace XamlCPP::Core::ProtoParser;

AttributeParser::AttributeParser()
{
}

std::shared_ptr<AttributeFeed> AttributeParser::Read(Interfaces::IXmlReader *reader)
{
    std::vector<AttributeAssignment> attributeAssignments;

    if (reader->MoveToFirstAttribute())
    {
        do
        {
            AttributeAssignment attr(PropertyLocator::Parse(reader->Name()), reader->Value());
            attributeAssignments.push_back(attr);

        } while (reader->MoveToNextAttribute());

    }

    reader->MoveToElement();

    ScanSpecialNsAndSetIfNoPreviousValue(attributeAssignments);

    return std::make_shared<AttributeFeed>(std::move(attributeAssignments), m_BuildInNs);
}

void AttributeParser::ScanSpecialNsAndSetIfNoPreviousValue(std::vector<AttributeAssignment>& attributeAssignments)
{
    if (!m_BuildInNs.empty())
        return;

    for (auto const& atns : attributeAssignments)
    {
        if (IsNsThatMatchesSpecialValue((AttributeAssignment *)&atns))
            m_BuildInNs.emplace_back(atns.GetLocator()->PropertyName, atns.GetValue());
    }
}

bool AttributeParser::IsNsThatMatchesSpecialValue(AttributeAssignment *assignment)
{
    if (assignment->GetLocator()->GetIsNsPrefixDefinition())
    {
        int count = System::Collection::GetArrLength(CoreTypes::SpecialNamespace);
        for (int i = 0; i != count; ++i)
        {
            if (assignment->GetValue() == CoreTypes::SpecialNamespace[i])
                return true;
        }
    }
    return false;
}

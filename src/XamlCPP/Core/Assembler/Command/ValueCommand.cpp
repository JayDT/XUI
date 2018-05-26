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

#include "ValueCommand.h"
#include "Core/Assembler/StateCommuter.h"
#include "Core/Assembler/Level/CurrentLevelWrapper.h"
#include "Core/ReflEx/TypeConversion/CommonValueConversion.h"
#include "Interfaces/IXamlCPP.h"

using namespace XamlCPP;
using namespace XamlCPP::Core::Assembler::Commands;

ValueCommand::ValueCommand(std::shared_ptr<XamlCPP::Core::Assembler::StateCommuter> const& stateCommuter, Interfaces::IValueContext *valueContext, const std::string &value)
    : Command(stateCommuter)
    , valueContext(valueContext)
    , value(value)
{
}

void ValueCommand::Execute()
{
    switch (m_StateCommuter->getValueProcessingMode())
    {
        case ValueProcessingMode::InitializationValue:
        {
            System::Reflection::Variant compatibleValue;
            if (ReflEx::TypeConversion::CommonValueConversion::TryConvert(value, m_StateCommuter->getCurrent()->getXamlType(), valueContext, compatibleValue))
                m_StateCommuter->getCurrent()->setInstance(compatibleValue);
            m_StateCommuter->setValueProcessingMode(ValueProcessingMode::AssignToMember);
            break;

        }
        case ValueProcessingMode::Key:
            m_StateCommuter->SetKey(value);
            m_StateCommuter->setValueProcessingMode(ValueProcessingMode::AssignToMember);
            break;

        case ValueProcessingMode::ConstructionParameter:
            m_StateCommuter->AddCtorArgument(value);
            break;

        case ValueProcessingMode::AssignToMember:
            m_StateCommuter->RaiseLevel();
            m_StateCommuter->getCurrent()->setInstance(value);
            m_StateCommuter->AssociateCurrentInstanceToParent();
            m_StateCommuter->DecreaseLevel();
            break;

        case ValueProcessingMode::Name:
        {
            m_StateCommuter->SetNameForCurrentInstance(value);
            if (m_StateCommuter->getCurrent()->getInstance().GetBase() && m_StateCommuter->getCurrent()->getInstance().ToObject())
            {
                const System::Reflection::Field& nameField = m_StateCommuter->getCurrent()->getInstance().ToObject()->GetType()->GetRuntimeProperty("Name");
                if (nameField.IsValid())
                    nameField.SetValue(m_StateCommuter->getCurrent()->getInstance().ToObject(), value);
            }
            m_StateCommuter->setValueProcessingMode(ValueProcessingMode::AssignToMember);
            break;
        }

        case ValueProcessingMode::Class:
            if (m_StateCommuter->getValueContext()->ClrClass.empty())
            {
                m_StateCommuter->getCurrent()->GetLevel()->SetIsTopLevel(true);
                m_StateCommuter->getValueContext()->ClrClass = value;
            }
            else
                throw XamlCPP::ParseException("Too many Class directive");
            m_StateCommuter->setValueProcessingMode(ValueProcessingMode::AssignToMember);
            break;

        case ValueProcessingMode::SubClass:
            if (m_StateCommuter->getValueContext()->ClrSubClass.empty())
                m_StateCommuter->getValueContext()->ClrSubClass = value;
            else
                throw XamlCPP::ParseException("Too many SubClass directive");
            m_StateCommuter->setValueProcessingMode(ValueProcessingMode::AssignToMember);
            break;

        default:
            throw ParseException("Unexpected mode was set trying to process a Value XAML instruction. " + System::String::format("We found \"%d\") and it cannot be handled.", m_StateCommuter->getValueProcessingMode()));
    }
}

std::string ValueCommand::ToString() const
{
    return System::String::format("Value: %s", value.c_str());
}


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

#include "EndMemberCommand.h"
#include "XamlType.h"
#include "Assembler/StateCommuter.h"
#include "Assembler/Level/CurrentLevelWrapper.h"
#include "Assembler/ConstructionArgument.h"
#include "ReflEx/TypeConversion/CommonValueConversion.h"

#include "cpplinq/linq.hpp"

using namespace XamlCPP;
using namespace XamlCPP::Core;
using namespace XamlCPP::Core::Assembler::Commands;

EndMemberCommand::EndMemberCommand(Interfaces::ITypeRepository *typeRepository, std::shared_ptr<XamlCPP::Core::Assembler::StateCommuter> const& stateCommuter, Interfaces::IValueContext *valueContext)
    : Command(stateCommuter)
    , typeRepository(typeRepository)
    , valueContext(valueContext)
{
}

void EndMemberCommand::Execute()
{
    if (m_StateCommuter->getValueProcessingMode() == ValueProcessingMode::ConstructionParameter)
    {
        AdaptCurrentCtorArgumentsToCurrentType();
        m_StateCommuter->setValueProcessingMode(ValueProcessingMode::AssignToMember);
    }

    if (getIsTherePendingInstanceWaitingToBeAssigned())
    {
        m_StateCommuter->AssociateCurrentInstanceToParent();
        m_StateCommuter->DecreaseLevel();
    }
}

bool EndMemberCommand::getIsTherePendingInstanceWaitingToBeAssigned() const
{
    return m_StateCommuter->getCurrent()->getHasInstance() && m_StateCommuter->getCurrent()->getMember() == nullptr;
}

void EndMemberCommand::AdaptCurrentCtorArgumentsToCurrentType()
{
    auto& arguments = const_cast<std::vector<ConstructionArgument>&>(m_StateCommuter->getCurrent()->getCtorArguments());
    auto xamlTypes = GetTypesOfBestCtorMatch(m_StateCommuter->getCurrent()->getXamlType(), arguments.size());

    auto i = 0;
    for (auto& ctorArg : arguments)
    {
        auto targetType = xamlTypes[i];
        System::Reflection::Variant compatibleValue;
        ReflEx::TypeConversion::CommonValueConversion::TryConvert(ctorArg.getStringValue(), targetType, valueContext, compatibleValue);
        ctorArg.setValue(compatibleValue);
        i++;
    }
}

std::vector<Proto::XamlType*> EndMemberCommand::GetTypesOfBestCtorMatch(Proto::XamlType *xamlType, int count)
{
    auto constructor = SelectConstructor(xamlType, count);

    std::vector<Proto::XamlType*> result;
    result.reserve(constructor->GetParameters().size());
    for (auto param : constructor->GetParameters())
    {
        System::Reflection::Type const* type = &typeofbytypeid(param);
        result.push_back(type != nullptr ? typeRepository->GetByType(type) : (Proto::XamlType*)nullptr);
    }

    return std::move(result);
}

const System::Reflection::meta::Constructor *EndMemberCommand::SelectConstructor(Proto::XamlType *xamlType, int count)
{
    auto declaredConstructors = xamlType->UnderlyingType()->GetConstructors();

    if (declaredConstructors)
    {
        for (System::Reflection::meta::Constructor* con : *declaredConstructors)
        {
            if (con->GetParameters().size() == count)
                return con;
        }
    }

    return nullptr;
}

std::string EndMemberCommand::ToString() const
{
    return "End of Member";
}

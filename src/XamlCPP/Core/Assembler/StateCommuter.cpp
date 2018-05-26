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

#include "StateCommuter.h"
#include "Core/Assembler/TopDownValueContext.h"
#include "Level/CurrentLevelWrapper.h"
#include "Level/PreviousLevelWrapper.h"
#include "InstanceProperties.h"
#include "Level/NullLevel.h"
#include "TypeOperations.h"
#include "ConstructionArgument.h"
#include "ReflEx/MutableMember.h"
#include "ReflEx/TypeConversion/CommonValueConversion.h"
#include "Interfaces/IBinding.h"
#include "Interfaces/IXamlCPP.h"
#include "Interfaces/ISupportInitialize.h"
#include "Interfaces/ISetter.h"
#include "Interfaces/ITriggerBase.h"
#include "Core/CoreTypes.h"
#include "Core/Dependencies/IPropertyObject.h"

#include "Command/ITopDownValueContext.h"

#include "cpplinq/linq.hpp"

using namespace XamlCPP;
using namespace XamlCPP::Core::Assembler;

NullLevel NullLevel::Empty;

StateCommuter::StateCommuter(std::vector<XamlCPP::Core::Assembler::Level*>* stack, Interfaces::IRuntimeTypeSource *typeSource, Interfaces::IInstanceLifeCycleListener *lifecycleListener, Interfaces::IValueContext *valueContext, bool isTemplateLoader /*= false*/)
    : lifecycleListener(lifecycleListener)
    , valueContext(valueContext)
    , isTemplateLoader(isTemplateLoader)
{
    System::ThrowIfFailed<System::InvalidOperationException>(typeSource, "typeSource");

    setStack(stack);
    valueContext->m_StateCommuter = this;
}

CurrentLevelWrapper *StateCommuter::getCurrent() const
{
    return const_cast<CurrentLevelWrapper*>(&m_Current);
}

PreviousLevelWrapper *StateCommuter::getPrevious() const
{
    return const_cast<PreviousLevelWrapper*>(&m_Previous);
}

int StateCommuter::getLevel() const
{
    return stack->size();
}

bool StateCommuter::getHasParentToAssociate() const
{
    return getLevel() > 1;
}

bool StateCommuter::getWasAssociatedRightAfterCreation() const
{
    return getCurrent()->getWasAssociatedRightAfterCreation();
}

Interfaces::IValueContext *StateCommuter::getValueContext() const
{
    return valueContext;
}

ValueProcessingMode StateCommuter::getValueProcessingMode() const
{
    return m_ValueProcessingMode;
}

void StateCommuter::setValueProcessingMode(ValueProcessingMode value)
{
    m_ValueProcessingMode = value;
}

System::Reflection::Variant StateCommuter::getValueOfPreviousInstanceAndItsMember() const
{
    return GetValueTuple(getPrevious()->getInstance().ToObject(), static_cast<ReflEx::MutableMember*>(getPrevious()->getMember()));
}

std::vector<Level*> *StateCommuter::getStack() const
{
    return stack;
}

void StateCommuter::setStack(std::vector<Level*> *value)
{
    stack = value;
    if (stack)
        stack->reserve(16);
    UpdateLevelWrappers();
}

bool StateCommuter::getParentIsOneToMany() const
{
    return getPrevious()->getXamlMemberIsOneToMany();
}

InstanceProperties *StateCommuter::getInstanceProperties() const
{
    return getCurrent()->getInstanceProperties();
}

bool StateCommuter::getHasParent() const
{
    return !getPrevious()->getIsEmpty();
}

Interfaces::ITopDownValueContext *StateCommuter::getTopDownValueContext() const
{
    return valueContext->getTopDownValueContext();
}

void StateCommuter::SetKey(System::Reflection::Variant const& value)
{
    getInstanceProperties()->setKey(value);
}

void StateCommuter::AssignChildToParentProperty()
{
    ReflEx::MemberBase* member = getPrevious()->getMember();
	if (member->IsDirective())
	{
		//Core::Assembler::Commands::DirectiveKind kind = CoreTypes::GetDirectiveKind(static_cast<ReflEx::Directive*>(member));
		//if (kind == Core::Assembler::Commands::DirectiveKind::MarkupExtensionArguments)
		//{
		//}
		return; // Currenty not implement yet diretive assign
	}

    auto previousMember = member->ToMutableMember();
    auto xamlType = previousMember->XamlType;
    System::Reflection::Variant const& value = getCurrent()->getInstance();

    System::Reflection::Object* instance = getPrevious()->getInstance().ToObject();
    System::Reflection::Variant compatibleValue;

    if (value.IsObject() && value.ToObject()->Is<XamlCPP::Interfaces::IBinding>())
    {
        compatibleValue = value;
    }
    /*else if (*xamlType->UnderlyingType() == typeid(System::Reflection::Variant))
    {
        compatibleValue = getCurrent()->getInstance();
    }*/
    else
    {
        auto success = ReflEx::TypeConversion::CommonValueConversion::TryConvert(value, xamlType, valueContext, compatibleValue);

        if (!success)
        {
            compatibleValue = getCurrent()->getInstance();
        }
    }

    //System::ThrowIfFailed<System::NotSupportedException>(compatibleValue.GetBase());

    previousMember->SetValue(instance, compatibleValue, valueContext);
}

void StateCommuter::RaiseLevel()
{
    stack->push_back(new Level());
    UpdateLevelWrappers();
}

void StateCommuter::UpdateLevelWrappers()
{
    m_Current = CurrentLevelWrapper(!stack->empty() ? stack->back() : &NullLevel::Empty, valueContext);
    m_Previous = PreviousLevelWrapper(stack->size() > 1 ? (*++stack->rbegin()) : &NullLevel::Empty);
}

void StateCommuter::DecreaseLevel()
{
    if (stack->empty())
        throw ParseException("No valid end tag");

    delete stack->back();
    stack->pop_back();
    UpdateLevelWrappers();
}

void StateCommuter::CreateInstanceOfCurrentXamlTypeIfNotCreatedBefore()
{
    if (!getCurrent()->getHasInstance())
    {
        MaterializeInstanceOfCurrentType();
    }
}

void StateCommuter::MaterializeInstanceOfCurrentType()
{
    auto xamlType = getCurrent()->getXamlType();
    if (xamlType == nullptr)
    {
        throw ParseException("A type must be set before invoking MaterializeInstanceOfCurrentType");
    }
    auto parameters = GatherConstructionArguments();
    System::Reflection::Variant instance = xamlType->CreateInstance(parameters);

    System::ThrowIfFailed<System::NotImplementedException>(instance.ToObject(), "[XAML]: failed create xaml type");

    getCurrent()->setInstance(instance);
    if (instance.IsObject())
    {
        System::Reflection::Object* object = instance.ToObject();

        if (getCurrent()->GetLevel()->GetIsTopLevel() && instance.GetBase()->IsSharedPointer())
            SetTopLevel(instance.GetValue<System::Reflection::RefObject>());

        lifecycleListener->OnBegin(object);

        Interfaces::ISupportInitialize* initializer = object->Dynamic_As<Interfaces::ISupportInitialize>();
        if (initializer)
        {
            initializer->BeginInit();
            // WorkAround for correctly init NamedScope
            if (IsTemplateLoader())
                initializer->BeginInit();
        }
    }
}

System::Reflection::Variant StateCommuter::GetValueProvidedByMarkupExtension(Interfaces::IMarkupExtension *instance)
{
    Core::MarkupExtensionContext context;
    GetExtensionContext(context);
    return instance->ProvideValue(&context);
}

void StateCommuter::GetExtensionContext(Core::MarkupExtensionContext& context)
{
    context.setTargetObject(getPrevious()->getInstance());
    context.setTargetProperty(&getPrevious()->getInstance().GetType()->GetRuntimeProperty(getPrevious()->getMember()->Name));
    context.setValueContext(getValueContext());
}

std::vector<System::Reflection::Variant> StateCommuter::GatherConstructionArguments()
{
    auto arguments = cpplinq::from(getCurrent()->getCtorArguments()).select([&](ConstructionArgument const& argument)
    {
        return argument.getValue();
    }).to_vector();
    return arguments;
}

void StateCommuter::AddChildToCurrentCollection()
{
    TypeOperations::AddToCollection(getPrevious()->getCollection(), getCurrent()->getInstance());
}

void StateCommuter::AddCtorArgument(const std::string &stringValue)
{
    const_cast<std::vector<ConstructionArgument>&>(getCurrent()->getCtorArguments()).push_back(ConstructionArgument(stringValue));
}

void StateCommuter::AssociateCurrentInstanceToParent()
{
    if (getHasParentToAssociate() && !getCurrent()->getIsMarkupExtension())
    {
        if (getPrevious()->getCanHostChildren())
        {
            AddChildToHost();
        }
        else
        {
            AssignChildToParentProperty();
        }

        lifecycleListener->OnAssociatedToParent(getCurrent()->getInstance().ToObject());
    }
}

void StateCommuter::RegisterInstanceNameToNamescope()
{
    if (!getInstanceProperties()->getName().empty())
    {
        auto nameScope = FindNamescope();
        if (nameScope)
            nameScope->Register(getInstanceProperties()->getName(), getCurrent()->getInstance().TryGetValue<System::Reflection::RefObject>());
    }

    getInstanceProperties()->setName("");
    getInstanceProperties()->setHadPreviousName(false);
}

void StateCommuter::PutNameToCurrentInstanceIfAny()
{
    if (!getInstanceProperties()->getName().empty())
    {
        if (!getCurrent()->getInstanceName().empty())
        {
            getCurrent()->getInstanceProperties()->setHadPreviousName(true);
        }

        getCurrent()->setInstanceName(getInstanceProperties()->getName());
    }
}

Interfaces::INameScope *StateCommuter::FindNamescope()
{
    if (getCurrent()->getInstanceProperties()->getHadPreviousName())
    {
        return FindNamescopeForInstanceThatHadPreviousName();
    }
    else
    {
        return FindNamescopeForInstanceWithNoName();
    }
}

Interfaces::INameScope *StateCommuter::FindNamescopeForInstanceWithNoName()
{
    return FindNamescopeSkippingAncestor(0);
}

Interfaces::INameScope *StateCommuter::FindNamescopeForInstanceThatHadPreviousName()
{
    return FindNamescopeSkippingAncestor(1);
}

Interfaces::INameScope *StateCommuter::FindNamescopeSkippingAncestor(int skip)
{
    return cpplinq::from(*stack).skip(skip).select([&](Level* level)
    {
        return level->GetXamlType() ? level->GetXamlType()->GetNamescope(level->GetInstance().ToObject()) : nullptr;
    }).first_or_default([](Interfaces::INameScope * x) { return x != nullptr; });
}

void StateCommuter::AddChildToHost()
{
    if (getPrevious()->getIsMapContainer())
    {
        AddChildToDictionary();
    }
    else
    {
        AddChildToCurrentCollection();
    }
}

void StateCommuter::AddChildToDictionary()
{
    TypeOperations::AddToDictionary(static_cast<System::Collection::Generic::IMapContainer*>(getPrevious()->getCollection()), (System::Reflection::meta::Variant&)getInstanceProperties()->getKey(), getCurrent()->getInstance());
    ClearKey();
}

void StateCommuter::ClearKey()
{
    SetKey(nullptr);
}

System::Reflection::Variant StateCommuter::GetValueTuple(System::Reflection::Object *instance, Core::ReflEx::MutableMember *member)
{
    auto xamlMemberBase = member;
    return xamlMemberBase->GetValue(instance);
}

void StateCommuter::SetNameForCurrentInstance(const std::string &value)
{
    getInstanceProperties()->setName(value);
}

void StateCommuter::AssociateCurrentInstanceToParentForCreation()
{
    AssociateCurrentInstanceToParent();
    getCurrent()->setWasAssociatedRightAfterCreation(true);
}

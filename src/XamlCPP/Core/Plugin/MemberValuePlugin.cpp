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

#include "MemberValuePlugin.h"
#include "MutableMember.h"
#include "AttachableMember.h"
#include "DataBind/DelayedBinding.h"
#include "Core/Assembler/StoredInstance.h"
#include "Interfaces/IBinding.h"
#include "Interfaces/ReflAttributes.h"
#include "Interfaces/IParser.h"
#include "Interfaces/IStyle.h"
#include "Runtime/ReflectionModule.h"

XamlCPP::Core::Dependency::IPropertyObject * XamlCPP::Core::Plugin::MemberValuePlugin::GetDependencyProperty(System::Reflection::Object * instance, Core::ReflEx::MutableMember * member)
{
    Core::Dependency::IDependencyObject* depObj = static_cast<Core::Dependency::IDependencyObject*>(instance);

    if (member->ToAttachableMember())
    {
        //ReflEx::AttachableMember* attach = member->ToAttachableMember();
        //
        //attach->DeclaringType()
    }
    else
    {
        return depObj->FindRegisteredGlob(instance->GetType(), member->Name);
    }
    return nullptr;
}

bool XamlCPP::Core::Plugin::MemberValuePlugin::ApplyBinding(System::Reflection::Object * instance, Core::Dependency::IPropertyObject * property, Interfaces::IValueContext * valueContext, std::shared_ptr<Interfaces::IBinding> binding)
{
    static const System::Reflection::Type* ControlType = &System::Reflection::ReflectionModulManager::Instance().GetType("XUI::UI::Controls::Control");
    static const System::Reflection::Type* StyleType = &typeof(Interfaces::IStyle);

    System::ThrowIfFailed<System::AggregateException>(ControlType->IsValid(), "Required XUI framework");
    System::ThrowIfFailed<System::AggregateException>(StyleType->IsValid(), "Required Style framework");

    Core::Dependency::IDependencyObject* dependencyobject = static_cast<Core::Dependency::IDependencyObject*>(instance);

    if (dependencyobject->Is(*ControlType))
    {
        if (property == dependencyobject->GetDataContextProperty())
        {
            dependencyobject->Bind(property, binding.get());
            return true;
        }
        else
        {
            Markup::DelayedBinding::Add(dependencyobject->shared_from_this(), property, binding.get());
            return true;
        }
    }
    else if (valueContext->getTopDownValueContext())
    {
        auto const& storedInstances = valueContext->getTopDownValueContext()->GetStoredInstances();

        System::Reflection::Object* anchor = nullptr;
        int orderID = -1;
        for (auto const& si : storedInstances)
        {
            System::Reflection::Object* i = si.GetInstance();
            if (i && orderID < si.GetOrderID() && (i->Is(*ControlType) || (!anchor && i->Is(*StyleType))))
            {
                orderID = si.GetOrderID();
                anchor = si.GetInstance();
            }
        }

        if (anchor)
        {
            dependencyobject->Bind(property, binding.get(), anchor);
        }
    }

    return false;
}

XamlCPP::Core::Plugin::MemberValuePlugin::MemberValuePlugin(ReflEx::MutableMember * member)
    : member(member)
{
}

void XamlCPP::Core::Plugin::MemberValuePlugin::SetValue(System::Reflection::Object * instance, System::Reflection::Variant const& value, Interfaces::IValueContext * valueContext)
{
    if (value.IsObject() && value.ToObject()->Is<XamlCPP::Interfaces::IBinding>())
    {
        System::Reflection::meta::Field const* field = member->RuntimeProperty();
        if (field && !field->GetMeta().GetCustomAttribute<XamlCPP::AssignBindingAttribute>())
        {
            if (!instance->Is<Core::Dependency::IDependencyObject>())
                return;

            Core::Dependency::IPropertyObject* property = GetDependencyProperty(instance, member->ToMutableMember());
            if (property)
                ApplyBinding(instance, property, valueContext, value.GetValue<std::shared_ptr<Interfaces::IBinding>>());
            return;
        }
    }

    //if (member->)

    if (member->Setter)
    {
        if (member->Setter->IsStatic())
            member->Setter->Invoke(System::Reflection::Variant(nullptr), instance, value);
        else
            member->Setter->Invoke(System::Reflection::Variant(instance), value);
    }
    else if (member->IsAttachable())
    {
        const System::Reflection::IMethod* method = &member->DeclaringType->UnderlyingType()->GetRuntimeMethod("Set" + member->Name);
        if (method && method->IsValid())
            method->Invoke(System::Reflection::Variant(instance), value);
        else
        {
            auto static_method = member->DeclaringType->UnderlyingType()->GetStaticMethod("Set" + member->Name);
            if (static_method && static_method->IsValid())
                static_method->Invoke(System::Reflection::Variant(nullptr), instance, value);
            else
                throw System::NotImplementedException("Not found Setter");
        }
    }
    else
    {
        System::Reflection::meta::Field const* field = member->RuntimeProperty();
        if (field && field->IsValid())
        {
            if (*field->GetFieldType() == typeid(System::Reflection::Variant))
            {
                System::Reflection::Variant variant(value, ReferenceVariantPolicy);
                field->SetValue(instance, variant);
            }
            else
            {
                if (!field->SetValue(instance, value))
                    throw System::NotImplementedException("Not found Setter");
            }
        }
        else
        {
            const System::Reflection::meta::Field* field = member->RuntimeField();
            if (field->IsValid())
            {
                if (field->GetMeta().GetCustomAttribute<XamlCPP::EventAttribute>())
                {
                    if (value.toPointer())
                    {
                        System::Reflection::Variant eventHandler = field->GetValueReference(instance);
                        System::Reflection::meta::InvokableSignature invokeArg(1);
                        invokeArg[0] = value.GetType();
                        const System::Reflection::Method& method = field->GetFieldType()->GetRuntimeMethod("AddHandler", invokeArg);
                        if (method.IsValid())
                        {
                            System::Reflection::Argument arg(value);
                            System::Reflection::ArgumentList arguments(1);
                            arguments[0] = &arg;
                            method.InternalInvoke(eventHandler, arguments);
                        }
                    }
                }
            }
        }
    }
}

System::Reflection::Variant XamlCPP::Core::Plugin::MemberValuePlugin::GetValue(System::Reflection::Object * instance)
{
    if (member->Getter)
    {
        return member->Getter->Invoke(System::Reflection::Variant(instance));
    }
    else if (member->IsAttachable())
    {
        auto method = member->DeclaringType->UnderlyingType()->GetMethod("Get" + member->Name);
        if (method)
            return method->Invoke(System::Reflection::Variant(instance));
        else
        {
            auto static_method = member->DeclaringType->UnderlyingType()->GetStaticMethod("Get" + member->Name);
            if (static_method)
                return static_method->Invoke(instance);
        }
    }
    else
    {
        System::Reflection::meta::Field const* field = member->RuntimeProperty();
        if (field && field->IsValid())
            return field->GetValue(instance);
    }

    return System::Reflection::Variant();
}

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

#include "Binding.h"

#include "DataBind/SourceObserver.h"
#include "Runtime/ReflectionModule.h"
#include "Core/Dependencies/IDependencyObject.h"
#include "DataBind/InstancedBinding.h"
#include "Core/TypeExtensions/TypeConverter.h"
#include "XamlCPP/Core/Parser/MarkupExpressionAssembler.h"

#include "Interfaces/INamedScope.h"

#include "XamlCPP.h"

#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>

#include "G3D/Rect2D.h"

using namespace XamlCPP;

std::shared_ptr<DataBind::InstancedBinding> XamlCPP::DataBind::Binding::Initiate(Core::Dependency::IDependencyObject * target, Core::Dependency::IPropertyObject * targetProperty, System::Reflection::Object * anchor, bool enableDataValidation, std::shared_ptr<XamlCPP::Interfaces::INameScope> namedScope)
{
    System::ThrowIfFailed<System::ArgumentNullException>(target != nullptr);

    auto pathInfo = ParsePath(Path);
    ValidateState(pathInfo);
    enableDataValidation = enableDataValidation && BindingLevel == BindingLevel::LocalValue;

    std::shared_ptr<SourceObserver> observer;

    if (!pathInfo.ElementName.empty() || !ElementName.empty())
    {
        observer = CreateElementObserver(target,
            pathInfo.ElementName.empty() ? ElementName : pathInfo.ElementName,
            pathInfo.Path,
            namedScope);
    }
    else if (Source.GetBase())
    {
        observer = CreateSourceObserver(Source.TryGetValue<System::Reflection::RefObject>(), pathInfo.Path, enableDataValidation);
    }
    else if (RelativeSource->Mode == RelativeSourceMode::None || RelativeSource->Mode == RelativeSourceMode::DataContext)
    {
        observer = CreateDataContexObserver(
            target,
            pathInfo.Path,
            targetProperty == target->GetDataContextProperty(),
            anchor,
            enableDataValidation);
    }
    else if (RelativeSource->Mode == RelativeSourceMode::Self)
    {
        observer = CreateSourceObserver(target->shared_from_base_dynamic<System::Reflection::Object>(), pathInfo.Path, enableDataValidation);
    }
    else if (RelativeSource->Mode == RelativeSourceMode::TemplatedParent)
    {
        observer = CreateTemplatedParentObserver(target, pathInfo.Path);
    }
	else if (RelativeSource->Mode == RelativeSourceMode::FindAncestor)
	{
		observer = CreateRelativeSourceObserver(target, pathInfo.Path, anchor, enableDataValidation);
	}
	else
    {
        throw System::NotSupportedException();
    }

    auto fallback = FallbackValue;

    // If we're binding to DataContext and our fallback is UnsetValue then override
    // the fallback value to null, as broken bindings to DataContext must reset the
    // DataContext in order to not propagate incorrect DataContexts to child controls.
    // See Avalonia.Markup.Xaml.UnitTests.Data.DataContext_Binding_Should_Produce_Correct_Results.
    if (targetProperty == target->GetDataContextProperty() && !fallback.GetBase())
    {
        fallback = nullptr;
    }

    auto instancedBinding = std::make_shared<InstancedBinding>(observer, fallback, Mode, BindingLevel);

    instancedBinding->Converter = Converter ? Converter : &Core::TypeExtensions::DefaultValueConverter::Instance;
    instancedBinding->ConverterParameter = ConverterParameter;
    instancedBinding->TargetProperty = targetProperty;

    return instancedBinding;
}

std::shared_ptr<XamlCPP::DataBind::SourceObserverNode> XamlCPP::DataBind::Binding::GetSourceObserver()
{
    auto pathInfo = ParsePath(Path);
    Core::ExpressionAssembler parser(false);
    parser.Parse(pathInfo.Path);

    return parser.Nodes.front();
}

bool IsEmptyOrWhitespace(const char* s)
{
    const char* buffer = s;
    if (*buffer == '\0')
        return true;

    while (*buffer != '\0')
    {
        if (!isspace(*buffer))
            return false;
    }

    return true;
}

XamlCPP::DataBind::Binding::PathInfo XamlCPP::DataBind::Binding::ParsePath(std::string const & path)
{
    PathInfo result;

    if (IsEmptyOrWhitespace(path.c_str()) || path == ".")
    {
        result.Path = "";
    }
    else if (boost::starts_with(path, "#"))
    {
        size_t dot = path.find_first_of('.');
    
        if (dot != -1)
        {
            result.Path = path.substr(dot + 1);
            result.ElementName = path.substr(1, dot - 1);
        }
        else
        {
            result.Path = "";
            result.ElementName = path.substr(1);
        }
    }
    else
    {
        result.Path = path;
    }

    return result;
}

void XamlCPP::DataBind::Binding::ValidateState(PathInfo pathInfo)
{
    if (!pathInfo.ElementName.empty() && !ElementName.empty())
    {
        throw System::InvalidOperationException("ElementName property cannot be set when an #elementName path is provided.");
    }

    if ((!pathInfo.ElementName.empty() || !ElementName.empty()) && RelativeSource->Mode != RelativeSourceMode::Self && RelativeSource->Mode != RelativeSourceMode::None)
    {
        throw System::InvalidOperationException("ElementName property cannot be set with a RelativeSource.");
    }
}

std::shared_ptr<XamlCPP::DataBind::SourceObserver> XamlCPP::DataBind::Binding::CreateDataContexObserver(Core::Dependency::IDependencyObject * target, std::string path, bool targetIsDataContext, System::Reflection::meta::Object * anchor, bool enableDataValidation)
{
    static const System::Reflection::Type* ControlType = &System::Reflection::ReflectionModulManager::Instance().GetType("XUI::UI::Controls::Control");

    System::ThrowIfFailed<System::AggregateException>(ControlType->IsValid(), "Required XUI framework");
    System::ThrowIfFailed<System::ArgumentNullException>(target != nullptr);

    Core::Dependency::IDependencyObject * control = target;

    if (!target->Is(*ControlType))
    {
        control = anchor->Dynamic_As<Core::Dependency::IDependencyObject>();
        System::ThrowIfFailed<System::InvalidOperationException>(target->Is(*ControlType), "Cannot find a DataContext to bind to.");
    }

    if (targetIsDataContext)
    {
        return std::make_shared<DataBind::SourceObserver>(
            GetParentDataContext(control),
            path,
            enableDataValidation,
            path.c_str());
    }

    auto dependencyService = Xaml::UIServiceContext->Get<XamlCPP::Core::Dependency::IDependencyPropertyMgr>();
    auto DataContextProperty = dependencyService->FindRegistered(ControlType, "DataContext");

    auto update = control->GetObservable(DataContextProperty);
    return std::make_shared<DataBind::SourceObserver>(
        update,
        path,
        enableDataValidation,
        path.c_str());
}

std::shared_ptr<XamlCPP::DataBind::SourceObserver> XamlCPP::DataBind::Binding::CreateRelativeSourceObserver(Core::Dependency::IDependencyObject * target, std::string path, System::Reflection::Object * anchor, bool enableDataValidation)
{
	/// on attached logical tree find parent
	auto update = target->GetRelativeObservable(RelativeSource);
	return std::make_shared<DataBind::SourceObserver>(
		update,
		path,
		enableDataValidation,
		path.c_str());
}

std::shared_ptr<XamlCPP::DataBind::SourceObserver> XamlCPP::DataBind::Binding::CreateElementObserver(XamlCPP::Core::Dependency::IDependencyObject* target, std::string const & elementName, std::string const & path, std::shared_ptr<XamlCPP::Interfaces::INameScope> namedScope)
{
    System::ThrowIfFailed<System::ArgumentNullException>(target != nullptr);

    auto description = System::String::format("#%s.%s", elementName.c_str(), path.c_str());
    std::shared_ptr<XamlCPP::DataBind::SourceObserver> observer = std::make_shared<SourceObserver>(
        elementName,
        path,
        false,
        description.c_str());

    observer->Initialize(elementName, namedScope ? namedScope : target->GetSharedNamedScope());
    return observer;
}

std::shared_ptr<XamlCPP::DataBind::SourceObserver> XamlCPP::DataBind::Binding::CreateSourceObserver(System::Reflection::RefObject source, std::string const & path, bool enabledDataValidation)
{
    System::ThrowIfFailed<System::ArgumentNullException>(source != nullptr);

    return std::make_shared<SourceObserver>(source, path, enabledDataValidation);
}

std::shared_ptr<XamlCPP::DataBind::SourceObserver> XamlCPP::DataBind::Binding::CreateTemplatedParentObserver(Core::Dependency::IDependencyObject * target, std::string const & path)
{
    static const System::Reflection::Type* ControlType = &System::Reflection::ReflectionModulManager::Instance().GetType("XUI::UI::Controls::UIElement");
    System::ThrowIfFailed<System::AggregateException>(ControlType->IsValid(), "Required XUI framework");
    
    auto dependencyService = Xaml::UIServiceContext->Get<XamlCPP::Core::Dependency::IDependencyPropertyMgr>();
    auto TemplatedParentProperty = dependencyService->FindRegistered(ControlType, "TemplatedParent");
    System::ThrowIfFailed<System::ArgumentNullException>(TemplatedParentProperty != nullptr);

    auto update = target->GetObservable(TemplatedParentProperty);
    std::shared_ptr<XamlCPP::DataBind::SourceObserver> observer = std::make_shared<SourceObserver>(
        update,
        path,
        false,
        path.c_str());

    return observer;
}

std::shared_ptr<XamlCPP::Core::Dependency::GenericObservable> XamlCPP::DataBind::Binding::GetParentDataContext(XamlCPP::Core::Dependency::IDependencyObject* target)
{
    // The DataContext is based on the visual parent and not the logical parent: this may
    // seem unintuitive considering the fact that property inheritance works on the logical
    // tree, but consider a ContentControl with a ContentPresenter. The ContentControl's
    // Content property is bound to a value which becomes the ContentPresenter's 
    // DataContext - it is from this that the child hosted by the ContentPresenter needs to
    // inherit its DataContext.
    if (!target->GetDataContext().GetBase() || Xaml::IsUnsetValue(target->GetDataContext()))
    {
        Core::Dependency::IDependencyObject* visual = target;
        while (visual)
        {
            if (visual->GetDataContext().GetBase() && !Xaml::IsUnsetValue(visual->GetDataContext()))
                return target->GetObservable(target->GetDataContextProperty());

            visual = visual->GetIDepVisualParent();
        }
    }
    else
        return target->GetObservable(target->GetDataContextProperty());

    return std::shared_ptr<Core::Dependency::GenericObservable>(nullptr);
}

/// <summary>
/// Applies an <see cref="InstancedBinding"/> a property on an <see cref="IAvaloniaObject"/>.
/// </summary>
/// <param name="target">The target object.</param>
/// <param name="property">The property to bind.</param>
/// <param name="binding">The instanced binding.</param>
/// <param name="anchor">
/// An optional anchor from which to locate required context. When binding to objects that
/// are not in the logical tree, certain types of binding need an anchor into the tree in 
/// order to locate named controls or resources. The <paramref name="anchor"/> parameter 
/// can be used to provice this context.
/// </param>
/// <returns>An <see cref="IDisposable"/> which can be used to cancel the binding.</returns>
rxcpp::subscription XamlCPP::DataBind::Binding::Apply(Core::Dependency::IDependencyObject * target, Core::Dependency::IPropertyObject * property, InstancedBinding * binding, System::Reflection::meta::Object * anchor)
{
    System::ThrowIfFailed<System::ArgumentNullException>(target != nullptr);
    System::ThrowIfFailed<System::ArgumentNullException>(property != nullptr);
    System::ThrowIfFailed<System::ArgumentNullException>(binding != nullptr);

    auto mode = binding->Mode;

    if (mode == BindingMode::Default)
        mode = property->GetDefaultBindingMode(target->GetType());

    switch (mode)
    {
        case BindingMode::Default:
        case BindingMode::OneWay:
        {
            auto bind = target->Bind(property, &binding->Subject->get_observable(), binding->Priority);

            binding->CreatePrimaryWay();
            return std::move(bind);
        }
        case BindingMode::TwoWay:
        {
            auto firstWay = target->Bind(property, &binding->Subject->get_observable(), binding->Priority);
            binding->CreatePrimaryWay();
            auto secondWay = binding->CreateSecondaryWay(target, property);

            rxcpp::composite_subscription binds;

            binds.add(firstWay);
            binds.add(secondWay);

            return std::move(binds);
        }
        case BindingMode::OneTime:
        {
            // @ToDo: improve this (currently status: hax)
            target->SetValue(property, binding->Value);
            return rxcpp::subscription();
        }
        case BindingMode::OneWayToSource:
            return binding->CreateSecondaryWay(target, property);
        default:
            throw System::ArgumentException("Invalid binding mode.");
    }
}

/// <summary>
/// Initializes a new instance of the <see cref="Binding"/> class.
/// </summary>
XamlCPP::DataBind::Binding::Binding()
{
    _relativeSource.Mode = RelativeSourceMode::None;
    //_fallbackValue = DependencyPropertyMgr::UnsetValue;
}

/// <summary>
/// Initializes a new instance of the <see cref="Binding"/> class.
/// </summary>
/// <param name="path">The binding path.</param>
/// <param name="mode">The binding mode.</param>

XamlCPP::DataBind::Binding::Binding(std::string path, BindingMode mode)
    : Binding()
{
    _path = path;
    _bindingMode = mode;
}

XamlCPP::Interfaces::IValueConverter * XamlCPP::DataBind::Binding::GetConverterOrDefault()
{
    return Converter ? Converter : &Core::TypeExtensions::DefaultValueConverter::Instance;
}

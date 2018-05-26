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

#include "SourceObserver.h"
#include "InstancedBinding.h"

#include "Core/NotifyPropertyChanged.h"
#include "Core/Dependencies/DependencyPropertyMgr.h"
#include "Core/Dependencies/DependencyObject.h"
#include "Platform/UIService.h"
#include "Interfaces/INamedScope.h"
#include "XamlCPP.h"
#include "XamlCPP/Core/ProtoParser/PropertyLocator.h"
#include "XamlCPP/Core/Proto/XamlType.h"
#include "XamlCPP/Core/Parser/MarkupExpressionAssembler.h"
#include "Log/Log.h"

#include <ctype.h>

using namespace XamlCPP;

std::shared_ptr<XamlCPP::Core::Dependency::GenericObservable> XamlCPP::DataBind::SourceObserverNode::StartListening()
{
    if (Next)
        return Next->StartListening();
    return std::shared_ptr<XamlCPP::Core::Dependency::GenericObservable>(nullptr);
}

void XamlCPP::DataBind::SourceObserverNode::SetUnsafeTarget(System::Reflection::Variant const& value)
{
    if (value != Target)
    {
        ValueSubscription.unsubscribe();
        _target.reset();
        _unSafeTarget = value;
    }
}

void XamlCPP::DataBind::SourceObserverNode::SetTarget(std::weak_ptr<System::Reflection::Object> const& value)
{
    if (value.lock().get() != Target)
    {
        ValueSubscription.unsubscribe();
        _unSafeTarget = System::Reflection::Variant::Empty;
        _target = value;
    }
}

void XamlCPP::DataBind::SourceObserverNode::ClearTarget()
{
    _unSafeTarget = System::Reflection::Variant::Empty;
    _target.reset();
}

System::Reflection::Variant XamlCPP::DataBind::PropertyAccessorNode::GetTargetValue(System::Reflection::meta::Object * target)
{
    InitializeFrom(target);

    if (!target)
        target = GetTarget();

    if (target && PropertyAccessor)
        return PropertyAccessor->GetValue(target);
    return System::Reflection::Variant::Empty;
}

bool XamlCPP::DataBind::PropertyAccessorNode::SetTargetValue(System::Reflection::meta::Object* target, System::Reflection::meta::Variant const& value, BindingLevel priority)
{
    InitializeFrom(target);

    if (PropertyAccessor)
    {
        PropertyAccessor->SetValue(target, value);
        return true;
    }

    return false;
}

void XamlCPP::DataBind::PropertyAccessorNode::InitializeFrom(System::Reflection::meta::Object* target)
{
    if (target && !MethodAccessor && !PropertyAccessor)
    {
        PropertyAccessor = &target->GetType()->GetRuntimeProperty(PropertyName);
        if (!PropertyAccessor->IsValid())
            PropertyAccessor = &target->GetType()->GetRuntimeField(PropertyName);
        PropertyType = PropertyAccessor->GetFieldType();
    }
}

/// <inheritdoc/>

void XamlCPP::DataBind::PropertyAccessorNode::OnNext(System::Reflection::meta::Variant const & value)
{
    if (!Next->OwnerSourceObserver)
        return;
    
    if (Next->Target != value.ToObject())
    {
        // Rebuild and notify
        OwnerSourceObserver->StartListening();
        return;
    }
    
    auto _next_value = Next->OnValueNext(value);
    if (Next->IntermediateSubject)
        Next->IntermediateSubject->get_observer().on_next(_next_value);
    else
        Next->OwnerSourceObserver->GetListenerSubject()->get_observer().on_next(_next_value);
}

bool XamlCPP::DataBind::PropertyAccessorNode::SetTargetValue(System::Reflection::meta::Variant const & value, BindingLevel priority)
{
    auto target = Target;
    if (!target)
        return false;

    if (MethodAccessor)
    {
        if (MethodAccessor->IsStatic())
            MethodAccessor->Invoke(System::Reflection::Variant(nullptr), target, value);
        else
            MethodAccessor->Invoke(System::Reflection::Variant(target), value);
        return true;
    }
    PropertyAccessor->SetValue(target, value);
    return true;
}

System::Reflection::meta::Variant XamlCPP::DataBind::PropertyAccessorNode::OnValueNext(System::Reflection::meta::Variant const & value)
{
    if (value.GetType() == &PropertyAccessor->DeclaringType())
        return PropertyAccessor->GetValue(value.ToObject());
    return value;
}

void XamlCPP::DataBind::PropertyAccessorNode::ListeningCore(PropertyAccessorNode * parent)
{
    if (parent)
    {
        SetUnsafeTarget(parent->GetTargetValue(nullptr));
        PropertyAccessor = &parent->PropertyType->GetRuntimeProperty(PropertyName);
        if (!PropertyAccessor->IsValid())
            PropertyAccessor = &parent->PropertyType->GetRuntimeField(PropertyName);
        System::ThrowIfFailed<System::TargetInvocationException>(PropertyAccessor);
        PropertyType = PropertyAccessor->GetFieldType();
    }

    if (Next)
        Next->ListeningCore(parent);
}

std::shared_ptr<XamlCPP::Core::Dependency::GenericObservable> XamlCPP::DataBind::PropertyAccessorNode::StartListening()
{
    auto target = Target;
    _enableListener = true;

    if (_intermediateSubcribe.is_subscribed())
    {
        _intermediateSubcribe.unsubscribe();
        delete IntermediateSubject;
        IntermediateSubject = nullptr;
    }

    if (!target || XamlCPP::Xaml::IsUnsetValue(Target))
        return XamlCPP::DataBind::SourceObserverNode::StartListening();

    auto propLoc = XamlCPP::Core::ProtoParser::PropertyLocator::Parse(PropertyName);
    if (propLoc->GetIsDotted()) // Attached
    {
        auto owner = Xaml::GetRuntimeTypeSource()->GetByPrefix(propLoc->Prefix, propLoc->GetOwnerName());
        System::ThrowIfFailed<System::TargetInvocationException>(owner);
        //Note: Attached property currently implementation prefer static methods
        MethodAccessor = owner->UnderlyingType()->GetStaticMethod("Set" + propLoc->PropertyName);
        if (!MethodAccessor || !MethodAccessor->IsValid())
            MethodAccessor = &owner->UnderlyingType()->GetRuntimeMethod("Set" + propLoc->PropertyName);
        System::ThrowIfFailed<System::TargetInvocationException>(MethodAccessor);
    }
    else
    {
        PropertyAccessor = &target->GetType()->GetRuntimeProperty(PropertyName);
        System::ThrowIfFailed<System::TargetInvocationException>(PropertyAccessor);
        PropertyType = PropertyAccessor->GetFieldType(); // GetFieldInfo()->m_fieldType;
    }

    static auto const& dependencytype = typeof(XamlCPP::Core::Dependency::IDependencyObject);
    static auto const& notifypropertytype = typeof(XamlCPP::Core::NotifyPropertyChanged);

    System::ThrowIfFailed<System::TargetInvocationException>(dependencytype.IsValid());
    System::ThrowIfFailed<System::TargetInvocationException>(notifypropertytype.IsValid());

    XamlCPP::Core::Dependency::IDependencyObject* dependencyObject = target->As<XamlCPP::Core::Dependency::IDependencyObject>();

    if (dependencyObject)
    {
        Core::Dependency::IPropertyObject* _property = dependencyObject->FindRegisteredGlob(dependencyObject->GetType(), propLoc->PropertyName);
        if (_property)
        {
            std::shared_ptr<XamlCPP::Core::Dependency::GenericObservable> observable = _unSafeTarget == nullptr ? dependencyObject->GetObservable(_property) : dependencyObject->GetUnsafeObservable(_property);
            if (Next)
            {
                IntermediateSubject = new Core::Dependency::GenericSubject;
                IntermediateSubject->get_observable().subscribe(
                    std::bind(&PropertyAccessorNode::OnNext, this, std::placeholders::_1),
                    std::bind(&PropertyAccessorNode::OnError, this, std::placeholders::_1),
                    std::bind(&PropertyAccessorNode::OnCompleted, this));

                _intermediateSubcribe = observable->subscribe(IntermediateSubject->get_observer().get_subscriber());

                auto _next_target = GetTargetValue(nullptr);
                if (_next_target.ToSharedObject())
                    Next->SetTarget(_next_target.ToSharedObject());
                else
                    Next->SetUnsafeTarget(_next_target);

                Next->OwnerSourceObserver = OwnerSourceObserver;
                return Next->StartListening();
                //Next->ListeningCore(this);
                //return std::make_shared<XamlCPP::Core::Dependency::GenericObservable>(subscription->map([this, subscription](System::Reflection::Variant& v) -> System::Reflection::Variant
                //{
                //    if (Next)
                //        return Next->OnNext(v);
                //    return v;
                //}));
            }
            return observable;
        }
    }

    if (target->GetType()->IsAssignableFrom<XamlCPP::Core::NotifyPropertyChanged>())
    {
        XamlCPP::Core::NotifyPropertyChanged* notify = static_cast<XamlCPP::Core::NotifyPropertyChanged*>(target);
        if (notify)
        {
            auto observable = notify->GetObservable(propLoc->PropertyName);
            if (Next)
            {
                IntermediateSubject = new Core::Dependency::GenericSubject;
                IntermediateSubject->get_observable().subscribe(
                    std::bind(&PropertyAccessorNode::OnNext, this, std::placeholders::_1),
                    std::bind(&PropertyAccessorNode::OnError, this, std::placeholders::_1),
                    std::bind(&PropertyAccessorNode::OnCompleted, this));

                _intermediateSubcribe = observable->subscribe(IntermediateSubject->get_observer().get_subscriber());

                auto _next_target = GetTargetValue(nullptr);
                if (_next_target.ToSharedObject())
                    Next->SetTarget(_next_target.ToSharedObject());
                else
                    Next->SetUnsafeTarget(_next_target);

                Next->OwnerSourceObserver = OwnerSourceObserver;
                return Next->StartListening();

                //Next->ListeningCore(this);
                //return std::make_shared<XamlCPP::Core::Dependency::GenericObservable>(subscription->map([this, subscription](System::Reflection::Variant& v) -> System::Reflection::Variant
                //{
                //    if (Next && v.ToObject())
                //        return Next->OnValueNext(v);
                //    return v;
                //}));
            }
            return observable;
        }
    }
    else // is non observable one time link
    {
        auto observable = std::make_shared<rxcpp::observable<System::Reflection::meta::Variant>>(rxcpp::dynamic_observable<System::Reflection::meta::Variant>(
            [this, propertyName = propLoc->PropertyName](rxcpp::subscriber<System::Reflection::meta::Variant> const& observer)
        {
            auto instanceObj = Target;
            if (!instanceObj)
                return;

            System::Reflection::meta::Field const* prop = &instanceObj->GetType()->GetRuntimeField(propertyName);
            if (!prop)
                prop = &instanceObj->GetType()->GetRuntimeProperty(propertyName);
            if (!prop->IsValid())
            {
                System::Console()->WriteErrorLine("invalid binding ToDo: improve this error message");
                return;
            }
            System::Reflection::meta::Variant _instance(instanceObj);
            auto value = prop->GetInternalValue(_instance);
            observer.on_next(value);
        }));
        if (Next)
        {
            IntermediateSubject = new Core::Dependency::GenericSubject;
            IntermediateSubject->get_observable().subscribe(
                std::bind(&PropertyAccessorNode::OnNext, this, std::placeholders::_1),
                std::bind(&PropertyAccessorNode::OnError, this, std::placeholders::_1),
                std::bind(&PropertyAccessorNode::OnCompleted, this));

            _intermediateSubcribe = observable->subscribe(IntermediateSubject->get_observer().get_subscriber());

            auto _next_target = GetTargetValue(nullptr);
            if (_next_target.ToSharedObject())
                Next->SetTarget(_next_target.ToSharedObject());
            else
                Next->SetUnsafeTarget(_next_target);

            Next->OwnerSourceObserver = OwnerSourceObserver;
            return Next->StartListening();
            //Next->ListeningCore(this);
            //return std::make_shared<XamlCPP::Core::Dependency::GenericObservable>(observable->map([this, observable](System::Reflection::Variant& v) -> System::Reflection::Variant
            //{
            //    if (Next)
            //        return Next->OnValueNext(v);
            //    return v;
            //}));
        }
        return observable;
    }

    System::ThrowIfFailed<System::NotSupportedException>(false);
    return nullptr;
}

void XamlCPP::DataBind::LogicalNotNode::NextValueChanged(System::Reflection::meta::Variant const & value) //override
{
    //base.NextValueChanged(Negate(value));
}

System::Reflection::meta::Variant XamlCPP::DataBind::LogicalNotNode::OnValueNext(System::Reflection::meta::Variant const & value)
{
    return value;
}

void XamlCPP::DataBind::LogicalNotNode::ListeningCore(PropertyAccessorNode * parent)
{
    if (Next)
        Next->ListeningCore(parent);
}

System::Reflection::meta::Variant const XamlCPP::DataBind::LogicalNotNode::Negate(System::Reflection::meta::Variant const & v)
{
    //if (v != UnsetValue)
    //{
    //    var s = v as string;
    //
    //    if (s != null)
    //    {
    //        bool result;
    //
    //        if (bool.TryParse(s, out result))
    //        {
    //            return !result;
    //        }
    //        else
    //        {
    //            return new BindingNotification(
    //                new InvalidCastException($"Unable to convert '{s}' to bool."),
    //                BindingErrorType.Error);
    //        }
    //    }
    //    else
    //    {
    //        try
    //        {
    //            var boolean = Convert.ToBoolean(v, CultureInfo.InvariantCulture);
    //            return !boolean;
    //        }
    //        catch (InvalidCastException)
    //        {
    //            // The error message here is "Unable to cast object of type 'System.Object'
    //            // to type 'System.IConvertible'" which is kinda useless so provide our own.
    //            return new BindingNotification(
    //                new InvalidCastException($"Unable to convert '{v}' to bool."),
    //                BindingErrorType.Error);
    //        }
    //        catch (Exception e)
    //        {
    //            return new BindingNotification(e, BindingErrorType.Error);
    //        }
    //    }
    //}

    return {};
}

System::Reflection::meta::Variant XamlCPP::DataBind::StreamNode::OnValueNext(System::Reflection::meta::Variant const & value)
{
    return value;
}

void XamlCPP::DataBind::StreamNode::ListeningCore(PropertyAccessorNode * parent)
{
    if (Next)
        Next->ListeningCore(parent);
}

System::Reflection::meta::Variant XamlCPP::DataBind::IndexerNode::OnValueNext(System::Reflection::meta::Variant const & value)
{
    return value;
}

void XamlCPP::DataBind::IndexerNode::ListeningCore(PropertyAccessorNode * parent)
{
    if (Next)
        Next->ListeningCore(parent);
}

/// <summary>
/// Initializes a new instance of the <see cref="ExpressionObserver"/> class.
/// </summary>
/// <param name="elementName">An observable which provides the target object.</param>
/// <param name="expression">The expression.</param>
/// <param name="enableDataValidation">Whether data validation should be enabled.</param>
/// <param name="description">
/// A description of the expression. If null, <paramref name="expression"/> will be used.
/// </param>
XamlCPP::DataBind::SourceObserver::SourceObserver(std::string elementName, std::string expression, bool enableDataValidation, const char * description)
{
    System::ThrowIfFailed<System::ArgumentNullException>(!elementName.empty());
    System::ThrowIfFailed<System::ArgumentNullException>(!expression.empty());

    Parse(expression, enableDataValidation);

    _expression = expression;
    _description = description;
}

XamlCPP::DataBind::SourceObserver::SourceObserver(
    std::function<System::Reflection::RefObject(System::Reflection::meta::Variant const& value)> fnRootGetter,
    std::shared_ptr<XamlCPP::Core::Dependency::GenericObservable> rootGetter,
    std::string expression
    , bool enableDataValidation
    , const char * description)
{
    System::ThrowIfFailed<System::ArgumentNullException>(rootGetter != nullptr);
    System::ThrowIfFailed<System::ArgumentNullException>(!expression.empty());

    Parse(expression, enableDataValidation);

    _expression = expression;
    _description = description;

    _rootChangeSubcribe = rootGetter->subscribe([&, getter = fnRootGetter](System::Reflection::meta::Variant const& value)
    {
        OnSourcePropertyChanged(getter(value));
    });
}

/// <summary>
/// Initializes a new instance of the <see cref="ExpressionObserver"/> class.
/// </summary>
/// <param name="rootGetter">A function which gets the root object.</param>
/// <param name="expression">The expression.</param>
/// <param name="update">An observable which triggers a re-read of the getter.</param>
/// <param name="enableDataValidation">Whether data validation should be enabled.</param>
/// <param name="description">
/// A description of the expression. If null, <paramref name="expression"/> will be used.
/// </param>

XamlCPP::DataBind::SourceObserver::SourceObserver(
    std::shared_ptr<XamlCPP::Core::Dependency::GenericObservable> rootGetter,
    std::string expression,
    bool enableDataValidation,
    const char * description)
{
    System::ThrowIfFailed<System::ArgumentNullException>(rootGetter != nullptr);
    System::ThrowIfFailed<System::ArgumentNullException>(!expression.empty());

    Parse(expression, enableDataValidation);

    _expression = expression;
    _description = description;

    _rootChangeSubcribe = rootGetter->subscribe(std::bind(&SourceObserver::OnSourcePropertyChanged, this, std::placeholders::_1));
}

XamlCPP::DataBind::SourceObserver::~SourceObserver()
{
    if (_rootChangeSubcribe.is_subscribed())
        _rootChangeSubcribe.unsubscribe();

    XamlCPP::Interfaces::INameScope* nameScope = _namedScope.lock().get();
    if (nameScope)
        nameScope = Xaml::UIServiceContext->Get<XamlCPP::Interfaces::INameScope>();

    if (nameScope)
    {
        nameScope->Registered -= System::make_eventhandler(&SourceObserver::OnNamedScopeObserver, shared_from_this());
        nameScope->Unregistered -= System::make_eventhandler(&SourceObserver::OnLeavedNamedScopeObserver, shared_from_this());
    }
}

void XamlCPP::DataBind::SourceObserver::Initialize(std::string const& elementName, std::shared_ptr<XamlCPP::Interfaces::INameScope> scope)
{
    if (SourceNodes)
    {
        _namedScope = scope;
        XamlCPP::Interfaces::INameScope* nameScope = scope.get();
        if (!nameScope)
            nameScope = Xaml::UIServiceContext->Get<XamlCPP::Interfaces::INameScope>();

        if (nameScope)
        {
            SourceNodes->TargetName = elementName;

            auto target = nameScope->Find(elementName);
            if (target)
                SourceNodes->Target = target;

            nameScope->Registered += System::make_eventhandler(&SourceObserver::OnNamedScopeObserver, shared_from_this());
            nameScope->Unregistered += System::make_eventhandler(&SourceObserver::OnLeavedNamedScopeObserver, shared_from_this());
        }
    }
}

void XamlCPP::DataBind::SourceObserver::OnNamedScopeObserver(void * sender, XamlCPP::Interfaces::NameScopeEventArgs & e)
{
    if (SourceNodes->Target == e.Element.get())
        return;

    if (e.Name != SourceNodes->TargetName)
        return;

    SourceNodes->Target = e.Element;

    if (SourceNodes->IsListener())
        StartListening();
}

void XamlCPP::DataBind::SourceObserver::OnLeavedNamedScopeObserver(void * sender, XamlCPP::Interfaces::NameScopeEventArgs & e)
{
    if (!SourceNodes->Target)
        return;

    if (e.Name != SourceNodes->TargetName)
        return;

    if (_resultSubcribe.is_subscribed())
        _resultSubcribe.unsubscribe();

    SourceNodes->ClearTarget();
}

void XamlCPP::DataBind::SourceObserver::OnSourcePropertyChanged(System::Reflection::meta::Variant const & sourceData)
{
    System::Reflection::meta::Object* dataSource = sourceData.ToObject();
    if (SourceNodes->Target == dataSource)
        return;

    if (_resultSubcribe.is_subscribed())
        _resultSubcribe.unsubscribe();

    SourceNodes->ClearTarget();

    if (dataSource && !XamlCPP::Xaml::IsUnsetValue(dataSource))
    {
        if (sourceData.GetBase()->IsSharedPointer())
        {
            System::Reflection::RefObject refObj = sourceData.TryGetValue<System::Reflection::RefObject>();
            if (refObj)
            {
                SourceNodes->Target = refObj;
                if (SourceNodes->IsListener())
                    StartListening();
            }
        }
        else
        {
            System::Reflection::Object* obj = sourceData.ToObject();
            if (obj)
            {
                SourceNodes->SetUnsafeTarget(obj);
                if (SourceNodes->IsListener())
                    StartListening();
            }
        }
    }
}

void XamlCPP::DataBind::SourceObserver::StartListening()
{
    auto leaf = SourceNodes;
    if (leaf)
    {
        if (_resultSubcribe.is_subscribed())
            _resultSubcribe.unsubscribe();

        if (leaf->OwnerSourceObserver)
            leaf->OwnerSourceObserver = this;
        auto observable = leaf->StartListening();

        if (observable)
        {
            System::ThrowIfFailed<System::InvalidOperationException>(_result || _owner);

            if (!_result || !_result->get_observer().get_subscriber().is_subscribed())
                _owner->CreatePrimaryWay();

            _resultSubcribe = observable->subscribe(_result->get_observer().get_subscriber()); // .get_subscriber()
        }
    }
}

System::Reflection::meta::Type const* XamlCPP::DataBind::SourceObserver::GetResultType() const
{
    auto leaf = GetLeafNode();
    if (leaf)
        return leaf->GetPropertyType();
    return nullptr;
}

std::string XamlCPP::DataBind::SourceObserver::ToString() const
{
    return _expression;
}

void XamlCPP::DataBind::SourceObserver::Parse(std::string const & expression, bool enableDataValidation)
{
    Core::ExpressionAssembler parser(enableDataValidation);
    parser.Parse(expression);

    SourceNodes = parser.Nodes.front();
}

void XamlCPP::DataBind::SourceObserver::SetOwner(InstancedBinding* o)
{
    _owner = o;
}

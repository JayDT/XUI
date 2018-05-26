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

#ifndef __OBSERVER_OBSERVER_HUB_H__
#define __OBSERVER_OBSERVER_HUB_H__

#include "Core/Dependencies/BindValue.h"
#include "Reflection/Runtime/Object.h"
#include "standard/misc.h"

#include "rxcpp/rx.hpp"

namespace XamlCPP::Interfaces
{
    struct NameScopeEventArgs;
    struct INameScope;
}

namespace XamlCPP::DataBind
{
    class InstancedBinding;
    class PropertyAccessorNode;

    class SourceObserverNode : public System::EnableSharedFromThisEx<SourceObserverNode> // ExpressionNode
    {
    protected:

        System::Reflection::Variant _unSafeTarget;
        std::weak_ptr<System::Reflection::meta::Object> _target;

        bool _enableListener = false;

    public:

        SourceObserver * OwnerSourceObserver = nullptr;
        Core::Dependency::GenericSubject* IntermediateSubject = nullptr;

        PROPERTY_(System::Reflection::meta::Object*, Target);
        System::Reflection::meta::Object* GetTarget()
        {
            if (!_target.expired())
                return _target.lock().get();
            if (_unSafeTarget.IsValid() && _unSafeTarget.ToObject())
                return _unSafeTarget.ToObject();
            return nullptr;
        }
        void SetUnsafeTarget(System::Reflection::Variant const& value);
        void SetTarget(std::weak_ptr<System::Reflection::meta::Object> const& value);
        void ClearTarget();

        rxcpp::subscription ValueSubscription;
        std::string Description;
        std::string TargetName;
        std::shared_ptr<SourceObserverNode> Next;

        bool IsListener() const { return _enableListener; }
        virtual bool IsSettable() const { return false; }
        virtual System::Reflection::Variant GetTargetValue(System::Reflection::meta::Object* target = nullptr) { return System::Reflection::Variant::Empty; }
        virtual bool SetTargetValue(System::Reflection::meta::Object* target, System::Reflection::meta::Variant const& value, BindingLevel priority) { return false; }
        virtual bool SetTargetValue(System::Reflection::meta::Variant const& value, BindingLevel priority) { return false; }
        virtual System::Reflection::meta::Type const* GetPropertyType() { return nullptr; }
        virtual std::shared_ptr<Core::Dependency::GenericObservable> StartListening();
        virtual System::Reflection::meta::Variant OnValueNext(System::Reflection::meta::Variant const& value) = 0;
        virtual void ListeningCore(PropertyAccessorNode * parent) = 0;
        virtual void InitializeFrom(System::Reflection::meta::Object* target) {}
    };

    struct PropertyAccessorNode : SourceObserverNode
    {
        bool _enableValidation = false;
        std::string PropertyName;
        System::Reflection::Type const* PropertyType = nullptr;
        System::Reflection::Field const* PropertyAccessor = nullptr;
        System::Reflection::IMethod const* MethodAccessor = nullptr;
        rxcpp::subscription _intermediateSubcribe;

        PropertyAccessorNode(std::string const& propertyName, bool enableValidation)
        {
            PropertyName = propertyName;
            _enableValidation = enableValidation;
        }

        virtual ~PropertyAccessorNode()
        {
            if (_intermediateSubcribe.is_subscribed())
            {
                _intermediateSubcribe.unsubscribe();
                delete IntermediateSubject;
                IntermediateSubject = nullptr;
            }
        }

        bool IsSettable() const override { return PropertyType && PropertyAccessor; }
        System::Reflection::Variant GetTargetValue(System::Reflection::meta::Object* target = nullptr) override;
        bool SetTargetValue(System::Reflection::meta::Object* target, System::Reflection::meta::Variant const& value, BindingLevel priority) override;
        bool SetTargetValue(System::Reflection::meta::Variant const& value, BindingLevel priority) override;
        System::Reflection::meta::Type const* GetPropertyType() override 
        {
            return PropertyType;
        }

        System::Reflection::meta::Variant OnValueNext(System::Reflection::meta::Variant const& value) override;

        void ListeningCore(PropertyAccessorNode* parent) override;
        std::shared_ptr<Core::Dependency::GenericObservable> StartListening() override;
        void InitializeFrom(System::Reflection::meta::Object* target) override;


        /// <inheritdoc/>
        void OnCompleted()
        {
        }

        /// <inheritdoc/>
        void OnError(std::exception_ptr e)
        {

        }

        /// <inheritdoc/>
        void OnNext(System::Reflection::meta::Variant const& value);
    };

    struct LogicalNotNode : SourceObserverNode
    {
        LogicalNotNode()
        {
            SourceObserverNode::Description = "!";
        }

    protected:
        
        void NextValueChanged(System::Reflection::meta::Variant const& value);
        System::Reflection::meta::Variant OnValueNext(System::Reflection::meta::Variant const& value) override;
        void ListeningCore(PropertyAccessorNode* parent) override;

        System::Reflection::meta::Variant const Negate(System::Reflection::meta::Variant const& v);
    };

    struct StreamNode : SourceObserverNode
    {
        StreamNode()
        {
            SourceObserverNode::Description = "^";
        }

        System::Reflection::meta::Variant OnValueNext(System::Reflection::meta::Variant const& value) override;
        void ListeningCore(PropertyAccessorNode* parent) override;
    };

    struct IndexerNode : SourceObserverNode
    {
        std::vector<std::string> Arguments;

        IndexerNode(std::vector<std::string> const& arguments)
            : Arguments(std::move(arguments))
        {
            SourceObserverNode::Description = "[_]";
        }

        System::Reflection::meta::Variant OnValueNext(System::Reflection::meta::Variant const& value) override;
        void ListeningCore(PropertyAccessorNode* parent) override;
    };

    class SourceObserver : public System::EnableSharedFromThisEx<SourceObserver> //ExpressionObserver
    {
        std::string _expression;
        std::string _description;

        std::shared_ptr<XamlCPP::DataBind::SourceObserverNode> SourceNodes;
        std::shared_ptr<XamlCPP::Core::Dependency::GenericSubject> _finished;
        rxcpp::subscription _resultSubcribe;
        rxcpp::subscription _rootChangeSubcribe;

        InstancedBinding* _owner = nullptr;
        Core::Dependency::GenericSubject* _result = nullptr;
        std::weak_ptr<XamlCPP::Interfaces::INameScope> _namedScope;

    public:
        /// <summary>
        /// Initializes a new instance of the <see cref="ExpressionObserver"/> class.
        /// </summary>
        /// <param name="target">The target object.</param>
        /// <param name="expression">The expression.</param>
        /// <param name="enableDataValidation">Whether data validation should be enabled.</param>
        /// <param name="description">
        /// A description of the expression. If null, <paramref name="expression"/> will be used.
        /// </param>
        SourceObserver(
            System::Reflection::RefObject const& target,
            std::string expression,
            bool enableDataValidation = false,
            const char* description = nullptr)
        {
            System::ThrowIfFailed<System::ArgumentNullException>(!expression.empty());

            Parse(expression, enableDataValidation);

            _expression = expression;
            if (description)
                _description = description;

            if (SourceNodes)
                SourceNodes->Target = target;
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
        SourceObserver(
            std::string elementName,
            std::string expression,
            bool enableDataValidation = false,
            const char* description = nullptr);

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
        SourceObserver(
            std::shared_ptr<XamlCPP::Core::Dependency::GenericObservable> rootGetter,
            std::string expression,
            bool enableDataValidation = false,
            const char* description = nullptr);

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
        SourceObserver(
            std::function<System::Reflection::RefObject(System::Reflection::meta::Variant const& value)> fnRootGetter,
            std::shared_ptr<XamlCPP::Core::Dependency::GenericObservable> rootGetter,
            std::string expression,
            bool enableDataValidation = false,
            const char* description = nullptr);

        virtual ~SourceObserver();

        void Initialize(std::string const& elementName, std::shared_ptr<XamlCPP::Interfaces::INameScope> scope);

        /// <summary>
        /// Attempts to set the value of a property expression.
        /// </summary>
        /// <param name="value">The value to set.</param>
        /// <param name="priority">The binding priority to use.</param>
        /// <returns>
        /// True if the value could be set; false if the expression does not evaluate to a 
        /// property. Note that the <see cref="ExpressionObserver"/> must be subscribed to
        /// before setting the target value can work, as setting the value requires the
        /// expression to be evaluated.
        /// </returns>
        bool SetValue(System::Reflection::meta::Variant const& value, BindingLevel priority = BindingLevel::LocalValue)
        {
            auto leaf = GetLeafNode();
            return leaf ? leaf->SetTargetValue(value, priority) : false;
        }

        System::Reflection::Variant GetValue()
        {
            auto leaf = GetLeafNode();
            return leaf ? leaf->GetTargetValue() : System::Reflection::Variant::Empty;
        }

        void OnNamedScopeObserver(void* sender, XamlCPP::Interfaces::NameScopeEventArgs& e);
        void OnLeavedNamedScopeObserver(void* sender, XamlCPP::Interfaces::NameScopeEventArgs& e);
        void OnSourcePropertyChanged(System::Reflection::meta::Variant const& sourceData);

        void StartListening();
        bool IsListening() const { return _resultSubcribe.is_subscribed(); }

        XamlCPP::Core::Dependency::GenericSubject* GetListenerSubject() { return _result; }
        void SetListenerSubject(XamlCPP::Core::Dependency::GenericSubject* s) { _result = s; }

        void SetOwner(InstancedBinding* o);

        System::Reflection::meta::Type const* GetResultType() const;

        std::string ToString() const;

    private:

        void Parse(std::string const& expression, bool enableDataValidation);

        /// <summary>
        /// Gets the leaf node.
        /// </summary>
        XamlCPP::DataBind::SourceObserverNode* GetLeafNode() const
        {
            auto node = SourceNodes.get();
            while (node && node->Next != nullptr) node = node->Next.get();
            return node;
        }

    };
}
#endif // !__OBSERVER_OBSERVER_HUB_H__

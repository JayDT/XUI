/*
 * Copyright (C) 2017-2018 Tauri JayD <https://www.>
 * Copyright (c) 2014 Steven Kirk <The Avalonia Project>
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

#ifndef __DEPENDENCY_OBJECT_H__
#define __DEPENDENCY_OBJECT_H__

#include "XamlCPP/Core/Dependencies/IDependencyObject.h"
#include "XamlCPP/Core/Dependencies/IPropertyObject.h"
#include "XamlCPP/Interfaces/ReflAttributes.h"
#include "XamlCPP/Interfaces/INamedScope.h"

#include "Reflection/Runtime/Object.h"
#include "Reflection/Runtime/Variant.h"
#include "standard/events.h"
#include "standard/Errors.h"

#include "rxcpp/rx.hpp"

#include "Core/Dependencies/Models/PropertyPrototype.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("Core/Dependencies/BindValue.h"))
MetaAssemblyEnd

#undef FindResource

#define DEPENDENCY_PROPERTY(t, p, n) IDENTITY(PROPERTY_SPEC(PNGETTER(n), PNSETTER(n), t, n)); \
        t PNGETTER(n)##() const   { return GetValue(p); } \
        void PNSETTER(n)(t value) { SetValue(p, value); }

#define DEPENDENCY_PROPERTY_(t, n) PROPERTY_(t, n); \
        t Get##n() const { return GetValue<t>(n##Property); } \
        void Set##n(t const& value) { SetValue(n##Property, value); }

namespace XamlCPP
{
    namespace Core::Dependency
    {
        struct IPropertyObject;
        struct DependencyPropertyChangedEventArgs;
    }

    namespace DataBind
    {
        class BindingNotification;
    }
}

namespace XUI::Core::Dependency
{
    struct BindValue;
    struct DependencyProperty;
    struct Unset;

    enum RenderDebugFlags
    {
        RDF_BOUND              = 0x00000001,
        RDF_FPS                = 0x00000002,
        RDF_DRAW_DIRTY_RECTS   = 0x00000004,
    };

    struct IBindValueOwner
    {
        /// <inheritdoc/>
        virtual void Changed(BindValue* sender, System::Reflection::meta::Variant const& oldValue, System::Reflection::meta::Variant const& newValue) = 0;
        /// <inheritdoc/>
        virtual void BindingNotificationReceived(BindValue* sender, XamlCPP::DataBind::BindingNotification* notification) = 0;
    };

    class TC_XUI_API DependencyObject : public IBindValueOwner, public XamlCPP::Core::Dependency::IDependencyObject
    {
    private:

        std::unordered_map<XamlCPP::Core::Dependency::IPropertyObject*, std::shared_ptr<BindValue>, XamlCPP::Core::Dependency::IPropertyObject::Hasher, XamlCPP::Core::Dependency::IPropertyObject::EqualTo> _values;
        std::unordered_map<XamlCPP::Core::Dependency::IPropertyObject*, std::set<std::shared_ptr<XamlCPP::DataBind::InstancedBinding>>, XamlCPP::Core::Dependency::IPropertyObject::Hasher, XamlCPP::Core::Dependency::IPropertyObject::EqualTo> _directBindingExpressions;

        std::list<std::shared_ptr<rxcpp::composite_subscription>> _directBindings;
        std::string _name;
        uint32 _gid = 0;
        uint16 _debugFlags = 0;
        mutable System::Reflection::Type const* _typeCache = nullptr;

        System::Reflection::meta::Variant const& GetDefaultValue(XamlCPP::Core::Dependency::IPropertyObject* _property) const;

    protected:

        std::shared_ptr<Core::Dependency::DependencyObject> _inheritanceParent;
        void SetInheritanceParent(Core::Dependency::DependencyObject* parent);
        virtual void OnInheritanceParentChanged()
		{
			static System::Events::EventArg e;
			if (InheritanceParentChanged)
				InheritanceParentChanged(static_cast<System::Reflection::Object*>(this), e);
		}

    public:

        DependencyObject();
        virtual ~DependencyObject();

        void Cleanup();

        System::Reflection::Type const* GetType(void) const override final;

        Core::Dependency::DependencyObject* GetInheritanceParent() const
        {
            return _inheritanceParent.get();
        }

        virtual Core::Dependency::DependencyObject* GetInheritTemplatedParent() const
        {
            return nullptr;
        }

        PROPERTY_T(uint32, GID)
        uint32 GetGID() const override { return _gid; }
        void SetGID(uint32 value) { _gid = value; }

        uint32 GetDebugFlag() const { return _debugFlags; }
        void SetDebugFlag(uint32 value) { _debugFlags = value; }

        PROPERTY_T(std::string, Name)
        std::string GetName() const override final { return _name; }
        void SetName(std::string const& value) { _name = value; }

        PROPERTY_T(bool, IsReadOnly)
        bool GetIsReadOnly() const { return false; }
        void SetIsReadOnly(bool value) { }

        System::Events::EventHandler<XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs> DependencyPropertyChanged;
		System::Events::EventHandler<System::Events::EventArg> InheritanceParentChanged;

        void RaisePropertyChanged(
            XamlCPP::Core::Dependency::IPropertyObject* _property,
            System::Reflection::meta::Variant const& oldValue,
            System::Reflection::meta::Variant const& newValue,
            BindingLevel priority = BindingLevel::LocalValue);

        /// <summary>
        /// Gets a <see cref="XamlCPP::Core::Dependency::IPropertyObject"/> value
        /// without check for registered as this can slow getting the value
        /// this method is intended for internal usage in dependencyObject only
        /// it's called only after check the _property is registered
        /// </summary>
        /// <param name="_property">The _property.</param>
        /// <returns>The value.</returns>
        System::Reflection::meta::Variant const& GetValueInternal(XamlCPP::Core::Dependency::IPropertyObject* _property) const;

        BindValue* GetValueSource(XamlCPP::Core::Dependency::IPropertyObject* _property) const;

        /// <summary>
        /// Sets the value of a direct _property.
        /// </summary>
        /// <param name="_property">The _property.</param>
        /// <param name="value">The value.</param>
        void SetDirectValue(XamlCPP::Core::Dependency::IPropertyObject* _property, System::Reflection::meta::Variant const& value);

        std::shared_ptr<BindValue> CreateBindValue(XamlCPP::Core::Dependency::IPropertyObject* _property);

        /// <summary>
        /// Sets the value of a styled _property.
        /// </summary>
        /// <param name="_property">The _property.</param>
        /// <param name="value">The value.</param>
        /// <param name="priority">The priority of the value.</param>
        void SetInternalValue(XamlCPP::Core::Dependency::IPropertyObject* _property, System::Reflection::meta::Variant const& value, BindingLevel level = BindingLevel::LocalValue);

        void StorePropertyData(XamlCPP::Core::Dependency::IPropertyObject* _property, System::Reflection::meta::Variant const& value, BindingLevel level = BindingLevel::LocalValue) override;

        System::Reflection::meta::Variant GetPropertyData(XamlCPP::Core::Dependency::IPropertyObject* _property) const;

        bool IsSet(XamlCPP::Core::Dependency::IPropertyObject* _property) override;

        void ReValidation(XamlCPP::Core::Dependency::IPropertyObject* _property);

        virtual void UpdateDataValidation(
            XamlCPP::Core::Dependency::IPropertyObject* _property,
            XamlCPP::DataBind::BindingNotification* status);

        XamlCPP::Core::Dependency::IPropertyObject* GetRegistered(XamlCPP::Core::Dependency::IPropertyObject* _property) const;

        System::Reflection::meta::Variant GetValue(XamlCPP::Core::Dependency::IPropertyObject* _property) const override;

        template<typename TValue>
        TValue GetValue(DependencyProperty* _property) const
        {
            System::ThrowIfFailed<System::ArgumentNullException>(_property != nullptr);

            return GetValue(static_cast<XamlCPP::Core::Dependency::IPropertyObject*>(_property)).GetValue<TValue>();
        }

        template<typename TValue>
        TValue GetValue(RefDependencyProperty const& _property) const
        {
            System::ThrowIfFailed<System::ArgumentNullException>(_property != nullptr);

            return GetValue(static_cast<XamlCPP::Core::Dependency::IPropertyObject*>(_property.get())).GetValue<TValue>();
        }

        void SetValue(XamlCPP::Core::Dependency::IPropertyObject* _property, System::Reflection::meta::Variant const& value, BindingLevel priority = BindingLevel::LocalValue) override;

        std::string Meta(Disable) GetDescription(XamlCPP::Core::Dependency::GenericObservable* o);

        /// <summary>
        /// Tries to cast a value to a type, taking into account that the value may be a
        /// <see cref="BindingNotification"/>.
        /// </summary>
        /// <param name="value">The value.</param>
        /// <param name="type">The type.</param>
        /// <returns>The cast value, or a <see cref="BindingNotification"/>.</returns>
        static System::Reflection::meta::Variant CastOrDefault(System::Reflection::meta::Variant const& value, System::Reflection::meta::Type const* type);

        void LogPropertySet(XamlCPP::Core::Dependency::IPropertyObject* _property, System::Reflection::meta::Variant const& value);

        /// <summary>
        /// Sets the backing field for a direct dependency _property, raising the 
        /// <see cref="PropertyChanged"/> event if the value has changed.
        /// </summary>
        /// <typeparam name="T">The type of the _property.</typeparam>
        /// <param name="_property">The _property.</param>
        /// <param name="field">The backing field.</param>
        /// <param name="value">The value.</param>
        /// <returns>
        /// True if the value changed, otherwise false.
        /// </returns>
        template<typename TValue>
        bool SetAndRaise(DependencyProperty* _property, TValue& field, TValue const& value)
        {
            if (field != value)
            {
                auto old = field;
                field = value;
                RaisePropertyChanged(_property, old, value, BindingLevel::LocalValue);
                return true;
            }
            return false;
        }

        template<typename TValue>
        bool SetAndRaise(RefDependencyProperty const& _property, TValue& field, TValue const& value)
        {
            if (field != value)
            {
                auto old = field;
                field = value;
                RaisePropertyChanged(_property.get(), old, value, BindingLevel::LocalValue);
                return true;
            }
            return false;
        }

        /// <summary>
        /// Clears a <see cref="DependencyProperty"/>'s local value.
        /// </summary>
        /// <param name="property">The property.</param>
        void ClearValue(DependencyProperty* _property);

        /// <summary>
        ///     Coerces the value of the specified dependency property. This is accomplished
        ///     by invoking any CoerceValueCallback function specified in property
        ///     metadata for the dependency property as it exists on the calling DependencyObject.
        /// </summary>
        /// <param name="property">The dependency property.</param>
        void CoerceValue(DependencyProperty* _property);

        bool IsDefaultValue(DependencyProperty* _property);

        // Depricate
        template<typename TValue>
        void SetValue(DependencyProperty* _property, TValue const& value, BindingLevel level = BindingLevel::LocalValue)
        {
            SetValue(
                static_cast<XamlCPP::Core::Dependency::IPropertyObject*>(_property),
                System::Reflection::Variant(value, (System::Reflection::variant_policy::Copy*)nullptr), // Required copy from original data for storage
                level);
        }

        void SetValue(DependencyProperty* _property, System::Reflection::Variant const& value, BindingLevel level = BindingLevel::LocalValue)
        {
            SetValue(
                static_cast<XamlCPP::Core::Dependency::IPropertyObject*>(_property),
                value,
                level);
        }

        void SetValue(RefDependencyProperty const& _property, System::Reflection::Variant const& value, BindingLevel level = BindingLevel::LocalValue)
        {
            SetValue(static_cast<XamlCPP::Core::Dependency::IPropertyObject*>(_property.get()), value, level);
        }

        // Implementation IBindValueOwner
        void Changed(BindValue* sender, System::Reflection::meta::Variant const& oldValue, System::Reflection::meta::Variant const& newValue) override;
        void BindingNotificationReceived(BindValue* sender, XamlCPP::DataBind::BindingNotification* notification) override;

        /// Observer Extensions

        std::shared_ptr<XamlCPP::Core::Dependency::GenericObservable> GetObservable(XamlCPP::Core::Dependency::IPropertyObject* _property) override /*final*/;
        std::shared_ptr<XamlCPP::Core::Dependency::GenericObservable> GetUnsafeObservable(XamlCPP::Core::Dependency::IPropertyObject* _property);
		std::shared_ptr<XamlCPP::Core::Dependency::GenericObservable> GetRelativeObservable(XamlCPP::Markup::RelativeSource * _source) override;
        std::shared_ptr<XamlCPP::Core::Dependency::GenericObservable> GetObservable(std::function<System::Events::IEventHook*(rxcpp::subscriber<System::Reflection::meta::Variant> const&)> const& addEvent, std::function<void(System::Events::IEventHook*)> const& removeEvent) override /*final*/;

        virtual std::shared_ptr<DependencyObject> ToSharedPtr() { return shared_from_base_static<DependencyObject>(); }

        rxcpp::subscription Meta(Disable) Bind(
            XamlCPP::Core::Dependency::IPropertyObject* _property,
            XamlCPP::Core::Dependency::GenericObservable* source,
            BindingLevel priority,
            XamlCPP::Core::Dependency::BindingSubject * subject = nullptr) override;

        rxcpp::subscription Meta(Disable) AddDirectBind(
            XamlCPP::Core::Dependency::IPropertyObject* _property,
            std::function<void(System::Reflection::meta::Variant const& value)> _func);

        //template<typename TValue>
        //rxcpp::subscription Bind(DependencyProperty* _property)
        //{
        //    System::ThrowIfFailed<System::ArgumentNullException>(_property != nullptr);
        //    auto onsubribe = [](rxcpp::subscriber<int> dest) {};
        //    auto output = rxcpp::sources::create<System::Reflection::meta::Variant>();
        //    //rxcpp::subjects::subject
        //
        //    //rxcpp::subjects::subject<System::Reflection::meta::Variant>::c
        //    //rxcpp::observable<>::create<int, decltype(onsubribe)>(onsubribe); //rxcpp::observable<T, rxcpp::detail::create<T, OnSubscribe> m = 
        //    Bind(_property, output);
        //    return nullptr;
        //}

        /// <summary>
        /// Binds a property on an <see cref="DependencyObject"/> to an <see cref="IBinding"/>.
        /// </summary>
        /// <param name="target">The object.</param>
        /// <param name="property">The property to bind.</param>
        /// <param name="binding">The binding.</param>
        /// <param name="anchor">
        /// An optional anchor from which to locate required context. When binding to objects that
        /// are not in the logical tree, certain types of binding need an anchor into the tree in 
        /// order to locate named controls or resources. The <paramref name="anchor"/> parameter 
        /// can be used to provice this context.
        /// </param>
        /// <returns>An <see cref="IDisposable"/> which can be used to cancel the binding.</returns>
        static rxcpp::subscription Meta(Disable) Bind(
            DependencyObject* target,
            DependencyProperty* property,
            XamlCPP::Interfaces::IBinding* binding,
            System::Reflection::meta::Object* anchor = nullptr);

        rxcpp::subscription Meta(Disable) Bind(
            XamlCPP::Core::Dependency::IPropertyObject* property,
            XamlCPP::Interfaces::IBinding* binding,
            System::Reflection::Object* anchor = nullptr) override /*final*/
        {
            return DependencyObject::Bind(this, static_cast<DependencyProperty*>(property), binding, anchor);
        }

        void AddBindingExpression(XamlCPP::Core::Dependency::IPropertyObject* dp, XamlCPP::DataBind::InstancedBinding* bind);
        void RemoveBindingExpression(XamlCPP::Core::Dependency::IPropertyObject* dp, XamlCPP::DataBind::InstancedBinding* bind);
        XamlCPP::DataBind::InstancedBinding* GetBindingExpression(XamlCPP::Core::Dependency::IPropertyObject* dp, System::Reflection::Variant const& element) const;
        std::set<std::shared_ptr<XamlCPP::DataBind::InstancedBinding>> const* GetBindingExpressions(XamlCPP::Core::Dependency::IPropertyObject* dp) const;

        void ParentPropertyChanged(void* sender, XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e);

        XamlCPP::Core::Dependency::IPropertyObject* GetDataContextProperty() const override /*final*/;
        virtual System::Reflection::Variant GetDataContext() const override { return nullptr; }
        virtual XamlCPP::Core::Dependency::IDependencyObject* GetIDepVisualParent() const override { return nullptr; }
        XamlCPP::Core::Dependency::IPropertyObject* FindRegisteredGlob(System::Reflection::meta::Type const* type, std::string const& name) const override /*final*/;
        virtual bool IsInited() const override { return true; }
        virtual System::Events::EventHandler<System::Events::EventArg, System::Events::DummyMutex>* OnInitialized() override { return nullptr; }
        virtual System::Reflection::Variant const & FindInternalResource(const std::string & name) const { return System::Reflection::Variant::Empty; }
        virtual System::Reflection::Variant const& FindResource(const std::string &name) const { return System::Reflection::Variant::Empty; }
        virtual std::shared_ptr<XamlCPP::Interfaces::INameScope> GetSharedNamedScope() const override { return std::shared_ptr<XamlCPP::Interfaces::INameScope>(nullptr); }

		template<class TParent>
		TParent* FindInheritanceAncestor()
		{
			DependencyObject* parent = GetInheritanceParent();
			if (!parent)
				return nullptr;

			if (parent->Is<TParent>())
				return static_cast<TParent*>(parent);

			return parent->FindInheritanceAncestor<TParent>();
		}

		DependencyObject* FindInheritanceAncestor(const System::Reflection::Type* type)
		{
			DependencyObject* parent = GetInheritanceParent();
			if (!parent)
				return nullptr;

			if (parent->Is(*type))
				return parent;

			return parent->FindInheritanceAncestor(type);
		}    };
}

#endif // !__DEPENDENCY_OBJECT_H__

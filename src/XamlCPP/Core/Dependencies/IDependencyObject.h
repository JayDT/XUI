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

#ifndef __XAMLCPP_IDEPENDENCYOBJECT_H__
#define __XAMLCPP_IDEPENDENCYOBJECT_H__

#include <System/System.h>
#include <standard/misc.h>
#include "XamlCPP/Core/NotifyPropertyChanged.h"
#include "XamlCPP/Core/Dependencies/IPropertyObject.h"
#include "XamlCPP/Interfaces/IBinding.h"
#include "XamlCPP/DataBind/RelativeSource.h"

#include <boost/uuid/uuid.hpp>

#undef FindResource

namespace XamlCPP
{
	namespace Interfaces
	{
		struct IBinding;
		struct IInstancedBindingManager;
		struct INameScope;
	}
}

namespace XamlCPP::Core::Dependency
{
    struct TC_XAMLCPP_API IDependencyObject : public Core::NotifyPropertyChanged, public System::EnableSharedFromThisEx<IDependencyObject>
    {
        virtual System::Reflection::meta::Variant GetValue(XamlCPP::Core::Dependency::IPropertyObject* _property) const = 0;
        virtual bool IsSet(IPropertyObject *_property) = 0;
        virtual void SetValue(XamlCPP::Core::Dependency::IPropertyObject* _property, System::Reflection::meta::Variant const& value, XamlCPP::BindingLevel priority = XamlCPP::BindingLevel::LocalValue) = 0;

        virtual rxcpp::subscription Meta(Disable) Bind(
            IPropertyObject* _property,
            XamlCPP::Core::Dependency::GenericObservable* source,
            XamlCPP::BindingLevel priority,
            BindingSubject * subject = nullptr) = 0;

        virtual rxcpp::subscription Meta(Disable) Bind(
            IPropertyObject* property,
            XamlCPP::Interfaces::IBinding* binding,
            System::Reflection::Object* anchor = nullptr) = 0;

        virtual std::shared_ptr<XamlCPP::Core::Dependency::GenericObservable> GetObservable(XamlCPP::Core::Dependency::IPropertyObject* _property) = 0;
        virtual std::shared_ptr<XamlCPP::Core::Dependency::GenericObservable> GetUnsafeObservable(XamlCPP::Core::Dependency::IPropertyObject* _property) = 0;
		virtual std::shared_ptr<XamlCPP::Core::Dependency::GenericObservable> GetRelativeObservable(XamlCPP::Markup::RelativeSource * _source) = 0;

        virtual void StorePropertyData(XamlCPP::Core::Dependency::IPropertyObject* _property, System::Reflection::meta::Variant const& value, BindingLevel level = BindingLevel::LocalValue) = 0;

        virtual XamlCPP::Core::Dependency::IPropertyObject* GetDataContextProperty() const = 0;
        virtual System::Reflection::Variant GetDataContext() const = 0;
        virtual IDependencyObject* GetIDepVisualParent() const = 0;
        virtual XamlCPP::Core::Dependency::IPropertyObject* FindRegisteredGlob(System::Reflection::meta::Type const* type, std::string const& name) const = 0;
        virtual bool IsInited() const = 0;
        virtual System::Events::EventHandler<System::Events::EventArg, System::Events::DummyMutex>* OnInitialized() = 0;
        virtual std::string GetName() const = 0;
        virtual System::Reflection::Variant const& FindResource(const std::string &name) const = 0;
        virtual std::shared_ptr<XamlCPP::Core::Dependency::GenericObservable> GetObservable(std::function<System::Events::IEventHook*(rxcpp::subscriber<System::Reflection::meta::Variant> const&)> const& addEvent, std::function<void(System::Events::IEventHook*)> const& removeEvent) = 0;
        virtual std::shared_ptr<XamlCPP::Interfaces::INameScope> GetSharedNamedScope() const = 0;
		virtual void AddBindingExpression(XamlCPP::Core::Dependency::IPropertyObject* dp, XamlCPP::DataBind::InstancedBinding* bind) = 0;
        virtual uint32 GetGID() const = 0;
    };

    struct TC_XAMLCPP_API IDependencyPropertyMgr
    {
        //// {5E896A26-5971-4480-8360-9308AA5632AF}
        static constexpr boost::uuids::uuid __uuid = { 0x5e, 0x89, 0x6a, 0x26, 0x59, 0x71, 0x44, 0x80, 0x83, 0x60, 0x93, 0x8, 0xaa, 0x56, 0x32, 0xaf };

        virtual System::Reflection::Variant const& GetUnSetValue() const = 0;
        virtual System::Reflection::Variant const& GetNullValue() const = 0; // not implement yet
        virtual XamlCPP::Core::Dependency::IPropertyObject* FindRegistered(System::Reflection::meta::Type const* type, std::string const& name) = 0;
        virtual bool IsRegistered(System::Reflection::meta::Type const* type, XamlCPP::Core::Dependency::IPropertyObject* property) = 0;
        virtual std::vector<XamlCPP::Core::Dependency::IPropertyObject*> GetRegistered(System::Reflection::meta::Type const* o) = 0;
    };
}


#endif	//#ifndef __XAMLCPP_IDEPENDENCYOBJECT_H__

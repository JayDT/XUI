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

#ifndef __XAMLCPP_BINDING_H__
#define __XAMLCPP_BINDING_H__

#include "standard/BasicPrimities.h"
#include "standard/events.h"
#include "Reflection/Runtime/Variant.h"
#include "Reflection/Runtime/Object.h"

#include "XamlCPP/Interfaces/IBinding.h"
#include "XamlCPP/Core/Dependencies/IPropertyObject.h"
#include "XamlCPP/Interfaces/IValueConverter.h"
#include "XamlCPP/DataBind/InstancedBinding.h"
#include "XamlCPP/DataBind/SourceObserver.h"

#include "rxcpp/rx.hpp"

namespace XamlCPP::Interfaces
{
    struct INameScope;
}

namespace XamlCPP::DataBind
{
    /// <summary>
    /// A XAML binding.
    /// </summary>
    class TC_XAMLCPP_API Binding : public Interfaces::IBinding
    {
    private:
        XamlCPP::Interfaces::IValueConverter* _converter = nullptr;
        System::Reflection::Variant _converterParam;
        System::Reflection::Variant _fallbackValue;
        System::Reflection::Variant _source;
        std::string _elementName;
        std::string _path;
        XamlCPP::BindingMode _bindingMode = XamlCPP::BindingMode::Default;
        XamlCPP::BindingLevel _bindPriority = XamlCPP::BindingLevel::LocalValue;
		mutable Markup::RelativeSource _relativeSource;
		const System::Reflection::Type* _ancestorType = nullptr;

    public:

        struct PathInfo
        {
            std::string Path;
            std::string ElementName;
        };

        /// <summary>
        /// Initializes a new instance of the <see cref="Binding"/> class.
        /// </summary>
        Binding();

        /// <summary>
        /// Initializes a new instance of the <see cref="Binding"/> class.
        /// </summary>
        /// <param name="path">The binding path.</param>
        /// <param name="mode">The binding mode.</param>
        Binding(std::string path, BindingMode mode = BindingMode::Default);

        virtual ~Binding() { }

        /// <summary>
        /// Gets or sets the <see cref="IValueConverter"/> to use.
        /// </summary>
        PROPERTY_GS(_converter, Converter);

        XamlCPP::Interfaces::IValueConverter* GetConverterOrDefault();

        /// <summary>
        /// Gets or sets a parameter to pass to <see cref="Converter"/>.
        /// </summary>
        PROPERTY_(System::Reflection::meta::Variant const&, ConverterParameter);
        System::Reflection::meta::Variant const& GetConverterParameter() const { return _converterParam; }
        void SetConverterParameter(System::Reflection::meta::Variant const& value) { _converterParam = value; }

        /// <summary>
        /// Gets or sets the name of the element to use as the binding source.
        /// </summary>
        PROPERTY_(std::string const&, ElementName);
        std::string const& GetElementName() const { return _elementName; }
        void SetElementName(std::string const& value) { _elementName = value; }

        /// <summary>
        /// Gets or sets the value to use when the binding is unable to produce a value.
        /// </summary>
        PROPERTY_(System::Reflection::meta::Variant const&, FallbackValue);
        System::Reflection::meta::Variant const& GetFallbackValue() const { return _fallbackValue; }
        void SetFallbackValue(System::Reflection::meta::Variant const& value) { _fallbackValue = value; }

        /// <summary>
        /// Gets or sets the binding mode.
        /// </summary>
        PROPERTY_(BindingMode, Mode);
        BindingMode GetMode() const { return _bindingMode; }
        void SetMode(BindingMode value) { _bindingMode = value; }

        /// <summary>
        /// Gets or sets the binding path.
        /// </summary>
        PROPERTY_(std::string const&, Path);
        std::string const& GetPath() const { return _path; }
        void SetPath(std::string const& value) { _path = value; }

        /// <summary>
        /// Gets or sets the binding priority.
        /// </summary>
		PROPERTY_(XamlCPP::BindingLevel, BindingLevel);
		XamlCPP::BindingLevel GetBindingLevel() const { return _bindPriority; }
		void SetBindingLevel(XamlCPP::BindingLevel level) override { _bindPriority = level; }

        /// <summary>
        /// Gets or sets the relative source for the binding.
        /// </summary>
		PROPERTY_(Markup::RelativeSource*, RelativeSource);
		Markup::RelativeSource* GetRelativeSource() const { return &_relativeSource; }
		void SetRelativeSource(Markup::RelativeSource* value) { _relativeSource = *value; }

		//AncestorType

        /// <summary>
        /// Gets or sets the source for the binding.
        /// </summary>
        PROPERTY_(System::Reflection::meta::Variant const&, Source);
        System::Reflection::meta::Variant const& GetSource() const { return _source; }
        void SetSource(System::Reflection::meta::Variant const& value) { _source = value; }

        /// <inheritdoc/>
        std::shared_ptr<DataBind::InstancedBinding> Initiate(
            Core::Dependency::IDependencyObject* target,
            Core::Dependency::IPropertyObject* targetProperty,
            System::Reflection::Object* anchor = nullptr,
            bool enableDataValidation = false,
            std::shared_ptr<XamlCPP::Interfaces::INameScope>* namedScope = nullptr) override;

        std::shared_ptr<XamlCPP::DataBind::SourceObserverNode> GetSourceObserver();

        static PathInfo ParsePath(std::string const& path);

        void ValidateState(PathInfo pathInfo);

        std::shared_ptr<XamlCPP::DataBind::SourceObserver> CreateDataContexObserver(
            Core::Dependency::IDependencyObject* target,
            std::string path,
            bool targetIsDataContext,
            System::Reflection::Object* anchor,
            bool enableDataValidation);

		std::shared_ptr<XamlCPP::DataBind::SourceObserver> CreateRelativeSourceObserver(
			Core::Dependency::IDependencyObject* target,
			std::string path,
			System::Reflection::Object* anchor,
			bool enableDataValidation);

        std::shared_ptr<XamlCPP::DataBind::SourceObserver> CreateElementObserver(XamlCPP::Core::Dependency::IDependencyObject* target, std::string const& elementName, std::string const& path, std::shared_ptr<XamlCPP::Interfaces::INameScope>* namedScope);

        std::shared_ptr<XamlCPP::DataBind::SourceObserver> CreateSourceObserver(
            System::Reflection::RefObject source,
            std::string const& path,
            bool enabledDataValidation);

        std::shared_ptr<XamlCPP::DataBind::SourceObserver> CreateTemplatedParentObserver(
            Core::Dependency::IDependencyObject* target,
            std::string const& path);

        std::shared_ptr<XamlCPP::Core::Dependency::GenericObservable> GetParentDataContext(XamlCPP::Core::Dependency::IDependencyObject* target);

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
        static rxcpp::subscription Apply(
            Core::Dependency::IDependencyObject* target,
            Core::Dependency::IPropertyObject* property,
            InstancedBinding* binding,
            System::Reflection::Object* anchor);
    };
}

#endif // !__XAMLCPP_BINDING_H__
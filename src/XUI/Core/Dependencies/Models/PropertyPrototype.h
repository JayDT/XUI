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

#ifndef __PROPERTY_PROTOTYPE_H__
#define __PROPERTY_PROTOTYPE_H__

#include "XamlCPP/Core/Dependencies/IPropertyObject.h"
#include "Core/Dependencies/DependencyDefines.h"

#include "standard/BasicPrimities.h"
#include "Reflection/Runtime/Variant.h"
#include "Reflection/Runtime/TypeConfig.h"

#include "rxcpp/rx.hpp"

MetaAssemblyBegin
Meta(modul: ReflInclude("Core/Dependencies/Models/PropertyMetadata.h"))
MetaAssemblyEnd

namespace XUI::Core::Dependency
{
    /// <summary>
    /// Base class for dependency property metadata.
    /// </summary>
    class PropertyMetadata;
}

namespace XUI::Core::Dependency
{
    class DependencyObject;

    class TC_XUI_API DependencyProperty : public XamlCPP::Core::Dependency::IPropertyObject
    {
    private:
        uint _id;

        std::string _name;
        System::Reflection::meta::Type const* _propertyType;
        System::Reflection::meta::Type const* _ownerType;

        property_change_subcriber_t _initialized;
        property_change_subcriber_t _changed;

        std::shared_ptr<PropertyMetadata> _defaultMetadata;
        NotifyFunction _notifying;

        typedef std::map<System::Reflection::meta::Type const*, std::shared_ptr<PropertyMetadata>> MetaDataMap;
        typedef google::dense_hash_map<System::Reflection::meta::Type const*, PropertyMetadata*> MetaDataCacheMap;

        mutable MetaDataMap _metadata;
        mutable MetaDataCacheMap _metadataCache;

    protected:
        DependencyProperty(std::string name,
            System::Reflection::meta::Type const* OwnerType,
            System::Reflection::meta::Type const* ValueType,
            std::shared_ptr<PropertyMetadata> const& metadata,
            NotifyFunction* Notifying = nullptr);

        DependencyProperty(
            DependencyProperty* source,
            System::Reflection::meta::Type const* OwnerType,
            std::shared_ptr<PropertyMetadata> metadata = nullptr);

        virtual ~DependencyProperty()
        {}

    public:
        PROPERTY_(uint, Id);
        virtual uint GetId() override { return _id; }
        virtual void SetId(uint const& value) { _id = value; }

        virtual bool GetInherits() override { return false; }
        virtual bool GetIsAttached() override { return false; }
        virtual bool GetIsDirect() override { return false; }
        virtual bool GetIsReadOnly() override { return false; }

        PROPERTY_(std::string const&, Name);
        virtual std::string const& GetName() override { return _name; }
        virtual void SetName(std::string const& value) { _name = value; }

        PROPERTY_GET_ONLY(System::Reflection::meta::Type const*, PropertyType);
        System::Reflection::meta::Type const* GetPropertyType() override { return _propertyType; }

        PROPERTY_GET_ONLY(System::Reflection::meta::Type const*, OwnerType);
        System::Reflection::meta::Type const* GetOwnerType() override { return _ownerType; }

        PROPERTY_(property_change_subcriber_t&, Initialized);
        virtual property_change_subcriber_t& GetInitialized() override { return _initialized; }
        virtual void SetInitialized(property_change_subcriber_t& value) { _initialized = value; }

        PROPERTY_(property_change_subcriber_t&, Changed);
        virtual property_change_subcriber_t& GetChanged() override { return _changed; }
        virtual void SetChanged(property_change_subcriber_t& value) { _changed = value; }

        PROPERTY_(const NotifyFunction &, Notifying);
        const NotifyFunction & GetNotifying() override { return _notifying; }
        void SetNotifying(const NotifyFunction & value) { _notifying = value; }

        //virtual void SetValue(XamlCPP::Core::Dependency::IDependencyObject* instance, System::Reflection::meta::Variant const& value) = 0;

        template<typename TOwner>
        DependencyProperty* AddOwner()
        {
            static_assert(std::is_base_of<DependencyObject, typename std::remove_pointer<typename std::remove_reference<TOwner>::type>::type>::value);
            DependencyPropertyMgr::Instance()->Register(&typeof(TOwner), shared_from_base_static<IPropertyObject>());
            return this;
        }

        /// <summary>
        /// Gets the property metadata for the specified type.
        /// </summary>
        /// <param name="type">The type.</param>
        /// <returns>
        /// The property metadata.
        /// </returns>
        ///
        PropertyMetadata* GetMetadata(System::Reflection::meta::Type const* type) const;

        /// <summary>
        /// Overrides the default value for the property on the specified type.
        /// </summary>
        /// <typeparam name="T">The type.</typeparam>
        /// <param name="defaultValue">The default value.</param>
        template<typename TOwner, typename TValue>
        void OverrideDefaultValue(TValue const& defaultValue)
        {
            static const System::Reflection::Type& ownerType = _template_typeof(TOwner);
            static_assert(typename std::is_base_of<DependencyObject, typename std::remove_pointer<typename std::remove_reference<TOwner>::type>::type>::value);
            OverrideDefaultValue(&ownerType, System::Reflection::Variant(defaultValue, (System::Reflection::variant_policy::Copy*)nullptr));
        }

        /// <summary>
        /// Overrides the default value for the property on the specified type.
        /// </summary>
        /// <param name="type">The type.</param>
        /// <param name="defaultValue">The default value.</param>
        virtual void OverrideDefaultValue(System::Reflection::meta::Type const* type, System::Reflection::meta::Variant const& defaultValue) = 0;

        template<typename TObject>
        std::shared_ptr<PropertyMetadata> GetMetadata() const
        {
            static const System::Reflection::Type& objectType = typeof(TObject);
            EXCEPT(objectType.IsValid());
            return GetMetadata(objectType);
        }

        /// <summary>
        /// Overrides the metadata for the property on the specified type.
        /// </summary>
        /// <typeparam name="T">The type.</typeparam>
        /// <param name="metadata">The metadata.</param>
        template<class TDepedencyObject>
        void OverrideMetadata(std::shared_ptr<PropertyMetadata> const& metadata)
        {
            auto type = &typeof(TDepedencyObject);
            //System::ThrowIfFailed<System::ArgumentException>(_propertyType->IsAssignableFrom(*metadata->GetDefaultValue(type).GetType()), "Incompatible default value");
            DependencyProperty::OverrideMetadata(type, metadata);
        }

        /// <summary>
        /// Overrides the metadata for the property on the specified type.
        /// </summary>
        /// <param name="type">The type.</param>
        /// <param name="metadata">The metadata.</param>
        void OverrideMetadata(System::Reflection::meta::Type const* type, std::shared_ptr<PropertyMetadata> metadata);

        BindingMode GetDefaultBindingMode(System::Reflection::Type const*) override final;
    };

    typedef std::shared_ptr<DependencyProperty> RefDependencyProperty;
}

#endif // !__PROPERTY_PROTOTYPE_H__
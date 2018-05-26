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

#ifndef __XAMLCPP_DYNAMIC_RESOURCE_BINDING_H__
#define __XAMLCPP_DYNAMIC_RESOURCE_BINDING_H__

#include "standard/BasicPrimities.h"
#include "standard/events.h"
#include "Reflection/Runtime/Variant.h"
#include "Reflection/Runtime/Object.h"

#include "XamlCPP/Interfaces/IBinding.h"
#include "XamlCPP/Core/Dependencies/IPropertyObject.h"
#include "XamlCPP/Interfaces/IValueConverter.h"
#include "XamlCPP/DataBind/InstancedBinding.h"
#include "XamlCPP/DataBind/SourceObserver.h"
#include "XamlCPP/Interfaces/IResourceMap.h"

#include "rxcpp/rx.hpp"

namespace XamlCPP::DataBind
{
    class DynamicResourceBinding : public Interfaces::IBinding
    {
    private:
        std::string m_ResourceKey;
        Interfaces::IResourceNode *_anchor = nullptr;
		std::shared_ptr<Interfaces::IMarkupExtension> m_markup;
		XamlCPP::BindingLevel _bindinglevel = BindingLevel::LocalValue;

    public:
        virtual ~DynamicResourceBinding()
        {
        }

        DynamicResourceBinding()
        {
        }

        DynamicResourceBinding(Interfaces::IResourceNode * anchor, std::string _ResourceKey, std::shared_ptr<Interfaces::IMarkupExtension> const& _markup)
        {
            _anchor = anchor;
            m_ResourceKey = _ResourceKey;
			m_markup = _markup;
        }

		void SetBindingLevel(XamlCPP::BindingLevel level) override
        {
			_bindinglevel = level;
        }

        /// <inheritdoc/>
        std::shared_ptr<DataBind::InstancedBinding> Initiate(
            Core::Dependency::IDependencyObject* target,
            Core::Dependency::IPropertyObject* targetProperty,
            System::Reflection::Object* anchor = nullptr,
            bool enableDataValidation = false,
            std::shared_ptr<XamlCPP::Interfaces::INameScope>* namedScope = nullptr) override;
    };
}

#endif //!__XAMLCPP_DYNAMIC_RESOURCE_BINDING_H__
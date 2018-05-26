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

#ifndef __XAMLCPP_DYNAMICRESOURCEEXTENSION_H__
#define __XAMLCPP_DYNAMICRESOURCEEXTENSION_H__

#include <System/System.h>
#include <standard/misc.h>
#include "Interfaces/IMarkupExtension.h"
#include "Interfaces/IValueConverter.h"
#include "Interfaces/IBinding.h"
#include "Interfaces/IResourceMap.h"

namespace XamlCPP::Markup
{
    class DynamicResourceExtension : public Interfaces::IMarkupExtension
    {
    private:
        std::string m_ResourceKey;

    public:
        virtual ~DynamicResourceExtension()
        {
        }

        DynamicResourceExtension()
        {
        }

        DynamicResourceExtension(const std::string &resourceKey)
        {
            SetResourceKey(resourceKey);
        }


		DynamicResourceExtension(Interfaces::IMarkupExtension* markup)
			: m_markup(markup->shared_from_this())
		{
		}

		std::shared_ptr<Interfaces::IMarkupExtension> m_markup;
        PROPERTY_(std::string, ResourceKey);
        std::string GetResourceKey() const
        {
            return m_ResourceKey;
        }
        void SetResourceKey(const std::string &value)
        {
            m_ResourceKey = value;
        }

        System::Reflection::Variant ProvideValue(Core::MarkupExtensionContext * markupExtensionContext) override;

    private:
        Interfaces::IResourceNode * GetAnchor(Core::MarkupExtensionContext * context);
    };
}


#endif	//#ifndef __XAMLCPP_DYNAMICRESOURCEEXTENSION_H__

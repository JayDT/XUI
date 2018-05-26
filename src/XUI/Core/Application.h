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

#ifndef __XUI_APPLICATION_H__
#define __XUI_APPLICATION_H__

#include "Core/Dependencies/DependencyObject.h"
#include "XamlCPP/Interfaces/IResourceMap.h"

namespace XUI::Core
{
    struct TC_XUI_API Application : Core::Dependency::DependencyObject, XamlCPP::Interfaces::IResourceNode
    {
        Application()
        {

        }

        virtual ~Application()
        {

        }

        static void InitializeDefaultResoures();

        static XamlCPP::Interfaces::ResourceMap m_Resources;
        static XamlCPP::Interfaces::ResourceMap m_DataTemplates;

        PROPERTY_(XamlCPP::Interfaces::ResourceMap const&, Resources);
        XamlCPP::Interfaces::ResourceMap const& GetResources() const override
        {
            return m_Resources;
        }

        void SetResources(const XamlCPP::Interfaces::ResourceMap &value)
        {
            for (auto i : value.GetNativeEnumerator())
            {
                m_Resources.Add(i.first, i.second);
            }
        }

        PROPERTY_(XamlCPP::Interfaces::ResourceMap const&, DataTemplates);
        XamlCPP::Interfaces::ResourceMap const& GetDataTemplates() const
        {
            return m_DataTemplates;
        }

        void SetDataTemplates(const XamlCPP::Interfaces::ResourceMap &value)
        {
            for (auto i : value.GetNativeEnumerator())
            {
                m_DataTemplates.Add(i.first, i.second);
            }
        }

        IResourceNode* GetResourceParent() const override { return const_cast<IResourceNode*>(static_cast<const XamlCPP::Interfaces::IResourceNode*>(this)); }
        XamlCPP::Core::Dependency::IDependencyObject* ToIDependencyObject2() override { return this; }
        System::Reflection::Variant const& FindResource(const std::string &name) const override;

    };
}

#endif // !__XUI_APPLICATION_H__

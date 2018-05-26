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

#ifndef __XUI_UI_DATATEMPLATES_FUNCTREEDATATEMPLATE_H__
#define __XUI_UI_DATATEMPLATES_FUNCTREEDATATEMPLATE_H__

#include "UI/DataTemplates/FuncDataTemplate.h"
#include "XamlCPP/DataBind/InstancedBinding.h"

namespace XUI::UI::DataTemplates
{
    /// <summary>
    /// A template used to build hierachical data.
    /// </summary>
    class
        Meta(XamlCPP::MapKeyPropertyAttribute("DataTypeName"))
        TC_XUI_API
        HierarchicalDataTemplate : public XUI::UI::DataTemplates::DataTemplate
    {
    private:
        std::shared_ptr<XamlCPP::Interfaces::IBinding> _itemsSource;
        
    public:

        PROPERTY_(std::shared_ptr<XamlCPP::Interfaces::IBinding>, ItemsSource);
        std::shared_ptr<XamlCPP::Interfaces::IBinding> GetItemsSource() const { return _itemsSource; }
        void SetItemsSource(std::shared_ptr<XamlCPP::Interfaces::IBinding> const& value) { _itemsSource = value; }
    };
}

#endif //!__XUI_UI_DATATEMPLATES_FUNCTREEDATATEMPLATE_H__
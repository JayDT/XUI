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

#ifndef __XUI_HEADEREDITEMSCONTROL_H__
#define __XUI_HEADEREDITEMSCONTROL_H__

#include "UI/Controls/ItemsControl.h"

namespace XUI::UI
{
    /// <summary>
    /// Represents an <see cref="ItemsControl"/> with a related header.
    /// </summary>
    class TC_XUI_API HeaderedItemsControl : public Controls::ItemsControl
    {
    private:
        std::shared_ptr<Presenters::ContentPresenter> m_HeaderPresenter;
        std::shared_ptr<Core::Dependency::DependencyObject> m_headerDataTemplateRoot;

    public:
        /// <summary>
        /// Defines the <see cref="Header"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty HeaderProperty;

        /// <summary>
        /// Defines the <see cref="ContentTemplate"/> property.
        /// </summary>
        //static Core::Dependency::RefDependencyProperty HeaderTemplateProperty;

        /// <summary>
        /// Initializes static members of the <see cref="ContentControl"/> class.
        /// </summary>
        static void StaticClassInitializer();

        /// <summary>
        /// Gets or sets the content of the control's header.
        /// </summary>
        PROPERTY_(System::Reflection::Variant  , Header);
        System::Reflection::Variant GetHeader() const;
        void SetHeader(System::Reflection::Variant value);

        /// <summary>
        /// Gets or sets the data template used to display the content of the control.
        /// </summary>
        //PROPERTY_(std::shared_ptr<UI::DataTemplates::DataTemplate>, HeaderTemplate);
        //std::shared_ptr<UI::DataTemplates::DataTemplate> GetHeaderTemplate() const;
        //void SetHeaderTemplate(std::shared_ptr<UI::DataTemplates::DataTemplate> const& value);

        /// <summary>
        /// Gets the header presenter from the control's template.
        /// </summary>
        PROPERTY_(Presenters::ContentPresenter * , HeaderPresenter);
        Presenters::ContentPresenter *GetHeaderPresenter() const;
        void SetHeaderPresenter(Presenters::ContentPresenter *value);

        //void ApplyDataTemplate() override;
        //void RemoveDataTemplate() override;
        //
        //std::shared_ptr<XUI::UI::DataTemplates::DataTemplate> FindDataTemplate(System::Reflection::meta::Variant const & data) override;

    protected:
        /// <inheritdoc/>
        void OnTemplateApplied(Controls::TemplateAppliedEventArgs const& e) override;
    };
}


#endif	//#ifndef __XUI_HEADEREDITEMSCONTROL_H__

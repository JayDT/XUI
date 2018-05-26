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

#ifndef __XUI_UI_CONTROLS_CONTENT_CONTROL_H__
#define __XUI_UI_CONTROLS_CONTENT_CONTROL_H__

#include "UI/Controls/Control.h"
#include "UI/Presenters/ContentPresenter.h"

namespace XUI::UI::Controls
{
    Meta(AbstractType) class TC_XUI_API ContentControl : public Control
    {
    private:
        friend class UI::DataTemplates::DataTemplate;

        std::shared_ptr<Presenters::ContentPresenter> m_contentPresenter;

    public:
        /// <summary>
        /// Defines the <see cref="Content"/> property.
        /// </summary>
        static  Core::Dependency::RefDependencyProperty ContentProperty;

        /// <summary>
        /// Defines the <see cref="ContentTemplate"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty ContentTemplateProperty;

        /// <summary>
        /// Defines the <see cref="HorizontalContentAlignment"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty HorizontalContentAlignmentProperty;

        /// <summary>
        /// Defines the <see cref="VerticalContentAlignment"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty VerticalContentAlignmentProperty;

        static void StaticClassInitializer();

        virtual ~ContentControl();

        /// <summary>
        /// Gets or sets the content to display.
        /// </summary>
        Meta(XamlCPP::ContentAttribute())
        PROPERTY_(System::Reflection::meta::Variant, Content);
        System::Reflection::meta::Variant GetContent() const;
        void SetContent(System::Reflection::meta::Variant const& value);

        /// <summary>
        /// Gets or sets the data template used to display the content of the control.
        /// </summary>
        PROPERTY_(std::shared_ptr<UI::DataTemplates::DataTemplate>, ContentTemplate);
        std::shared_ptr<UI::DataTemplates::DataTemplate> GetContentTemplate() const;
        void SetContentTemplate(std::shared_ptr<UI::DataTemplates::DataTemplate> const& value);

        /// <summary>
        /// Gets the presenter from the control's template.
        /// </summary>
        PROPERTY_(Presenters::ContentPresenter*, Presenter);
        Presenters::ContentPresenter* GetPresenter() const { return m_contentPresenter.get(); }
        void SetPresenter(Presenters::ContentPresenter* value);

        /// <summary>
        /// Gets or sets the horizontal alignment of the content within the control.
        /// </summary>
        PROPERTY_(XUI::UI::Controls::HorizontalAlignment, HorizontalContentAlignment);
        XUI::UI::Controls::HorizontalAlignment GetHorizontalContentAlignment() const;
        void SetHorizontalContentAlignment(XUI::UI::Controls::HorizontalAlignment value);

        /// <summary>
        /// Gets or sets the vertical alignment of the content within the control.
        /// </summary>
        PROPERTY_(XUI::UI::Controls::VerticalAlignment, VerticalContentAlignment);
        XUI::UI::Controls::VerticalAlignment GetVerticalContentAlignment() const;
        void SetVerticalContentAlignment(XUI::UI::Controls::VerticalAlignment value);
    };
}

#endif //!__XUI_UI_CONTROLS_CONTENT_CONTROL_H__

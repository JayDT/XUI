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

#ifndef __XUI_UI_PRESENTERS_CONTENTPRESENTER_H__
#define __XUI_UI_PRESENTERS_CONTENTPRESENTER_H__

#include "UI/Controls/Control.h"

namespace XUI::UI
{
    class HeaderedItemsControl;
    class HeaderedSelectingItemsControl;

    namespace Controls
    {
        class ContentControl;
    }
}

namespace XUI::UI::Presenters
{
    class TC_XUI_API ContentPresenter : public Controls::Control
    {
        friend class UI::HeaderedItemsControl;
        friend class UI::HeaderedSelectingItemsControl;

    public:

        /// <summary>
        /// Defines the <see cref="Content"/> property.
        /// </summary>
        static  Core::Dependency::RefDependencyProperty RecognizesAccessKeyProperty;

        /// <summary>
        /// Defines the <see cref="Content"/> property.
        /// </summary>
        static  Core::Dependency::RefDependencyProperty ContentProperty;

        /// <summary>
        /// Defines the <see cref="Child"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty ChildProperty;

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

        /// <summary>
        /// Defines the <see cref="CornerRadius"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty CornerRadiusProperty;

        static void StaticClassInitializer();
    private:

        std::shared_ptr<Controls::Control> _child;
        bool _createdChild = false;
        bool _inBuildHost = false;
        std::shared_ptr<Core::Dependency::DependencyObject> m_dataTemplateRoot;
        std::shared_ptr<UI::DataTemplates::DataTemplate> m_dataTemplate;

    public:

        ContentPresenter();
        virtual ~ContentPresenter();

        /// <summary>
        /// Gets or sets the content to display.
        /// </summary>
        PROPERTY_(bool, RecognizesAccessKey);
        bool GetRecognizesAccessKey() const;
        void SetRecognizesAccessKey(bool const& value);

        /// <summary>
        /// Gets or sets the content to display.
        /// </summary>
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

        /// <summary>
        /// Gets or sets the decorated control.
        /// </summary>
        PROPERTY_(float, CornerRadius);
        float GetCornerRadius() const;
        void SetCornerRadius(float value);

        /// <summary>
        /// Gets or sets the decorated control.
        /// </summary>
        Meta(XamlCPP::ContentAttribute())
            PROPERTY_(std::shared_ptr<UI::Controls::Control>, Child);
        std::shared_ptr<UI::Controls::Control> const& GetChild() const;
        void SetChild(std::shared_ptr<UI::Controls::Control> const& value);

        void Render(Interfaces::IDrawingContextImpl* context) override;

        /// <inheritdoc/>
        void ApplyTemplate() override
        {
            if (!_createdChild && IsAttachedToLogicalTree)
            {
                UpdateChild();
            }
        }

        /// <summary>
        /// Updates the <see cref="Child"/> control based on the control's <see cref="Content"/>.
        /// </summary>
        /// <remarks>
        /// Usually the <see cref="Child"/> control is created automatically when 
        /// <see cref="ApplyTemplate"/> is called; however for this to happen, the control needs to
        /// be attached to a logical tree (if the control is not attached to the logical tree, it
        /// is reasonable to expect that the DataTemplates needed for the child are not yet 
        /// available). This method forces the <see cref="Child"/> control's creation at any point, 
        /// and is particularly useful in unit tests.
        /// </remarks>
        void UpdateChild();

        void ApplyDataTemplate() override;
        void RemoveDataTemplate() override;

        std::shared_ptr<XUI::UI::DataTemplates::DataTemplate> FindDataTemplate(System::Reflection::meta::Variant const& data) override;

    protected:

        /// <inheritdoc/>
        void OnAttachedToVisualTree(Controls::VisualTreeAttachmentEventArgs const& e) override;

        /// <inheritdoc/>
        void OnAttachedToLogicalTree(Controls::LogicalTreeAttachmentEventArgs& e) override;

        /// <summary>
        /// Creates the child control.
        /// </summary>
        /// <returns>The child control or null.</returns>
        Controls::Control* CreateChild();

        /// <inheritdoc/>
        Core::Media::Size MeasureOverride(Core::Media::Size const& availableSize) override;

        /// <inheritdoc/>
        Core::Media::Size ArrangeOverride(Core::Media::Size const& finalSize) override;

        /// <summary>
        /// Called when the <see cref="Content"/> property changes.
        /// </summary>
        /// <param name="e">The event args.</param>
        void ContentChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const& e);

        void TemplatedParentChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const& e);

        void SyncProperties(XUI::UI::Controls::Control * e);
    };
}

#endif // !__XUI_UI_PRESENTERS_CONTENTPRESENTER_H__
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

#ifndef __XUI_UI_CONTROLS_UI_ELEMENT_H__
#define __XUI_UI_CONTROLS_UI_ELEMENT_H__

#include "UI/Controls/InputElement.h"
#include "UI/DataTemplates/ControlTemplate.h"
#include "Core/Media/FontStyle.h"
#include "Interfaces/UIControls/IControl.h"

namespace XUI::UI
{
    class Popup;
    class HeaderedItemsControl;
    class HeaderedSelectingItemsControl;

    namespace Presenters
    {
        class ContentPresenter;
    }
}

namespace XUI::UI::Controls
{
    class UIElement;
    class Control;

    /// <summary>
    /// Holds the event arguments for the <see cref="ILogical.AttachedToLogicalTree"/> and 
    /// <see cref="ILogical.DetachedFromLogicalTree"/> events.
    /// </summary>
    struct LogicalTreeAttachmentEventArgs : System::Events::EventArg
    {
        /// <summary>
        /// Initializes a new instance of the <see cref="LogicalTreeAttachmentEventArgs"/> class.
        /// </summary>
        /// <param name="root">The root of the logical tree.</param>
        LogicalTreeAttachmentEventArgs(Control* root);

        /// <summary>
        /// Gets the root of the logical tree that the control is being attached to or detached from.
        /// </summary>
        std::weak_ptr<Control> Root;
    };

    class TC_XUI_API UIElement : public InputElement
    {
        friend class XUI::Core::Dependency::DependencyObject;
        friend class XUI::UI::Popup;
        friend class XUI::UI::HeaderedItemsControl;
        friend class XUI::UI::HeaderedSelectingItemsControl;
        friend class XUI::UI::Controls::Control;
        friend class XUI::UI::Presenters::ContentPresenter;

    public:

        /// <summary>
        /// Defines the <see cref="Background"/> property.
        /// </summary>
        static  Core::Dependency::RefDependencyProperty BackgroundProperty;

        /// <summary>
        /// Defines the <see cref="BorderBrush"/> property.
        /// </summary>
        static  Core::Dependency::RefDependencyProperty BorderBrushProperty;

        /// <summary>
        /// Defines the <see cref="BorderThickness"/> property.
        /// </summary>
        static  Core::Dependency::RefDependencyProperty BorderThicknessProperty;

        /// <summary>
        /// Defines the <see cref="FontFamily"/> property.
        /// </summary>
        static  Core::Dependency::RefDependencyProperty FontFamilyProperty;

        /// <summary>
        /// Defines the <see cref="FontSize"/> property.
        /// </summary>
        static  Core::Dependency::RefDependencyProperty FontSizeProperty;

        /// <summary>
        /// Defines the <see cref="FontStyle"/> property.
        /// </summary>
        static  Core::Dependency::RefDependencyProperty FontStyleProperty;

        /// <summary>
        /// Defines the <see cref="FontWeight"/> property.
        /// </summary>
        static  Core::Dependency::RefDependencyProperty FontWeightProperty;

        /// <summary>
        /// Defines the <see cref="Foreground"/> property.
        /// </summary>
        static  Core::Dependency::RefDependencyProperty ForegroundProperty;

        /// <summary>
        /// Defines the <see cref="Padding"/> property.
        /// </summary>
        static  Core::Dependency::RefDependencyProperty PaddingProperty;

        static  Core::Dependency::RefDependencyProperty SnapsToDevicePixelsProperty;
        static  Core::Dependency::RefDependencyProperty OverridesDefaultStyleProperty;

        /// <summary>
        /// Defines the <see cref="TemplateApplied"/> routed event.
        /// </summary>
        static std::shared_ptr<Core::Observer::RoutedEvent> TemplateAppliedEvent;

        static void StaticClassInitializer();

    protected:
        static  Core::Dependency::RefDependencyProperty TemplatedParentProperty;

    private:
        mutable std::shared_ptr<Core::Dependency::DependencyObject> m_templatedParent;
        mutable bool m_cachedTemplateParent = false;
        bool m_snapsToDevicePixels = false;
        bool m_overridesDefaultStyle = false;

    public:
        UIElement();
        virtual ~UIElement();

        Core::Observer::SpecPropertyRoutedEventHandler<&TemplateAppliedEvent>    TemplateApplied;

        PROPERTY_(bool, SnapsToDevicePixels);
        bool GetSnapsToDevicePixels() const { return m_snapsToDevicePixels; }
        void SetSnapsToDevicePixels(bool value);

        PROPERTY_(bool, OverridesDefaultStyle);
        bool GetOverridesDefaultStyle() const { return m_overridesDefaultStyle; }
        void SetOverridesDefaultStyle(bool value);

        PROPERTY_(std::shared_ptr<Core::Dependency::DependencyObject>, TemplatedParent);
        std::shared_ptr<Core::Dependency::DependencyObject> const& GetTemplatedParent() const;
    protected:
        virtual void SetTemplatedParent(std::shared_ptr<Core::Dependency::DependencyObject> const& value);

    public:

        Core::Dependency::DependencyObject* GetInheritTemplatedParent() const override final
        {
            return m_templatedParent.get();
        }

        void OnInheritanceParentChanged() override;

        /// <summary>
        /// Gets or sets the brush used to draw the control's background.
        /// </summary>
        PROPERTY_(std::shared_ptr<XUI::Interfaces::IBrush>, Background);
        std::shared_ptr<XUI::Interfaces::IBrush> GetBackground() const;
        void SetBackground(std::shared_ptr<XUI::Interfaces::IBrush> value);

        /// <summary>
        /// Gets or sets the brush used to draw the control's border.
        /// </summary>
        PROPERTY_(std::shared_ptr<XUI::Interfaces::IBrush>, BorderBrush);
        std::shared_ptr<XUI::Interfaces::IBrush> GetBorderBrush() const;
        void SetBorderBrush(std::shared_ptr<XUI::Interfaces::IBrush> value);

        /// <summary>
        /// Gets or sets the thickness of the control's border.
        /// </summary>
        PROPERTY_(double, BorderThickness);
        double GetBorderThickness() const;
        void SetBorderThickness(double value);

        /// <summary>
        /// Gets or sets the font family used to draw the control's text.
        /// </summary>
        PROPERTY_(std::string, FontFamily);
        std::string GetFontFamily() const;
        void SetFontFamily(std::string value);

        /// <summary>
        /// Gets or sets the size of the control's text in points.
        /// </summary>
        PROPERTY_(double, FontSize);
        double GetFontSize() const;
        void SetFontSize(double value);

        /// <summary>
        /// Gets or sets the font style used to draw the control's text.
        /// </summary>
        PROPERTY_(Core::Media::FontStyle, FontStyle);
        Core::Media::FontStyle GetFontStyle() const;
        void SetFontStyle(Core::Media::FontStyle value);

        /// <summary>
        /// Gets or sets the font weight used to draw the control's text.
        /// </summary>
        PROPERTY_(Core::Media::FontWeight, FontWeight);
        Core::Media::FontWeight GetFontWeight() const;
        void SetFontWeight(Core::Media::FontWeight value);

        /// <summary>
        /// Gets or sets the brush used to draw the control's text and other foreground elements.
        /// </summary>
        PROPERTY_(std::shared_ptr<XUI::Interfaces::IBrush>, Foreground);
        std::shared_ptr<XUI::Interfaces::IBrush> GetForeground() const;
        void SetForeground(std::shared_ptr<XUI::Interfaces::IBrush> value);

        /// <summary>
        /// Gets or sets the padding placed between the border of the control and its content.
        /// </summary>
        PROPERTY_(Core::Media::Thickness, Padding);
        Core::Media::Thickness GetPadding() const;
        void SetPadding(Core::Media::Thickness value);

        /// <inheritdoc/>
        UIElement* GetTemplateFocusTarget();
    };
}

#endif //!__XUI_UI_CONTROLS_UI_ELEMENT_H__

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

#ifndef __XUI_UI_TEXTBLOCK_H__
#define __XUI_UI_TEXTBLOCK_H__

#include "UI/Controls/Control.h"
#include "XamlCPP/Interfaces/ReflAttributes.h"
#include "Core/Media/FormattedText_t.h"

namespace XUI::UI
{
    class TC_XUI_API TextBlock : public Controls::Control
    {
    public:

        /// <summary>
        /// Defines the <see cref="Text"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty TextProperty;

        /// <summary>
        /// Defines the <see cref="TextAlignment"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty TextAlignmentProperty;

        /// <summary>
        /// Defines the <see cref="TextWrapping"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty TextWrappingProperty;

        static void StaticClassInitializer();

    protected:

        mutable Core::Media::FormattedTextWrapper _formattedText;
        mutable bool _isValidFormattedText = false;

    public:

        TextBlock();
        virtual ~TextBlock();

        /// <summary>
        /// Gets or sets the text.
        /// </summary>
        Meta(XamlCPP::ContentAttribute())
        PROPERTY_(std::wstring, Text);
        std::wstring const& GetText() const { return _formattedText._text; }
        virtual void SetText(std::wstring const& value);

        PROPERTY_(Core::Media::TextWrapping, TextWrapping);
        Core::Media::TextWrapping GetTextWrapping() const;
        void SetTextWrapping(Core::Media::TextWrapping value);

        PROPERTY_(Core::Media::TextAlignment, TextAlignment);
        Core::Media::TextAlignment GetTextAlignment() const;
        void SetTextAlignment(Core::Media::TextAlignment value);

        PROPERTY_GET_ONLY(Core::Media::FormattedTextWrapper&, FormattedText);
        Core::Media::FormattedTextWrapper& GetFormattedText() const;

        /// <summary>
        /// Renders the <see cref="TextBlock"/> to a drawing context.
        /// </summary>
        /// <param name="context">The drawing context.</param>
        void Render(Interfaces::IDrawingContextImpl* context) override;

    protected:

        /// <summary>
        /// Creates the <see cref="FormattedText"/> used to render the text.
        /// </summary>
        /// <param name="constraint">The constraint of the text.</param>
        /// <returns>A <see cref="FormattedText"/> object.</returns>
        virtual void InitializeFormattedText() const;

        /// <summary>
        /// Invalidates <see cref="FormattedText"/>.
        /// </summary>
        void InvalidateFormattedText();

        /// <summary>
        /// Measures the control.
        /// </summary>
        /// <param name="availableSize">The available size for the control.</param>
        /// <returns>The desired size.</returns>
        Core::Media::Size MeasureOverride(Core::Media::Size const& availableSize) override;

        void OnAttachedToLogicalTree(Controls::LogicalTreeAttachmentEventArgs& e) override;
    };
}

#endif // !__XUI_UI_TEXTBLOCK_H__
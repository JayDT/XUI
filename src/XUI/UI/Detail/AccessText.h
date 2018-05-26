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

#ifndef __XUI_ACCESSTEXT_H__
#define __XUI_ACCESSTEXT_H__

#include "UI/TextBlock.h"

namespace XUI::UI
{
    /// <summary>
    /// A text block that displays a character prefixed with an underscore as an access key.
    /// </summary>
    class AccessText : public TextBlock
    {
    private:
        char m_AccessKey = '\0';

    public:
        /// <summary>
        /// Defines the <see cref="ShowAccessKey"/> attached property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty ShowAccessKeyProperty;

    private:
        /// <summary>
        /// The access key handler for the current window.
        /// </summary>
        //IAccessKeyHandler *_accessKeys;

    public:
        /// <summary>
        /// Initializes static members of the <see cref="Popup"/> class.
        /// </summary>
        static void StaticClassInitializer();

        /// <summary>
        /// Initializes a new instance of the <see cref="AccessText"/> class.
        /// </summary>
        AccessText();

        /// <summary>
        /// Gets the access key.
        /// </summary>
        PROPERTY_(char , AccessKey);
        char GetAccessKey() const;
        void SetAccessKey(const char &value);

        /// <summary>
        /// Gets or sets a value indicating whether the access key should be underlined.
        /// </summary>
        PROPERTY_(bool , ShowAccessKey);
        bool GetShowAccessKey() const;
        void SetShowAccessKey(const bool &value);

        /// <summary>
        /// Renders the <see cref="AccessText"/> to a drawing context.
        /// </summary>
        /// <param name="context">The drawing context.</param>
        void Render(Interfaces::IDrawingContextImpl* context) override;
        virtual void SetText(std::wstring const& value) override;

    protected:
        /// <summary>
        /// Creates the <see cref="FormattedText"/> used to render the text.
        /// </summary>
        /// <param name="constraint">The constraint of the text.</param>
        /// <returns>A <see cref="FormattedText"/> object.</returns>
        void InitializeFormattedText() const override;

        /// <summary>
        /// Measures the control.
        /// </summary>
        /// <param name="availableSize">The available size for the control.</param>
        /// <returns>The desired size.</returns>
        Core::Media::Size MeasureOverride(Core::Media::Size const& availableSize) override;

        /// <inheritdoc/>
        void OnAttachedToVisualTree(Controls::VisualTreeAttachmentEventArgs const& e) override;

        /// <inheritdoc/>
        void OnDetachedFromVisualTree(Controls::VisualTreeAttachmentEventArgs const& e) override;

        /// <summary>
        /// Returns a string with the first underscore stripped.
        /// </summary>
        /// <param name="text">The text.</param>
        /// <returns>The text with the first underscore stripped.</returns>
    private:
        std::wstring StripAccessKey(const std::wstring &text);

        /// <summary>
        /// Called when the <see cref="TextBlock.Text"/> property changes.
        /// </summary>
        /// <param name="text">The new text.</param>
        void TextChanged(const std::wstring &text);
    };
}


#endif	//#ifndef __XUI_ACCESSTEXT_H__

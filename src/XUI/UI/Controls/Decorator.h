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

#ifndef __XUI_UI_CONTROLS_DECORATOR_H__
#define __XUI_UI_CONTROLS_DECORATOR_H__

#include "UI/Controls/Control.h"

namespace XUI
{
    namespace Core
    {
        namespace Media
        {
            struct Transform;
        }
    }
}

namespace XUI::UI::Controls
{
    class
        Meta(AbstractType)
        TC_XUI_API Decorator : public Control
    {
    public:
        /// <summary>
        /// Defines the <see cref="Child"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty ChildProperty;

        static void StaticClassInitializer();

        /// <summary>
        /// Gets or sets the decorated control.
        /// </summary>
        Meta(XamlCPP::ContentAttribute())
        PROPERTY_(std::shared_ptr<UI::Controls::Control>, Child);
        std::shared_ptr<UI::Controls::Control> const& GetChild() const;
        void SetChild(std::shared_ptr<UI::Controls::Control> const& value);

    private:

        std::shared_ptr<Controls::Control> _child;

    protected:
        /// <inheritdoc/>
        Core::Media::Size MeasureOverride(Core::Media::Size const& availableSize) override;

        /// <inheritdoc/>
        Core::Media::Size ArrangeOverride(Core::Media::Size const& finalSize) override;

        /// <summary>
        /// Called when the <see cref="Child"/> property changes.
        /// </summary>
        /// <param name="e">The event args.</param>
        void ChildChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const& e);
    };
}

#endif // !__XUI_UI_CONTROLS_CONTROL_H__
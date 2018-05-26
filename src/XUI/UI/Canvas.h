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

#ifndef __XUI_CANVAS_H__
#define __XUI_CANVAS_H__

#include "UI/Panel.h"
#include "Interfaces/ICommand.h"
#include "Core/Input/KeyGesture.h"

namespace XUI::UI
{
    namespace Presenters
    {
        enum class NavigationDirection : uint8;
    }

    /// <summary>
    /// A panel that displays child controls at arbitrary locations.
    /// </summary>
    /// <remarks>
    /// Unlike other <see cref="Panel"/> implementations, the <see cref="Canvas"/> doesn't lay out
    /// its children in any particular layout. Instead, the positioning of each child control is
    /// defined by the <code>Canvas.Left</code>, <code>Canvas.Top</code>, <code>Canvas.Right</code>
    /// and <code>Canvas.Bottom</code> attached properties.
    /// </remarks>
    class TC_XUI_API Canvas : public Panel, public Interfaces::UIControls::INavigableContainer
    {
        /// <summary>
        /// Defines the Left attached property.
        /// </summary>
    public:
        static Core::Dependency::RefDependencyProperty LeftProperty;

        /// <summary>
        /// Defines the Top attached property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty TopProperty;

        /// <summary>
        /// Defines the Right attached property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty RightProperty;

        /// <summary>
        /// Defines the Bottom attached property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty BottomProperty;

        /// <summary>
        /// Initializes static members of the <see cref="Canvas"/> class.
        /// </summary>
        static void StaticClassInitializer();

        /// <summary>
        /// Gets the value of the Left attached property for a control.
        /// </summary>
        /// <param name="element">The control.</param>
        /// <returns>The control's left coordinate.</returns>
        static double GetLeft(Core::Dependency::DependencyObject *element);

        /// <summary>
        /// Sets the value of the Left attached property for a control.
        /// </summary>
        /// <param name="element">The control.</param>
        /// <param name="value">The left value.</param>
        static void SetLeft(Core::Dependency::DependencyObject *element, double value);

        /// <summary>
        /// Gets the value of the Top attached property for a control.
        /// </summary>
        /// <param name="element">The control.</param>
        /// <returns>The control's top coordinate.</returns>
        static double GetTop(Core::Dependency::DependencyObject *element);

        /// <summary>
        /// Sets the value of the Top attached property for a control.
        /// </summary>
        /// <param name="element">The control.</param>
        /// <param name="value">The top value.</param>
        static void SetTop(Core::Dependency::DependencyObject *element, double value);

        /// <summary>
        /// Gets the value of the Right attached property for a control.
        /// </summary>
        /// <param name="element">The control.</param>
        /// <returns>The control's right coordinate.</returns>
        static double GetRight(Core::Dependency::DependencyObject *element);

        /// <summary>
        /// Sets the value of the Right attached property for a control.
        /// </summary>
        /// <param name="element">The control.</param>
        /// <param name="value">The right value.</param>
        static void SetRight(Core::Dependency::DependencyObject *element, double value);

        /// <summary>
        /// Gets the value of the Bottom attached property for a control.
        /// </summary>
        /// <param name="element">The control.</param>
        /// <returns>The control's bottom coordinate.</returns>
        static double GetBottom(Core::Dependency::DependencyObject *element);

        /// <summary>
        /// Sets the value of the Bottom attached property for a control.
        /// </summary>
        /// <param name="element">The control.</param>
        /// <param name="value">The bottom value.</param>
        static void SetBottom(Core::Dependency::DependencyObject *element, double value);

        /// <summary>
        /// Gets the next control in the specified direction.
        /// </summary>
        /// <param name="direction">The movement direction.</param>
        /// <param name="from">The control from which movement begins.</param>
        /// <returns>The control.</returns>
        Controls::InputElement *GetControl(Presenters::NavigationDirection direction, Controls::InputElement *from) override;

    protected:
        /// <summary>
        /// Measures the control.
        /// </summary>
        /// <param name="availableSize">The available size.</param>
        /// <returns>The desired size of the control.</returns>
        Core::Media::Size MeasureOverride(Core::Media::Size const& availableSize) override;

        /// <summary>
        /// Arranges the control's children.
        /// </summary>
        /// <param name="finalSize">The size allocated to the control.</param>
        /// <returns>The space taken.</returns>
        Core::Media::Size ArrangeOverride(Core::Media::Size const& finalSize) override;

        /// <summary>
        /// Marks a property on a child as affecting the canvas' arrangement.
        /// </summary>
        /// <param name="properties">The properties.</param>
    private:
        static void AffectsCanvasArrange(std::vector<Core::Dependency::DependencyProperty*> properties);

        /// <summary>
        /// Calls <see cref="Layoutable.InvalidateArrange"/> on the parent of the control whose
        /// property changed, if that parent is a canvas.
        /// </summary>
        /// <param name="e">The event args.</param>
        static void AffectsCanvasArrangeInvalidate(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e);
    };
}


#endif	//#ifndef __XUI_CANVAS_H__

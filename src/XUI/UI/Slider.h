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

#ifndef __XUI_SLIDER_H__
#define __XUI_SLIDER_H__

#include "UI/Detail/RangeBase.h"
#include "UI/Detail/Track.h"
#include "UI/Button.h"
#include "UI/Panel.h"

namespace XUI::UI
{
    /// <summary>
    /// A control that lets the user select from a range of values by moving a Thumb control along a Track.
    /// </summary>
    class Slider : public RangeBase
    {
    public:
        /// <summary>
        /// Defines the <see cref="Orientation"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty OrientationProperty;

        /// <summary>
        /// Defines the <see cref="IsSnapToTickEnabled"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty IsSnapToTickEnabledProperty;

        /// <summary>
        /// Defines the <see cref="TickFrequency"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty TickFrequencyProperty;

        // Slider required parts
    private:
        std::shared_ptr<Track> _track;
        std::shared_ptr<Button> _decreaseButton;
        std::shared_ptr<Button> _increaseButton;

    public:
        /// <summary>
        /// Initializes static members of the <see cref="Slider"/> class.
        /// </summary>
        static void StaticClassInitializer();

        /// <summary>
        /// Instantiates a new instance of the <see cref="Slider"/> class. 
        /// </summary>
    public:
        Slider();
        virtual ~Slider() {}

        /// <summary>
        /// Gets or sets the orientation of a <see cref="Slider"/>.
        /// </summary>
        PROPERTY_(UI::Orientation , Orientation);
        UI::Orientation GetOrientation() const;
        void SetOrientation(UI::Orientation value);

        /// <summary>
        /// Gets or sets a value that indicates whether the <see cref="Slider"/> automatically moves the <see cref="Thumb"/> to the closest tick mark.
        /// </summary>
        PROPERTY_(bool , IsSnapToTickEnabled);
        bool GetIsSnapToTickEnabled() const;
        void SetIsSnapToTickEnabled(const bool &value);

        /// <summary>
        /// Gets or sets the interval between tick marks.
        /// </summary>
        PROPERTY_(double , TickFrequency);
        double GetTickFrequency() const;
        void SetTickFrequency(const double &value);

        /// <inheritdoc/>
    protected:
        void OnTemplateApplied(Controls::TemplateAppliedEventArgs const& e) override;

    private:
        void DecreaseClick(void* sender, Core::Observer::RoutedEventArgs& e);

        void IncreaseClick(void* sender, Core::Observer::RoutedEventArgs& e);

        void ChangeValueBy(double by);

        /// <summary>
        /// Called when user start dragging the <see cref="Thumb"/>.
        /// </summary>
        /// <param name="e"></param>
    protected:
        virtual void OnThumbDragStarted(Interfaces::VectorEventArgs& e);

        /// <summary>
        /// Called when user dragging the <see cref="Thumb"/>.
        /// </summary>
        /// <param name="e"></param>
        virtual void OnThumbDragDelta(Interfaces::VectorEventArgs& e);

        /// <summary>
        /// Called when user stop dragging the <see cref="Thumb"/>.
        /// </summary>
        /// <param name="e"></param>
        virtual void OnThumbDragCompleted(Interfaces::VectorEventArgs& e);

        /// <summary>
        /// Searches for the closest tick and sets Value to that tick.
        /// </summary>
        /// <param name="value">Value that want to snap to closest Tick.</param>
    private:
        void MoveToNextTick(double value);

        /// <summary>
        /// Snap the input 'value' to the closest tick.
        /// </summary>
        /// <param name="value">Value that want to snap to closest Tick.</param>
        double SnapToTick(double value);
    };
}

#endif	//#ifndef __XUI_SLIDER_H__

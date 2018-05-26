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

#ifndef __XUI_TRACK_H__
#define __XUI_TRACK_H__

#include "UI/Controls/Control.h"
#include "UI/Thumb.h"
#include "UI/Button.h"

namespace XUI::UI
{
    enum class Orientation : uint8;

    class TC_XUI_API Track : public Controls::Control
    {
    public:
        static Core::Dependency::RefDependencyProperty MinimumProperty;

        static Core::Dependency::RefDependencyProperty MaximumProperty;

        static Core::Dependency::RefDependencyProperty ValueProperty;

        static Core::Dependency::RefDependencyProperty ViewportSizeProperty;

        static Core::Dependency::RefDependencyProperty OrientationProperty;

        static Core::Dependency::RefDependencyProperty ThumbProperty;

        static Core::Dependency::RefDependencyProperty IncreaseButtonProperty;

        static Core::Dependency::RefDependencyProperty DecreaseButtonProperty;

    private:
        double _minimum = 0;
        double _maximum = 100.0;
        double _value = 0;

    public:
        static void StaticClassInitializer();

        Track();
        virtual ~Track();

        PROPERTY_(double, Minimum);
        double GetMinimum() const;
        void SetMinimum(const double &value);

        PROPERTY_(double, Maximum);
        double GetMaximum() const;
        void SetMaximum(const double &value);

        __PROPERTY_DECL(GetSCValue, SetSCValue, double, Value);
        double GetSCValue() const;
        void SetSCValue(const double &value);

        PROPERTY_(double, ViewportSize);
        double GetViewportSize() const;
        void SetViewportSize(const double &value);

        PROPERTY_(UI::Orientation, Orientation);
        UI::Orientation GetOrientation() const;
        void SetOrientation(UI::Orientation value);

        Meta(XamlCPP::ContentAttribute())
        PROPERTY_(std::shared_ptr<UI::Thumb>, Thumb);
        std::shared_ptr<UI::Thumb> GetThumb() const;
        void SetThumb(std::shared_ptr<UI::Thumb> const& value);

        PROPERTY_(std::shared_ptr<Button>, IncreaseButton);
        std::shared_ptr<Button> GetIncreaseButton() const;
        void SetIncreaseButton(std::shared_ptr<Button> const& value);

        PROPERTY_(std::shared_ptr<Button>, DecreaseButton);
        std::shared_ptr<Button> GetDecreaseButton() const;
        void SetDecreaseButton(std::shared_ptr<Button> const& value);

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

    private:
        void ThumbChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e);

        void ButtonChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e);

        void ThumbDragged(void *sender, Interfaces::VectorEventArgs& e);
    };
}


#endif	//#ifndef __XUI_TRACK_H__

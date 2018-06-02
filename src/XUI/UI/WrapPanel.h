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

#ifndef __XUI_WRAPPANEL_H__
#define __XUI_WRAPPANEL_H__

#include "UI/Panel.h"
#include "UI/Presenters/ItemsPresenterBase.h"

namespace XUI::UI
{
    /// <summary>
    /// Positions child elements in sequential position from left to right, 
    /// breaking content to the next line at the edge of the containing box. 
    /// Subsequent ordering happens sequentially from top to bottom or from right to left, 
    /// depending on the value of the Orientation property.
    /// </summary>
    class TC_XUI_API WrapPanel : public Panel, public Interfaces::UIControls::INavigableContainer
    {
    public:
        /// <summary>
        /// Defines the <see cref="Orientation"/> property.
        /// </summary>
        static XUI::Core::Dependency::RefDependencyProperty OrientationProperty;

        /// <summary>
        /// Initializes static members of the <see cref="WrapPanel"/> class.
        /// </summary>
        static void StaticClassInitializer();

        WrapPanel() {}
        virtual ~WrapPanel() {}

        /// <summary>
        /// Gets or sets the orientation in which child controls will be layed out.
        /// </summary>
        PROPERTY_(UI::Orientation , Orientation);
        UI::Orientation GetOrientation() const;
        void SetOrientation(UI::Orientation value);

        /// <summary>
        /// Gets the next control in the specified direction.
        /// </summary>
        /// <param name="direction">The movement direction.</param>
        /// <param name="from">The control from which movement begins.</param>
        /// <returns>The control.</returns>
        virtual Controls::InputElement *GetControl(Presenters::NavigationDirection direction, Controls::InputElement *from) override;

    protected:
        /// <inheritdoc/>
        Core::Media::Size MeasureOverride(Core::Media::Size const& availableSize) override;

        /// <inheritdoc/>
        Core::Media::Size ArrangeOverride(Core::Media::Size const& finalSize) override;

    private:
        std::vector<Controls::Control*> GetControlsBetween(int first, int last);

        void ArrangeLine(double v, double lineV, std::vector<Controls::Control*> &contols);
    private:
        /// <summary>
        /// Used to not not write sepearate code for horizontal and vertical orientation.
        /// U is direction in line. (x if orientation is horizontal)
        /// V is direction of lines. (y if orientation is horizonral)
        /// </summary>
        class UVSize
        {
        private:
            UI::Orientation _orientation;

        public:
            double U = 0;
            double V = 0;

        private:
            UVSize(UI::Orientation orientation, double width, double height);

        public:
            UVSize(UI::Orientation orientation, Core::Media::Size const& size);

            UVSize(UI::Orientation orientation);

        private:
            PROPERTY_(double , Width);
            double GetWidth() const;
            void SetWidth(const double &value);

            PROPERTY_(double , Height);
            double GetHeight() const;
            void SetHeight(const double &value);

        public:
            Core::Media::Size ToSize();
        };

        UVSize CreateUVSize(Core::Media::Size const& size);
        UVSize CreateUVSize();
    };
}


#endif	//#ifndef __XUI_WRAPPANEL_H__

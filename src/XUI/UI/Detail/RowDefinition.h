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

#ifndef __XUI_ROWDEFINITION_H__
#define __XUI_ROWDEFINITION_H__

#include "UI/Detail/DefinitionBase.h"
#include "Core/Dependencies/Models/DataProperty.h"
#include "UI/Detail/GridLength.h"
#include <limits>

namespace XUI::UI
{
    /// <summary>
    /// Holds a row definitions for a <see cref="Grid"/>.
    /// </summary>
    class TC_XUI_API RowDefinition : public DefinitionBase
    {
        /// <summary>
        /// Defines the <see cref="MaxHeight"/> property.
        /// </summary>
    public:
        static Core::Dependency::RefDependencyProperty MaxHeightProperty;

        /// <summary>
        /// Defines the <see cref="MinHeight"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty MinHeightProperty;

        /// <summary>
        /// Defines the <see cref="Height"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty HeightProperty;

        static void StaticClassInitializer();

        /// <summary>
        /// Initializes a new instance of the <see cref="RowDefinition"/> class.
        /// </summary>
        RowDefinition();

        /// <summary>
        /// Initializes a new instance of the <see cref="RowDefinition"/> class.
        /// </summary>
        /// <param name="value">The height of the row.</param>
        /// <param name="type">The height unit of the column.</param>
        RowDefinition(double value, GridUnitType type)
        {
            Height = GridLength(value, type);
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="RowDefinition"/> class.
        /// </summary>
        /// <param name="height">The height of the column.</param>
        RowDefinition(GridLength height)
        {
            Height = height;
        }

        /// <summary>
        /// Gets the actual calculated height of the row.
        /// </summary>
        double ActualHeight = 0;

        /// <summary>
        /// Gets or sets the maximum height of the row in DIPs.
        /// </summary>
        PROPERTY_(double, MaxHeight);
        double GetMaxHeight() { return GetValue<double>(MaxHeightProperty.get()); }
        void SetMaxHeight(double const& value) { SetValue(MaxHeightProperty.get(), value); }


        /// <summary>
        /// Gets or sets the minimum height of the row in DIPs.
        /// </summary>
        PROPERTY_(double, MinHeight);
        double GetMinHeight() { return GetValue<double>(MinHeightProperty.get()); }
        void SetMinHeight(double const& value) { SetValue(MinHeightProperty.get(), value); }

        /// <summary>
        /// Gets or sets the height of the row.
        /// </summary>
        PROPERTY_(XUI::UI::GridLength, Height);
        XUI::UI::GridLength GetHeight() { return GetValue<XUI::UI::GridLength>(HeightProperty.get()); }
        void SetHeight(XUI::UI::GridLength const& value) { SetValue(HeightProperty.get(), value); }

        static std::shared_ptr<RowDefinition> Parse(std::string const& s)
        {
            return std::make_shared<RowDefinition>(GridLength::Parse(s));
        }
    };
}


#endif	//#ifndef __XUI_ROWDEFINITION_H__

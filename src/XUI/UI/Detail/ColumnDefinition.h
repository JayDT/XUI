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

#ifndef __XUI_COLUMNDEFINITION_H__
#define __XUI_COLUMNDEFINITION_H__

#include "UI/Detail/DefinitionBase.h"
#include "Core/Dependencies/Models/DataProperty.h"
#include "UI/Detail/GridLength.h"
#include <limits>

namespace XUI::UI
{
    /// <summary>
    /// Holds a column definitions for a <see cref="Grid"/>.
    /// </summary>
    class TC_XUI_API ColumnDefinition : public DefinitionBase
    {
    public:
        /// <summary>
        /// Defines the <see cref="MaxWidth"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty MaxWidthProperty;

        /// <summary>
        /// Defines the <see cref="MinWidth"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty MinWidthProperty;

        /// <summary>
        /// Defines the <see cref="Width"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty WidthProperty;

        static void StaticClassInitializer();

        /// <summary>
        /// Initializes a new instance of the <see cref="ColumnDefinition"/> class.
        /// </summary>
        ColumnDefinition();

        /// <summary>
        /// Initializes a new instance of the <see cref="ColumnDefinition"/> class.
        /// </summary>
        /// <param name="value">The width of the column.</param>
        /// <param name="type">The width unit of the column.</param>
        ColumnDefinition(double value, GridUnitType type)
        {
            Width = GridLength(value, type);
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="ColumnDefinition"/> class.
        /// </summary>
        /// <param name="width">The width of the column.</param>
        ColumnDefinition(GridLength width)
        {
            Width = width;
        }

        /// <summary>
        /// Gets the actual calculated width of the column.
        /// </summary>
        double ActualWidth = 0;

        /// <summary>
        /// Gets or sets the maximum width of the column in DIPs.
        /// </summary>
        PROPERTY_(double, MaxWidth);
        double GetMaxWidth() { return GetValue<double>(MaxWidthProperty.get()); }
        void SetMaxWidth(double const& value) { SetValue(MaxWidthProperty.get(), value); }

        /// <summary>
        /// Gets or sets the minimum width of the column in DIPs.
        /// </summary>
        PROPERTY_(double, MinWidth);
        double GetMinWidth() { return GetValue<double>(MinWidthProperty.get()); }
        void SetMinWidth(double const& value) { SetValue(MinWidthProperty.get(), value); }

        /// <summary>
        /// Gets or sets the width of the column.
        /// </summary>
        PROPERTY_(GridLength, Width);
        GridLength GetWidth() { return GetValue<GridLength>(WidthProperty.get()); }
        void SetWidth(GridLength const& value) { SetValue(WidthProperty.get(), value); }

        static std::shared_ptr<ColumnDefinition> Parse(std::string const& s)
        {
            return std::make_shared<ColumnDefinition>(GridLength::Parse(s));
        }
    };
}


#endif	//#ifndef __XUI_COLUMNDEFINITION_H__

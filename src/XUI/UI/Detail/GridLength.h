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

#ifndef __XUI_GRIDLENGTH_H__
#define __XUI_GRIDLENGTH_H__

#include <string>
#include <vector>
#include <cmath>
#include <stdexcept>

namespace XUI::UI
{
    /// <summary>
    /// Defines the valid units for a <see cref="GridLength"/>.
    /// </summary>
    enum class GridUnitType
    {
        /// <summary>
        /// The row or column is auto-sized to fit its content.
        /// </summary>
        Auto = 0,

        /// <summary>
        /// The row or column is sized in device independent pixels.
        /// </summary>
        Pixel = 1,

        /// <summary>
        /// The row or column is sized as a weighted proportion of available space.
        /// </summary>
        Star = 2,

        /// <summary>
        ///     The value indicates that content should be be calculated based on the
        ///     unconstrained sizes of all cells in a column.
        /// </summary>
        SizeToCells = 3,

        /// <summary>
        ///     The value indicates that content should be calculated based on the
        ///     unconstrained size of the column header.
        /// </summary>
        SizeToHeader = 4,
    };

    /// <summary>
    /// Holds the width or height of a <see cref="Grid"/>'s column and row definitions.
    /// </summary>
    class
        Meta(AbstractType)
        TC_XUI_API
        GridLength : public System::Reflection::Object
    {
    private:
        XUI::UI::GridUnitType _type;
        double _value;

    public:

        GridLength()
            : GridLength(0, GridUnitType::Auto)
        {

        }

        /// <summary>
        /// Initializes a new instance of the <see cref="GridLength"/> struct.
        /// </summary>
        /// <param name="value">The size of the GridLength in device independent pixels.</param>
        GridLength(double value) : GridLength(value, GridUnitType::Pixel)
        {
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="GridLength"/> struct.
        /// </summary>
        /// <param name="value">The size of the GridLength.</param>
        /// <param name="type">The unit of the GridLength.</param>
        GridLength(double value, GridUnitType type)
            : _type(type), _value(value)
        {
            if (value < 0 || std::isnan(value) || std::isinf(value))
            {
                throw std::invalid_argument("Invalid value");
            }

            if (type < GridUnitType::Auto || type > GridUnitType::Star)
            {
                throw std::invalid_argument("Invalid value");
            }
        }

        /// <summary>
        /// Gets an instance of <see cref="GridLength"/> that indicates that a row or column should
        /// auto-size to fit its content.
        /// </summary>
        static GridLength GetAuto()
        {
            return GridLength(0, GridUnitType::Auto);
        }

        /// <summary>
        /// Gets the unit of the <see cref="GridLength"/>.
        /// </summary>
        PROPERTY_GET_ONLY(XUI::UI::GridUnitType, GridUnitType);
        XUI::UI::GridUnitType GetGridUnitType() const { return _type; }

        /// <summary>
        /// Gets a value that indicates whether the <see cref="GridLength"/> has a <see cref="GridUnitType"/> of Pixel.
        /// </summary>
        PROPERTY_GET_ONLY(bool, IsAbsolute);
        bool GetIsAbsolute() const { return _type == GridUnitType::Pixel; }

        /// <summary>
        /// Gets a value that indicates whether the <see cref="GridLength"/> has a <see cref="GridUnitType"/> of Auto.
        /// </summary>
        PROPERTY_GET_ONLY(bool, IsAuto);
        bool GetIsAuto() const { return _type == GridUnitType::Auto; }

        /// <summary>
        /// Gets a value that indicates whether the <see cref="GridLength"/> has a <see cref="GridUnitType"/> of Star.
        /// </summary>
        PROPERTY_GET_ONLY(bool, IsStar);
        bool GetIsStar() const { return _type == GridUnitType::Star; }

        PROPERTY_GET_ONLY(bool, IsSizeToCells);
        bool GetIsSizeToCells() const { return _type == GridUnitType::SizeToCells; }

        PROPERTY_GET_ONLY(bool, IsSizeToHeader);
        bool GetIsSizeToHeader() const { return _type == GridUnitType::SizeToHeader; }

        /// <summary>
        /// Gets the length.
        /// </summary>
        PROPERTY_GET_ONLY(double, Value);
        double GetValue() const { return _value; }

        /// <summary>
        /// Compares two GridLength structures for equality.
        /// </summary>
        /// <param name="a">The first GridLength.</param>
        /// <param name="b">The second GridLength.</param>
        /// <returns>True if the structures are equal, otherwise false.</returns>
        bool operator == (GridLength const& b) const
        {
            return (this->IsAuto && b.IsAuto) || (this->_value == b._value && this->_type == b._type);
        }

        /// <summary>
        /// Compares two GridLength structures for inequality.
        /// </summary>
        /// <param name="gl1">The first GridLength.</param>
        /// <param name="gl2">The first GridLength.</param>
        /// <returns>True if the structures are unequal, otherwise false.</returns>
        bool operator != (GridLength const& gl2)
        {
            return !((*this) == (gl2));
        }

        bool Equal(System::Reflection::VariantBase const & variant) const override
        {
            if (!variant || !variant.ToObject() || variant.GetType() != typeid(GridLength))
                return false;

            return *this == *(GridLength*)(variant.ToPointer());
        }

        /// <summary>
        /// Gets a string representation of the <see cref="GridLength"/>.
        /// </summary>
        /// <returns>The string representation.</returns>
        std::string ToString() const
        {
            if (IsAuto)
            {
                return "Auto";
            }

            std::string s = std::to_string(_value);
            return IsStar ? s + "*" : s;
        }

        /// <summary>
        /// Parses a string to return a <see cref="GridLength"/>.
        /// </summary>
        /// <param name="s">The string.</param>
        /// <param name="culture">The current culture.</param>
        /// <returns>The <see cref="GridLength"/>.</returns>
        static GridLength Parse(const std::string &_s);

        /// <summary>
        /// Parses a string to return a collection of <see cref="GridLength"/>s.
        /// </summary>
        /// <param name="s">The string.</param>
        /// <param name="culture">The current culture.</param>
        /// <returns>The <see cref="GridLength"/>.</returns>
        static std::vector<GridLength> ParseLengths(const std::string &s)
        {
            std::vector<GridLength> lengths;
            for (auto x : System::String(s).Split(','))
                lengths.push_back(Parse(x));

            return lengths;
        }
    };
}


#endif	//#ifndef __XUI_GRIDLENGTH_H__

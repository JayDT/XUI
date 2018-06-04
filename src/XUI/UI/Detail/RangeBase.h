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

#ifndef __XUI_RANGEBASE_H__
#define __XUI_RANGEBASE_H__

#include "UI/Controls/Control.h"

namespace XUI::UI
{
    /// <summary>
    /// Base class for controls that display a value within a range.
    /// </summary>
    class TC_XUI_API RangeBase : public Controls::Control
    {
        /// <summary>
        /// Defines the <see cref="Minimum"/> property.
        /// </summary>
    public:
        static Core::Dependency::RefDependencyProperty MinimumProperty;

        /// <summary>
        /// Defines the <see cref="Maximum"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty MaximumProperty;

        /// <summary>
        /// Defines the <see cref="Value"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty ValueProperty;

        /// <summary>
        /// Defines the <see cref="SmallChange"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty SmallChangeProperty;

        /// <summary>
        /// Defines the <see cref="LargeChange"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty LargeChangeProperty;

        static Core::Observer::RefRoutedEvent ValueChangedEvent;

    private:
        double _minimum = 0;
        double _maximum = 100.0;
        double _value = 0;

    public:
        /// <summary>
        /// Initializes a new instance of the <see cref="RangeBase"/> class.
        /// </summary>
        static void StaticClassInitializer();

        RangeBase();
        virtual ~RangeBase()
        {}

        Meta(XamlCPP::EventAttribute())
        Meta(XamlCPP::TypeConverterAttribute(typeid(XamlCPP::Core::ReflEx::TypeConversion::EventHookValueConverter)))
        Core::Observer::SpecPropertyRoutedEventHandler<&ValueChangedEvent> ValueChanged;

        /// <summary>
        /// Gets or sets the minimum value.
        /// </summary>
        PROPERTY_(double, Minimum);
        double GetMinimum() const;

        void SetMinimum(const double &value);

        /// <summary>
        /// Gets or sets the maximum value.
        /// </summary>
        PROPERTY_(double, Maximum);
        double GetMaximum() const;

        void SetMaximum(const double &value);

        /// <summary>
        /// Gets or sets the current value.
        /// </summary>
        __PROPERTY_DECL(GetSCValue, SetSCValue, double, Value);
        double GetSCValue() const;
        void SetSCValue(const double &value);

        PROPERTY_(double, SmallChange);
        double GetSmallChange() const;
        void SetSmallChange(const double &value);

        PROPERTY_(double, LargeChange);
        double GetLargeChange() const;
        void SetLargeChange(const double &value);

        /// <summary>
        /// Throws an exception if the double valus is NaN or Inf.
        /// </summary>
        /// <param name="value">The value.</param>
        /// <param name="property">The name of the property being set.</param>
    private:
        static void ValidateDouble(double value, const std::string &property_Renamed);

        /// <summary>
        /// Validates the <see cref="Minimum"/> property.
        /// </summary>
        /// <param name="value">The value.</param>
        /// <returns>The coerced value.</returns>
        double ValidateMinimum(double value);

        /// <summary>
        /// Validates/coerces the <see cref="Maximum"/> property.
        /// </summary>
        /// <param name="value">The value.</param>
        /// <returns>The coerced value.</returns>
        double ValidateMaximum(double value);

        /// <summary>
        /// Validates/coerces the <see cref="Value"/> property.
        /// </summary>
        /// <param name="value">The value.</param>
        /// <returns>The coerced value.</returns>
        double ValidateValue(double value);
    };
}


#endif	//#ifndef __XUI_RANGEBASE_H__

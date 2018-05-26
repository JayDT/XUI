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

#ifndef __XUI_LINEARGRADIENTBRUSH_H__
#define __XUI_LINEARGRADIENTBRUSH_H__

#include "Core/Media/GradientBrush.h"
#include "Core/Media/RelativePoint_t.h"

namespace XUI::Core::Media
{
    /// <summary>
    /// A brush that draws with a linear gradient.
    /// </summary>
    class TC_XUI_API LinearGradientBrush final : public XUI::Core::Media::GradientBrush
    {
    public:
        /// <summary>
        /// Defines the <see cref="StartPoint"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty StartPointProperty;

        /// <summary>
        /// Defines the <see cref="EndPoint"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty EndPointProperty;

        /// <summary>
        /// Gets or sets the start point for the gradient.
        /// </summary>
        PROPERTY_(RelativePoint, StartPoint);
        RelativePoint GetStartPoint() const
        {
            return GetValue<RelativePoint>(StartPointProperty);
        }
        void SetStartPoint(RelativePoint const& value)
        {
            SetValue(StartPointProperty, value);
        }

        /// <summary>
        /// Gets or sets the end point for the gradient.
        /// </summary>
        PROPERTY_(RelativePoint, EndPoint);
        RelativePoint GetEndPoint() const
        {
            return GetValue<RelativePoint>(EndPointProperty);
        }
        void SetEndPoint(RelativePoint const& value)
        {
            SetValue(EndPointProperty, value);
        }

        virtual Interfaces::BurshTypes GetTypeId() const override { return Interfaces::BurshTypes::LINEAR_GRADIENT_BRUSH; }
    };
}


#endif	//#ifndef __XUI_LINEARGRADIENTBRUSH_H__

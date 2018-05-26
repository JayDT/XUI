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

#ifndef __XUI_POLYGON_H__
#define __XUI_POLYGON_H__

#include "UI/Shapes/Shape.h"

namespace XUI::UI::Shapes
{
    class TC_XUI_API Polygon : public Shape
    {
    public:
        static Core::Dependency::RefDependencyProperty PointsProperty;
        static Core::Dependency::RefDependencyProperty FillRuleProperty;

        static void StaticConstructor();

    public:

        PROPERTY_(System::Collection::Vector<XUI::Core::Media::Point> const&, Points);
        System::Collection::Vector<XUI::Core::Media::Point> const& GetPoints() const { return _points; }
        void SetPoints(System::Collection::Vector<XUI::Core::Media::Point> const& value) { SetAndRaise(PointsProperty, _points, value); }

        PROPERTY_(XUI::Interfaces::FillRule, FillRule);
        XUI::Interfaces::FillRule GetFillRule() const { return GetValue<XUI::Interfaces::FillRule>(FillRuleProperty); }
        void SetFillRule(XUI::Interfaces::FillRule const& value) { SetValue(FillRuleProperty, value); }


    protected:
        std::shared_ptr<Core::Media::Geometry> CreateDefiningGeometry() override;

        System::Collection::Vector<XUI::Core::Media::Point> _points;
    };
}

#endif	//#ifndef __XUI_POLYGON_H__

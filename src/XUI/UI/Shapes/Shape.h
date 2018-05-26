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

#ifndef __XUI_SHAPE_H__
#define __XUI_SHAPE_H__

#include "Core/Dependencies/Models/DataProperty.h"
#include "Core/Media/MediaTypes.h"
#include "Core/Media/Geometry.h"
#include "Core/Media/Pen.h"
#include "Core/Media/TileBrush.h"
#include "UI/Controls/Control.h"

namespace XUI::UI::Shapes
{
    typedef void(*AffectGeometryFn)(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs&);

    class TC_XUI_API Shape : public UI::Controls::Control
    {
    public:
        static Core::Dependency::RefDependencyProperty FillProperty;
        static Core::Dependency::RefDependencyProperty StretchProperty;
        static Core::Dependency::RefDependencyProperty StrokeProperty;
        static Core::Dependency::RefDependencyProperty StrokeDashArrayProperty;
        static Core::Dependency::RefDependencyProperty StrokeThicknessProperty;

    private:
        Core::Media::Matrix _transform = Core::Media::Matrix::Identity();
        std::shared_ptr<Core::Media::Geometry> _definingGeometry;
        std::shared_ptr<Core::Media::Geometry> _renderedGeometry;
        std::shared_ptr<Core::Media::DashStyle> _dashStyle;
        System::Collection::Vector<double>  _strokeDashArray;
        bool _calculateTransformOnArrange = false;

    public:
        virtual ~Shape()
        {
        }

        static void StaticConstructor();
        
        PROPERTY_GET_ONLY(std::shared_ptr<Core::Media::Geometry>, DefiningGeometry);
        std::shared_ptr<Core::Media::Geometry> GetDefiningGeometry();

        PROPERTY_GET_ONLY(std::shared_ptr<Core::Media::Geometry>, RenderedGeometry);
        std::shared_ptr<Core::Media::Geometry> GetRenderedGeometry();

        PROPERTY_(std::shared_ptr<Interfaces::IBrush>, Fill);
        std::shared_ptr<Interfaces::IBrush> GetFill() const { return GetValue<std::shared_ptr<Interfaces::IBrush>>(FillProperty); }
        void SetFill(std::shared_ptr<Interfaces::IBrush> const& value) { SetValue(FillProperty, value); }

        PROPERTY_(Core::Media::Stretch, Stretch);
        Core::Media::Stretch GetStretch() const { return GetValue<Core::Media::Stretch>(StretchProperty); }
        void SetStretch(Core::Media::Stretch const& value) { SetValue(StretchProperty, value); }

        PROPERTY_(std::shared_ptr<Interfaces::IBrush>, Stroke);
        std::shared_ptr<Interfaces::IBrush> GetStroke() const { return GetValue<std::shared_ptr<Interfaces::IBrush>>(StrokeProperty); }
        void SetStroke(std::shared_ptr<Interfaces::IBrush> const& value) { SetValue(StrokeProperty, value); }

        PROPERTY_(System::Collection::Vector<double>&, StrokeDashArray);
        System::Collection::Vector<double> const& GetStrokeDashArray() const { return _strokeDashArray; }
        void SetStrokeDashArray(System::Collection::Vector<double> const& value);

        PROPERTY_(double, StrokeThickness);
        double GetStrokeThickness() const { return GetValue<double>(StrokeThicknessProperty); }
        void SetStrokeThickness(double const& value) { SetValue(StrokeThicknessProperty, value); }

        PROPERTY_AUTO_QT(XUI::Core::Media::PenLineCap, StrokeDashCap)
        PROPERTY_AUTO_QT(XUI::Core::Media::PenLineCap, StrokeStartLineCap)
        PROPERTY_AUTO_QT(XUI::Core::Media::PenLineCap, StrokeEndLineCap)
        PROPERTY_AUTO_QT(XUI::Core::Media::PenLineJoin, StrokeJoin)

        void Render(Interfaces::IDrawingContextImpl* context) override;

        void InvalidateGeometry();

    protected:

        virtual std::shared_ptr<Core::Media::Geometry> CreateDefiningGeometry() = 0;

        Core::Media::Size MeasureOverride(Core::Media::Size const& availableSize) override;

        Core::Media::Size ArrangeOverride(Core::Media::Size const& finalSize) override;
    private:
        Core::Media::Size CalculateShapeSizeAndSetTransform(Core::Media::Size const& availableSize);

        static void CalculateSizeAndTransform(Core::Media::Size const& availableSize, Core::Media::Rect const& shapeBounds, Core::Media::Stretch Stretch, Core::Media::Size& outSize, Core::Media::Matrix* outMatrix);

        void AffectsGeometryInvalidate(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const& e);
    };
}


#endif	//#ifndef __XUI_SHAPE_H__

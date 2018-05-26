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

#include "Shape.h"
#include "Core/Media/Transform.h"
#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;

STATIC_CLASS_INITIALIZER(Shape, XUI::UI::Shapes::Shape::StaticConstructor)

Core::Dependency::RefDependencyProperty XUI::UI::Shapes::Shape::FillProperty;
Core::Dependency::RefDependencyProperty XUI::UI::Shapes::Shape::StretchProperty;
Core::Dependency::RefDependencyProperty XUI::UI::Shapes::Shape::StrokeProperty;
Core::Dependency::RefDependencyProperty XUI::UI::Shapes::Shape::StrokeDashArrayProperty;
Core::Dependency::RefDependencyProperty XUI::UI::Shapes::Shape::StrokeThicknessProperty;

XUI::UI::Shapes::AffectGeometryFn AffectGeometryFunction = [](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    UI::Shapes::Shape* visual = e.Sender->As<UI::Shapes::Shape>();
    if (visual)
        visual->InvalidateGeometry();
};

void XUI::UI::Shapes::Shape::StaticConstructor()
{
    FillProperty = (Core::Dependency::BindProperty<std::shared_ptr<Interfaces::IBrush>>::Register<XUI::UI::Shapes::Shape>(nameof(Fill)));
    StretchProperty = (Core::Dependency::BindProperty<Core::Media::Stretch>::Register<XUI::UI::Shapes::Shape>(nameof(Stretch)));
    StrokeProperty = (Core::Dependency::BindProperty<std::shared_ptr<Interfaces::IBrush>>::Register<XUI::UI::Shapes::Shape>(nameof(Stroke)));
    StrokeDashArrayProperty = (Core::Dependency::DirectProperty<XUI::UI::Shapes::Shape, System::Collection::Vector<double>>::RegisterDirect(nameof(StrokeDashArray),
        [](Shape* x) { return x->StrokeDashArray; },
        [](Shape* x, System::Collection::Vector<double> const& v) { x->StrokeDashArray = v; })
        );

    StrokeThicknessProperty = (Core::Dependency::BindProperty<double>::Register<XUI::UI::Shapes::Shape>(nameof(StrokeThickness)));

    StretchProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        UI::Controls::Visual* visual = e.Sender->As<UI::Controls::Visual>();
        if (visual)
            visual->InvalidateMeasure();
    });

    StrokeThicknessProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        UI::Controls::Visual* visual = e.Sender->As<UI::Controls::Visual>();
        if (visual)
            visual->InvalidateMeasure();
    });

    FillProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        UI::Controls::Visual* visual = e.Sender->As<UI::Controls::Visual>();
        if (visual)
            visual->InvalidateVisual();
    });

    StrokeProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        UI::Controls::Visual* visual = e.Sender->As<UI::Controls::Visual>();
        if (visual)
            visual->InvalidateVisual();
    });

    StrokeDashArrayProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        UI::Controls::Visual* visual = e.Sender->As<UI::Controls::Visual>();
        if (visual)
            visual->InvalidateVisual();
    });
}

std::shared_ptr<Core::Media::Geometry> XUI::UI::Shapes::Shape::GetDefiningGeometry()
{
    if (!_definingGeometry)
        _definingGeometry = CreateDefiningGeometry();

    return _definingGeometry;
}

std::shared_ptr<Core::Media::Geometry> XUI::UI::Shapes::Shape::GetRenderedGeometry()
{
    if (_renderedGeometry == nullptr)
    {
        if (DefiningGeometry != nullptr)
        {
            _renderedGeometry = DefiningGeometry; // DefiningGeometry->Clone();
            _renderedGeometry->Transform = std::make_shared<Core::Media::MatrixTransform>(_transform);
        }
    }

    return _renderedGeometry;
}

void XUI::UI::Shapes::Shape::SetStrokeDashArray(System::Collection::Vector<double> const & value)
{
    SetAndRaise(StrokeDashArrayProperty, _strokeDashArray, value);
    _dashStyle = std::make_shared<Core::Media::DashStyle>(StrokeDashArray.GetNativeEnumerator());
}

void XUI::UI::Shapes::Shape::Render(Interfaces::IDrawingContextImpl* context)
{
    auto geometry = RenderedGeometry.get();

    if (geometry != nullptr)
    {
        Core::Media::Pen pen(Stroke.get(), StrokeThickness, _dashStyle.get(), StrokeDashCap, StrokeStartLineCap, StrokeEndLineCap, StrokeJoin);
        context->DrawGeometry(Fill.get(), pen, geometry);
    }
}

void XUI::UI::Shapes::Shape::InvalidateGeometry()
{
    _renderedGeometry = nullptr;
    _definingGeometry = nullptr;
    InvalidateMeasure();
}

void XUI::UI::Shapes::Shape::CalculateSizeAndTransform(Core::Media::Size const & availableSize, Core::Media::Rect const & shapeBounds, Core::Media::Stretch Stretch, Core::Media::Size & outSize, Core::Media::Matrix* outMatrix)
{
    Core::Media::Size shapeSize(shapeBounds.x1(), shapeBounds.y1());
    Core::Media::Matrix translate = Core::Media::Matrix::Identity();
    double desiredX = availableSize.Width;
    double desiredY = availableSize.Height;
    double sx = 0.0;
    double sy = 0.0;
    
    if (Stretch != Core::Media::Stretch::None)
    {
        shapeSize = shapeBounds.GetSize();
    }
    
    if (isinf(availableSize.Width))
    {
        desiredX = shapeSize.Width;
    }
    
    if (isinf(availableSize.Height))
    {
        desiredY = shapeSize.Height;
    }
    
    if (shapeBounds.width() > 0)
    {
        sx = desiredX / shapeSize.Width;
    }
    
    if (shapeBounds.height() > 0)
    {
        sy = desiredY / shapeSize.Height;
    }
    
    if (isinf(availableSize.Width))
    {
        sx = sy;
    }
    
    if (isinf(availableSize.Height))
    {
        sy = sx;
    }
    
    switch (Stretch)
    {
        case Core::Media::Stretch::Uniform:
            sx = sy = std::min(sx, sy);
            break;
        case Core::Media::Stretch::UniformToFill:
            sx = sy = std::max(sx, sy);
            break;
        case Core::Media::Stretch::Fill:
            if (isinf(availableSize.Width))
            {
                sx = 1.0;
            }
    
            if (isinf(availableSize.Height))
            {
                sy = 1.0;
            }
    
            break;
        default:
            sx = sy = 1;
            break;
    }

    if (outMatrix)
    {
        if (Stretch != Core::Media::Stretch::None)
        {
            translate = Core::Media::Matrix::CreateTranslation(-shapeBounds.x0() * sx, -shapeBounds.y0() * sy, 0.f); //-G3D::Vector3(shapeBounds.x0() * sx, shapeBounds.y0() * sy, 0));
        }

        *outMatrix = translate * Core::Media::Matrix::CreateScale(sx, sy, 1.0);
    }
    outSize = Core::Media::Size(shapeSize.Width * sx, shapeSize.Height * sy);
}

Core::Media::Size XUI::UI::Shapes::Shape::MeasureOverride(Core::Media::Size const& availableSize)
{
    bool deferCalculateTransform;
    switch (Stretch)
    {
        case Core::Media::Stretch::Fill:
        case Core::Media::Stretch::UniformToFill:
            deferCalculateTransform = std::isinf(availableSize.Width) || std::isinf(availableSize.Height);
            break;
        case Core::Media::Stretch::Uniform:
            deferCalculateTransform = std::isinf(availableSize.Width) && std::isinf(availableSize.Height);
            break;
        case Core::Media::Stretch::None:
        default:
            deferCalculateTransform = false;
            break;
    }

    if (deferCalculateTransform)
    {
        _calculateTransformOnArrange = true;
        return DefiningGeometry->Bounds.GetSize();
    }
    else
    {
        _calculateTransformOnArrange = false;
        return CalculateShapeSizeAndSetTransform(availableSize);
    }
}

Core::Media::Size XUI::UI::Shapes::Shape::ArrangeOverride(Core::Media::Size const& finalSize)
{
    if (_calculateTransformOnArrange)
    {
        _calculateTransformOnArrange = false;
        return CalculateShapeSizeAndSetTransform(finalSize);
    }

    Core::Media::Size size;
    Core::Media::Matrix transform;
    if (DefiningGeometry)
        CalculateSizeAndTransform(finalSize, DefiningGeometry->Bounds, Stretch, size, nullptr);
    return size;
}

Core::Media::Size XUI::UI::Shapes::Shape::CalculateShapeSizeAndSetTransform(Core::Media::Size const& availableSize)
{
    // This should probably use GetRenderBounds(strokeThickness) but then the calculations
    // will multiply the stroke thickness as well, which isn't correct.
    Core::Media::Size size;
    Core::Media::Matrix transform;

    if (DefiningGeometry)
        CalculateSizeAndTransform(availableSize, DefiningGeometry->Bounds, Stretch, size, &transform);

    if (_transform != transform)
    {
        _transform = transform;
        _renderedGeometry = nullptr;
    }

    return size;
}

void XUI::UI::Shapes::Shape::AffectsGeometryInvalidate(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs const& e)
{
    auto control = e.Sender->As<Shape>();

    if (control != nullptr)
    {
        // If the geometry is invalidated when Bounds changes, only invalidate when the Size
        // portion changes.
        if (e.Property == BoundsProperty.get())
        {
            auto oldBounds = e.OldValue.GetValue<Core::Media::Rect>();
            auto newBounds = e.NewValue.GetValue<Core::Media::Rect>();

            if (oldBounds.wh() == newBounds.wh())
                return;
        }

        control->InvalidateGeometry();
    }
}


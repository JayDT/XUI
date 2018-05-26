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

#include "Brush.h"
#include "GradientBrush.h"
#include "LinearGradientBrush.h"
#include "RadialGradientBrush.h"
#include "TileBrush.h"
#include "VisualBrush.h"
#include "Core/Dependencies/DependencyPropertyMgr.h"
#include "UI/Controls/Visual.h"

#include "Core/GlobalInitializerScheduler.h"

using namespace XUI;

Core::Dependency::RefDependencyProperty Core::Media::Brush::OpacityProperty;
Core::Dependency::RefDependencyProperty Core::Media::SolidColorBrush::ColorProperty;
Core::Dependency::RefDependencyProperty Core::Media::GradientBrush::SpreadMethodProperty;
Core::Dependency::RefDependencyProperty Core::Media::GradientBrush::GradientStopsProperty;
Core::Dependency::RefDependencyProperty Core::Media::LinearGradientBrush::StartPointProperty;
Core::Dependency::RefDependencyProperty Core::Media::LinearGradientBrush::EndPointProperty;
Core::Dependency::RefDependencyProperty Core::Media::RadialGradientBrush::CenterProperty;
Core::Dependency::RefDependencyProperty Core::Media::RadialGradientBrush::GradientOriginProperty;
Core::Dependency::RefDependencyProperty Core::Media::RadialGradientBrush::RadiusProperty;

Core::Dependency::RefDependencyProperty Core::Media::TileBrush::AlignmentXProperty;
Core::Dependency::RefDependencyProperty Core::Media::TileBrush::AlignmentYProperty;
Core::Dependency::RefDependencyProperty Core::Media::TileBrush::ViewportProperty;
Core::Dependency::RefDependencyProperty Core::Media::TileBrush::ViewboxProperty;
Core::Dependency::RefDependencyProperty Core::Media::TileBrush::StretchProperty;
Core::Dependency::RefDependencyProperty Core::Media::TileBrush::TileModeProperty;
Core::Dependency::RefDependencyProperty Core::Media::VisualBrush::VisualProperty;

void Static_Brush()
{
    Core::Media::Brush::OpacityProperty = (Core::Dependency::BindProperty<double>::Register<Core::Media::Brush>(nameof(Opacity), 1.0));
    Core::Media::SolidColorBrush::ColorProperty = (Core::Dependency::BindProperty<Core::Media::Color>::Register<Core::Media::SolidColorBrush>(nameof(Color)));
    Core::Media::GradientBrush::SpreadMethodProperty = (Core::Dependency::BindProperty<Core::Media::GradientSpreadMethod>::Register<Core::Media::GradientBrush>(nameof(SpreadMethod)));
    Core::Media::GradientBrush::GradientStopsProperty = (Core::Dependency::BindProperty<System::Collection::Vector<std::shared_ptr<Core::Media::GradientStop>>>::Register<Core::Media::GradientBrush>(nameof(GradientStops)));
    Core::Media::LinearGradientBrush::StartPointProperty = (Core::Dependency::BindProperty<Core::Media::RelativePoint>::Register<Core::Media::LinearGradientBrush>(nameof(StartPoint)));
    Core::Media::LinearGradientBrush::EndPointProperty = (Core::Dependency::BindProperty<Core::Media::RelativePoint>::Register<Core::Media::LinearGradientBrush>(nameof(EndPoint)));
    Core::Media::RadialGradientBrush::CenterProperty = (Core::Dependency::BindProperty<Core::Media::RelativePoint>::Register<Core::Media::RadialGradientBrush>(nameof(Center)));
    Core::Media::RadialGradientBrush::GradientOriginProperty = (Core::Dependency::BindProperty<Core::Media::RelativePoint>::Register<Core::Media::RadialGradientBrush>(nameof(GradientOrigin)));
    Core::Media::RadialGradientBrush::RadiusProperty = (Core::Dependency::BindProperty<double>::Register<Core::Media::RadialGradientBrush>(nameof(Radius)));

    Core::Media::TileBrush::AlignmentXProperty = (Core::Dependency::BindProperty<Core::Media::AlignmentX>::Register<Core::Media::RadialGradientBrush>(nameof(AlignmentX)));
    Core::Media::TileBrush::AlignmentYProperty = (Core::Dependency::BindProperty<Core::Media::AlignmentY>::Register<Core::Media::RadialGradientBrush>(nameof(AlignmentY)));
    Core::Media::TileBrush::StretchProperty = (Core::Dependency::BindProperty<Core::Media::Stretch>::Register<Core::Media::RadialGradientBrush>(nameof(Stretch)));
    Core::Media::TileBrush::TileModeProperty = (Core::Dependency::BindProperty<Core::Media::TileMode>::Register<Core::Media::RadialGradientBrush>(nameof(TileMode)));
    Core::Media::TileBrush::ViewportProperty = (Core::Dependency::BindProperty<Core::Media::Rect>::Register<Core::Media::RadialGradientBrush>(nameof(Viewport)));
    Core::Media::TileBrush::ViewboxProperty = (Core::Dependency::BindProperty<Core::Media::Rect>::Register<Core::Media::RadialGradientBrush>(nameof(Viewbox)));

    Core::Media::VisualBrush::VisualProperty = (Core::Dependency::BindProperty<std::shared_ptr<UI::Controls::Visual>>::Register<Core::Media::VisualBrush>(nameof(Visual)));
}

STATIC_INITIALIZER(Static_Brush)

double XUI::Core::Media::Brush::GetOpacity() const
{
    return GetValue<double>(OpacityProperty.get());
}

void XUI::Core::Media::Brush::SetOpacity(double value)
{
    SetValue(OpacityProperty.get(), value);
}

std::shared_ptr<Interfaces::IBrush> XUI::Core::Media::Brush::Parse(std::string const & s)
{
    return std::make_shared<SolidColorBrush>(Color::Parse(s));
}

XUI::Core::Media::SolidColorBrush::SolidColorBrush(XUI::Core::Media::Color const& c)
{
    Color = c;
    Opacity = double(c.color.a) / 255.0;
}

XUI::Core::Media::Color XUI::Core::Media::SolidColorBrush::GetColor() const
{
    return GetValue<XUI::Core::Media::Color>(ColorProperty.get());
}
void XUI::Core::Media::SolidColorBrush::SetColor(XUI::Core::Media::Color const& value)
{
    SetValue(ColorProperty.get(), value);
}

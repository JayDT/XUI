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

#include "Geometry.h"
#include "Platform/UIService.h"
#include "Platform/UIRenderer.h"
#include "Core/Media/EllipticalArc.h"
#include "Core/Media/Geometries/PathMarkupParser.h"
#include "Core/Media/Geometries/PathGeometry.h"
#include "Core/Media/Transform.h"
#include "Interfaces/IPlatformRenderInterface.h"
#include "Core/GlobalInitializerScheduler.h"

#include "XamlCPP/Core/Dependencies/IPropertyObject.h"

using namespace XUI;
using namespace XUI::Interfaces;

XUI::Core::Dependency::RefDependencyProperty XUI::Core::Media::Geometry::TransformProperty;

void Static_Geometry()
{
    XUI::Core::Media::Geometry::TransformProperty = (Core::Dependency::BindProperty<std::shared_ptr<Core::Media::Transform>>::Register<XUI::Core::Media::Geometry>(nameof(Transform)));

    XUI::Core::Media::Geometry::TransformProperty->Changed.get_observable().subscribe([](XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
    {
        static_cast<XUI::Core::Media::Geometry*>(e.Sender)->TransformChanged(e);
    });
}

STATIC_POST_INITIALIZER(Static_Geometry)

XUI::Core::Media::Geometry::Geometry()
{
    auto renderService = XUI::Platform::UIService::Instance()->Get<XUI::Interfaces::IPlatformRenderInterface>();
    if (renderService)
        m_contextImpl = renderService->CreateStreamGeometry();
}

void XUI::Core::Media::Geometry::TransformChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e)
{
    auto transform = e.NewValue.TryGetValue<std::shared_ptr<XUI::Core::Media::Transform>>();
    m_contextImpl->SetTransform(transform);
    WithTransform(transform->GetTransform());
}

std::shared_ptr<Core::Media::Transform> XUI::Core::Media::Geometry::GetTransform() const
{
    return GetValue<std::shared_ptr<Core::Media::Transform>>(TransformProperty);
}

void XUI::Core::Media::Geometry::SetTransform(std::shared_ptr<Core::Media::Transform> const& value)
{
    SetValue(TransformProperty, value);
    m_contextImpl->SetTransform(value);
}

XUI::Core::Media::Rect XUI::Core::Media::Geometry::GetRenderBounds(double strokeThickness)
{
    return m_contextImpl->GetRenderBounds(strokeThickness);
}

bool XUI::Core::Media::Geometry::FillContains(Core::Media::Point const & point)
{
    return m_contextImpl->FillContains(point);
}

XUI::Interfaces::IGeometry * XUI::Core::Media::Geometry::Intersect(IGeometry * geometry)
{
    return m_contextImpl->Intersect(geometry);
}

bool XUI::Core::Media::Geometry::StrokeContains(Core::Media::Pen const & pen, Core::Media::Point const & point)
{
    return m_contextImpl->StrokeContains(pen, point);
}

XUI::Interfaces::IGeometry * XUI::Core::Media::Geometry::WithTransform(Core::Media::Matrix const & transform)
{
    return m_contextImpl->WithTransform(transform);
}

std::shared_ptr<XUI::Core::Media::Geometry> XUI::Core::Media::Geometry::Parse(std::string const& s)
{
    std::shared_ptr<XUI::Core::Media::Geometry> geometry = std::make_shared<XUI::Core::Media::Geometries::PathGeometry>();
    auto context = geometry->GetStreamGeometryContext()->Open();
    Core::Media::Geometries::PathMarkupParser parser(context);
    parser.Parse(s);
    return geometry;
}

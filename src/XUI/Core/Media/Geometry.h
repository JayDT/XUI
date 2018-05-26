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

#ifndef _XUI_MEDIA_GEOMETRY_H__
#define _XUI_MEDIA_GEOMETRY_H__

#include "Interfaces/IGeometry.h"
#include "Core/Media/Rect_t.h"
#include "Core/Dependencies/DependencyObject.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("Core/Media/Pen.h"))
Meta(modul: ReflInclude("Core/Media/Transform.h"))
Meta(modul: ReflInclude("Core/Media/GeometryContext.h"))
MetaAssemblyEnd

namespace XUI::Interfaces
{
    struct IStreamGeometry;
}

namespace XUI::Core::Media
{
    struct Transform;

    /// <summary>
    /// Defines a geometric shape.
    /// </summary>
    Meta(AbstractType) class TC_XUI_API Geometry : public Interfaces::IGeometry, public Dependency::DependencyObject
    {
    protected:
        std::shared_ptr<Interfaces::IStreamGeometry> m_contextImpl = nullptr;

    public:
        /// <summary>
        /// Defines the <see cref="Transform"/> property.
        /// </summary>
        static Core::Dependency::RefDependencyProperty TransformProperty;

        Geometry();

        /// <summary>
        /// Gets the geometry's bounding rectangle.
        /// </summary>
        PROPERTY_T(Core::Media::Rect, Bounds)
        Core::Media::Rect GetBounds() override { return m_contextImpl->Bounds; }
        void SetBounds(Core::Media::Rect const& value) { }

        /// <summary>
        /// Gets or sets a transform to apply to the geometry.
        /// </summary>
        PROPERTY_T(std::shared_ptr<Core::Media::Transform>, Transform)
        std::shared_ptr<Core::Media::Transform> GetTransform() const override;
        void SetTransform(std::shared_ptr<Core::Media::Transform> const& value);
        void TransformChanged(XamlCPP::Core::Dependency::DependencyPropertyChangedEventArgs& e);

        static std::shared_ptr<Geometry> Parse(std::string const& s);

        virtual std::shared_ptr<Core::Media::Geometry> RefClone() { return nullptr; }

        // Inherited via Geometry
        virtual Core::Media::Rect GetRenderBounds(double strokeThickness) override;
        virtual bool FillContains(Core::Media::Point const & point) override;
        virtual Interfaces::IGeometry * Intersect(Interfaces::IGeometry * geometry) override;
        virtual bool StrokeContains(Core::Media::Pen const & pen, Core::Media::Point const & point) override;
        virtual Interfaces::IGeometry * WithTransform(Core::Media::Matrix const & transform) override;

        virtual Interfaces::IStreamGeometry* GetStreamGeometryContext() const override { return m_contextImpl.get(); }
    };
}
#endif //!_XUI_MEDIA_GEOMETRY_H__
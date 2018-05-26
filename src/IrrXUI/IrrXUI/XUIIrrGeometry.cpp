#include "XUIIrrGeometry.h"

#include "IrrLicht/include/irrlicht.h"
#include "Core/Media/Triangulator.h"

using namespace XUI;
using namespace XUI::Interfaces;

void IrrBackend::XUIIrrGeometry::EndFigure(bool isClosed)
{
    XUI::Core::Media::GeometryContext::EndFigure(isClosed);

    UpdateMeshGeometry();
}

void IrrBackend::XUIIrrGeometry::UpdateMeshGeometry()
{
    if (_bounds.width() == 0 || _bounds.height() == 0)
        return;

    auto transform = GetTransform().get();

    XUI::Core::Media::Matrix transformMatrix;

    if (transform)
        transformMatrix = transform->GetTransform();
    else
        transformMatrix = XUI::Core::Media::Matrix::Identity();

    if (m_filled)
    {
        m_internalBuffer.setRenderPrimitive(irr::scene::E_PRIMITIVE_TYPE::EPT_TRIANGLES);
    
    
        m_internalBuffer.Vertices.clear();
        m_internalBuffer.Indices.clear();
        m_internalBuffer.InitSubBuffers(m_primitivBounds.size());
        std::list<Primitivies> primitiviesHoles;

        for (auto const& bound : m_primitivBounds)
        {
            int16 istart = m_internalBuffer.Indices.size();
            int16 iend = istart;
            int16 vstart = m_internalBuffer.Vertices.size();
            int16 vend = vstart;
    
            std::vector<Core::Media::Vector> verticies;
            std::vector<uint16> indicies;
            Primitivies primitivies;
            primitivies.reserve(bound.end - bound.start + 1);
            for (int i = bound.start; i <= bound.end; ++i)
            {
                primitivies.push_back(m_primitivies[i]);
            }

            for (auto const& clipbound : m_primitivBounds)
            {
                if (bound.start == clipbound.start && bound.end == clipbound.end)
                    continue;

                primitiviesHoles.emplace_back();
                for (int i = clipbound.start; i <= clipbound.end; ++i)
                {
                    primitiviesHoles.back().push_back(m_primitivies[i]);
                }
            }
    
            auto result = Core::Media::Triangulator::PolyEarClipGeometry(primitivies, primitiviesHoles, 0.001f, nullptr, verticies, indicies);
            primitiviesHoles.clear();

            iend += indicies.size();
            vend += verticies.size();
    
            for (int i = 0; i != verticies.size(); ++i)
            {
                XUI::Core::Media::Vector point = glm::vec2(glm::vec4(verticies[i], 0, 0) * transformMatrix);
    
                m_internalBuffer.Vertices.push_back(irr::video::S3DVertex(irr::core::vector3df(point.x, point.y, 0), irr::core::vector3df(0, 0, -1), irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF), irr::core::vector2df(0, 0)));
            }
    
            for (auto idx : indicies)
            {
                m_internalBuffer.Indices.push_back(vstart + idx);
            }
    
            m_internalBuffer.AddSubBuffer(istart, iend, vstart, vend);
        }
    }
    else
    {
        m_internalBuffer.setRenderPrimitive(irr::scene::E_PRIMITIVE_TYPE::EPT_LINES);

        m_internalBuffer.Vertices.clear();
        for (int i = 0; i != m_primitivies.size(); ++i)
        {
            XUI::Core::Media::Point point = m_primitivies[i].Transform(transformMatrix);

            m_internalBuffer.Vertices.push_back(irr::video::S3DVertex(irr::core::vector3df(point.x, point.y, 0), irr::core::vector3df(0, 0, -1), irr::video::SColor(0xFF, 0xFF, 0xFF, 0xFF), irr::core::vector2df(0, 0)));
        }

        m_internalBuffer.Indices.clear();
        m_internalBuffer.InitSubBuffers(m_primitivBounds.size());
        for (auto const& bound : m_primitivBounds)
        {
            int16 start = bound.start;
            int16 end = bound.start;
            for (int i = bound.start; i <= bound.end; ++i)
            {
                m_internalBuffer.Indices.push_back(i);
                if (bound.end == i)
                    m_internalBuffer.Indices.push_back(bound.start);
                else
                    m_internalBuffer.Indices.push_back(i + 1);

                end += 2;
            }
            m_internalBuffer.AddSubBuffer(start, end, bound.start, bound.end);
        }
    }

    m_internalBuffer.setDirty();
    m_internalBuffer.recalculateBoundingBox();
}

IStreamGeometryContext * IrrBackend::XUIIrrGeometry::Open()
{
    return this;
}

XUI::Core::Media::Rect IrrBackend::XUIIrrGeometry::GetBounds()
{
    return XUI::Core::Media::GeometryContext::GetBounds();
}

XUI::Core::Media::Rect IrrBackend::XUIIrrGeometry::GetRenderBounds(double strokeThickness)
{
    if (m_internalBuffer.Vertices.empty())
        return XUI::Core::Media::GeometryContext::GetRenderBounds(strokeThickness);
    return Core::Media::Rect::xyxy(m_internalBuffer.BoundingBox.MinEdge.X, m_internalBuffer.BoundingBox.MinEdge.Y, m_internalBuffer.BoundingBox.MaxEdge.X, m_internalBuffer.BoundingBox.MaxEdge.Y);
}

bool IrrBackend::XUIIrrGeometry::FillContains(Core::Media::Point const & point)
{
    return XUI::Core::Media::GeometryContext::FillContains(point);
}

Interfaces::IGeometry * IrrBackend::XUIIrrGeometry::Intersect(Interfaces::IGeometry * geometry)
{
    return XUI::Core::Media::GeometryContext::Intersect(geometry);
}

bool IrrBackend::XUIIrrGeometry::StrokeContains(Core::Media::Pen const & pen, Core::Media::Point const & point)
{
    return XUI::Core::Media::GeometryContext::StrokeContains(pen, point);
}

Interfaces::IGeometry * IrrBackend::XUIIrrGeometry::WithTransform(Core::Media::Matrix const & transform)
{
    // hack until implement clone for geometry
    UpdateMeshGeometry();
    return XUI::Core::Media::GeometryContext::WithTransform(transform);
}

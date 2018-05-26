#ifndef __WCLIENT_XUI_PLATFORM_GEOMETRY_H__
#define __WCLIENT_XUI_PLATFORM_GEOMETRY_H__

#include "Reflection/Runtime/CXXReflection.h"
#include "Reflection/Runtime/Variant.h"
#include "Reflection/Runtime/Object.h"

#include "XUI/Core/Media/MediaTypes.h"
#include "XUI/Core/Media/GeometryContext.h"
#include "XUI/Core/Media/Transform.h"
#include "XUI/Interfaces/IGeometry.h"
#include "IrrLicht/include/CMeshBuffer.h"

MetaAssemblyBegin
Meta(modul: ReflInclude("XUI/Core/Media/Pen.h"))
MetaAssemblyEnd

namespace irr
{
    class IrrlichtDevice;
}

namespace IrrBackend
{
    class XUIIrrGeometry : public XUI::Core::Media::GeometryContext, public XUI::Interfaces::IStreamGeometry
    {
    private:
        irr::scene::CMeshBufferEx<irr::video::S3DVertex> m_internalBuffer;
        std::shared_ptr<XUI::Core::Media::Transform> m_transform;

        void UpdateMeshGeometry();
    public:
        irr::scene::CMeshBufferEx<irr::video::S3DVertex> const& GetBuffer() const { return m_internalBuffer; }

        // Inherited via PathGeometry
        void EndFigure(bool isClosed) override final;
        IStreamGeometryContext * Open() override final;

        std::shared_ptr<XUI::Core::Media::Transform> GetTransform() const override { return m_transform; }
        void SetTransform(std::shared_ptr<XUI::Core::Media::Transform> const& value) override { m_transform = value; }

        XUI::Core::Media::Rect GetBounds() override final;
        XUI::Core::Media::Rect GetRenderBounds(double strokeThickness) override final;
        bool FillContains(XUI::Core::Media::Point const & point) override final;
        XUI::Interfaces::IGeometry * Intersect(XUI::Interfaces::IGeometry * geometry) override final;
        bool StrokeContains(XUI::Core::Media::Pen const & pen, XUI::Core::Media::Point const & point) override final;
        XUI::Interfaces::IGeometry * WithTransform(XUI::Core::Media::Matrix const & transform) override final;

        virtual XUI::Interfaces::IStreamGeometry* GetStreamGeometryContext() const { return const_cast<XUI::Interfaces::IStreamGeometry*>(static_cast<const XUI::Interfaces::IStreamGeometry*>(this)); }
    };
}

#endif //!__WCLIENT_XUI_PLATFORM_GEOMETRY_H__
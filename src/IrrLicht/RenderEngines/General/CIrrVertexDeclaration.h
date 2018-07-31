// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_VIDEO_NULL_VERTEX_DECL_H_INCLUDED__
#define __C_VIDEO_NULL_VERTEX_DECL_H_INCLUDED__

#include "IGPUProgrammingServices.h"
#include "SVertexElement.h"
#include "irrArray.h"
#include "IReferenceCounted.h"

namespace irr
{
    namespace video
    {
        struct IRRLICHT_API VertexDeclaration : irr::IReferenceCounted
        {
            core::array<SVertexElement> mVertexElements;

            VertexDeclaration()
            {
            }

            ~VertexDeclaration()
            {
            }

            const SVertexElement& addElement(E_VERTEX_ELEMENT_SEMANTIC semanticType, E_VERTEX_ELEMENT_TYPE type, s32 offset,
                u32 semanticIndex = 0, u32 slotIndex = 0,
                u32 instanceStep = 0, bool perInstance = false);
        };
    }
}

#endif //!__C_VIDEO_NULL_VERTEX_DECL_H_INCLUDED__
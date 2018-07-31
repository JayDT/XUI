#include "StdAfx.h"
#include "CDynamicVertexBufferObject.h"

#include "CNullDriver.h"

using namespace irr;
using namespace irr::scene;

void CDynamicVertexBufferObject::SetHWInstanceBuffer(video::IShaderDataBuffer* instanceBuffer)
{
    //if (InstanceBuffer)
    //    sShaderMgr->RemoveToReloadSet(InstanceBuffer);

    InstanceBuffer = instanceBuffer; 

    if (InstanceBuffer)
    {
        //sShaderMgr->AddToReloadSet(instanceBuffer, GetGPUProgram());
        InstanceBuffer->InitializeFormShader(GetGPUProgram(), nullptr);
    }
}

void CDynamicVertexSubBuffer::recalculateBoundingBox(CDynamicVertexBufferObject* buffer)
{
    //! Recalculate bounding box
    if (!buffer->getVertexBuffer().size())
        BoundingBox.reset(0, 0, 0);
    else
    {
        uint16 const* indicies = (uint16 const*)buffer->getIndices();
        BoundingBox.reset(buffer->getVertexBuffer()[indicies[m_indicesStart]].Pos);

        for (uint32 i = m_indicesStart + 1; i < (m_indicesStart + m_indicesCount); ++i)
            BoundingBox.addInternalPoint(buffer->getVertexBuffer()[indicies[i]].Pos);
    }
}

void CDynamicVertexBufferObject::SetGPUProgram(video::IShader* gpuProgram)
{
    //if (m_gpuProgram)
    //    sShaderMgr->RemoveToReloadMeshBufferSet(this);
    if (m_gpuProgramId != -1)
        m_gpuProgramId = -1;
    m_gpuProgram = gpuProgram;

    //if (m_gpuProgram)
    //    sShaderMgr->AddToReloadMeshBufferSet(this, m_gpuProgram);
}

irr::video::IShader * irr::scene::CDynamicVertexBufferObject::GetGPUProgram() const
{
    if (m_gpuProgramId == -1)
        return m_gpuProgram;
    return Driver->GetRuntimeShader(m_gpuProgramId);
}

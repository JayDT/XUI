// Copyright (C) 2002-2009 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_OPENGL_

#define _IRR_DONT_DO_MEMORY_DEBUGGING_HERE
#include "COpenGLHardwareBuffer.h"
#include "COpenGLVertexDeclaration.h"
#include "COpenGLDriver.h"
#include "os.h"

#define SAFE_RELEASE(x)                     \
	if(x)				                    \
		Driver->extGlDeleteBuffers(1, &x); 	\
	x = 0;

using namespace irr;
using namespace video;

COpenGLHardwareBuffer::COpenGLHardwareBuffer(COpenGLDriver* driver, scene::IMeshBuffer *meshBuffer, video::IShaderDataBuffer* instanceBuffer, E_HARDWARE_BUFFER_TYPE type, E_HARDWARE_BUFFER_ACCESS accessType, u32 flags, COpenGLVertexDeclaration* Desc)
    : IHardwareBuffer(meshBuffer, instanceBuffer)
    , Driver(driver)
    , Device(0)
    , Flags(flags)
    , VertexDeclaration(Desc)
    , UniformDeclaration(nullptr)
    , Binded(false)
{
#ifdef _DEBUG
    setDebugName("COpenGLHardwareBuffer");
#endif

    Device = driver->getExposedVideoData().OpenGLWin32.HDc;

    s32 bindedData;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &bindedData);
    _IRR_DEBUG_BREAK_IF(bindedData);

    //Driver->extGLGenVertexArray(1, &vba_verticesID);
    glCreateVertexArrays(1, &vba_verticesID);
}

irr::video::COpenGLHardwareBuffer::COpenGLHardwareBuffer(COpenGLDriver * driver, E_HARDWARE_BUFFER_TYPE type, E_HARDWARE_BUFFER_ACCESS accessType, u32 flags, CGlslBufferDesc & Desc)
    : IHardwareBuffer(nullptr, nullptr)
    , Driver(driver)
    , Device(0)
    , Flags(flags)
    , VertexDeclaration(nullptr)
    , UniformDeclaration(&Desc)
    , vba_verticesID(0)
    , Binded(false)
{
}

COpenGLHardwareBuffer::~COpenGLHardwareBuffer()
{
    // Remove from driver cache
    //Driver->removeHardwareBuffer(this);

    for (u32 i = 0; i != VertexBufferStreams.size(); ++i)
    {
        if (VertexBufferStreams[i].TempBuffer)
        {
            delete[] VertexBufferStreams[i].TempBuffer;
            VertexBufferStreams[i].TempBuffer = 0;
        }

        SAFE_RELEASE(VertexBufferStreams[i].buffer);
    }

    Driver->extGlDeleteVertexArrays(1, &vba_verticesID);
    vba_verticesID = 0;

    Device = nullptr;
}

//! Lock function.
void* COpenGLHardwareBuffer::lock(E_HARDWARE_BUFFER_TYPE Type, u32 size, bool readOnly)
{
    return nullptr;
}

//! Unlock function. Must be called after a lock() to the buffer.
void COpenGLHardwareBuffer::unlock(E_HARDWARE_BUFFER_TYPE Type)
{
}

//! Copy data from system memory
void COpenGLHardwareBuffer::copyFromMemory(E_HARDWARE_BUFFER_TYPE type, const void* sysData, u32 offset, u32 length)
{

}

//! Copy data from another buffer
void COpenGLHardwareBuffer::copyFromBuffer(E_HARDWARE_BUFFER_TYPE type, IHardwareBuffer* buffer, u32 srcOffset, u32 descOffset, u32 length)
{

}

//! Get size of buffer in bytes
u32 COpenGLHardwareBuffer::size(E_HARDWARE_BUFFER_TYPE type) const
{
    return VertexBufferStreams[(u32)type].Size;
}

//! Get driver type of buffer.
E_DRIVER_TYPE COpenGLHardwareBuffer::getDriverType() const
{
    return EDT_OPENGL;
}

//! Get type of buffer.
E_HARDWARE_BUFFER_TYPE COpenGLHardwareBuffer::getType(E_HARDWARE_BUFFER_TYPE type) const
{
    return VertexBufferStreams[(u32)type].Type;
}

//! Get flags
u32 COpenGLHardwareBuffer::getFlags() const
{
    return Flags;
}

//! Get vertex buffer
GLuint COpenGLHardwareBuffer::getVertexBuffer(E_HARDWARE_BUFFER_TYPE type) const
{
    if (VertexBufferStreams.size() <= (u32)type)
        return 0;

    return (GLuint)VertexBufferStreams[(u32)type].buffer;
}

//! Get index buffer
GLuint COpenGLHardwareBuffer::getIndexBuffer() const
{
    if (VertexBufferStreams.size() <= (u32)E_HARDWARE_BUFFER_TYPE::EHBT_INDEX)
        return 0;

    return (GLuint)VertexBufferStreams[(u32)E_HARDWARE_BUFFER_TYPE::EHBT_INDEX].buffer;
}

void COpenGLHardwareBuffer::notifyOnDeviceLost()
{
}

void COpenGLHardwareBuffer::notifyOnDeviceReset(COpenGLDriver* device)
{
    // Recreate buffer with data from temp buffer
    this->Device = device;
}

u32 COpenGLHardwareBuffer::GetMemoryAccessType(E_HARDWARE_BUFFER_ACCESS access)
{
    switch (access)
    {
        case E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC:
            return GL_DYNAMIC_DRAW;
        case E_HARDWARE_BUFFER_ACCESS::EHBA_IMMUTABLE:
        case E_HARDWARE_BUFFER_ACCESS::EHBA_DEFAULT:
            return GL_STATIC_DRAW;
        case E_HARDWARE_BUFFER_ACCESS::EHBA_STREAM:
        case E_HARDWARE_BUFFER_ACCESS::EHBA_SYSTEM_MEMORY:
            return GL_STREAM_DRAW;
        default:
            break;
    }

    return GL_STATIC_DRAW;
}

bool COpenGLHardwareBuffer::UpdateBuffer(E_HARDWARE_BUFFER_TYPE Type, E_HARDWARE_BUFFER_ACCESS AccessType, const void* initialData, u32 size)
{
    HRESULT hr = S_OK;

    if (VertexBufferStreams.size() <= (int)Type)
        VertexBufferStreams.resize(int(Type) + 1);

    COpenGLHardwareBuffer::BufferDesc& desc = VertexBufferStreams[(u32)Type];
    if (Type == E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX || Type == E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX_INSTANCE_STREAM)
    {
        if (!desc.initialize)
        {
            if (Type == E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX)
                VertexDeclaration->createInputLayout(this);
            desc.Stride = VertexDeclaration->GetVertexPitch(Type == E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX_INSTANCE_STREAM ? 1 : 0);
            desc.initialize = true;
            desc.Type = Type;

            //glBindVertexBuffer
            glCreateBuffers(1, &desc.buffer);
            glBindBuffer(GL_ARRAY_BUFFER, desc.buffer);
            glBindVertexArray(GetVAO());
            glBindVertexBuffer(Type == E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX_INSTANCE_STREAM ? 1 : 0, desc.buffer, 0, desc.Stride);
            if (Type == E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX_INSTANCE_STREAM) // ToDo: fix me
                glVertexBindingDivisor(1, 1); // decl.InstanceDivisor);
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        if (initialData)
        {
            if (desc.Size < size || desc.AccessType != AccessType)
            {
                GLenum resourcePool = (GLenum)GetMemoryAccessType(AccessType);

                desc.AccessType = AccessType;
                desc.Size = size;

                glBindBuffer(GL_ARRAY_BUFFER, desc.buffer);
                glBufferData(GL_ARRAY_BUFFER, size, initialData, resourcePool);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }
            else
            {
                glBindBuffer(GL_ARRAY_BUFFER, desc.buffer);
                void* pMapped = glMapBufferRange(GL_ARRAY_BUFFER, 0, size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
                memcpy(pMapped, initialData, size);
                glUnmapBuffer(GL_ARRAY_BUFFER);
                //glBufferSubData(GL_ARRAY_BUFFER, 0, size, initialData);
                glBindBuffer(GL_ARRAY_BUFFER, 0);
            }
        }

        if (desc.Stride)
            desc.Element = (size / desc.Stride);
    }
    else if (Type == E_HARDWARE_BUFFER_TYPE::EHBT_INDEX)
    {
        if (!desc.initialize)
        {
            desc.Stride = (this->Flags & (u32)E_HARDWARE_BUFFER_FLAGS::EHBF_INDEX_32_BITS) ? sizeof(u32) : sizeof(u16);
            desc.initialize = true;
            desc.Type = Type;

            glCreateBuffers(1, &desc.buffer);
            //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, desc.buffer);
            glVertexArrayElementBuffer(GetVAO(), desc.buffer);
            //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

        if (initialData)
        {
            if (desc.Size < size || desc.AccessType != AccessType)
            {
                GLenum resourcePool = (GLenum)GetMemoryAccessType(AccessType);

                desc.AccessType = AccessType;
                desc.Size = size;

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, desc.buffer);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, initialData, resourcePool);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            }
            else
            {
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, desc.buffer);
                //void* pMapped = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0, size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
                //memcpy(pMapped, initialData, size);
                //glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
                glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, size, initialData);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            }
        }

        if (desc.Stride)
            desc.Element = (size / desc.Stride);
    }
    else if (Type == E_HARDWARE_BUFFER_TYPE::EHBT_CONSTANTS)
    {
        if (!desc.initialize)
        {
            desc.Stride = UniformDeclaration->dataSize;
            glCreateBuffers(1, &desc.buffer);
            //Driver->extGlGenBuffers(1, &desc.buffer);

            desc.Size = 0;
            desc.initialize = true;
            desc.Type = Type;
        }

        if (desc.Size < size || desc.AccessType != AccessType)
        {
            GLenum resourcePool = GL_DYNAMIC_COPY;
            if (AccessType == E_HARDWARE_BUFFER_ACCESS::EHBA_IMMUTABLE)
                resourcePool = GL_STATIC_COPY;
            if (AccessType == E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC)
                resourcePool = GL_DYNAMIC_COPY;
            else if (AccessType == E_HARDWARE_BUFFER_ACCESS::EHBA_STREAM)
                resourcePool = GL_STREAM_COPY;

            glBindBuffer(GL_SHADER_STORAGE_BUFFER, desc.buffer);
            glBufferData(GL_SHADER_STORAGE_BUFFER, size, initialData, resourcePool);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }
        else if (initialData)
        {
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, desc.buffer);
            if (AccessType == E_HARDWARE_BUFFER_ACCESS::EHBA_IMMUTABLE)
            {
                void* pMapped = glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, size, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
                memcpy(pMapped, initialData, size);
                glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
            }
            else 
            {
                glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, size, initialData);
            }
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        }

        desc.AccessType = AccessType;
        desc.Size = size;
    }
    else
    {
        os::Printer::log("Buffer type not supported", ELL_ERROR);
        return false;
    }
    return true;
}


void COpenGLHardwareBuffer::Bind()
{
    Binded = true;

    if (vba_verticesID)
    {
        glBindVertexArray(vba_verticesID);
    }

    //if (GLuint buffer = getBufferResource(E_HARDWARE_BUFFER_TYPE::EHBT_INDEX))
    //{
    //    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    //}

    if (GLuint buffer = getBufferResource(E_HARDWARE_BUFFER_TYPE::EHBT_CONSTANTS))
    {
        //glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, UniformDeclaration->binding, buffer);
    }

    IHardwareBuffer::Bind();
}

void COpenGLHardwareBuffer::Unbind()
{
    Binded = false;

    if (vba_verticesID)
    {
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    //if (GLuint buffer = getBufferResource(E_HARDWARE_BUFFER_TYPE::EHBT_INDEX))
    //{
    //    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    //}

    if (GLuint buffer = getBufferResource(E_HARDWARE_BUFFER_TYPE::EHBT_CONSTANTS))
    {
        //glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, UniformDeclaration->binding, 0);
    }

    IHardwareBuffer::Unbind();
}

void COpenGLHardwareBuffer::Initialize()
{
}

void COpenGLHardwareBuffer::Finalize()
{
}

#endif
// Copyright (C) 2002-2009 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_OPENGL_

#define _IRR_DONT_DO_MEMORY_DEBUGGING_HERE
#include "COpenGLHardwareBuffer.h"
#include "COpenGLDriver.h"
#include "os.h"

#define SAFE_RELEASE(x)                     \
	if(x)				                    \
		Driver->extGlDeleteBuffers(1, &x); 	\
	x = 0;

using namespace irr;
using namespace video;

u32 GetVertexDeclStride(core::array<OpenGLVERTEXELEMENT>& VertexDescription, u32 stream)
{
    u32 Stride = 0;
    for (int i = 0; i != VertexDescription.size(); ++i)
    {
        if (VertexDescription[i].Stream == stream)
        {
            switch (VertexDescription[i].Type)
            {
                case GL_FLOAT:
                case GL_INT:
                case GL_UNSIGNED_INT:
                    Stride += (4 * VertexDescription[i].Size);
                    break;
                case GL_BYTE:
                case GL_UNSIGNED_BYTE:
                    Stride += VertexDescription[i].Size;
                    break;
                case GL_FLOAT_MAT4:
                    Stride += (4 * 4 * 4);
                    break;
                default:
                    _IRR_DEBUG_BREAK_IF(true);
                    break;
            }
        }
    }
    return Stride;
}

COpenGLHardwareBuffer::COpenGLHardwareBuffer(COpenGLDriver* driver, scene::IMeshBuffer *meshBuffer, video::IShaderDataBuffer* instanceBuffer, E_HARDWARE_BUFFER_TYPE type, E_HARDWARE_BUFFER_ACCESS accessType, u32 flags, core::array<OpenGLVERTEXELEMENT>& Desc)
    : IHardwareBuffer(meshBuffer, instanceBuffer)
    , Driver(driver)
    , Device(0)
    , Flags(flags)
    , VertexDescription(Desc)
{
#ifdef _DEBUG
    setDebugName("COpenGLHardwareBuffer");
#endif

    s32 bindedData;
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &bindedData);
    _IRR_DEBUG_BREAK_IF(bindedData);

    Device = driver->getExposedVideoData().OpenGLWin32.HDc;
    driver->extGLGenVertexArray(1, &vba_verticesID);
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
    //for (u32 i = 0; i != VertexBufferStreams.size(); ++i)
    //{
    //    // Only reset if pool is DEFAULT
    //    if (VertexBufferStreams[i].AccessType == E_HARDWARE_BUFFER_ACCESS::EHBA_SYSTEM_MEMORY)
    //        continue;
    //
    //    // Only backup data if buffer is not dynamic
    //    if (VertexBufferStreams[i].AccessType != E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC)
    //    {
    //        // Allocate temp buffer
    //        VertexBufferStreams[i].TempBuffer = new char[VertexBufferStreams[i].Size];
    //
    //        // Store data on temp buffer
    //        void* ptr = this->lock((E_HARDWARE_BUFFER_TYPE)i, true);
    //        ::memcpy(VertexBufferStreams[i].TempBuffer, ptr, VertexBufferStreams[i].Size);
    //        this->unlock((E_HARDWARE_BUFFER_TYPE)i);
    //    }
    //
    //    // Release DX 9 buffer
    //    SAFE_RELEASE(VertexBufferStreams[i].buffer);
    //}
}

void COpenGLHardwareBuffer::notifyOnDeviceReset(COpenGLDriver* device)
{
    // Recreate buffer with data from temp buffer
    this->Device = device;

    //for (u32 i = 0; i != VertexBufferStreams.size(); ++i)
    //{
    //    if (VertexBufferStreams[i].TempBuffer)
    //    {
    //        UpdateBuffer((E_HARDWARE_BUFFER_TYPE)i, VertexBufferStreams[i].AccessType, VertexBufferStreams[i].TempBuffer, VertexBufferStreams[i].Size);
    //        delete[] VertexBufferStreams[i].TempBuffer;
    //        VertexBufferStreams[i].TempBuffer = 0;
    //    }
    //}
}

// Don't use D3DPOOL_MANAGED, is not valid with IDirect3DDevice9Ex
u32 COpenGLHardwareBuffer::GetMemoryAccessType(E_HARDWARE_BUFFER_ACCESS access)
{
    switch (access)
    {
        // Don't use D3DPOOL_MANAGED, is not valid with IDirect3DDevice9Ex
        case E_HARDWARE_BUFFER_ACCESS::EHBA_DEFAULT:
            return GL_STATIC_DRAW;
        case E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC:
            return GL_DYNAMIC_DRAW;
        case E_HARDWARE_BUFFER_ACCESS::EHBA_IMMUTABLE:
            return GL_STREAM_DRAW;
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
    {
        for (s32 i = 0; i <= (((s32)Type - (s32)VertexBufferStreams.size()) + 1); ++i)
            VertexBufferStreams.push_back(COpenGLHardwareBuffer::BufferDesc());
    }

    if (Type == E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX || Type == E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX_INSTANCE_STREAM)
    {
        if (!VertexBufferStreams[(u32)Type].initialize)
        {
            Driver->extGlGenBuffers(1, &VertexBufferStreams[(u32)Type].buffer);
            Driver->extGlBindBuffer(GL_ARRAY_BUFFER, VertexBufferStreams[(u32)Type].buffer);

            GLenum resourcePool = (GLenum)GetMemoryAccessType(AccessType);
            Driver->extGlBufferData(GL_ARRAY_BUFFER, size, initialData, resourcePool);

            VertexBufferStreams[(u32)Type].Stride = GetVertexDeclStride(VertexDescription, (u32)Type - 1);

            Driver->testGLError();

#ifdef GL_BGRA
            const GLint colorSize = (Driver->queryOpenGLFeature(COpenGLExtensionHandler::IRR_ARB_vertex_array_bgra) || Driver->queryOpenGLFeature(COpenGLExtensionHandler::IRR_EXT_vertex_array_bgra)) ? GL_BGRA : 4;
#else
            const GLint colorSize = 4;
#endif

            for (int i = 0; i != VertexDescription.size(); ++i)
            {
                if (VertexDescription[i].Stream != ((u32)Type - 1))
                    continue;

                GLint Size = VertexDescription[i].Usage == irr::video::EGVAT_COLOR ? colorSize : VertexDescription[i].Size;

                if (GetBuffer()->GetGPUProgram())
                {
                    int attribloc = (int)GetBuffer()->GetGPUProgram()->getGPUProgramAttribLocation((E_GPU_PROGRAM_VERTEX_ATTRIB_TYPE)VertexDescription[i].Usage);
                    if (attribloc > -1)
                    {
                        Driver->extGLEnableVertexAttribArray(attribloc);
                        if (VertexDescription[i].Type == GL_FLOAT_MAT4)
                        {
                            for (unsigned int ib = 0; ib < 4; ++ib)
                            {
                                Driver->extGLEnableVertexAttribArray(attribloc + ib);
                                Driver->extGLVertexAttribPointer(attribloc + ib, 4, GL_FLOAT, GL_FALSE, VertexBufferStreams[(u32)Type].Stride, (const GLvoid*)(VertexDescription[i].Offset + (sizeof(GLfloat) * ib * 4)));
                                if (VertexDescription[i].Stream == ((u32)Type - 1) && Type == E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX_INSTANCE_STREAM)
                                    Driver->extGLVertexAttribDivisor(attribloc + ib, 1); // Instance Divisior always 1
                            }
                        }
                        else
                        {
                            switch ( VertexDescription[i].Type )
                            {
                                case GL_INT:
                                case GL_UNSIGNED_INT:
                                case GL_BYTE:
                                case GL_UNSIGNED_BYTE:
                                    if ( !VertexDescription[i].Normalized )
                                    {
                                        Driver->extGLVertexAttribIPointer(attribloc, Size, VertexDescription[i].Type, VertexBufferStreams[(u32)Type].Stride, (const GLvoid*)VertexDescription[i].Offset);
                                        break;
                                    }
                                default:
                                    Driver->extGLVertexAttribPointer(attribloc, Size, VertexDescription[i].Type, VertexDescription[i].Normalized, VertexBufferStreams[(u32)Type].Stride, (const GLvoid*)VertexDescription[i].Offset);
                                    break;
                            }
                            if (VertexDescription[i].Stream == ((u32)Type - 1) && Type == E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX_INSTANCE_STREAM)
                                Driver->extGLVertexAttribDivisor(attribloc, 1); // Instance Divisior always 1
                        }

                        Driver->testGLError();
                    }
                }
                else
                {
                    switch (VertexDescription[i].Usage)
                    {
                        case irr::video::EGVAT_POSITION:
                            glEnableClientState(GL_VERTEX_ARRAY);
                            break;
                        case irr::video::EGVAT_NORMAL:
                            glEnableClientState(GL_NORMAL_ARRAY);
                            break;
                        case irr::video::EGVAT_COLOR:
                            glEnableClientState(GL_COLOR_ARRAY);
                            break;
                        case irr::video::EGVAT_TCoord:
                            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
                            break;
                        default:
                            continue;
                    }

                    glVertexPointer(Size, VertexDescription[i].Type, VertexBufferStreams[(u32)Type].Stride, (void *)VertexDescription[i].Offset);
                    Driver->testGLError();
                }
            }

            VertexBufferStreams[(u32)Type].Size = size;
            VertexBufferStreams[(u32)Type].initialize = true;
            VertexBufferStreams[(u32)Type].Type = Type;
        }
        else //if (VertexBufferStreams[(u32)Type].Size < size)
        {
            GLenum resourcePool = (GLenum)GetMemoryAccessType(AccessType);

            // Copy initial data to buffer
            if (initialData)
            {
                GLenum resourcePool = (GLenum)GetMemoryAccessType(AccessType);
                Driver->extGlBindBuffer(GL_ARRAY_BUFFER, VertexBufferStreams[(u32)Type].buffer);
                Driver->extGlBufferData(GL_ARRAY_BUFFER, size, initialData, resourcePool);
            }

            VertexBufferStreams[(u32)Type].AccessType = AccessType;
            VertexBufferStreams[(u32)Type].Size = size;
            VertexBufferStreams[(u32)Type].Element = (VertexBufferStreams[(u32)Type].Size / VertexBufferStreams[(u32)Type].Stride);
        }
    }
    else if (Type == E_HARDWARE_BUFFER_TYPE::EHBT_INDEX)
    {
        if (!VertexBufferStreams[(u32)Type].initialize)
        {
            VertexBufferStreams[(u32)Type].Stride = (this->Flags & (u32)E_HARDWARE_BUFFER_FLAGS::EHBF_INDEX_32_BITS) ? sizeof(u32) : sizeof(u16);

            Driver->extGlGenBuffers(1, &VertexBufferStreams[(u32)Type].buffer);
            Driver->extGlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VertexBufferStreams[(u32)Type].buffer);

            GLenum resourcePool = (GLenum)GetMemoryAccessType(AccessType);
            Driver->extGlBufferData(GL_ELEMENT_ARRAY_BUFFER, size, initialData, resourcePool);

            glIndexPointer((this->Flags & (u32)E_HARDWARE_BUFFER_FLAGS::EHBF_INDEX_32_BITS) ? GL_INT : GL_SHORT, 0, 0);

            VertexBufferStreams[(u32)Type].Size = size;
            VertexBufferStreams[(u32)Type].initialize = true;
            VertexBufferStreams[(u32)Type].Type = Type;
        }
        else //if (VertexBufferStreams[(u32)Type].Size < size)
        {
            //D3DFORMAT idxFormat = (this->Flags & (u32)E_HARDWARE_BUFFER_FLAGS::EHBF_INDEX_32_BITS) ? D3DFMT_INDEX32 : D3DFMT_INDEX16;
            GLenum resourcePool = (GLenum)GetMemoryAccessType(AccessType);

            // Copy initial data to buffer
            if (initialData)
            {
                GLenum resourcePool = (GLenum)GetMemoryAccessType(AccessType);
                Driver->extGlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VertexBufferStreams[(u32)Type].buffer);
                Driver->extGlBufferData(GL_ELEMENT_ARRAY_BUFFER, size, initialData, resourcePool);
            }

            VertexBufferStreams[(u32)Type].AccessType = AccessType;
            VertexBufferStreams[(u32)Type].Size = size;
            VertexBufferStreams[(u32)Type].Element = (VertexBufferStreams[(u32)Type].Size / VertexBufferStreams[(u32)Type].Stride);
        }
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
    Driver->extGLBindVertexArray(vba_verticesID);
    IHardwareBuffer::Bind();
}

void COpenGLHardwareBuffer::Unbind()
{
    Driver->extGLBindVertexArray(0);
    Driver->extGlBindBuffer(GL_ARRAY_BUFFER, 0);
    Driver->extGlBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    IHardwareBuffer::Unbind();
}

void COpenGLHardwareBuffer::Initialize()
{
}

void COpenGLHardwareBuffer::Finalize()
{
}

#endif
// Copyright (C) 2002-2009 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_OPENGL_HARDWARE_BUFFER_H_INCLUDED__
#define __C_OPENGL_HARDWARE_BUFFER_H_INCLUDED__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_OPENGL_

#include "IHardwareBuffer.h"
#include "COpenGLExtensionHandler.h"
#include "irrArray.h"

namespace irr
{
    namespace video
    {
        struct IShaderDataBuffer;
        class COpenGLDriver;

        struct OpenGLVERTEXELEMENT
        {
            WORD    Stream;     // Stream index
            WORD    Offset;     // Offset in the stream in bytes
            GLint   Type;       // Data type
            GLint   Size;       // Data size
            GLint   Usage;      // Semantics
            BYTE    Normalized; // Normal Value
        };

        class COpenGLHardwareBuffer : public IHardwareBuffer
        {
            COpenGLHardwareBuffer(COpenGLDriver* driver, scene::IMeshBuffer *meshBuffer, video::IShaderDataBuffer* instanceBuffer, E_HARDWARE_BUFFER_TYPE type, E_HARDWARE_BUFFER_ACCESS accessType, u32 flags, core::array<OpenGLVERTEXELEMENT>& Desc);

            virtual ~COpenGLHardwareBuffer();

            //! Lock function.
            virtual void* lock(E_HARDWARE_BUFFER_TYPE type, u32 size, bool readOnly = false);

            //! Unlock function. Must be called after a lock() to the buffer.
            virtual void unlock(E_HARDWARE_BUFFER_TYPE type);

            //! Copy data from system memory
            virtual void copyFromMemory(E_HARDWARE_BUFFER_TYPE type, const void* sysData, u32 offset, u32 length);

            //! Copy data from another buffer
            virtual void copyFromBuffer(E_HARDWARE_BUFFER_TYPE type, IHardwareBuffer* buffer, u32 srcOffset, u32 descOffset, u32 length);

            //! Get size of buffer in bytes
            virtual u32 size(E_HARDWARE_BUFFER_TYPE type) const;

            virtual u32 GetMemoryAccessType(E_HARDWARE_BUFFER_ACCESS access);

            virtual u32 getChangeID(E_HARDWARE_BUFFER_TYPE type) const
            {
                if (VertexBufferStreams.size() <= (u32)type)
                    return 0;
                return VertexBufferStreams[(u32)type].ChangeId;
            }

            virtual void setChangeID(E_HARDWARE_BUFFER_TYPE type, u32 id)
            {
                if (VertexBufferStreams.size() <= (u32)type)
                    return;
                VertexBufferStreams[(u32)type].ChangeId = id;
            }

            virtual u32 GetInstanceCount()
            {
                if (VertexBufferStreams.size() <= (u32)E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX_INSTANCE_STREAM)
                    return 1;
                return (u32)VertexBufferStreams[(u32)E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX_INSTANCE_STREAM].Element;
            }

            //! Get driver type of buffer.
            virtual E_DRIVER_TYPE getDriverType() const;

            //! Get type of buffer.
            virtual E_HARDWARE_BUFFER_TYPE getType(E_HARDWARE_BUFFER_TYPE type) const;

            //! Get flags
            virtual u32 getFlags() const;

            // Methods for Direct3D 9 implementation
        public:
            //! Get vertex buffer
            GLuint getVertexBuffer(E_HARDWARE_BUFFER_TYPE type) const;

            //! Get index buffer
            GLuint getIndexBuffer() const;

            virtual void Bind();
            virtual void Unbind();
            virtual void Initialize();
            virtual void Finalize();

        private:
            friend class COpenGLDriver;

            struct BufferDesc
            {
                BufferDesc()
                {
                    memset(this, 0, sizeof(BufferDesc));
                }

                GLuint buffer;
                char* TempBuffer; // Buffer to store data during device reset
                u32 Size;
                u32 Stride;
                u32 Element;
                u32 ChangeId;
                E_HARDWARE_BUFFER_TYPE Type : 8;
                E_HARDWARE_BUFFER_ACCESS AccessType : 8;
                bool initialize : 1;
            };

            void* Device;
            core::array<BufferDesc> VertexBufferStreams;
            core::array<OpenGLVERTEXELEMENT>& VertexDescription;
            GLuint vba_verticesID;

            COpenGLDriver* Driver;
            u32 Flags;

            void notifyOnDeviceLost();
            void notifyOnDeviceReset(COpenGLDriver* device);
            bool UpdateBuffer(E_HARDWARE_BUFFER_TYPE Type, E_HARDWARE_BUFFER_ACCESS AccessType, const void* initialData, u32 size);
        };

    }
}


//struct SHWBufferLink_opengl : public IHardwareBuffer
//{
//    SHWBufferLink_opengl(const scene::IMeshBuffer *_MeshBuffer, COpenGLDriver* context) :
//        IHardwareBuffer(_MeshBuffer),
//        vba_verticesID(0), vbo_verticesID(0), vbo_indicesID(0), vbo_BoxVerticesID(0), vbo_BoxIndicesID(0),
//        mContext(context)
//    {
//        memset(vbo_IDs, 0, sizeof(vbo_IDs));
//        std::fill_n(vbo_StorageMode, (u32)EGVAT_MAX_VALUE, 1);
//    }
//
//    virtual void Bind();
//    virtual void Unbind();
//
//    virtual void Initialize();
//    virtual void Finalize();
//    virtual u32& GetVBOIdByAttributeType(E_GPU_PROGRAM_VERTEX_ATTRIB_TYPE type)
//    {
//        return vbo_IDs[type];
//    }
//
//    virtual u8& GetVBOStoreModeByAttributeType(E_GPU_PROGRAM_VERTEX_ATTRIB_TYPE type)
//    {
//        return vbo_StorageMode[type];
//    }
//
//    GLuint vba_verticesID;
//    GLuint vbo_verticesID;
//    GLuint vbo_indicesID;
//    GLuint vbo_IDs[EGVAT_MAX_VALUE];
//    u8 vbo_StorageMode[EGVAT_MAX_VALUE];
//
//    GLuint vba_BoxVerticesID;
//    GLuint vbo_BoxVerticesID;
//    GLuint vbo_BoxIndicesID;
//
//
//    COpenGLDriver* mContext;
//};
#endif
#endif
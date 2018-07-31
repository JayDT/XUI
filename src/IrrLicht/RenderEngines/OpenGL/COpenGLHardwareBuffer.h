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

#include <vector>

namespace irr
{
    namespace video
    {
        struct CGlslBufferDesc;
        struct IShaderDataBuffer;
        class COpenGLDriver;
        class COpenGLVertexDeclaration;

        class COpenGLHardwareBuffer : public IHardwareBuffer
        {
        public:
            COpenGLHardwareBuffer(COpenGLDriver* driver, scene::IMeshBuffer *meshBuffer, video::IShaderDataBuffer* instanceBuffer, E_HARDWARE_BUFFER_TYPE type, E_HARDWARE_BUFFER_ACCESS accessType, u32 flags, COpenGLVertexDeclaration* Desc);
            COpenGLHardwareBuffer(COpenGLDriver* driver, E_HARDWARE_BUFFER_TYPE type, E_HARDWARE_BUFFER_ACCESS accessType, u32 flags, CGlslBufferDesc& Desc);

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

            GLuint getBufferResource(E_HARDWARE_BUFFER_TYPE type) const
            {
                if (VertexBufferStreams.size() <= (u32)type)
                    return 0;

                return VertexBufferStreams[(u32)type].buffer;
            }
            
            GLuint GetVAO() const { return vba_verticesID; }

            //! Get vertex buffer
            GLuint getVertexBuffer(E_HARDWARE_BUFFER_TYPE type) const;

            //! Get index buffer
            GLuint getIndexBuffer() const;

            virtual void Bind();
            virtual void Unbind();
            virtual void Initialize();
            virtual void Finalize();

            bool IsBinded() { return Binded; }

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
            std::vector<BufferDesc> VertexBufferStreams;
            GLuint vba_verticesID;
            COpenGLVertexDeclaration* VertexDeclaration;
            CGlslBufferDesc*       UniformDeclaration;

            COpenGLDriver* Driver;
            u32 Flags;
            bool Binded;

            void notifyOnDeviceLost();
            void notifyOnDeviceReset(COpenGLDriver* device);
            bool UpdateBuffer(E_HARDWARE_BUFFER_TYPE Type, E_HARDWARE_BUFFER_ACCESS AccessType, const void* initialData, u32 size);
        };

    }
}

#endif
#endif
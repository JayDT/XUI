// Copyright (C) 2002-2009 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_DIRECTX9_HARDWARE_BUFFER_H_INCLUDED__
#define __C_DIRECTX9_HARDWARE_BUFFER_H_INCLUDED__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_DIRECT3D_9_

#include "IHardwareBuffer.h"
#include <d3d9.h>
#include "irrArray.h"

namespace irr
{
    namespace video
    {
        struct IShaderDataBuffer;
        class CD3D9Driver;

        class CD3D9HardwareBuffer : public IHardwareBuffer
        {
            CD3D9HardwareBuffer(CD3D9Driver* driver, scene::IMeshBuffer *meshBuffer, video::IShaderDataBuffer* instanceBuffer, u32 flags, LPDIRECT3DVERTEXDECLARATION9 declaration);

            virtual ~CD3D9HardwareBuffer();

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
            IDirect3DVertexBuffer9* getVertexBuffer(E_HARDWARE_BUFFER_TYPE type) const;

            //! Get index buffer
            IDirect3DIndexBuffer9* getIndexBuffer() const;

            virtual void Bind();
            virtual void Unbind();
            virtual void Initialize();
            virtual void Finalize();

        private:
            friend class CD3D9Driver;

            struct BufferDesc
            {
                BufferDesc()
                {
                    memset(this, 0, sizeof(BufferDesc));
                }

                IDirect3DResource9* buffer;
                char* TempBuffer; // Buffer to store data during device reset
                u32 Size;
                u32 Stride;
                u32 Element;
                u32 ChangeId;
                E_HARDWARE_BUFFER_TYPE Type : 8;
                E_HARDWARE_BUFFER_ACCESS AccessType : 8;
                bool initialize : 1;
            };

            IDirect3DDevice9* Device;
            core::array<BufferDesc> VertexBufferStreams;
            LPDIRECT3DVERTEXDECLARATION9 Desc;

            CD3D9Driver* Driver;
            u32 Flags;

            void notifyOnDeviceLost();
            void notifyOnDeviceReset(IDirect3DDevice9* device);
            bool UpdateBuffer(E_HARDWARE_BUFFER_TYPE Type, E_HARDWARE_BUFFER_ACCESS AccessType, const void* initialData, u32 size);
        };

    }
}

#endif
#endif
// Copyright (C) 2002-2009 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __I_HARDWARE_BUFFER_H_INCLUDED__
#define __I_HARDWARE_BUFFER_H_INCLUDED__

#include "IReferenceCounted.h"
#include "EDriverTypes.h"
#include <stdexcept>

#define SAFE_RELEASE(p)       { if (p) { (p)->Release();  (p) = nullptr; } }
#define SAFE_ADDREF(p)        { if (p) { (p)->AddRef(); } }

#define SAFE_DELETE_ARRAY(p)  { delete [](p); p = nullptr; }
#define SAFE_DELETE(p)        { delete (p); p = nullptr;  }

namespace irr
{
    namespace scene
    {
        class IMeshBuffer;
    }

    namespace video
    {
        enum E_GPU_PROGRAM_VERTEX_ATTRIB_TYPE;
        struct IShaderDataBuffer;

        enum class E_HARDWARE_BUFFER_TYPE : u32
        {
            //! Change the index mapping
            EHBT_INDEX = 0,
            //! Change the vertex mapping
            EHBT_VERTEX,
            //! Change the vertex Instance mapping
            EHBT_VERTEX_INSTANCE_STREAM,
            //! Change the stream output/feedback mapping (stream output also include vertex)
            EHBT_STREAM_OUTPUT,
            //! Change the shader resource mapping
            EHBT_SHADER_RESOURCE,
            //! Change the compute mapping
            EHBT_COMPUTE,
            //! Change the constants mapping
            EHBT_CONSTANTS,
            //! Change the system mapping
            EHBT_SYSTEM
        };

        enum class E_HARDWARE_BUFFER_ACCESS : u32
        {
            EHBA_DYNAMIC = 0,
            EHBA_DEFAULT,
            EHBA_IMMUTABLE,
            EHBA_SYSTEM_MEMORY,
            EHBA_STREAM
        };

        enum class E_HARDWARE_BUFFER_FLAGS : u32
        {
            EHBF_INDEX_16_BITS = 1,
            EHBF_INDEX_32_BITS = 1 << 1,
            EHBF_VERTEX_ADDITIONAL_BIND = 1 << 2,
            EHBF_INDEX_ADDITIONAL_BIND = 1 << 3,
            EHBF_SHADER_ADDITIONAL_BIND = 1 << 4,
            EHBF_DRAW_INDIRECT_ARGS = 1 << 5,
            EHBF_COMPUTE_RAW = 1 << 6,
            EHBF_COMPUTE_STRUCTURED = 1 << 7,
            EHBF_COMPUTE_APPEND = 1 << 8,
            EHBF_COMPUTE_CONSUME = 1 << 9
        };

        class IRRLICHT_API IHardwareBuffer : public virtual IReferenceCounted
        {
            const scene::IMeshBuffer *MeshBuffer;
            IShaderDataBuffer* InstanceBuffer;
            bool m_manualBind : 1;
            bool m_binded : 1;

            IHardwareBuffer(IHardwareBuffer const&);
        public:

            IHardwareBuffer(scene::IMeshBuffer *meshBuffer, IShaderDataBuffer* instanceBuffer)
                : MeshBuffer(meshBuffer)
                , InstanceBuffer(instanceBuffer)
                , m_manualBind(false)
                , m_binded(false)
            {
                //if (instanceBuffer && !GetBuffer()->GetGPUProgram())
                //    throw std::runtime_error("invalid instance init");
            }

            virtual ~IHardwareBuffer() {}

            //! Lock function.
            /** Locks the Buffer and returns a pointer to access the
            data. After lock() has been called and all operations on the data
            are done, you must call unlock().
            Locks are not accumulating, hence one unlock will do for an arbitrary
            number of previous locks.
            \param readOnly Specifies that no changes to the locked buffer are
            made. Unspecified behavior will arise if still write access happens.
            \return Returns a pointer to the data. 0 is returned, if
            the texture cannot be locked. */
            virtual void* lock(E_HARDWARE_BUFFER_TYPE type, u32 size, bool readOnly = false) = 0;

            //! Unlock function. Must be called after a lock() to the buffer.
            /** One should avoid to call unlock more than once before another lock. */
            virtual void unlock(E_HARDWARE_BUFFER_TYPE type) = 0;

            //! Copy data from system memory
            virtual void copyFromMemory(E_HARDWARE_BUFFER_TYPE type, const void* sysData, u32 offset, u32 length) = 0;

            //! Copy data from another buffer
            virtual void copyFromBuffer(E_HARDWARE_BUFFER_TYPE type, IHardwareBuffer* buffer, u32 srcOffset, u32 descOffset, u32 length) = 0;

            //! Get size of buffer in bytes
            virtual u32 size(E_HARDWARE_BUFFER_TYPE type) const = 0;

            virtual u32 getChangeID(E_HARDWARE_BUFFER_TYPE type) const = 0;
            virtual void setChangeID(E_HARDWARE_BUFFER_TYPE type, u32 id) = 0;

            virtual u32 GetMemoryAccessType(E_HARDWARE_BUFFER_ACCESS access) = 0;

            virtual u32 GetInstanceCount() = 0;

            //! Get driver type of buffer.
            /** This is the driver, which created the buffer. This method is used
            internally by the video devices, to check, if they may use a texture
            because textures may be incompatible between different devices.
            \return Driver type of buffer. */
            virtual E_DRIVER_TYPE getDriverType() const = 0;

            //! Get type of buffer.
            virtual E_HARDWARE_BUFFER_TYPE getType(E_HARDWARE_BUFFER_TYPE type) const = 0;

            //! Get flags for buffer
            virtual u32 getFlags() const = 0;

            const scene::IMeshBuffer * GetBuffer() const { return MeshBuffer; }
            scene::IMeshBuffer * GetBuffer() { return (scene::IMeshBuffer *)MeshBuffer; }

            IShaderDataBuffer* GetInstanceBuffer() { return InstanceBuffer; }

            virtual void Bind()
            {
                m_binded = true;
            }
            
            virtual void Unbind()
            {
                m_binded = false;
            }

            virtual void Initialize() {}
            virtual void Finalize() {}

            virtual u32& GetVBOIdByAttributeType(E_GPU_PROGRAM_VERTEX_ATTRIB_TYPE type)
            {
                static u32 empty = 0;
                return empty;
            }
            
            virtual u8& GetVBOStoreModeByAttributeType(E_GPU_PROGRAM_VERTEX_ATTRIB_TYPE type)
            {
                static u8 empty = 1;
                return empty;
            }
            
            void SetManualBind(bool on)
            {
                m_manualBind = on;
            }
            
            bool IsManualBind()
            {
                return m_manualBind;
            }
            
            bool IsBinded()
            {
                return m_binded;
            }
        };

    }
}


//struct IRRLICHT_API SHWBufferLink
//{
//    SHWBufferLink(const scene::IMeshBuffer *_MeshBuffer)
//        :MeshBuffer(_MeshBuffer), m_instanceBuffer(nullptr), ChangedID_Vertex(0), ChangedID_Index(0), m_manualBind(false), m_binded(false)
//    {
//        if (MeshBuffer)
//            MeshBuffer->grab();
//    }
//
//    virtual ~SHWBufferLink()
//    {
//        if (MeshBuffer)
//            MeshBuffer->drop();
//    }
//
//    virtual void Bind()
//    {
//        m_binded = true;
//    }
//
//    virtual void Unbind()
//    {
//        m_binded = false;
//    }
//
//    virtual void Initialize() {}
//    virtual void Finalize() {}
//    virtual u32& GetVBOIdByAttributeType(E_GPU_PROGRAM_VERTEX_ATTRIB_TYPE type)
//    {
//        static u32 empty = 0;
//        return empty;
//    }
//
//    virtual u8& GetVBOStoreModeByAttributeType(E_GPU_PROGRAM_VERTEX_ATTRIB_TYPE type)
//    {
//        static u8 empty = 1;
//        return empty;
//    }
//
//    const scene::IMeshBuffer * GetBuffer() const { return MeshBuffer; }
//    scene::IMeshBuffer * GetBuffer() { return (scene::IMeshBuffer *)MeshBuffer; }
//    const scene::IMeshBuffer *MeshBuffer;
//
//    void SetManualBind(bool on)
//    {
//        m_manualBind = on;
//    }
//
//    bool IsManualBind()
//    {
//        return m_manualBind;
//    }
//
//    bool IsBinded()
//    {
//        return m_binded;
//    }
//
//    void AddInstanceBuffer(IShaderDataBuffer* buf, void* descriptor)
//    {
//        if (!GetBuffer()->GetGPUProgram())
//            throw std::runtime_error("invalid instance init");
//
//        m_instanceBuffer = buf;
//        //m_instanceBuffer->InitializeFormShader(GetBuffer()->GetGPUProgram(), descriptor);
//    }
//
//    IShaderDataBuffer* GetInstanceBuffer() { return m_instanceBuffer; }
//
//    u32 GetInstanceNum()
//    {
//        return m_instanceBuffer ? m_instanceBuffer->getInstanceCount() : 0;
//    }
//
//    IShaderDataBuffer* m_instanceBuffer;
//    u32 ChangedID_Vertex;
//    u32 ChangedID_Index;
//    bool m_manualBind : 1;
//    bool m_binded : 1;
//};

#endif
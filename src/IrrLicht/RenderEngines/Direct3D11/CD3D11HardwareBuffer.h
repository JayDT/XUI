
#ifndef __C_DIRECTX11_HARDWARE_BUFFER_H_INCLUDED__
#define __C_DIRECTX11_HARDWARE_BUFFER_H_INCLUDED__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_DIRECT3D_11_

#include "IHardwareBuffer.h"
#include "CD3D11Resources.h"

namespace irr
{
    namespace video
    {

        struct IShaderDataBuffer;
        class CD3D11Driver;

        class CD3D11HardwareBuffer : public IHardwareBuffer, protected D3D11DeviceResource
        {
        private:
            friend class CD3D11Driver;

            struct BufferDesc
            {
                BufferDesc()
                {
                    memset(this, 0, sizeof(BufferDesc));
                }

                ID3D11Buffer* buffer;  // ID3D11Buffer ID3D11Resource
                ID3D11UnorderedAccessView* UAView;
                ID3D11ShaderResourceView* SRView;
                bool UseTempStagingBuffer;
                D3D11_MAP LastMapDirection;
                CD3D11HardwareBuffer* TempStagingBuffer;
                u32 Size;
                u32 Stride;
                u32 Element;
                u32 ChangeId;
                u32 offset;
                E_HARDWARE_BUFFER_TYPE Type : 8;
                E_HARDWARE_BUFFER_ACCESS AccessType : 8;
                bool initialize : 1;
            };

            std::vector<BufferDesc> VertexBufferStreams;

            ID3D11Device* Device;
            ID3D11DeviceContext* ImmediateContext;
            ID3D11InputLayout*   InputLayout;
            u32 Flags;
            E_VERTEX_TYPE vType : 8;

            bool UpdateBuffer(E_HARDWARE_BUFFER_TYPE Type, E_HARDWARE_BUFFER_ACCESS AccessType, const void* initialData, u32 size, u32 offset = 0, u32 endoffset = 0);
            u32 getVertexDeclarationStride(u8 inputSlot) const;

        public:
            // Implementation of public methods
            CD3D11HardwareBuffer(CD3D11Driver* driver, scene::IMeshBuffer *meshBuffer, video::IShaderDataBuffer* instanceBuffer, u32 flags, E_VERTEX_TYPE vType = EVT_STANDARD);
            CD3D11HardwareBuffer(CD3D11Driver* driver, E_HARDWARE_BUFFER_TYPE type, E_HARDWARE_BUFFER_ACCESS accessType, u32 size, u32 flags = 0, const void* initialData = 0);

            virtual ~CD3D11HardwareBuffer();

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

            // Methods for Direct3D 11 implementation

            //! return DX 11 buffer
            ID3D11Buffer * getBufferResource(E_HARDWARE_BUFFER_TYPE type) const;

            //! return unordered access view
            ID3D11UnorderedAccessView* getUnorderedAccessView(E_HARDWARE_BUFFER_TYPE type) const;

            //! return shader resource view
            ID3D11ShaderResourceView* getShaderResourceView(E_HARDWARE_BUFFER_TYPE type) const;

            virtual void Bind();
            virtual void Unbind();
            virtual void Initialize();
            virtual void Finalize();

            ID3D11InputLayout* GetInputLayout() { return InputLayout; }

            // Inherited via D3D11DeviceResource
            virtual void OnDeviceLost(CD3D11Driver * device) override;
            virtual void OnDeviceRestored(CD3D11Driver * device) override;
        };
    }
}
#endif
#endif
#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_DIRECT3D_11_

#define _IRR_DONT_DO_MEMORY_DEBUGGING_HERE

#include "CD3D11Driver.h"
#include "CD3D11HardwareBuffer.h"
#include "CD3D11Shader.h"
#include "os.h"

using namespace irr;
using namespace video;

CD3D11HardwareBuffer::CD3D11HardwareBuffer(CD3D11Driver* driver, scene::IMeshBuffer *meshBuffer, video::IShaderDataBuffer* instanceBuffer, u32 flags, E_VERTEX_TYPE vtype)
    : IHardwareBuffer(meshBuffer, instanceBuffer)
    , D3D11DeviceResource(driver)
    , Device(nullptr)
    , ImmediateContext(nullptr)
    , InputLayout(nullptr)
    , Flags(flags)
    , vType(vtype)
{
#ifdef _DEBUG
    setDebugName("CD3D11HardwareBuffer");
#endif

    Device = driver->getExposedVideoData().D3D11.D3DDev11;
    SAFE_ADDREF(Device);

    Device->GetImmediateContext(&ImmediateContext);

    // If has dedicate gpu program cache input layout
    if (GetBuffer()->GetGPUProgram() && meshBuffer->GetVertexDeclaration())
    {
        InputLayout = static_cast<CD3D11VertexDeclaration*>(meshBuffer->GetVertexDeclaration())->getInputLayout(static_cast<irr::video::D3D11HLSLProgram*>(GetBuffer()->GetGPUProgram()));
        ASSERT(InputLayout);
        InputLayout->AddRef();
    }
}

irr::video::CD3D11HardwareBuffer::CD3D11HardwareBuffer(CD3D11Driver * driver, E_HARDWARE_BUFFER_TYPE type, E_HARDWARE_BUFFER_ACCESS accessType, u32 size, u32 flags, const void * initialData)
    : IHardwareBuffer(nullptr, nullptr)
    , D3D11DeviceResource(driver)
    , Device(nullptr)
    , ImmediateContext(nullptr)
    , InputLayout(nullptr)
    , Flags(flags)
{
    Device = driver->getExposedVideoData().D3D11.D3DDev11;
    SAFE_ADDREF(Device);

    Device->GetImmediateContext(&ImmediateContext);

    UpdateBuffer(type, accessType, initialData, size, 0);
}

CD3D11HardwareBuffer::~CD3D11HardwareBuffer()
{
    // Remove from driver cache
    //Driver->removeHardwareBuffer(this);

    if (InputLayout)
        InputLayout->Release();
    InputLayout = nullptr;

    for (u32 i = 0; i != VertexBufferStreams.size(); ++i)
    {
        if (VertexBufferStreams[i].TempStagingBuffer)
        {
            SAFE_DELETE(VertexBufferStreams[i].TempStagingBuffer);
        }

        SAFE_RELEASE(VertexBufferStreams[i].buffer);
    }

    SAFE_RELEASE(ImmediateContext);
    SAFE_RELEASE(Device);
}

// Don't use D3DPOOL_MANAGED, is not valid with IDirect3DDevice9Ex
u32 CD3D11HardwareBuffer::GetMemoryAccessType(E_HARDWARE_BUFFER_ACCESS access)
{
    //switch ( access )
    //{
    //    // Don't use D3DPOOL_MANAGED, is not valid with IDirect3DDevice9Ex
    //    case E_HARDWARE_BUFFER_ACCESS::EHBA_DEFAULT:
    //        return D3DPOOL_MANAGED;
    //    case E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC:
    //        return D3DPOOL_DEFAULT;
    //    case E_HARDWARE_BUFFER_ACCESS::EHBA_IMMUTABLE:
    //        return D3DPOOL_SCRATCH;
    //    case E_HARDWARE_BUFFER_ACCESS::EHBA_SYSTEM_MEMORY:
    //        return D3DPOOL_SYSTEMMEM;
    //    default:
    //        break;
    //}

    return 0;
}

//! Lock function.
void* CD3D11HardwareBuffer::lock(E_HARDWARE_BUFFER_TYPE type, u32 size, bool readOnly /*= false*/)
{
    if (VertexBufferStreams.size() <= (u32)type)
        return nullptr;

    CD3D11HardwareBuffer::BufferDesc& desc = VertexBufferStreams[(u32)type];

    if (!desc.buffer)
    	return 0;
    
    // Define map direction
    switch( desc.AccessType )
    {
    case E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC:
    	if( readOnly )
    		return 0;
        desc.LastMapDirection = D3D11_MAP_WRITE_DISCARD;		// always write with DX 11 dynamic buffers
    	break;
    
    case E_HARDWARE_BUFFER_ACCESS::EHBA_IMMUTABLE:
    	if( !readOnly )
    		return 0;		// Cannot be mapped for read			
        desc.LastMapDirection = D3D11_MAP_READ;
    	break;
    
    case E_HARDWARE_BUFFER_ACCESS::EHBA_SYSTEM_MEMORY:
        desc.LastMapDirection = (D3D11_MAP)(D3D11_MAP_WRITE | D3D11_MAP_READ);
    	break;
    
    default:
    	if( readOnly )
            desc.LastMapDirection = D3D11_MAP_READ;
    	else
            desc.LastMapDirection = (D3D11_MAP)(D3D11_MAP_WRITE | D3D11_MAP_READ);
    	break;
    }
    
    // If using staging, return its pointer
    if(desc.UseTempStagingBuffer)
    {
    	if( !desc.TempStagingBuffer )
            desc.TempStagingBuffer = new CD3D11HardwareBuffer(Driver, GetBuffer(), GetInstanceBuffer(), desc.Size, vType);
    
    	// Copy to staging if needs to read data
    	if (desc.LastMapDirection && D3D11_MAP_READ)
            desc.TempStagingBuffer->copyFromBuffer(type, this, 0, 0, desc.Size);
    
    	return desc.TempStagingBuffer->lock(type, size, readOnly);
    }
    
    // Otherwise, map this buffer
    D3D11_MAPPED_SUBRESOURCE mappedData;
    HRESULT hr = ImmediateContext->Map(desc.buffer, 0, desc.LastMapDirection, 0, &mappedData);
    if (FAILED(hr))
    	return 0;
    
    return mappedData.pData;
}

//! Unlock function. Must be called after a lock() to the buffer.
void CD3D11HardwareBuffer::unlock(E_HARDWARE_BUFFER_TYPE type)
{
    CD3D11HardwareBuffer::BufferDesc& desc = VertexBufferStreams[(u32)type];

    if (!desc.buffer)
    	return;
    
    // If using staging, return its pointer
    if(desc.UseTempStagingBuffer)
    {
        desc.TempStagingBuffer->unlock(type);
    	
    	// If write, copy staging to this
    	if (desc.LastMapDirection && D3D11_MAP_WRITE)
    		this->copyFromBuffer(type, desc.TempStagingBuffer, 0, 0, desc.Size);
    
    	return;
    }
    
    // Otherwise, unmap this
    ImmediateContext->Unmap(desc.buffer, 0);
}

// Remarks
// For a shader-constant buffer; set pDstBox to NULL. It is not possible to use this method to partially update a shader-constant buffer.
// 
// A resource cannot be used as a destination if:
// 
// the resource is created with immutable or dynamic usage.
// the resource is created as a depth-stencil resource.
// the resource is created with multisampling capability (see DXGI_SAMPLE_DESC).
//! Copy data from system memory
void CD3D11HardwareBuffer::copyFromMemory(E_HARDWARE_BUFFER_TYPE type, const void* sysData, u32 offset, u32 length)
{
    if (VertexBufferStreams.size() <= (u32)type)
        return;

    CD3D11HardwareBuffer::BufferDesc& desc = VertexBufferStreams[(u32)type];

    if( desc.AccessType == E_HARDWARE_BUFFER_ACCESS::EHBA_IMMUTABLE )
    	return;		// Immutable cannot be modified
    
    if (desc.buffer)
    {
        // 1D resource
    	D3D11_BOX box;
    	box.left = offset;
    	box.right = length;
    	box.front = 0;
    	box.bottom = 1;
    	box.top = 0;
    	box.back = 1;
    	ImmediateContext->UpdateSubresource(desc.buffer, 0, &box, sysData, 0, 0);

        //UINT res = D3D11CalcSubresource(0, 0, 0);
        //ImmediateContext->UpdateSubresource(desc.buffer, res, nullptr, sysData, static_cast<UINT>(length), static_cast<UINT>(length));

    }
}

//! Copy data from another buffer
void CD3D11HardwareBuffer::copyFromBuffer(E_HARDWARE_BUFFER_TYPE type, IHardwareBuffer* buffer, u32 srcOffset, u32 destOffset, u32 length)
{
    if (!buffer || buffer->getDriverType() != EDT_DIRECT3D11)
    {
        os::Printer::log("Fatal Error: Tried to copy data from a buffer not owned by this driver.", ELL_ERROR);
        return;
    }

    if (VertexBufferStreams.size() <= (u32)type)
        return;

    CD3D11HardwareBuffer::BufferDesc& desc = VertexBufferStreams[(u32)type];

    if( desc.AccessType == E_HARDWARE_BUFFER_ACCESS::EHBA_IMMUTABLE )
    	return;		// Immutable cannot be modified
    
    if (!desc.buffer)
    	return;
    
    CD3D11HardwareBuffer* srcBuffer = static_cast<CD3D11HardwareBuffer*>(buffer);
    
    // try fast copy if possible
    if (srcOffset == 0 && destOffset == 0 && length == desc.Size
    	&& desc.Size == buffer->size(type) )
    {
    	ImmediateContext->CopyResource(desc.buffer, srcBuffer->getBufferResource(type) );
    }
    else	// else, copy subregion
    {
    	D3D11_BOX srcBox;
    	srcBox.left = (UINT)srcOffset;
    	srcBox.right = (UINT)srcOffset + length;
    	srcBox.top = 0;
    	srcBox.bottom = 1;
    	srcBox.front = 0;
    	srcBox.back = 1;
    
    	ImmediateContext->CopySubresourceRegion(desc.buffer, 0, (UINT)destOffset, 0, 0,
    											srcBuffer->getBufferResource(type), 0, &srcBox);
    }
}

//! Get size of buffer in bytes
u32 CD3D11HardwareBuffer::size(E_HARDWARE_BUFFER_TYPE type) const
{
    if (VertexBufferStreams.size() <= (u32)type)
        return 0;

    return VertexBufferStreams[(u32)type].Size;
}

//! Get driver type of buffer.
E_DRIVER_TYPE CD3D11HardwareBuffer::getDriverType() const
{
    return EDT_DIRECT3D11;
}

//! Get type of buffer.
E_HARDWARE_BUFFER_TYPE CD3D11HardwareBuffer::getType(E_HARDWARE_BUFFER_TYPE type) const
{
    return E_HARDWARE_BUFFER_TYPE(0); // Type;
}

u32 CD3D11HardwareBuffer::getFlags() const
{
    return Flags;
}

//! return DX 11 buffer
ID3D11Buffer* CD3D11HardwareBuffer::getBufferResource(E_HARDWARE_BUFFER_TYPE type) const
{
    if (VertexBufferStreams.size() <= (u32)type)
        return nullptr;

    return (ID3D11Buffer*)VertexBufferStreams[(u32)type].buffer;
}

//! return unordered access view
ID3D11UnorderedAccessView* CD3D11HardwareBuffer::getUnorderedAccessView(E_HARDWARE_BUFFER_TYPE type) const
{
    if ( VertexBufferStreams.size() <= (u32)type )
        return nullptr;

    return VertexBufferStreams[(u32)type].UAView;
}

ID3D11ShaderResourceView* CD3D11HardwareBuffer::getShaderResourceView(E_HARDWARE_BUFFER_TYPE type) const
{
    if ( VertexBufferStreams.size() <= (u32)type )
        return nullptr;

    return VertexBufferStreams[(u32)type].SRView;
}

void BuildBufferDesc(D3D11_BUFFER_DESC& desc, E_HARDWARE_BUFFER_TYPE Type, E_HARDWARE_BUFFER_ACCESS AccessType, u32 size)
{
    desc.ByteWidth = size;
    desc.StructureByteStride = 0;
    desc.MiscFlags = 0;
    desc.CPUAccessFlags = (AccessType == E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC) ? D3D11_CPU_ACCESS_WRITE :
        (AccessType == E_HARDWARE_BUFFER_ACCESS::EHBA_SYSTEM_MEMORY) ? D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE :
        0;

    desc.Usage = (AccessType == E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC) ? D3D11_USAGE_DYNAMIC :
        (AccessType == E_HARDWARE_BUFFER_ACCESS::EHBA_DEFAULT) ? D3D11_USAGE_DEFAULT :
        (AccessType == E_HARDWARE_BUFFER_ACCESS::EHBA_IMMUTABLE) ? D3D11_USAGE_IMMUTABLE :
        D3D11_USAGE_STAGING;

    desc.BindFlags = 0;

    // Check bind flags
    switch ( Type )
    {
        case E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX:
        case E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX_INSTANCE_STREAM:
            desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
            break;

        case E_HARDWARE_BUFFER_TYPE::EHBT_INDEX:
            desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
            break;

        case E_HARDWARE_BUFFER_TYPE::EHBT_STREAM_OUTPUT:
            desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;
            break;

        case E_HARDWARE_BUFFER_TYPE::EHBT_COMPUTE:
            desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
            break;

        case E_HARDWARE_BUFFER_TYPE::EHBT_SHADER_RESOURCE:
            desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
            break;

        case E_HARDWARE_BUFFER_TYPE::EHBT_CONSTANTS:
            desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
            break;

        case E_HARDWARE_BUFFER_TYPE::EHBT_SYSTEM:
            desc.BindFlags = 0;
            break;
        default:
            break;
    }

}

u32 CD3D11HardwareBuffer::getVertexDeclarationStride(u8 inputSlot) const
{
    return static_cast<CD3D11VertexDeclaration*>(GetBuffer()->GetVertexDeclaration())->GetVertexPitch(inputSlot);
}

void irr::video::CD3D11HardwareBuffer::OnDeviceLost(CD3D11Driver * device)
{
}

void irr::video::CD3D11HardwareBuffer::OnDeviceRestored(CD3D11Driver * device)
{
}

bool CD3D11HardwareBuffer::UpdateBuffer(E_HARDWARE_BUFFER_TYPE Type, E_HARDWARE_BUFFER_ACCESS AccessType, const void* initialData, u32 size, u32 offset /*= 0*/, u32 endoffset /*= 0*/)
{
    if ( !size )
        return false;

    HRESULT hr = S_OK;

    if (VertexBufferStreams.size() <= (int)Type)
        VertexBufferStreams.resize(int(Type) + 1);

    CD3D11HardwareBuffer::BufferDesc& desc = VertexBufferStreams[(u32)Type];

    // set need of staging buffer
    if (/*AccessType == E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC ||*/ AccessType == E_HARDWARE_BUFFER_ACCESS::EHBA_IMMUTABLE)
        desc.UseTempStagingBuffer = true;

    if (Type == E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX || Type == E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX_INSTANCE_STREAM)
    {
        if (!desc.initialize)
        {
            desc.Size = 0;
            desc.Stride = getVertexDeclarationStride((u32)Type - 1);
            desc.initialize = true;
            desc.Type = Type;
        }
    }
    else if (Type == E_HARDWARE_BUFFER_TYPE::EHBT_INDEX)
    {
        if ( !desc.initialize )
        {
            desc.Size = 0;
            desc.Stride = (this->Flags & (u32)E_HARDWARE_BUFFER_FLAGS::EHBF_INDEX_32_BITS) ? sizeof(u32) : sizeof(u16);
            desc.initialize = true;
            desc.Type = Type;
        }
    }
    else if (Type == E_HARDWARE_BUFFER_TYPE::EHBT_CONSTANTS)
    {
        if (!desc.initialize)
        {
            desc.Size = 0;
            desc.Stride = size;
            desc.initialize = true;
            desc.Type = Type;
        }
    }
    else
    {
        os::Printer::log("Buffer type not supported", ELL_ERROR);
        return false;
    }

    //if ((AccessType != E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC && desc.AccessType == E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC) || (desc.AccessType != E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC && desc.Size && desc.buffer))
    //    AccessType = E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC;

    if ( desc.Size < size || desc.AccessType != AccessType )
    {
        SAFE_RELEASE(desc.buffer);

        // Load initial data
        D3D11_SUBRESOURCE_DATA* data = 0;
        if (initialData)
        {
        	data = new D3D11_SUBRESOURCE_DATA();
        	data->pSysMem = initialData;
        	data->SysMemPitch = 0;
        	data->SysMemSlicePitch = 0;
        }

        D3D11_BUFFER_DESC bufferDesc;
        BuildBufferDesc(bufferDesc, Type, AccessType, size);

        hr = Device->CreateBuffer(&bufferDesc, data, &desc.buffer);
        if ( FAILED(hr) )
        {
            os::Printer::log("Error, failed to create dynamic vertex buffer", ELL_ERROR);
            return false;
        }

        SAFE_DELETE(data);

        desc.Size = bufferDesc.ByteWidth;
        desc.AccessType = AccessType;
        desc.Element = (desc.Size / desc.Stride);

        // If buffer is of type shader resource, create view
        if (Type == E_HARDWARE_BUFFER_TYPE::EHBT_SHADER_RESOURCE)
        {
            SAFE_RELEASE(desc.SRView);

        	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
        	SRVDesc.ViewDimension = D3D_SRV_DIMENSION_BUFFER;
        	SRVDesc.Format = DXGI_FORMAT_R32_FLOAT;
        	SRVDesc.Buffer.ElementOffset = 0;
        	SRVDesc.Buffer.ElementWidth = desc.Size / 4;
        
        	hr = Device->CreateShaderResourceView(desc.buffer, &SRVDesc, &desc.SRView);
        	if (FAILED(hr))
        	{
        		os::Printer::log("Error creating shader resource view for buffer", ELL_ERROR);
        		return false;
        	}
        }
        
        // If buffer if of type compute, create view
        if (Type == E_HARDWARE_BUFFER_TYPE::EHBT_COMPUTE )
        {
            SAFE_RELEASE(desc.UAView);

            D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc;
        	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        	UAVDesc.Buffer.FirstElement = 0;
        	UAVDesc.Buffer.Flags = 0;
        
        	if (Driver->queryFeature(EVDF_COMPUTING_SHADER_5_0))
        	{
        		UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
        		UAVDesc.Buffer.NumElements = desc.Size / 4;	// size in floats
        	}
        	else if (Driver->queryFeature(EVDF_COMPUTING_SHADER_4_0))
        	{
        		UAVDesc.Format = DXGI_FORMAT_R32_TYPELESS;
        		UAVDesc.Buffer.NumElements = desc.Size;		// size in bytes
        	}
        
        	hr = Device->CreateUnorderedAccessView(desc.buffer, &UAVDesc, &desc.UAView);
        	if (FAILED(hr))
        	{
        		os::Printer::log("Error creating unordered access view for buffer", ELL_ERROR);
        		return false;
        	}
        }
    }
    else if ( initialData ) // Copy initial data to buffer
    {
        if (AccessType == E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC || desc.UseTempStagingBuffer)
        {
            void* ptr = this->lock(Type, size);
            ::CopyMemory(ptr, initialData, size);
            this->unlock(Type);
        }
        else
        {
            CD3D11HardwareBuffer::BufferDesc& desc = VertexBufferStreams[(u32)Type];
            if (endoffset != 0 && Type != E_HARDWARE_BUFFER_TYPE::EHBT_CONSTANTS)
            {
                D3D11_BOX box;
                box.left = offset;
                box.right = endoffset;
                box.front = 0;
                box.bottom = 1;
                box.top = 0;
                box.back = 1;

                ImmediateContext->UpdateSubresource(desc.buffer, 0, &box, initialData, 0, 0);
            }
            else
                ImmediateContext->UpdateSubresource(desc.buffer, 0, nullptr, initialData, 0, 0);
        }

        if (desc.Stride)
            desc.Element = (size / desc.Stride);
    }

    return true;
}

void CD3D11HardwareBuffer::Bind()
{
    if (ID3D11Buffer* buffer = getBufferResource(E_HARDWARE_BUFFER_TYPE::EHBT_INDEX))
    {
        CD3D11HardwareBuffer::BufferDesc& desc = VertexBufferStreams[(u32)E_HARDWARE_BUFFER_TYPE::EHBT_INDEX];

        const DXGI_FORMAT indexFormat = desc.Stride == sizeof(short) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;

        // set index buffer
        ImmediateContext->IASetIndexBuffer(buffer, indexFormat, desc.offset);
    }

    // We are now binding two buffers to the input assembler, one for the vertex data,
    UINT vertexBufferNum = 0;
    static UINT strides[2] = { 0, 0 };
    static UINT offsets[2] = { 0, 0 };

    // Store the vertex and instance buffers into an array
    static ID3D11Buffer* vertBuffers[2] = { nullptr, nullptr };

    if (ID3D11Buffer* buffer = getBufferResource(E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX))
    {
        CD3D11HardwareBuffer::BufferDesc& desc = VertexBufferStreams[(u32)E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX];

        ++vertexBufferNum;

        strides[0]      = desc.Stride;
        offsets[0]      = desc.offset;
        vertBuffers[0]  = buffer;
    }

    if (ID3D11Buffer* buffer = getBufferResource(E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX_INSTANCE_STREAM))
    {
        CD3D11HardwareBuffer::BufferDesc& desc = VertexBufferStreams[(u32)E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX_INSTANCE_STREAM];

        ++vertexBufferNum;

        strides[1]      = desc.Stride;
        offsets[1]      = desc.offset;
        vertBuffers[1]  = buffer;
    }
    else if ( vertBuffers[1] )
    {
        ++vertexBufferNum;

        strides[1] = 0;
        offsets[1] = 0;
        vertBuffers[1] = nullptr;
    }

    // set vertex buffer
    if ( vertexBufferNum )
        ImmediateContext->IASetVertexBuffers(0, vertexBufferNum, vertBuffers, strides, offsets);

    IHardwareBuffer::Bind();
}

void CD3D11HardwareBuffer::Unbind()
{
    IHardwareBuffer::Unbind();
}

void CD3D11HardwareBuffer::Initialize()
{
}

void CD3D11HardwareBuffer::Finalize()
{
}

#endif
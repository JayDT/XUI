// Copyright (C) 2002-2009 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_DIRECT3D_9_

#define _IRR_DONT_DO_MEMORY_DEBUGGING_HERE
#include "CD3D9HardwareBuffer.h"
#include "CD3D9Driver.h"
#include "os.h"

using namespace irr;
using namespace video;

CD3D9HardwareBuffer::CD3D9HardwareBuffer(CD3D9Driver* driver, scene::IMeshBuffer *meshBuffer, video::IShaderDataBuffer* instanceBuffer, u32 flags, LPDIRECT3DVERTEXDECLARATION9 declaration)
: IHardwareBuffer(meshBuffer, instanceBuffer)
, Driver(driver)
, Device(0)
, Flags(flags)
, Desc(declaration)
{
	#ifdef _DEBUG
	setDebugName("CD3D9HardwareBuffer");
	#endif

	Device=driver->getExposedVideoData().D3D9.D3DDev9;
    SAFE_ADDREF(Device);

	//UpdateBuffer(initialData);
}

CD3D9HardwareBuffer::~CD3D9HardwareBuffer()
{
	// Remove from driver cache
	//Driver->removeHardwareBuffer(this);

    for (u32 i = 0; i != VertexBufferStreams.size(); ++i)
    {
        if (VertexBufferStreams[i].TempBuffer)
        {
            SAFE_DELETE_ARRAY(VertexBufferStreams[i].TempBuffer);
        }

        SAFE_RELEASE(VertexBufferStreams[i].buffer);
    }
	SAFE_RELEASE(Device);
}

//! Lock function.
void* CD3D9HardwareBuffer::lock(E_HARDWARE_BUFFER_TYPE Type, u32 size, bool readOnly)
{
    if (VertexBufferStreams.size() <= (u32)Type)
        return nullptr;

	HRESULT hr = S_OK;
	void* ptr = 0;

	DWORD flags = 0;
	if (readOnly)
	{
		if (VertexBufferStreams[(u32)Type].AccessType == E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC)
			return 0;

		flags = D3DLOCK_READONLY;
	}
	else
	{
		if (VertexBufferStreams[(u32)Type].AccessType == E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC)
			flags |= D3DLOCK_DISCARD;
	}

	if (Type == E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX || Type == E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX_INSTANCE_STREAM)
	{
		if (!getVertexBuffer(Type))
			return 0;

		hr = getVertexBuffer(Type)->Lock(0, size, &ptr, flags);
		if (FAILED(hr))
			return 0;
	}
	else if (Type == E_HARDWARE_BUFFER_TYPE::EHBT_INDEX)
	{
		if (!getIndexBuffer())
			return 0;

		hr = getIndexBuffer()->Lock(0, size, &ptr, flags);
		if (FAILED(hr))
			return 0;
	}

	return ptr;
}

//! Unlock function. Must be called after a lock() to the buffer.
void CD3D9HardwareBuffer::unlock(E_HARDWARE_BUFFER_TYPE Type)
{
	if (Type == E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX)
	{
        getVertexBuffer(Type)->Unlock();
	}
	else if (Type == E_HARDWARE_BUFFER_TYPE::EHBT_INDEX)
	{
        getIndexBuffer()->Unlock();
	}
}

//! Copy data from system memory
void CD3D9HardwareBuffer::copyFromMemory(E_HARDWARE_BUFFER_TYPE type, const void* sysData, u32 offset, u32 length)
{

}

//! Copy data from another buffer
void CD3D9HardwareBuffer::copyFromBuffer(E_HARDWARE_BUFFER_TYPE type, IHardwareBuffer* buffer, u32 srcOffset, u32 descOffset, u32 length)
{

}

//! Get size of buffer in bytes
u32 CD3D9HardwareBuffer::size(E_HARDWARE_BUFFER_TYPE type) const
{
    if (VertexBufferStreams.size() <= (u32)type)
        return 0;

	return VertexBufferStreams[(u32)type].Size;
}

//! Get driver type of buffer.
E_DRIVER_TYPE CD3D9HardwareBuffer::getDriverType() const
{
	return EDT_DIRECT3D9;
}

//! Get type of buffer.
E_HARDWARE_BUFFER_TYPE CD3D9HardwareBuffer::getType(E_HARDWARE_BUFFER_TYPE type) const
{
	return VertexBufferStreams[(u32)type].Type;
}

//! Get flags
u32 CD3D9HardwareBuffer::getFlags() const
{
	return Flags;
}

//! Get vertex buffer
IDirect3DVertexBuffer9* CD3D9HardwareBuffer::getVertexBuffer(E_HARDWARE_BUFFER_TYPE type) const
{
    if (VertexBufferStreams.size() <= (u32)type)
        return nullptr;

	return (IDirect3DVertexBuffer9*)VertexBufferStreams[(u32)type].buffer;
}
	
//! Get index buffer
IDirect3DIndexBuffer9* CD3D9HardwareBuffer::getIndexBuffer() const
{
    if (VertexBufferStreams.size() <= (u32)E_HARDWARE_BUFFER_TYPE::EHBT_INDEX)
        return nullptr;

    return (IDirect3DIndexBuffer9*)VertexBufferStreams[(u32)E_HARDWARE_BUFFER_TYPE::EHBT_INDEX].buffer;
}

void CD3D9HardwareBuffer::notifyOnDeviceLost()
{
    for (u32 i = 0; i != VertexBufferStreams.size(); ++i)
    {
        // Only reset if pool is DEFAULT
        if (VertexBufferStreams[i].AccessType == E_HARDWARE_BUFFER_ACCESS::EHBA_SYSTEM_MEMORY)
            continue;

        // Only backup data if buffer is not dynamic
        if (VertexBufferStreams[i].AccessType != E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC)
        {
            // Allocate temp buffer
            VertexBufferStreams[i].TempBuffer = new char[VertexBufferStreams[i].Size];

            // Store data on temp buffer
            void* ptr = this->lock((E_HARDWARE_BUFFER_TYPE)i, true);
            ::memcpy(VertexBufferStreams[i].TempBuffer, ptr, VertexBufferStreams[i].Size);
            this->unlock((E_HARDWARE_BUFFER_TYPE)i);
        }

        // Release DX 9 buffer
        SAFE_RELEASE(VertexBufferStreams[i].buffer);
    }
}

void CD3D9HardwareBuffer::notifyOnDeviceReset(IDirect3DDevice9* device)
{
	// Recreate buffer with data from temp buffer
	this->Device = device;

    for (u32 i = 0; i != VertexBufferStreams.size(); ++i)
    {
        if (VertexBufferStreams[i].TempBuffer)
        {
            UpdateBuffer((E_HARDWARE_BUFFER_TYPE)i, VertexBufferStreams[i].AccessType, VertexBufferStreams[i].TempBuffer, VertexBufferStreams[i].Size);
            delete[] VertexBufferStreams[i].TempBuffer;
            VertexBufferStreams[i].TempBuffer = 0;
        }
    }
}

// Don't use D3DPOOL_MANAGED, is not valid with IDirect3DDevice9Ex
u32 CD3D9HardwareBuffer::GetMemoryAccessType(E_HARDWARE_BUFFER_ACCESS access)
{
    switch(access)
    {
        // Don't use D3DPOOL_MANAGED, is not valid with IDirect3DDevice9Ex
        case E_HARDWARE_BUFFER_ACCESS::EHBA_DEFAULT:
            return D3DPOOL_MANAGED;
        case E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC:
            return D3DPOOL_DEFAULT;
        case E_HARDWARE_BUFFER_ACCESS::EHBA_IMMUTABLE:
            return D3DPOOL_SCRATCH;
        case E_HARDWARE_BUFFER_ACCESS::EHBA_SYSTEM_MEMORY:
            return D3DPOOL_SYSTEMMEM;
        default:
            break;
    }

    return D3DPOOL_DEFAULT;
}

bool CD3D9HardwareBuffer::UpdateBuffer(E_HARDWARE_BUFFER_TYPE Type, E_HARDWARE_BUFFER_ACCESS AccessType, const void* initialData, u32 size)
{
	HRESULT hr = S_OK;

    if (VertexBufferStreams.size() <= (int)Type)
    {
        for (s32 i = 0; i <= (((s32)Type - (s32)VertexBufferStreams.size()) + 1); ++i)
            VertexBufferStreams.push_back(CD3D9HardwareBuffer::BufferDesc());
    }

	if (Type == E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX || Type == E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX_INSTANCE_STREAM)
	{
        if (!VertexBufferStreams[(u32)Type].initialize)
        {
            D3DVERTEXELEMENT9 decl[MAXD3DDECLLENGTH];
            UINT numElements;
            HRESULT hr = Desc->GetDeclaration(decl, &numElements);

            VertexBufferStreams[(u32)Type].Stride = D3DXGetDeclVertexSize(decl, (u32)Type - 1);
            VertexBufferStreams[(u32)Type].Size = 0;
            VertexBufferStreams[(u32)Type].initialize = true;
            VertexBufferStreams[(u32)Type].Type = Type;
        }

        if (VertexBufferStreams[(u32)Type].Size < size)
        {
            D3DPOOL resourcePool = (D3DPOOL)GetMemoryAccessType(AccessType);

            DWORD flags = 0;
            if (AccessType == E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC)
                flags |= (D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY);

            //DWORD FVF;
            //// Get the vertex sizes and cvf
            //switch (vType)
            //{
            //	case EVT_STANDARD:
            //		FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1;
            //		break;
            //	case EVT_2TCOORDS:
            //		FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX2;
            //		break;
            //	case EVT_TANGENTS:
            //		FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX3;
            //		break;
            //    case EVT_TERRAIN:
            //        FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX2;
            //        break;
            //    case EVT_SKINNING:
            //        FVF = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX2 | D3DFVF_XYZB5;
            //        break;
            //    default:
            //		return false;
            //}

            IDirect3DVertexBuffer9*& vertexBuffer = (IDirect3DVertexBuffer9*&)(VertexBufferStreams[(u32)Type].buffer);
            SAFE_RELEASE(vertexBuffer);
            hr = Device->CreateVertexBuffer(static_cast<UINT>(size),
                flags,
                0,
                resourcePool,
                &vertexBuffer,
                0);

            if (FAILED(hr))
            {
                os::Printer::log("Error creating hardware vertex buffer", ELL_ERROR);
                return false;
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
            VertexBufferStreams[(u32)Type].Size = 0;
            VertexBufferStreams[(u32)Type].initialize = true;
            VertexBufferStreams[(u32)Type].Type = Type;
        }

        if (VertexBufferStreams[(u32)Type].Size < size)
        {
            D3DFORMAT idxFormat = (this->Flags & (u32)E_HARDWARE_BUFFER_FLAGS::EHBF_INDEX_32_BITS) ? D3DFMT_INDEX32 : D3DFMT_INDEX16;
            D3DPOOL resourcePool = (D3DPOOL)GetMemoryAccessType(AccessType);

            DWORD flags = 0;
            if (AccessType == E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC)
                flags |= (D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY);

            IDirect3DIndexBuffer9*& indexBuffer = (IDirect3DIndexBuffer9*&)(VertexBufferStreams[(u32)Type].buffer);
            SAFE_RELEASE(indexBuffer);
            hr = Device->CreateIndexBuffer(static_cast<UINT>(size),
                flags,
                idxFormat,
                resourcePool,
                &indexBuffer,
                0);

            if (FAILED(hr))
            {
                os::Printer::log("Error creating hardware index buffer", ELL_ERROR);
                return false;
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

	// Copy initial data to buffer
	if (initialData)
	{
		void* ptr = this->lock(Type, size);
		::memcpy(ptr, initialData, size);
		this->unlock(Type);
        VertexBufferStreams[(u32)Type].Element = (size / VertexBufferStreams[(u32)Type].Stride);
	}
	
	return true;
}


void CD3D9HardwareBuffer::Bind()
{
    if (getIndexBuffer())
        Device->SetIndices(getIndexBuffer());

    if (getVertexBuffer(E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX))
    {
        Device->SetVertexDeclaration(Desc);
        Device->SetStreamSource(0, getVertexBuffer(E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX), 0, VertexBufferStreams[(u32)E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX].Stride);

        if (getVertexBuffer(E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX_INSTANCE_STREAM))
        {
            //_IRR_DEBUG_BREAK_IF(GetInstanceBuffer()->isChanged());
            //_IRR_DEBUG_BREAK_IF(!instanceCount);

            Device->SetStreamSourceFreq(0, (u32)(D3DSTREAMSOURCE_INDEXEDDATA | (u32)GetInstanceBuffer()->getInstanceCount()));
            Device->SetStreamSourceFreq(1, (u32)(D3DSTREAMSOURCE_INSTANCEDATA | 1ul));
            Device->SetStreamSource(1, getVertexBuffer(E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX_INSTANCE_STREAM), 0, VertexBufferStreams[(u32)E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX_INSTANCE_STREAM].Stride);
        }
    }

    IHardwareBuffer::Bind();
}

void CD3D9HardwareBuffer::Unbind()
{
    //Device->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
    //Device->SetSamplerState(0, D3DSAMP_MAXMIPLEVEL, 0);

    Device->SetVertexDeclaration(nullptr);
    Device->SetStreamSourceFreq(0, 1);
    Device->SetStreamSource(0, nullptr, 0, 0);
    Device->SetStreamSourceFreq(1, 1);
    Device->SetStreamSource(1, nullptr, 0, 0);
    Device->SetIndices(0);

    IHardwareBuffer::Unbind();
}

void CD3D9HardwareBuffer::Initialize()
{
}

void CD3D9HardwareBuffer::Finalize()
{
}

#endif
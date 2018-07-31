// Copyright (C) 2002-2009 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_DIRECT3D_11_

#define _IRR_DONT_DO_MEMORY_DEBUGGING_HERE

#include "CD3D11Driver.h"
#include "CD3D11MaterialRenderer.h"
#include "CD3D11VertexDeclaration.h"
#include "CD3D11Shader.h"

#include "os.h"

using namespace irr;
using namespace video;

LPCSTR getSemanticName(E_VERTEX_ELEMENT_SEMANTIC semantic)
{
    switch (semantic)
    {
        case EVES_POSITION:
            return "POSITION";
        case EVES_NORMAL:
            return "NORMAL";
        case EVES_TEXTURE_COORD:
            return "TEXCOORD";
        case EVES_COLOR:
            return "COLOR";
        case EVES_TANGENT:
            return "TANGENT";
        case EVES_BINORMAL:
            return "BINORMAL";
        case EVES_BLEND_WEIGHTS:
            return "BLENDWEIGHT";
        case EVES_BLEND_INDICES:
            return "BLENDINDICES";
    }

    return "POSITION";
}

DXGI_FORMAT getFormat(E_VERTEX_ELEMENT_TYPE type)
{
    switch (type)
    {
        case EVET_FLOAT1:
            return DXGI_FORMAT_R32_FLOAT;
        case EVET_FLOAT2:
            return DXGI_FORMAT_R32G32_FLOAT;
        case EVET_FLOAT3:
            return DXGI_FORMAT_R32G32B32_FLOAT;
        case EVET_FLOAT4:
            return DXGI_FORMAT_R32G32B32A32_FLOAT;

        case EVET_UINT1:
            return DXGI_FORMAT_R32_UINT;
        case EVET_UINT2:
            return DXGI_FORMAT_R32G32_UINT;
        case EVET_UINT3:
            return DXGI_FORMAT_R32G32B32_UINT;
        case EVET_UINT4:
            return DXGI_FORMAT_R32G32B32A32_UINT;

        case EVET_SHORT2:
            return DXGI_FORMAT_R16G16_SINT;
        case EVET_SHORT4:
            return DXGI_FORMAT_R16G16B16A16_SINT;

        case EVET_UBYTE4_NORM:
        case EVET_COLOUR:
            return DXGI_FORMAT_R8G8B8A8_UNORM;
        case EVET_UBYTE4:
            return DXGI_FORMAT_R8G8B8A8_UINT;
    }

    return DXGI_FORMAT_R32G32B32A32_FLOAT;
}


CD3D11VertexDeclaration::CD3D11VertexDeclaration(CD3D11Driver* driver)
    : D3D11DeviceResource(driver)
    , VertexPitch(0)
{
	auto Device = Driver->getExposedVideoData().D3D11.D3DDev11;
	if (Device)
	{
		Device->AddRef();
	}
}

CD3D11VertexDeclaration::~CD3D11VertexDeclaration()
{
	// Delete input layout elements
    for (auto node : InputLayouts)
        node.second->Release();
    InputLayouts.clear();

    auto Device = Driver->getExposedVideoData().D3D11.D3DDev11;
    if (Device)
    {
        Device->Release();
    }
}

core::array<D3D11_INPUT_ELEMENT_DESC> const & irr::video::CD3D11VertexDeclaration::getVertexDeclaration()
{
    if (VertexDeclarationMap.empty())
    {
        D3D11_INPUT_ELEMENT_DESC desc;
        for (u32 i = 0; i < mVertexElements.size(); i++)
        {
            desc.SemanticName           = getSemanticName(mVertexElements[i].SemanticType);
            desc.SemanticIndex          = mVertexElements[i].SemanticIndex;
            desc.Format                 = getFormat(mVertexElements[i].Type);
            desc.InputSlot              = mVertexElements[i].SlotIndex;
            desc.AlignedByteOffset      = mVertexElements[i].Offset == -1 ? D3D11_APPEND_ALIGNED_ELEMENT : mVertexElements[i].Offset;
            desc.InputSlotClass         = mVertexElements[i].PerInstance ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
            desc.InstanceDataStepRate   = mVertexElements[i].InstanceStep;

            VertexDeclarationMap.push_back(desc);

            if (VertexPitch.size() < desc.InputSlot + 1)
                VertexPitch.resize(desc.InputSlot + 1);
            VertexPitch[desc.InputSlot] += Driver->getBitsPerPixel(getFormat(mVertexElements[i].Type)) / 8;
        }
    }

    return VertexDeclarationMap;
}


void irr::video::CD3D11VertexDeclaration::OnDeviceLost(CD3D11Driver * device)
{
    for (auto node : InputLayouts)
        node.second->Release();
    InputLayouts.clear();
}

void irr::video::CD3D11VertexDeclaration::OnDeviceRestored(CD3D11Driver * device)
{
}

//D3D11_SO_DECLARATION_ENTRY* CD3D11VertexDeclaration::getStreamOutputDeclaration() const
const core::array<D3D11_SO_DECLARATION_ENTRY>& CD3D11VertexDeclaration::getStreamOutputDeclaration() const
{
	if (VertexOutputDeclarationMap.empty())
	{
		D3D11_SO_DECLARATION_ENTRY entry;
		for(u32 i = 0; i < VertexDeclarationMap.size(); i++)
		{
			entry.Stream = VertexDeclarationMap[i].InputSlot;
			entry.SemanticName = VertexDeclarationMap[i].SemanticName;
			entry.SemanticIndex = VertexDeclarationMap[i].SemanticIndex;
			
			entry.StartComponent = 0;
			entry.ComponentCount = Driver->getNumberOfComponents(VertexDeclarationMap[i].Format);

			//! plan how to use more than one output slot
			entry.OutputSlot = 0;

            VertexOutputDeclarationMap.push_back(entry);
		}
	}

	return VertexOutputDeclarationMap;
}

ID3D11InputLayout* CD3D11VertexDeclaration::getInputLayout(const D3D11HLSLProgram * shader)
{
	ID3D11InputLayout* layout = 0;
	auto node = InputLayouts.find(shader);
	if (node != InputLayouts.end() && node->second)
	{
		layout = node->second;
	}
	else
	{
        core::array<D3D11_INPUT_ELEMENT_DESC> const & vertDeclImpl = getVertexDeclaration();

        // create input layout from previosly created description
        HRESULT hr = Driver->getExposedVideoData().D3D11.D3DDev11->CreateInputLayout(
            vertDeclImpl.const_pointer(),
            vertDeclImpl.size(),
            shader->GetVertexShaderByteCode()->GetBufferPointer(),
            shader->GetVertexShaderByteCode()->GetBufferSize(),
            &layout
        );

        if (FAILED(hr))
        {
            // impossible to create input layout
            return nullptr;
        }

        InputLayouts[shader] = layout;
	}

	return layout;
}

#endif
//// Copyright (C) 2002-2009 Nikolaus Gebhardt
//// This file is part of the "Irrlicht Engine".
//// For conditions of distribution and use, see copyright notice in irrlicht.h
//
//#include "IrrCompileConfig.h"
//#ifdef _IRR_COMPILE_WITH_DIRECT3D_11_
//
//#define _IRR_DONT_DO_MEMORY_DEBUGGING_HERE
//
//#include "CD3D11Driver.h"
//#include "CD3D11MaterialRenderer.h"
//#include "CD3D11VertexDeclaration.h"
//
//#include "os.h"
//
//namespace irr
//{
//namespace video
//{
//
//CD3D11VertexDeclaration::CD3D11VertexDeclaration(CD3D11Driver* driver, core::array<SVertexElement>& elements, E_VERTEX_TYPE type)
//: Driver(driver)
//, IAElements(0)
//, SOElements(0)
//, Size(elements.size())
//, VertexPitch(0)
//, VertexType(type)
//, Device(0)
//, ImmediateContext(0)
//{
//	Device = Driver->getExposedVideoData().D3D11.D3DDev11;
//	if (Device)
//	{
//		Device->AddRef();
//	}
//
//	//IAElements = new D3D11_INPUT_ELEMENT_DESC[Size];
//	D3D11_INPUT_ELEMENT_DESC desc;
//	for(u32 i = 0; i < Size; i++)
//	{
//		desc.SemanticName = getSemanticName(elements[i].SemanticType);
//		desc.SemanticIndex = elements[i].SemanticIndex;
//		desc.Format = getFormat(elements[i].Type);
//		desc.InputSlot = elements[i].SlotIndex;
//		desc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
//		desc.InputSlotClass = elements[i].PerInstance ? D3D11_INPUT_PER_INSTANCE_DATA : D3D11_INPUT_PER_VERTEX_DATA;
//		desc.InstanceDataStepRate = elements[i].InstanceStep;
//
//		IAElements.push_back(desc);
//
//		VertexPitch += Driver->getBitsPerPixel(getFormat(elements[i].Type)) / 8;
//	}
//}
//
//CD3D11VertexDeclaration::~CD3D11VertexDeclaration()
//{
//	// Delete input layout elements
//	LayoutIterator it = layoutMap.getIterator();
//	while(!it.atEnd())
//	{
//		it->getValue()->Release();
//		it++;
//	}
//	layoutMap.clear();
//
//	SAFE_RELEASE( Device );
//}
//
////D3D11_INPUT_ELEMENT_DESC* CD3D11VertexDeclaration::getInputAssemblyDeclaration() const
//const core::array<D3D11_INPUT_ELEMENT_DESC>& CD3D11VertexDeclaration::getInputAssemblyDeclaration() const
//{
//	return IAElements;
//}
//
////D3D11_SO_DECLARATION_ENTRY* CD3D11VertexDeclaration::getStreamOutputDeclaration() const
//const core::array<D3D11_SO_DECLARATION_ENTRY>& CD3D11VertexDeclaration::getStreamOutputDeclaration() const
//{
//	if (SOElements.size() == 0)
//	{
//		D3D11_SO_DECLARATION_ENTRY entry;
//		for(u32 i = 0; i < Size; i++)
//		{
//			entry.Stream = IAElements[i].InputSlot;
//			entry.SemanticName = IAElements[i].SemanticName;
//			entry.SemanticIndex = IAElements[i].SemanticIndex;
//			
//			entry.StartComponent = 0;
//			entry.ComponentCount = Driver->getNumberOfComponents(IAElements[i].Format);
//
//			//! plan how to use more than one output slot
//			entry.OutputSlot = 0;
//
//			SOElements.push_back(entry);
//		}
//	}
//
//	return SOElements;
//}
//
//ID3D11InputLayout* CD3D11VertexDeclaration::getInputLayout(IMaterialRenderer* rend)
//{
//	CD3D11MaterialRenderer* renderer = static_cast<CD3D11MaterialRenderer*>(rend);
//	u32 signature = reinterpret_cast<u32>(renderer->getShaderByteCode());
//
//	ID3D11InputLayout* layout = 0;
//	LayoutMapNode* node = layoutMap.find(signature);
//	if (node)
//	{
//		layout = node->getValue();
//	}
//	else
//	{
//		// if don't exists, create layout
//		if(FAILED(Device->CreateInputLayout( IAElements.pointer(), Size, 
//											 renderer->getShaderByteCode(),
//											 renderer->getShaderByteCodeSize(), 
//											 &layout )))
//		{
//			os::Printer::log("Error, failed to create input layout", ELL_ERROR);
//			return 0;
//		}
//
//		layoutMap.insert(signature, layout);
//	}
//
//	return layout;
//}
//
//LPCSTR CD3D11VertexDeclaration::getSemanticName(E_VERTEX_ELEMENT_SEMANTIC semantic) const
//{
//	switch (semantic)
//	{
//	case EVES_POSITION:
//		return "POSITION";
//	case EVES_NORMAL:
//		return "NORMAL";
//	case EVES_TEXTURE_COORD:
//		return "TEXCOORD";
//	case EVES_COLOR:
//		return "COLOR";
//	case EVES_TANGENT:
//		return "TANGENT";
//	case EVES_BINORMAL:
//		return "BINORMAL";
//	case EVES_BLEND_WEIGHTS:
//		return "BLENDWEIGHT";
//	case EVES_BLEND_INDICES:
//		return "BLENDINDICES";
//	}
//
//	return "POSITION";
//}
//
//DXGI_FORMAT CD3D11VertexDeclaration::getFormat(E_VERTEX_ELEMENT_TYPE type) const
//{
//	switch (type)
//	{
//	case EVET_FLOAT1:
//		return DXGI_FORMAT_R32_FLOAT;
//	case EVET_FLOAT2:
//		return DXGI_FORMAT_R32G32_FLOAT;
//	case EVET_FLOAT3:
//		return DXGI_FORMAT_R32G32B32_FLOAT;
//	case EVET_FLOAT4:
//		return DXGI_FORMAT_R32G32B32A32_FLOAT;
//
//	case EVET_UINT1:
//		return DXGI_FORMAT_R32_UINT;
//	case EVET_UINT2:
//		return DXGI_FORMAT_R32G32_UINT;
//	case EVET_UINT3:
//		return DXGI_FORMAT_R32G32B32_UINT;
//	case EVET_UINT4:
//		return DXGI_FORMAT_R32G32B32A32_UINT;
//
//	case EVET_SHORT2:
//		return DXGI_FORMAT_R16G16_SINT;
//	case EVET_SHORT4:
//		return DXGI_FORMAT_R16G16B16A16_SINT;
//	
//	case EVET_COLOUR:
//		return DXGI_FORMAT_R8G8B8A8_UNORM;
//	case EVET_UBYTE4:
//		return DXGI_FORMAT_R8G8B8A8_UINT;
//	}
//
//	return DXGI_FORMAT_R32G32B32A32_FLOAT;
//}
//
//u32 CD3D11VertexDeclaration::getVertexPitch() const
//{
//	return VertexPitch;
//}
//
//}
//}
//
//#endif
	// Copyright (C) 2002-2009 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_DIRECT3D_11_
//
//#include "CD3D11CustomMaterialRenderer.h"
//#include "CD3D11VertexDeclaration.h"
//
//#include "IShaderConstantSetCallBack.h"
//#include "IVideoDriver.h"
//#include "os.h"
//#include "irrString.h"
//
//#ifndef _IRR_D3D_NO_SHADER_DEBUGGING
//#include <stdio.h>
//#endif
//
//
//namespace irr
//{
//namespace video
//{
//
////! Public constructor
//CD3D11CustomMaterialRenderer::CD3D11CustomMaterialRenderer(ID3D11Device* d3ddev, 
//		video::IVideoDriver* driver,
//		s32& outMaterialTypeNr,
//		const c8* vertexShaderProgram,
//		const c8* vertexShaderEntryPointName,
//		E_VERTEX_SHADER_TYPE vsCompileTarget,
//		const c8* pixelShaderProgram,
//		const c8* pixelShaderEntryPointName,
//		E_PIXEL_SHADER_TYPE psCompileTarget,
//		const c8* geometryShaderProgram,
//		const c8* geometryShaderEntryPointName,
//		E_GEOMETRY_SHADER_TYPE gsCompileTarget,
//		scene::E_PRIMITIVE_TYPE inType, 
//		scene::E_PRIMITIVE_TYPE outType,
//		u32 verticesOut,
//		E_VERTEX_TYPE vertexTypeOut,
//		IShaderConstantSetCallBack* callback,
//		IMaterialRenderer* baseMaterial,
//		s32 userData)
//	: CD3D11MaterialRenderer(static_cast<CD3D11Driver*>(driver))
//	, Effect(0)
//	, CallBack(callback)
//	, UserData(userData)
//	, isStreamOutput(false)
//{
//	#ifdef _DEBUG
//	setDebugName("CD3D11CustomMaterialRenderer");
//	#endif
//
//	if (Device)
//	{
//		Device->AddRef();
//	}
//	Device->GetImmediateContext( &ImmediateContext );
//
//	BaseRenderer = static_cast<CD3D11MaterialRenderer*>(baseMaterial);
//	if (BaseRenderer)
//		BaseRenderer->grab();
//
//	if (CallBack)
//		CallBack->grab();
//
//	outMaterialTypeNr = -1;
//	ZeroMemory(&PassDescription, sizeof(D3DX11_PASS_DESC));
//
//	// Create effect
//	if (!init(vertexShaderProgram, vertexShaderEntryPointName, vsCompileTarget,
//		 pixelShaderProgram, pixelShaderEntryPointName, psCompileTarget,
//		 geometryShaderProgram, geometryShaderEntryPointName, gsCompileTarget,
//		 inType, outType, verticesOut, vertexTypeOut))
//	{
//		if (BaseRenderer)
//			BaseRenderer->drop();
//
//		if (CallBack)
//			CallBack->drop();
//
//		SAFE_RELEASE(Effect);
//		SAFE_RELEASE(ImmediateContext);
//		SAFE_RELEASE(Device);
//		return;
//	}
//
//	// register myself as new material
//	outMaterialTypeNr = Driver->addMaterialRenderer(this);
//}
//
//CD3D11CustomMaterialRenderer::~CD3D11CustomMaterialRenderer()
//{
//	if (BaseRenderer)
//		BaseRenderer->drop();
//
//	if (CallBack)
//		CallBack->drop();
//
//	SAFE_RELEASE(Effect);
//	SAFE_RELEASE(ImmediateContext);
//	SAFE_RELEASE(Device);
//}
//
//bool CD3D11CustomMaterialRenderer::setVariable(const c8* name, const f32* floats, int count)
//{
//	if (!Effect)
//		return false;
//
//	ID3DX11EffectVariable* var = Effect->GetVariableByName(name);
//	if( var->IsValid() )
//	{
//		switch( count )
//		{
//		case 16:
//			var->AsMatrix()->SetMatrix((f32*)floats);
//			break;
//
//		case 4:
//			var->AsVector()->SetFloatVector((f32*)floats);
//			break;
//
//		case 1:
//			var->AsScalar()->SetFloat((f32)*floats);
//			break;
//
//		default:
//			var->SetRawValue(floats, 0, count);
//		};
//
//		return true;
//	}
//
//	return false;
//}
//
//void CD3D11CustomMaterialRenderer::OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
//				bool resetAllRenderstates, IMaterialRendererServices* services)
//{
//	BaseRenderer->OnSetMaterial(material, lastMaterial, resetAllRenderstates, services);
//}
//
//bool CD3D11CustomMaterialRenderer::OnRender(IMaterialRendererServices* service, E_VERTEX_TYPE vtxtype)
//{
//	if (!Effect)
//		return false;
//
//	if (CallBack)
//		CallBack->OnSetConstants(service, UserData);
//
//	// Apply pass
//	Technique->GetPassByIndex(0)->Apply( 0, ImmediateContext );
//
//	return true;
//}
//
///** Called during the IVideoDriver::setMaterial() call before the new
//material will get the OnSetMaterial() call. */
//void CD3D11CustomMaterialRenderer::OnUnsetMaterial()
//{
//	BaseRenderer->OnUnsetMaterial();
//}
//
////! get shader signature
//void* CD3D11CustomMaterialRenderer::getShaderByteCode() const
//{
//	return PassDescription.pIAInputSignature;
//}
//
////! get shader signature size
//u32 CD3D11CustomMaterialRenderer::getShaderByteCodeSize() const
//{
//	return PassDescription.IAInputSignatureSize;
//}
//
//bool CD3D11CustomMaterialRenderer::isTransparent() const
//{
//	return BaseRenderer->isTransparent();
//}
//
//bool CD3D11CustomMaterialRenderer::init(const core::stringc vertexShaderProgram,
//										const c8* vertexShaderEntryPointName,
//										E_VERTEX_SHADER_TYPE vsCompileTarget,
//										const core::stringc pixelShaderProgram,
//										const c8* pixelShaderEntryPointName,
//										E_PIXEL_SHADER_TYPE psCompileTarget,
//										const core::stringc geometryShaderProgram,
//										const c8* geometryShaderEntryPointName,
//										E_GEOMETRY_SHADER_TYPE gsCompileTarget,
//										scene::E_PRIMITIVE_TYPE inType,			// Only for OpenGL
//										scene::E_PRIMITIVE_TYPE outType,		// Only for OpenGL
//										u32 verticesOut,						// Only for OpenGL
//										E_VERTEX_TYPE vertexTypeOut)			// Only for Direct3D 11
//{
//	// now create shaders
//	if (vsCompileTarget < EVST_VS_4_0 || vsCompileTarget > EVST_COUNT)
//	{
//		os::Printer::log("Invalid HLSL vertex shader compilation target", ELL_ERROR);
//		return false;
//	}
//
//	// Create effect string
//	
//	core::stringc effectString = vertexShaderProgram;
//	effectString += "\n";
//
//	// Prevent overwriting
//	if (geometryShaderProgram != vertexShaderProgram)
//	{
//		effectString += geometryShaderProgram;
//		effectString += "\n";
//	}
//	if (pixelShaderProgram != geometryShaderProgram &&
//		pixelShaderProgram != vertexShaderProgram)
//	{
//		effectString += pixelShaderProgram;
//		effectString += "\n";
//	}
//
//	// Preparing technique
//	core::stringc techString = "technique11 CustomTech\n";
//	techString += "\n";
//	techString += "{\n";
//	techString += "	pass p0\n";
//	techString += "	{";
//
//	// Set vertex shader
//	techString += "		SetVertexShader( CompileShader( ";
//	techString += VERTEX_SHADER_TYPE_NAMES[vsCompileTarget];
//	techString += ", ";
//	techString += vertexShaderEntryPointName;
//	techString += "() ) );\n";
//
//	// Consider stream output for effect if pixel shader is null
//	//if (!pixelShaderProgram.size())
//	//{
//	//	isStreamOutput = true;
//    //
//	//	// if not exists geometry shader, use stream output with vertex shader
//	//	if (geometryShaderProgram.size())
//	//	{
//	//		techString += "		SetGeometryShader( ConstructGSWithSO( CompileShader( ";
//	//		techString += GEOMETRY_SHADER_TYPE_NAMES[gsCompileTarget];
//	//		techString += ", ";
//	//		techString += geometryShaderEntryPointName;
//	//		techString += "() ), ";
//	//		techString += parseStreamOutputDeclaration( Driver->getVertexDeclaration(vertexTypeOut) );
//	//		techString += " ) );\n";
//	//	}
//	//	else
//	//	{
//	//		techString += "		SetGeometryShader( ConstructGSWithSO( CompileShader( ";
//	//		techString += VERTEX_SHADER_TYPE_NAMES[vsCompileTarget];
//	//		techString += ", ";
//	//		techString += vertexShaderEntryPointName;
//	//		techString += "() ), ";
//	//		techString += parseStreamOutputDeclaration( Driver->getVertexDeclaration(vertexTypeOut) );
//	//		techString += " ) );\n";
//	//	}
//    //
//	//	// Add NULL pixel shader
//	//	techString += "		SetPixelShader( NULL );\n";
//	//	techString += "	}\n";
//	//}
//	//else
//	//{
//	//	if (geometryShaderProgram.size())
//	//	{
//	//		// Set geometry shader
//	//		techString += "		SetGeometryShader( CompileShader( ";
//	//		techString += GEOMETRY_SHADER_TYPE_NAMES[gsCompileTarget];
//	//		techString += ", ";                                                                                                        
//	//		techString += geometryShaderEntryPointName;
//	//		techString += "() ) );\n";
//	//	}
//	//	else
//	//	{
//	//		// Add NULL geometry shader
//	//		techString += "		SetGeometryShader( NULL );\n";
//	//	}
//    //
//	//	// Set pixel shader
//	//	techString += "		SetPixelShader( CompileShader( ";
//	//	techString += PIXEL_SHADER_TYPE_NAMES[psCompileTarget];
//	//	techString += ", ";
//	//	techString += pixelShaderEntryPointName;
//	//	techString += "() ) );\n";
//	//	techString += "	}\n";
//	//}
//
//	techString += "}";
//
//	effectString += techString;
//	effectString += "\n";
//
//	UINT flags = 0;
//	//flags |= D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY;
//#ifdef _DEBUG
//	// These values allow use of PIX and shader debuggers
//	flags |= D3D10_SHADER_DEBUG;
//	flags |= D3D10_SHADER_SKIP_OPTIMIZATION;
//	flags |= D3D10_SHADER_WARNINGS_ARE_ERRORS;
//#else
//	// These flags allow maximum performance
//	flags |= D3D10_SHADER_ENABLE_STRICTNESS;
//	flags |= D3D10_SHADER_OPTIMIZATION_LEVEL3;
//#endif
//
//	// Compile effect
//	HRESULT hr = S_OK;
//	ID3DBlob* ppCode = NULL;
//	ID3DBlob* ppErrors = NULL;
//	hr = D3DCompile( effectString.c_str(), effectString.size(), "",
//					 NULL, // macros
//					 NULL, // includes
//					 NULL, // entrypoint
//					 "fx_5_0",
//					 flags,
//					 0,
//					 &ppCode,
//					 &ppErrors );
//	if (FAILED(hr))
//	{
//		core::stringc errorMsg = "Error, could not compile custom effect";
//		if (ppErrors)
//		{
//			errorMsg += ": ";
//			errorMsg += static_cast<const char*>(ppErrors->GetBufferPointer());
//			ppErrors->Release();
//		}
//		os::Printer::log(errorMsg.c_str(), ELL_ERROR);
//		return false;
//	}
//#ifdef _DEBUG
//	else if (ppErrors)
//	{
//		core::stringc errorMsg = "Effect compilation warning: ";
//		if (ppErrors)
//		{
//			errorMsg += static_cast<const char*>(ppErrors->GetBufferPointer());
//			errorMsg += "\n";
//			ppErrors->Release();
//		}
//		os::Printer::log(errorMsg.c_str(), ELL_ERROR);
//	}
//#endif
//
//	// Create effect
//	hr = D3DX11CreateEffectFromMemory( ppCode->GetBufferPointer(), ppCode->GetBufferSize(), 0, Device, &Effect );
//	ppCode->Release();
//	if (FAILED(hr))
//	{
//		LPTSTR errorText = NULL;
//		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS,
//					  NULL,
//					  hr,
//					  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//					  (LPTSTR)&errorText,
//					  0,
//					  NULL);
//		if ( NULL != errorText )
//		{
//			core::stringc errorMsg = "Error, could not create custom effect: ";
//			errorMsg += errorText;
//			os::Printer::log(errorMsg.c_str(), ELL_ERROR);
//
//			// release memory allocated by FormatMessage()
//			LocalFree(errorText);
//			errorText = NULL;
//		}
//		else
//			os::Printer::log("Error, could not create custom effect", ELL_ERROR);
//		
//		return false;
//	}
//
//	// Get technique
//	Technique = Effect->GetTechniqueByName("CustomTech");
//	if (!Technique->IsValid())
//	{
//		os::Printer::log("Error, could not obtain effect technique", ELL_ERROR);
//		return false;
//	}
//
//	// Get pass description
//	ID3DX11EffectPass* pass = Technique->GetPassByIndex(0);
//	if (!pass->IsValid())
//	{
//		os::Printer::log("Error, could not obtain effect pass", ELL_ERROR);
//		return false;
//	}
//	pass->GetDesc(&PassDescription);
//
//	return true;
//}
//
//core::stringc CD3D11CustomMaterialRenderer::parseStreamOutputDeclaration(CD3D11VertexDeclaration* declaration)
//{
//	// Parse declaration
//	const core::array<D3D11_SO_DECLARATION_ENTRY>& entries = declaration->getStreamOutputDeclaration();
//	u32 size = declaration->size();
//
//	core::stringc decl = "\"";
//
//	for(u32 i = 0; i < size; i++)
//	{
//		// Add semantic index
//		//decl += i;
//		//decl += ":";
//
//		// Add semantic name
//		//if(strcmp("POSITION", entries[i].SemanticName) == 0)
//		//	decl += "SV_Position";
//		//else
//		decl += entries[i].SemanticName;
//		if (entries[i].SemanticIndex > 0)
//			decl += entries[i].SemanticIndex;
//
//		// Add coords used
//		decl += ".";
//		switch (entries[i].ComponentCount)
//		{
//		case 4:
//			if(strcmp("COLOR", entries[i].SemanticName) == 0)
//				decl += "rgba";
//			else
//				decl += "xyzw";
//			break;
//
//		case 3:
//			decl += "xyz";
//			break;
//
//		case 2:
//			decl += "xy";
//			break;
//
//		case 1:
//			decl += "x";
//			break;
//		}
//
//		// Finish element with ";"
//		if (i < (size - 1))
//			decl += "; ";
//	}
//
//	decl += "\"";
//
//	return decl;
//}
//
//} // end namespace video
//} // end namespace irr

#endif
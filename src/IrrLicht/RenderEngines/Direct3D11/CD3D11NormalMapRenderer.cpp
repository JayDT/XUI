// Copyright (C) 2002-2009 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_DIRECT3D_11_
//
//#include "CD3D11NormalMapRenderer.h"
//#include "IVideoDriver.h"
//#include "IMaterialRendererServices.h"
//#include "os.h"
//#include "SLight.h"
//
//namespace irr
//{
//namespace video  
//{
//
//	CD3D11NormalMapRenderer::CD3D11NormalMapRenderer(ID3D11Device* d3dDev, video::IVideoDriver* driver,
//		io::IFileSystem* fileSystem, s32& outMaterialTypeNr, IMaterialRenderer* baseMaterial)
//		: CD3D11MaterialRenderer(static_cast<CD3D11Driver*>(driver))
//		, BaseMaterial(baseMaterial)
//		, Effect(0)
//		, Technique(0)
//		, WorldMatrix(0)
//		, WorldViewProjMatrix(0)
//		, LightPos1(0)
//		, LightColor1(0)
//		, LightAtten1(0)
//		, LightPos2(0)
//		, LightColor2(0)
//		, LightAtten2(0)		
//	{
//#ifdef _DEBUG
//		setDebugName("CD3D11NormalMapRenderer");
//#endif
//		HRESULT hr = S_OK;
//		ZeroMemory(&PassDescription, sizeof(D3DX11_PASS_DESC));
//
//		video::IMaterialRenderer* renderer = driver->getMaterialRenderer(EMT_NORMAL_MAP_SOLID);
//		if(renderer)
//		{
//			// Reuse effect if already exists
//			Effect = ((video::CD3D11NormalMapRenderer*)renderer)->Effect;
//			if(Effect)
//				Effect->AddRef();
//		}
//		else
//		{
//			// Create effect if this is first
//			UINT flags = 0;
//			//flags |= D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY;
//#ifdef _DEBUG
//			// These values allow use of PIX and shader debuggers
//			flags |= D3D10_SHADER_DEBUG;
//			flags |= D3D10_SHADER_SKIP_OPTIMIZATION;
//#else
//			// These flags allow maximum performance
//			flags |= D3D10_SHADER_ENABLE_STRICTNESS;
//			flags |= D3D10_SHADER_OPTIMIZATION_LEVEL3;
//#endif
//	
//			io::IReadFile* effectFile = fileSystem->createAndOpenFile("../../media/d3d11_material_normalmap.fx");
//			if(!effectFile)
//			{
//				os::Printer::log("Error, could not open effect file");
//				return;
//			}
//			char* src = new char[effectFile->getSize()];
//			effectFile->read( src, effectFile->getSize() );
//	
//			ID3DBlob* ppCode = NULL;
//			ID3DBlob* ppErrors = NULL;
//			HRESULT hr = D3DCompile( src, effectFile->getSize(), "none", NULL, NULL, NULL, "fx_5_0", flags, D3DCOMPILE_EFFECT_ALLOW_SLOW_OPS, &ppCode, &ppErrors );
//			if (FAILED(hr))
//			{
//				core::stringc errorMsg = "Error, could not compile normal map effect";
//				if (ppErrors)
//				{
//					errorMsg += ": ";
//					errorMsg += static_cast<const char*>(ppErrors->GetBufferPointer());
//					ppErrors->Release();
//				}
//				os::Printer::log(errorMsg.c_str(), ELL_ERROR);
//				return;
//			}
//
//			hr = D3DX11CreateEffectFromMemory( ppCode->GetBufferPointer(), ppCode->GetBufferSize(), flags, Device, &Effect );
//			delete src;
//			effectFile->drop();
//			if (FAILED(hr))
//			{
//				os::Printer::log("Error, could not create normal map effect", ELL_ERROR);
//				return;
//			}
//		}
//
//		if(Effect)
//		{
//			Technique = Effect->GetTechniqueByName("NormalMapTechnique");
//			Technique->GetPassByIndex(0)->GetDesc(&PassDescription);
//			WorldMatrix = Effect->GetVariableByName("g_mWorld")->AsMatrix();
//			WorldViewProjMatrix = Effect->GetVariableByName("g_mWorldViewProj")->AsMatrix();
//			LightPos1 = Effect->GetVariableByName("g_lightPos1")->AsVector();
//			LightColor1 = Effect->GetVariableByName("g_lightColor1")->AsVector();
//			LightAtten1 = Effect->GetVariableByName("g_lightAtten1")->AsScalar();
//			LightPos2 = Effect->GetVariableByName("g_lightPos2")->AsVector();
//			LightColor2 = Effect->GetVariableByName("g_lightColor2")->AsVector();
//			LightAtten2 = Effect->GetVariableByName("g_lightAtten2")->AsScalar();
//		}
//
//		//
//		outMaterialTypeNr = Driver->addMaterialRenderer(this);
//	}
//
//	CD3D11NormalMapRenderer::~CD3D11NormalMapRenderer()
//	{
//		SAFE_RELEASE(Effect);
//	}
//
//	bool CD3D11NormalMapRenderer::setVariable(const c8* name, const f32* floats, int count)
//	{
//		os::Printer::log("Could not set variable on normal map material renderer");
//		return false;
//	}
//
//	void CD3D11NormalMapRenderer::OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
//						bool resetAllRenderstates, IMaterialRendererServices* services)
//	{
//		BaseMaterial->OnSetMaterial(material, lastMaterial, resetAllRenderstates, services);
//	}
//
//	bool CD3D11NormalMapRenderer::OnRender(IMaterialRendererServices* service, E_VERTEX_TYPE vtxtype)
//	{
//		if (vtxtype != video::EVT_TANGENTS)
//		{
//			os::Printer::log("Error: Normal map renderer only supports vertices of type EVT_TANGENTS", ELL_ERROR);
//			return false;
//		}
//
//		// Set matrices
//		core::matrix4 worldInverseMatrix;
//		Driver->getTransform(video::ETS_WORLD).getInverse(worldInverseMatrix);
//		WorldMatrix->SetMatrix((float*)Driver->getTransform(video::ETS_WORLD).pointer());
//		//WorldMatrix->SetMatrix((float*)worldInverseMatrix.pointer());
//
//		core::matrix4 mat = Driver->getTransform(video::ETS_PROJECTION);
//		mat *= Driver->getTransform(video::ETS_VIEW);
//		mat *= Driver->getTransform(video::ETS_WORLD);
//		WorldViewProjMatrix->SetMatrix((float*)mat.pointer());
//
//		// here we've got to fetch the fixed function lights from the
//		// driver and set them as constants
//		u32 cnt = Driver->getDynamicLightCount();
//		if(cnt >= 2)
//		{
//			SLight light = Driver->getDynamicLight(0);	
//			light.DiffuseColor.a = 1.0f/(light.Radius*light.Radius); // set attenuation
//
//			LightPos1->SetFloatVector(reinterpret_cast<float*>(&light.Position.X));
//			LightColor1->SetFloatVector(reinterpret_cast<float*>(&light.DiffuseColor));
//			LightAtten1->SetFloat(light.DiffuseColor.a);
//
//			light = Driver->getDynamicLight(1);	
//			light.DiffuseColor.a = 1.0f/(light.Radius*light.Radius); // set attenuation
//			LightPos2->SetFloatVector(reinterpret_cast<float*>(&light.Position.X));
//			LightColor2->SetFloatVector(reinterpret_cast<float*>(&light.DiffuseColor));
//			LightAtten2->SetFloat(light.DiffuseColor.a);
//		}
//		else
//		{
//			SLight light;
//			light.DiffuseColor.set(0,0,0); // make light dark
//			light.Radius = 1.0f;
//
//			LightPos1->SetFloatVector(reinterpret_cast<float*>(&light.Position.X));
//			LightColor1->SetFloatVector(reinterpret_cast<float*>(&light.DiffuseColor));
//			LightAtten1->SetFloat(light.DiffuseColor.a);
//			LightPos2->SetFloatVector(reinterpret_cast<float*>(&light.Position.X));
//			LightColor2->SetFloatVector(reinterpret_cast<float*>(&light.DiffuseColor));
//			LightAtten2->SetFloat(light.DiffuseColor.a);
//		}
//
//		// Apply effect
//		Technique->GetPassByIndex(0)->Apply(0, ImmediateContext );
//
//		return true;
//	}
//
//	void CD3D11NormalMapRenderer::OnUnsetMaterial()
//	{
//		BaseMaterial->OnUnsetMaterial();
//	}
//
//	//! Returns the render capability of the material.
//	s32 CD3D11NormalMapRenderer::getRenderCapability() const
//	{
//		return 0;
//	}
//
//	//! get shader signature
//	void* CD3D11NormalMapRenderer::getShaderByteCode() const
//	{
//		return PassDescription.pIAInputSignature;
//	}
//
//	//! get shader signature size
//	u32 CD3D11NormalMapRenderer::getShaderByteCodeSize() const
//	{
//		return PassDescription.IAInputSignatureSize;
//	}
//
//	bool CD3D11NormalMapRenderer::isTransparent() const
//	{
//		return BaseMaterial->isTransparent();
//	}
//}
//}
//
#endif
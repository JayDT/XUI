// Copyright (C) 2002-2009 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_D3D11_NORMAL_MAPMATERIAL_RENDERER_H_INCLUDED__
#define __C_D3D11_NORMAL_MAPMATERIAL_RENDERER_H_INCLUDED__

//#include "IrrCompileConfig.h"
#ifdef _IRR_WINDOWS_

#ifdef _IRR_COMPILE_WITH_DIRECT3D_11_
//#define WIN32_LEAN_AND_MEAN
//#include <d3d11_1.h>
////#include <Effects11\pchfx.h>
//
//#include "CD3D11MaterialRenderer.h"
//#include "IShaderConstantSetCallBack.h"
//#include "CD3D11Driver.h"
//#include "CD3D11Texture.h"
//
//namespace irr
//{
//namespace video
//{
//
////! Renderer for normal maps
//class CD3D11NormalMapRenderer : public CD3D11MaterialRenderer
//{
//public:
//
//	CD3D11NormalMapRenderer(
//		ID3D11Device* d3dDev, video::IVideoDriver* driver, io::IFileSystem* fileSystem,
//		s32& outMaterialTypeNr, IMaterialRenderer* baseMaterial);
//
//	~CD3D11NormalMapRenderer();
//
//	//! sets a variable in the shader.
//	//! \param name: Name of the variable
//	//! \param floats: Pointer to array of floats
//	//! \param count: Amount of floats in array.
//	virtual bool setVariable(const c8* name, const f32* floats, int count);
//	
//	virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
//						bool resetAllRenderstates, IMaterialRendererServices* services);
//
//	virtual bool OnRender(IMaterialRendererServices* service, E_VERTEX_TYPE vtxtype);
//
//	/** Called during the IVideoDriver::setMaterial() call before the new
//	material will get the OnSetMaterial() call. */
//	virtual void OnUnsetMaterial();
//
//	//! Returns the render capability of the material.
//	virtual s32 getRenderCapability() const;
//
//	//! get shader signature
//	virtual void* getShaderByteCode() const;
//
//	//! get shader signature size
//	virtual u32 getShaderByteCodeSize() const;
//
//	virtual bool isTransparent() const;
//
//private:
//
//	// Base renderer
//	IMaterialRenderer* BaseMaterial;
//
//	// Effects and techniques
//	ID3DX11Effect* Effect;
//	ID3DX11EffectTechnique* Technique;
//	D3DX11_PASS_DESC PassDescription;
//
//	// Transformation matrix variables
//	ID3DX11EffectMatrixVariable* WorldMatrix;
//	ID3DX11EffectMatrixVariable* WorldViewProjMatrix;
//	ID3DX11EffectVectorVariable* LightPos1;
//	ID3DX11EffectVectorVariable* LightColor1;
//	ID3DX11EffectScalarVariable* LightAtten1;
//	ID3DX11EffectVectorVariable* LightPos2;
//	ID3DX11EffectVectorVariable* LightColor2;
//	ID3DX11EffectScalarVariable* LightAtten2;
//};
//
//} // end namespace video
//} // end namespace irr
//
#endif
#endif
#endif


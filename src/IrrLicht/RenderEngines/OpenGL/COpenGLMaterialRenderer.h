// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __C_OPENGL_MATERIAL_RENDERER_H_INCLUDED__
#define __C_OPENGL_MATERIAL_RENDERER_H_INCLUDED__

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_OPENGL_

#include "COpenGLDriver.h"
#include "IMaterialRenderer.h"

namespace irr
{
namespace video
{

//! Base class for all internal OpenGL material renderers
class COpenGLMaterialRenderer : public IMaterialRenderer
{
public:

	//! Constructor
	COpenGLMaterialRenderer(video::COpenGLDriver* driver) : Driver(driver)
	{
	}

protected:

	video::COpenGLDriver* Driver;
};

//! Solid material renderer
class COpenGLMaterialRenderer_RAW : public COpenGLMaterialRenderer
{
public:

    COpenGLMaterialRenderer_RAW(video::COpenGLDriver* d)
        : COpenGLMaterialRenderer(d) {}

    virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
        bool resetAllRenderstates, IMaterialRendererServices* services)
    {
        Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);
    }
};

//! Solid material renderer
class COpenGLMaterialRenderer_SOLID : public COpenGLMaterialRenderer
{
public:

	COpenGLMaterialRenderer_SOLID(video::COpenGLDriver* d)
		: COpenGLMaterialRenderer(d) {}

	virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
		bool resetAllRenderstates, IMaterialRendererServices* services)
	{
		//Driver->disableTextures(1);
		Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

		//if (resetAllRenderstates || (material.MaterialType != lastMaterial.MaterialType))
		//{
		//	// thanks to Murphy, the following line removed some
		//	// bugs with several OpenGL implementations.
		//	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		//}
	}
};


//! Generic Texture Blend
class COpenGLMaterialRenderer_ONETEXTURE_BLEND : public COpenGLMaterialRenderer
{
    bool transparent : 1;

public:

	COpenGLMaterialRenderer_ONETEXTURE_BLEND(video::COpenGLDriver* d)
		: COpenGLMaterialRenderer(d), transparent(true) {}

	virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
		bool resetAllRenderstates, IMaterialRendererServices* services)
	{
		//Driver->disableTextures(1);
		Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

		if (material.MaterialType != lastMaterial.MaterialType ||
			material.uMaterialTypeParam != lastMaterial.uMaterialTypeParam ||
			resetAllRenderstates)
		{
            E_BLEND_FACTOR srcFact, dstFact, srcFactAlpha, dstFactAlpha;
            E_BLEND_OPERATION blendOp, blendOpAlpha;
            E_MODULATE_FUNC modulate;
            u32 alphaSource;
            unpack_textureBlendFunc(srcFact, dstFact, modulate, alphaSource, material.uMaterialTypeParam, &srcFactAlpha, &dstFactAlpha, &blendOp, &blendOpAlpha);

            if (srcFact == EBF_SRC_COLOR && dstFact == EBF_ZERO)
            {
                transparent = false;

                Driver->GetStateCache()->setEnabled(OGLStateEntries::OGL_BLEND, false);
                Driver->GetStateCache()->setEnabled(OGLStateEntries::OGL_ALPHA_TEST, false);
            }
            else
            {
                transparent = true;

                Driver->GetStateCache()->setEnabled(OGLStateEntries::OGL_BLEND, true);
                Driver->GetStateCache()->setEnabled(OGLStateEntries::OGL_ALPHA_TEST, true);
                Driver->GetStateCache()->setBlendEquation(Driver->getOGLBlendOp((E_BLEND_OPERATION)blendOp), Driver->getOGLBlendOp((E_BLEND_OPERATION)blendOpAlpha));
                Driver->GetStateCache()->setBlendFunc(Driver->getGLBlend(srcFact), Driver->getGLBlend(dstFact), Driver->getGLBlend(srcFactAlpha), Driver->getGLBlend(dstFactAlpha));
            }
		}
	}

	virtual void OnUnsetMaterial()
	{
	}

 	//! Returns if the material is transparent.
 	/** Is not always transparent, but mostly. */
 	virtual bool isTransparent() const
 	{
 		return transparent;
 	}
};


//! Solid 2 layer material renderer
class COpenGLMaterialRenderer_SOLID_2_LAYER : public COpenGLMaterialRenderer
{
public:

	COpenGLMaterialRenderer_SOLID_2_LAYER(video::COpenGLDriver* d)
		: COpenGLMaterialRenderer(d) {}

	virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
		bool resetAllRenderstates, IMaterialRendererServices* services)
	{
		//Driver->disableTextures(2);
		Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);
	}

	virtual void OnUnsetMaterial()
	{
	}
};


//! Transparent add color material renderer
class COpenGLMaterialRenderer_TRANSPARENT_ADD_COLOR : public COpenGLMaterialRenderer
{
public:

	COpenGLMaterialRenderer_TRANSPARENT_ADD_COLOR(video::COpenGLDriver* d)
		: COpenGLMaterialRenderer(d) {}

	virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
		bool resetAllRenderstates, IMaterialRendererServices* services)
	{
		//Driver->disableTextures(1);
		Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

		if ((material.MaterialType != lastMaterial.MaterialType) || resetAllRenderstates)
		{
            Driver->GetStateCache()->setEnabled(OGLStateEntries::OGL_BLEND, true);
            Driver->GetStateCache()->setBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_COLOR, GL_ZERO, GL_ZERO);
		}
	}

	virtual void OnUnsetMaterial()
	{
	}

	//! Returns if the material is transparent.
	virtual bool isTransparent() const
	{
		return true;
	}
};


//! Transparent vertex alpha material renderer
class COpenGLMaterialRenderer_TRANSPARENT_VERTEX_ALPHA : public COpenGLMaterialRenderer
{
public:

	COpenGLMaterialRenderer_TRANSPARENT_VERTEX_ALPHA(video::COpenGLDriver* d)
		: COpenGLMaterialRenderer(d) {}

	virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
		bool resetAllRenderstates, IMaterialRendererServices* services)
	{
		//Driver->disableTextures(1);
		Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

		if (material.MaterialType != lastMaterial.MaterialType || resetAllRenderstates)
		{
            Driver->GetStateCache()->setEnabled(OGLStateEntries::OGL_BLEND, true);
            Driver->GetStateCache()->setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
	}

	virtual void OnUnsetMaterial()
	{
	}

	//! Returns if the material is transparent.
	virtual bool isTransparent() const
	{
		return true;
	}
};


//! Transparent alpha channel material renderer
class COpenGLMaterialRenderer_TRANSPARENT_ALPHA_CHANNEL : public COpenGLMaterialRenderer
{
public:

	COpenGLMaterialRenderer_TRANSPARENT_ALPHA_CHANNEL(video::COpenGLDriver* d)
		: COpenGLMaterialRenderer(d) {}

	virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
		bool resetAllRenderstates, IMaterialRendererServices* services)
	{
		//Driver->disableTextures(1);
		Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

		if (material.MaterialType != lastMaterial.MaterialType || resetAllRenderstates
			|| material.uMaterialTypeParam != lastMaterial.uMaterialTypeParam )
		{
            Driver->GetStateCache()->setEnabled(OGLStateEntries::OGL_BLEND, true);
            Driver->GetStateCache()->setEnabled(OGLStateEntries::OGL_ALPHA_TEST, true);
            Driver->GetStateCache()->setBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
	}

	virtual void OnUnsetMaterial()
	{
	}

	//! Returns if he material is transparent.
	virtual bool isTransparent() const
	{
		return true;
	}
};


//! Transparent alpha channel material renderer
class COpenGLMaterialRenderer_TRANSPARENT_ALPHA_CHANNEL_REF : public COpenGLMaterialRenderer
{
public:

	COpenGLMaterialRenderer_TRANSPARENT_ALPHA_CHANNEL_REF(video::COpenGLDriver* d)
		: COpenGLMaterialRenderer(d) {}

	virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
		bool resetAllRenderstates, IMaterialRendererServices* services)
	{
		//Driver->disableTextures(1);
		Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

		if (material.MaterialType != lastMaterial.MaterialType || resetAllRenderstates)
		{
            Driver->GetStateCache()->setEnabled(OGLStateEntries::OGL_BLEND, false);
            Driver->GetStateCache()->setEnabled(OGLStateEntries::OGL_ALPHA_TEST, true);

			//glEnable(GL_ALPHA_TEST);
			//glAlphaFunc(GL_GREATER, 0.5f);
			//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		}
	}

	virtual void OnUnsetMaterial()
	{
	}

	//! Returns if the material is transparent.
	virtual bool isTransparent() const
	{
		return false;  // this material is not really transparent because it does no blending.
	}
};


//! material renderer for all kinds of lightmaps
class COpenGLMaterialRenderer_LIGHTMAP : public COpenGLMaterialRenderer
{
public:

	COpenGLMaterialRenderer_LIGHTMAP(video::COpenGLDriver* d)
		: COpenGLMaterialRenderer(d) {}

	virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
		bool resetAllRenderstates, IMaterialRendererServices* services)
	{
		//Driver->disableTextures(2);
		Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);
	}

	virtual void OnUnsetMaterial()
	{
	}
};



//! detail map  material renderer
class COpenGLMaterialRenderer_DETAIL_MAP : public COpenGLMaterialRenderer
{
public:

	COpenGLMaterialRenderer_DETAIL_MAP(video::COpenGLDriver* d)
		: COpenGLMaterialRenderer(d) {}

	virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
		bool resetAllRenderstates, IMaterialRendererServices* services)
	{
		//Driver->disableTextures(2);
		Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);
	}

	virtual void OnUnsetMaterial()
	{
	}
};


//! sphere map material renderer
class COpenGLMaterialRenderer_SPHERE_MAP : public COpenGLMaterialRenderer
{
public:

	COpenGLMaterialRenderer_SPHERE_MAP(video::COpenGLDriver* d)
		: COpenGLMaterialRenderer(d) {}

	virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
		bool resetAllRenderstates, IMaterialRendererServices* services)
	{
		//Driver->disableTextures(1);
		// texture needs to be flipped for OpenGL
		core::matrix4 tmp = Driver->getTransform(ETS_TEXTURE_0);
		tmp[5]*=-1;
		Driver->setTransform(ETS_TEXTURE_0, tmp);
		Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

		//if (material.MaterialType != lastMaterial.MaterialType || resetAllRenderstates)
		//{
		//	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
		//	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
        //
		//	glEnable(GL_TEXTURE_GEN_S);
		//	glEnable(GL_TEXTURE_GEN_T);
		//}
	}

	virtual void OnUnsetMaterial()
	{
		//glDisable(GL_TEXTURE_GEN_S);
		//glDisable(GL_TEXTURE_GEN_T);
	}
};


//! reflection 2 layer material renderer
class COpenGLMaterialRenderer_REFLECTION_2_LAYER : public COpenGLMaterialRenderer
{
public:

	COpenGLMaterialRenderer_REFLECTION_2_LAYER(video::COpenGLDriver* d)
		: COpenGLMaterialRenderer(d) {}

	virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
		bool resetAllRenderstates, IMaterialRendererServices* services)
	{
		//Driver->disableTextures(2);
		Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

//		if (material.MaterialType != lastMaterial.MaterialType || resetAllRenderstates)
//		{
//			if (Driver->queryFeature(EVDF_MULTITEXTURE))
//			{
//				Driver->extGlActiveTexture(GL_TEXTURE1_ARB);
//#ifdef GL_ARB_texture_env_combine
//				//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
//				//glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_MODULATE);
//				//glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
//				//glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_PREVIOUS_ARB);
//#else
//				//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
//				//glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_MODULATE);
//				//glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_TEXTURE);
//				//glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_PREVIOUS_EXT);
//#endif
//			}
//			glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
//			glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
//			glEnable(GL_TEXTURE_GEN_S);
//			glEnable(GL_TEXTURE_GEN_T);
//		}
	}

	virtual void OnUnsetMaterial()
	{
		//if (Driver->queryFeature(EVDF_MULTITEXTURE))
		//{
		//	Driver->extGlActiveTexture(GL_TEXTURE1_ARB);
		//	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		//}
		//glDisable(GL_TEXTURE_GEN_S);
		//glDisable(GL_TEXTURE_GEN_T);
		//if (Driver->queryFeature(EVDF_MULTITEXTURE))
		//{
		//	Driver->extGlActiveTexture(GL_TEXTURE0_ARB);
		//}
	}
};


//! reflection 2 layer material renderer
class COpenGLMaterialRenderer_TRANSPARENT_REFLECTION_2_LAYER : public COpenGLMaterialRenderer
{
public:

	COpenGLMaterialRenderer_TRANSPARENT_REFLECTION_2_LAYER(video::COpenGLDriver* d)
		: COpenGLMaterialRenderer(d) {}

	virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
		bool resetAllRenderstates, IMaterialRendererServices* services)
	{
		//Driver->disableTextures(2);
		Driver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

//		if (material.MaterialType != lastMaterial.MaterialType || resetAllRenderstates)
//		{
//#ifdef GL_ARB_texture_env_combine
//				//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
//				//glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_MODULATE);
//				//glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
//				//glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_PREVIOUS_ARB);
//				//glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_REPLACE);
//				//glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, GL_PREVIOUS_ARB);
//#else
//				//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
//				//glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_MODULATE);
//				//glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_TEXTURE);
//				//glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_PREVIOUS_EXT);
//				//glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_EXT, GL_REPLACE);
//				//glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_EXT, GL_PREVIOUS_ARB);
//#endif
//			if (Driver->queryFeature(EVDF_MULTITEXTURE))
//			{
//				Driver->extGlActiveTexture(GL_TEXTURE1_ARB);
//#ifdef GL_ARB_texture_env_combine
//				//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_ARB);
//				//glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_ARB, GL_MODULATE);
//				//glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_ARB, GL_TEXTURE);
//				//glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_ARB, GL_PREVIOUS_ARB);
//				//glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_ARB, GL_REPLACE);
//				//glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_ARB, GL_PREVIOUS_ARB);
//#else
//				//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE_EXT);
//				//glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB_EXT, GL_MODULATE);
//				//glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_RGB_EXT, GL_TEXTURE);
//				//glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE1_RGB_EXT, GL_PREVIOUS_EXT);
//				//glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_ALPHA_EXT, GL_REPLACE);
//				//glTexEnvi(GL_TEXTURE_ENV, GL_SOURCE0_ALPHA_EXT, GL_PREVIOUS_ARB);
//#endif
//			}
//			glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
//			glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
//			glEnable(GL_TEXTURE_GEN_S);
//			glEnable(GL_TEXTURE_GEN_T);
//
//			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//			glEnable(GL_BLEND);
//		}
	}

	virtual void OnUnsetMaterial()
	{
		//if (Driver->queryFeature(EVDF_MULTITEXTURE))
		//{
		//	Driver->extGlActiveTexture(GL_TEXTURE1_ARB);
		//	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		//}
		//glDisable(GL_TEXTURE_GEN_S);
		//glDisable(GL_TEXTURE_GEN_T);
		//if (Driver->queryFeature(EVDF_MULTITEXTURE))
		//{
		//	Driver->extGlActiveTexture(GL_TEXTURE0_ARB);
		//}
		//glDisable(GL_BLEND);
	}

	//! Returns if the material is transparent.
	virtual bool isTransparent() const
	{
		return true;
	}
};

} // end namespace video
} // end namespace irr

#endif
#endif


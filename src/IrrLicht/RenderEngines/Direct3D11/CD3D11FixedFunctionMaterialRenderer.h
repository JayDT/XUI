
#ifndef __C_D3D11_FIXED_FUNCTION_MATERIAL_RENDERER_H_INCLUDED__
#define __C_D3D11_FIXED_FUNCTION_MATERIAL_RENDERER_H_INCLUDED__

#include "IrrCompileConfig.h"
#ifdef _IRR_WINDOWS_
#define WIN32_LEAN_AND_MEAN
#endif

#ifdef _IRR_COMPILE_WITH_DIRECT3D_11_

#include "CD3D11Driver.h"
#include "CD3D11MaterialRenderer.h"

namespace irr
{
namespace video
{

typedef irr::core::vector3df float3;
typedef irr::video::SColorf  float4;

#define MAX_LIGHTS    4

class CD3D11FixedFunctionMaterialRenderer : public CD3D11MaterialRenderer
{

public:
    //! Constructor for fixed function effect
    CD3D11FixedFunctionMaterialRenderer(IVideoDriver* driver, io::IFileSystem* fileSystem);

    ~CD3D11FixedFunctionMaterialRenderer()
    {
        //SAFE_RELEASE(Effect);
    }

    virtual bool OnRender(IMaterialRendererServices* service, E_VERTEX_TYPE vtxtype);

    /** Called during the IVideoDriver::setMaterial() call before the new
    material will get the OnSetMaterial() call. */
    virtual void OnUnsetMaterial() { }

    /** Called by other renderers to set some variables*/
    virtual bool setVariable(const c8* name, const f32* floats, int count)
    {
        return false;
    }

    //! get shader signature
    virtual void* getShaderByteCode() const
    {
        //D3DX11_PASS_DESC desc;
        //if (CurrentPass)
        //{
        //    CurrentPass->GetDesc( &desc );
        //    return desc.pIAInputSignature;
        //}
    
        return 0;
    }

    //! get shader signature size
    virtual u32 getShaderByteCodeSize() const
    {
        //D3DX11_PASS_DESC desc;
        //if (CurrentPass)
        //{
        //    CurrentPass->GetDesc( &desc );
        //    return desc.IAInputSignatureSize;
        //}
    
        return 0;
    }

    virtual bool isTransparent() const { return false; }

    irr::video::D3D11HLSLProgram* GetDefaultShader() const { return m_defaultShader; }

protected:
    E_VERTEX_TYPE CurrentVertexType;

    irr::video::D3D11HLSLProgram* m_defaultShader;

    ID3D11Buffer*       m_matrixBuffer;
    ID3D11Buffer*       m_pixelConstantBuffer;

    bool init(io::IFileSystem* fileSystem);
};

class CD3D11MaterialRenderer_SOLID : public CD3D11FixedFunctionMaterialRenderer
{
public:
    CD3D11MaterialRenderer_SOLID(IVideoDriver* driver, io::IFileSystem* fSystem)
    : CD3D11FixedFunctionMaterialRenderer(driver, fSystem) {}

    virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
        bool resetAllRenderstates, IMaterialRendererServices* services);
    
};

class CD3D11MaterialRenderer_RAW : public CD3D11FixedFunctionMaterialRenderer
{
    public:
    CD3D11MaterialRenderer_RAW(IVideoDriver* driver, io::IFileSystem* fSystem)
        : CD3D11FixedFunctionMaterialRenderer(driver, fSystem)
    {
    }

    virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
        bool resetAllRenderstates, IMaterialRendererServices* services);

};


//! Generic Texture Blend
class CD3D11MaterialRenderer_ONETEXTURE_BLEND : public CD3D11FixedFunctionMaterialRenderer
{
    public:
    CD3D11MaterialRenderer_ONETEXTURE_BLEND(IVideoDriver* driver, io::IFileSystem* fSystem)
        : CD3D11FixedFunctionMaterialRenderer(driver, fSystem), transparent(true)
    {
    }

    virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
        bool resetAllRenderstates, IMaterialRendererServices* services);

    virtual void OnUnsetMaterial();

    //! Returns if the material is transparent.
    /** The scene management needs to know this for being able to sort the
    materials by opaque and transparent.
    The return value could be optimized, but we'd need to know the
    MaterialTypeParam for it. */
    virtual bool isTransparent() const
    {
        return transparent;
    }

    private:

    static D3D11_BLEND getD3DBlend(E_BLEND_FACTOR factor)
    {
        D3D11_BLEND r = D3D11_BLEND_ZERO;
        switch ( factor )
        {
            case EBF_ZERO:					r = D3D11_BLEND_ZERO; break;
            case EBF_ONE:					r = D3D11_BLEND_ONE; break;
            case EBF_DST_COLOR:				r = D3D11_BLEND_DEST_COLOR; break;
            case EBF_ONE_MINUS_DST_COLOR:	r = D3D11_BLEND_INV_DEST_COLOR; break;
            case EBF_SRC_COLOR:				r = D3D11_BLEND_SRC_COLOR; break;
            case EBF_ONE_MINUS_SRC_COLOR:	r = D3D11_BLEND_INV_SRC_COLOR; break;
            case EBF_SRC_ALPHA:				r = D3D11_BLEND_SRC_ALPHA; break;
            case EBF_ONE_MINUS_SRC_ALPHA:	r = D3D11_BLEND_INV_SRC_ALPHA; break;
            case EBF_DST_ALPHA:				r = D3D11_BLEND_DEST_ALPHA; break;
            case EBF_ONE_MINUS_DST_ALPHA:	r = D3D11_BLEND_INV_DEST_ALPHA; break;
            case EBF_SRC_ALPHA_SATURATE:	r = D3D11_BLEND_SRC_ALPHA_SAT; break;
        }
        return r;
    }

	static D3D11_BLEND_OP getD3DBlendOp(E_BLEND_OPERATION operation)
    {
		switch (operation)
		{
			case EBO_NONE:			return D3D11_BLEND_OP_ADD;
			case EBO_ADD:			return D3D11_BLEND_OP_ADD;
			case EBO_SUBTRACT:		return D3D11_BLEND_OP_SUBTRACT;
			case EBO_REVSUBTRACT:	return D3D11_BLEND_OP_REV_SUBTRACT;
			case EBO_MIN:			return D3D11_BLEND_OP_MIN;
			case EBO_MAX:			return D3D11_BLEND_OP_MAX;
			case EBO_MIN_FACTOR:	return D3D11_BLEND_OP_MIN;
			case EBO_MAX_FACTOR:	return D3D11_BLEND_OP_MAX;
			case EBO_MIN_ALPHA:		return D3D11_BLEND_OP_MIN;
			case EBO_MAX_ALPHA:		return D3D11_BLEND_OP_MAX;
		}

		return D3D11_BLEND_OP_ADD;
    }

    bool transparent : 1;

};

class CD3D11MaterialRenderer_SOLID_2_LAYER : public CD3D11FixedFunctionMaterialRenderer
{
public:
    CD3D11MaterialRenderer_SOLID_2_LAYER(IVideoDriver* driver, io::IFileSystem* fSystem);

    virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
        bool resetAllRenderstates, IMaterialRendererServices* services);
    
};

class CD3D11MaterialRenderer_LIGHTMAP : public CD3D11FixedFunctionMaterialRenderer
{
public:
    CD3D11MaterialRenderer_LIGHTMAP(IVideoDriver* driver, io::IFileSystem* fSystem);

    virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
        bool resetAllRenderstates, IMaterialRendererServices* services);
    
};
    
class CD3D11MaterialRenderer_DETAIL_MAP : public CD3D11FixedFunctionMaterialRenderer
{
public:
    CD3D11MaterialRenderer_DETAIL_MAP(IVideoDriver* driver, io::IFileSystem* fSystem)
    : CD3D11FixedFunctionMaterialRenderer(driver, fSystem) {}

    virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
        bool resetAllRenderstates, IMaterialRendererServices* services);
    
};

class CD3D11MaterialRenderer_SPHERE_MAP : public CD3D11FixedFunctionMaterialRenderer
{
public:
    CD3D11MaterialRenderer_SPHERE_MAP(IVideoDriver* driver, io::IFileSystem* fSystem)
    : CD3D11FixedFunctionMaterialRenderer(driver, fSystem) {}

    virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
        bool resetAllRenderstates, IMaterialRendererServices* services);

    virtual void OnUnsetMaterial();
};

class CD3D11MaterialRenderer_REFLECTION_2_LAYER : public CD3D11FixedFunctionMaterialRenderer
{
public:
    CD3D11MaterialRenderer_REFLECTION_2_LAYER(IVideoDriver* driver, io::IFileSystem* fSystem)
    : CD3D11FixedFunctionMaterialRenderer(driver, fSystem) {}

    virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
        bool resetAllRenderstates, IMaterialRendererServices* services);

    virtual void OnUnsetMaterial();
};

class CD3D11MaterialRenderer_TRANSPARENT_ADD_COLOR : public CD3D11FixedFunctionMaterialRenderer
{
public:
    CD3D11MaterialRenderer_TRANSPARENT_ADD_COLOR(IVideoDriver* driver, io::IFileSystem* fSystem)
    : CD3D11FixedFunctionMaterialRenderer(driver, fSystem) {}

    virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
        bool resetAllRenderstates, IMaterialRendererServices* services);

    virtual void OnUnsetMaterial();

    virtual bool isTransparent() const { return true; }    
};

class CD3D11MaterialRenderer_TRANSPARENT_ALPHA_CHANNEL : public CD3D11FixedFunctionMaterialRenderer
{
public:
    CD3D11MaterialRenderer_TRANSPARENT_ALPHA_CHANNEL(IVideoDriver* driver, io::IFileSystem* fSystem)
    : CD3D11FixedFunctionMaterialRenderer(driver, fSystem) {}

    virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
        bool resetAllRenderstates, IMaterialRendererServices* services);

    virtual void OnUnsetMaterial();

    //! Returns if the material is transparent. The scene managment needs to know this
    //! for being able to sort the materials by opaque and transparent.
    virtual bool isTransparent() const
    {
        return true;
    }
};

class CD3D11MaterialRenderer_TRANSPARENT_ALPHA_CHANNEL_REF : public CD3D11FixedFunctionMaterialRenderer
{
public:
    CD3D11MaterialRenderer_TRANSPARENT_ALPHA_CHANNEL_REF(IVideoDriver* driver, io::IFileSystem* fSystem)
    : CD3D11FixedFunctionMaterialRenderer(driver, fSystem) {}

    virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
        bool resetAllRenderstates, IMaterialRendererServices* services);

    //! Returns if the material is transparent. The scene managment needs to know this
    //! for being able to sort the materials by opaque and transparent.
    virtual bool isTransparent() const
    {
        return true; // this material is not really transparent because it does no blending.
    }
};

class CD3D11MaterialRenderer_TRANSPARENT_VERTEX_ALPHA : public CD3D11FixedFunctionMaterialRenderer
{
public:
    CD3D11MaterialRenderer_TRANSPARENT_VERTEX_ALPHA(IVideoDriver* driver, io::IFileSystem* fSystem)
    : CD3D11FixedFunctionMaterialRenderer(driver, fSystem) {}

    virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
        bool resetAllRenderstates, IMaterialRendererServices* services);

    virtual void OnUnsetMaterial();

    //! Returns if the material is transparent. The scene managment needs to know this
    //! for being able to sort the materials by opaque and transparent.
    virtual bool isTransparent() const
    {
        return true;
    }
};

class CD3D11MaterialRenderer_TRANSPARENT_REFLECTION_2_LAYER : public CD3D11FixedFunctionMaterialRenderer
{
public:
    CD3D11MaterialRenderer_TRANSPARENT_REFLECTION_2_LAYER(IVideoDriver* driver, io::IFileSystem* fSystem)
    : CD3D11FixedFunctionMaterialRenderer(driver, fSystem) {}

    virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
        bool resetAllRenderstates, IMaterialRendererServices* services);

    virtual void OnUnsetMaterial();

    //! Returns if the material is transparent. The scene managment needs to know this
    //! for being able to sort the materials by opaque and transparent.
    virtual bool isTransparent() const
    {
        return true;
    }
};
}
}

#endif
#endif
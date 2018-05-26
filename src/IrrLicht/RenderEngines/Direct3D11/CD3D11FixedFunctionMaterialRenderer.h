
#ifndef __C_D3D11_FIXED_FUNCTION_MATERIAL_RENDERER_H_INCLUDED__
#define __C_D3D11_FIXED_FUNCTION_MATERIAL_RENDERER_H_INCLUDED__

#include "IrrCompileConfig.h"
#ifdef _IRR_WINDOWS_
#define WIN32_LEAN_AND_MEAN
#endif

#ifdef _IRR_COMPILE_WITH_DIRECT3D_11_

#include "CD3D11Driver.h"
#include "CD3D11MaterialRenderer.h"
#include "System/Uri.h"
#include "System/Resource/ResourceManager.h"
#include "System/Resource/ResourceObject.h"
#include "standard/client/DataSource_Standard.h"

namespace irr
{
namespace video
{

    DirectX::XMMATRIX UnitMatrixD3D11;
    DirectX::XMMATRIX SphereMapMatrixD3D11;

typedef irr::core::vector3df float3;
typedef irr::video::SColorf  float4;

#define MAX_LIGHTS    4

class CD3D11FixedFunctionMaterialRenderer : public CD3D11MaterialRenderer
{

public:
    //! Constructor for fixed function effect
    CD3D11FixedFunctionMaterialRenderer(IVideoDriver* driver, io::IFileSystem* fileSystem)
    : CD3D11MaterialRenderer(static_cast<CD3D11Driver*>(driver))
    , CurrentVertexType(EVT_STANDARD), m_matrixBuffer(nullptr), m_pixelConstantBuffer(nullptr)
    {
        IMaterialRenderer* solidRend = Driver->getMaterialRenderer(EMT_SOLID);
        if (solidRend)
        {
            m_GUIShader = static_cast<CD3D11FixedFunctionMaterialRenderer*>(solidRend)->m_GUIShader;
            //Effect->AddRef();
        }
        else
        {
            if (!init(fileSystem))
                return;
        }
    }

    ~CD3D11FixedFunctionMaterialRenderer()
    {
        //SAFE_RELEASE(Effect);
    }

    virtual bool OnRender(IMaterialRendererServices* service, E_VERTEX_TYPE vtxtype)
    {
        if (!m_GUIShader)
            return false;

        // DX 11 needs a renderer, so configure solid
        Driver->activeGPUProgram(m_GUIShader);
        
        m_GUIShader->UpdateValues(video::IShaderDataBuffer::EUT_PER_FRAME_PER_MESH, (scene::IMeshBuffer*)this, nullptr, nullptr, video::IShaderDataBuffer::EUF_COMMIT_VALUES | video::IShaderDataBuffer::EUF_CUSTOM_INPUT_VALUES);
        //m_GUIShader->UpdateValues(video::IShaderDataBuffer::EUT_PER_FRAME_PER_MESH, (scene::IMeshBuffer*)this, nullptr, nullptr, video::IShaderDataBuffer::EUF_COMMIT_VALUES);
        Driver->FinalizeShaderConstant();
        return true;
    }

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

protected:
    E_VERTEX_TYPE CurrentVertexType;

    ShaderHLSLD11* m_GUIShader;

    ID3D11Buffer*       m_matrixBuffer;
    ID3D11Buffer*       m_pixelConstantBuffer;

    struct ShaderGenericValuesBuffer : public video::IShaderDataBuffer
    {
        __declspec(align(16)) struct MatrixBufferType
        {
            core::matrix4 world;
            core::matrix4 view;
            core::matrix4 projection;
        };

        __declspec(align(16)) struct Light
        {
            DirectX::XMVECTOR Position;
            DirectX::XMVECTOR Atten;
            float4 Diffuse;
            float4 Specular;
            float4 Ambient;

            inline bool operator==(const Light& other) const
            {
                return memcmp(this, &other, sizeof(Light)) == 0;
            }

            inline bool operator!=(const Light& other) const
            {
                return memcmp(this, &other, sizeof(Light)) != 0;
            }
        };

        __declspec(align(16)) struct Material
        {
            float4    Ambient;
            float4    Diffuse;
            float4    Specular;
            float4    Emissive;
            float     Shininess;
            int       Type;

            inline bool operator==(const Material& other) const
            {
                return memcmp(this, &other, sizeof(Material)) == 0;
            }

            inline bool operator!=(const Material& other) const
            {
                return memcmp(this, &other, sizeof(Material)) != 0;
            }
        };

        __declspec(align(16)) struct PixelConstantBufferType
        {
            int         nTexture;
            int         AlphaTest;
            int         LightCount; // Number of lights enabled
            float       AlphaRef;
            Material    material;
            Light       lights[MAX_LIGHTS];
        };

        video::ShaderDataBufferElementObject<core::matrix4>* world;
        video::ShaderDataBufferElementObject<core::matrix4>* view;
        video::ShaderDataBufferElementObject<core::matrix4>* projection;

        video::ShaderDataBufferElementObject<int>* nTexture;
        video::ShaderDataBufferElementObject<int>* AlphaTest;
        video::ShaderDataBufferElementObject<int>* LightCount;
        video::ShaderDataBufferElementObject<float>* AlphaRef;
        video::ShaderDataBufferElementObject<ShaderGenericValuesBuffer::Material>* ShaderMaterial;
        video::ShaderDataBufferElementArray<ShaderGenericValuesBuffer::Light>* ShaderLight;

        ShaderGenericValuesBuffer(video::IShaderDataBuffer::E_UPDATE_TYPE updateType) : video::IShaderDataBuffer(updateType)
        {
            world       = nullptr;
            view        = nullptr;
            projection  = nullptr;
            nTexture = nullptr;
            AlphaTest = nullptr;
            LightCount = nullptr;
            AlphaRef = nullptr;
            ShaderMaterial = nullptr;
            ShaderLight = nullptr;
        }

        virtual ~ShaderGenericValuesBuffer()
        {
        }

#define UNSAFE_ADD_DATABUFFER_ELEMENT(var, type, name) \
    var = new video::ShaderDataBufferElementObject<type>(gpuProgram->GetGPUVariableDesc(name)); \
    AddBufferElement(var);


        virtual void InitializeFormShader(video::IShader* gpuProgram, void* Descriptor)
        {
            UNSAFE_ADD_DATABUFFER_ELEMENT(world,      core::matrix4,   "worldMatrix");
            UNSAFE_ADD_DATABUFFER_ELEMENT(view,       core::matrix4,   "viewMatrix");
            UNSAFE_ADD_DATABUFFER_ELEMENT(projection, core::matrix4,   "projectionMatrix");

            UNSAFE_ADD_DATABUFFER_ELEMENT(nTexture,   int,             "g_nTexture");
            UNSAFE_ADD_DATABUFFER_ELEMENT(AlphaTest,  int,             "g_bAlphaTest");
            UNSAFE_ADD_DATABUFFER_ELEMENT(LightCount, int,             "g_iLightCount");
            UNSAFE_ADD_DATABUFFER_ELEMENT(AlphaRef,   float,           "g_fAlphaRef");

            UNSAFE_ADD_DATABUFFER_ELEMENT(ShaderMaterial, ShaderGenericValuesBuffer::Material,           "g_material");

            ShaderLight = new video::ShaderDataBufferElementArray<ShaderGenericValuesBuffer::Light>(gpuProgram->GetGPUVariableDesc("g_lights"));
            AddBufferElement(ShaderLight);
        }

#undef UNSAFE_ADD_DATABUFFER_ELEMENT

        virtual void UpdateBuffer(video::IShader* gpuProgram, scene::IMeshBuffer* meshBuffer, scene::IMesh* mesh, scene::ISceneNode* node, u32 updateFlags)
        {
            if ( !meshBuffer )
                return;

            CD3D11FixedFunctionMaterialRenderer* renderer = updateFlags & video::IShaderDataBuffer::EUF_CUSTOM_INPUT_VALUES ? (CD3D11FixedFunctionMaterialRenderer*)meshBuffer : nullptr;
            CD3D11Driver* Driver = (CD3D11Driver*)gpuProgram->getVideoDriver();
            SMaterial& CurrentMaterial = renderer ? renderer->CurrentMaterial : meshBuffer->getMaterial();

            *world       = Driver->getTransform(ETS_WORLD);
            *view        = Driver->getTransform(ETS_VIEW);
            *projection  = Driver->getTransform(ETS_PROJECTION);


            *AlphaRef = CurrentMaterial.MaterialTypeParam ? CurrentMaterial.MaterialTypeParam : 0.5f;
            *AlphaTest = Driver->BlendDesc.RenderTarget[0].BlendEnable || (renderer && renderer->isTransparent());
            int _nTexture = 0;

            for ( u8 i = 0; i < MATERIAL_MAX_TEXTURES; ++i )
                if ( Driver->CurrentTexture[i] != nullptr )
                    ++_nTexture;

            *nTexture = _nTexture;

            int n = CurrentMaterial.Lighting ? Driver->getDynamicLightCount() : 0;
            *LightCount = n;

            if ( n )
            {
                ShaderLight->ResetShaderValues();
                for ( int i = 0; i<n; i++ )
                {
                    ShaderGenericValuesBuffer::Light l;
                    ZeroMemory(&l, sizeof(ShaderGenericValuesBuffer::Light));
                    SLight dl = Driver->getDynamicLight(i);
                    dl.Position.getAs4Values((f32*)&l.Position.m128_f32);
                    dl.Attenuation.getAs4Values((f32*)&l.Atten.m128_f32);
                    l.Ambient = dl.AmbientColor;
                    l.Diffuse = dl.DiffuseColor;
                    l.Specular = dl.SpecularColor;
                    *ShaderLight += l;
                }
            }

            ShaderGenericValuesBuffer::Material material;
            ZeroMemory(&material, sizeof(ShaderGenericValuesBuffer::Material));

            material.Ambient.set((float)CurrentMaterial.AmbientColor.getAlpha() / 255.f
                , (float)CurrentMaterial.AmbientColor.getRed() / 255.f
                , (float)CurrentMaterial.AmbientColor.getGreen() / 255.f
                , (float)CurrentMaterial.AmbientColor.getBlue() / 255.f);

            material.Diffuse.set((float)CurrentMaterial.DiffuseColor.getAlpha() / 255.f
                , (float)CurrentMaterial.DiffuseColor.getRed() / 255.f
                , (float)CurrentMaterial.DiffuseColor.getGreen() / 255.f
                , (float)CurrentMaterial.DiffuseColor.getBlue() / 255.f);

            material.Emissive.set((float)CurrentMaterial.EmissiveColor.getAlpha() / 255.f
                , (float)CurrentMaterial.EmissiveColor.getRed() / 255.f
                , (float)CurrentMaterial.EmissiveColor.getGreen() / 255.f
                , (float)CurrentMaterial.EmissiveColor.getBlue() / 255.f);

            material.Shininess = CurrentMaterial.Shininess;
            material.Type = CurrentMaterial.MaterialType;

            *ShaderMaterial = material;
        }
    };

    bool init(io::IFileSystem* fileSystem)
    {
        UINT FeatureLevel = Driver->getFeatureLevel();

        auto resource = System::Resource::ResourceManager::Instance()->FindResource(System::URI("pack://application:,,,/Direct3D11;/HLSL/Default/d3d11.hlsl", true));

        m_GUIShader = new ShaderHLSLD11(Driver, EST_HIGH_LEVEL_SHADER);
        if ( m_GUIShader )
        {
            const char* ShaderModel = "vs_3_0";     // DirectX 9c
            if ( FeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
                ShaderModel = "vs_5_0";
            else if ( FeatureLevel >= D3D_FEATURE_LEVEL_10_0 )
                ShaderModel = "vs_4_0";

            const char* ShaderEntryPoint = "vs_main";

            auto stream = resource->ToMemoryStreamReader();
            m_GUIShader->createVertexShader(Device, stream, ShaderEntryPoint, ShaderModel);
            delete stream;
        }

        if ( m_GUIShader )
        {
            const char* ShaderModel = "ps_3_0";     // DirectX 9c
            if ( FeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
                ShaderModel = "ps_5_0";
            else if ( FeatureLevel >= D3D_FEATURE_LEVEL_10_0 )
                ShaderModel = "ps_4_0";

            const char* ShaderEntryPoint = "ps_main";

            auto stream = resource->ToMemoryStreamReader();
            m_GUIShader->createPixelShader(Device, stream, ShaderEntryPoint, ShaderModel);
            delete stream;
        }

        m_GUIShader->Init();

        if ( m_GUIShader )
            m_GUIShader->addDataBuffer(new CD3D11FixedFunctionMaterialRenderer::ShaderGenericValuesBuffer(video::IShaderDataBuffer::EUT_PER_FRAME_PER_MESH), nullptr);

        Driver->m_defaultShader = m_GUIShader;
        return true;
    }
};

class CD3D11MaterialRenderer_SOLID : public CD3D11FixedFunctionMaterialRenderer
{
public:
    CD3D11MaterialRenderer_SOLID(IVideoDriver* driver, io::IFileSystem* fSystem)
    : CD3D11FixedFunctionMaterialRenderer(driver, fSystem) {}

    virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
                    bool resetAllRenderstates, IMaterialRendererServices* services)
    {
        // Store current material type
        CurrentMaterial = material;

        // set basic states
        services->setBasicRenderStates( material, lastMaterial, resetAllRenderstates );
            
        D3D11_BLEND_DESC& blendDesc = Driver->getBlendDesc();
            
        blendDesc.RenderTarget[0].BlendEnable = FALSE;
    }
    
};

class CD3D11MaterialRenderer_RAW : public CD3D11FixedFunctionMaterialRenderer
{
    public:
    CD3D11MaterialRenderer_RAW(IVideoDriver* driver, io::IFileSystem* fSystem)
        : CD3D11FixedFunctionMaterialRenderer(driver, fSystem)
    {
    }

    virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
        bool resetAllRenderstates, IMaterialRendererServices* services)
    {
        // Store current material type
        CurrentMaterial = material;

        // set basic states
        services->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

        D3D11_BLEND_DESC& blendDesc = Driver->getBlendDesc();

        blendDesc.RenderTarget[0].BlendEnable = FALSE;
    }

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
        bool resetAllRenderstates, IMaterialRendererServices* services)
    {
        services->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

        //if (material.MaterialType != lastMaterial.MaterialType ||
        //	material.MaterialTypeParam != lastMaterial.MaterialTypeParam ||
        //    material.BlendOperation != lastMaterial.BlendOperation ||
        //	resetAllRenderstates)
        {

            E_BLEND_FACTOR srcFact, dstFact, srcFactAlpha, dstFactAlpha;
			E_BLEND_OPERATION blendOp, blendOpAlpha;
            E_MODULATE_FUNC modulate;
            u32 alphaSource;
            unpack_textureBlendFunc(srcFact, dstFact, modulate, alphaSource, material.MaterialTypeParam, &srcFactAlpha, &dstFactAlpha, &blendOp, &blendOpAlpha);

            D3D11_BLEND_DESC& blendDesc = Driver->getBlendDesc();

            if ( srcFact == EBF_SRC_COLOR && dstFact == EBF_ZERO )
            {
                transparent = false;
                blendDesc.RenderTarget[0].BlendEnable = FALSE;
            }
            else
            {
                transparent = true;

                blendDesc.RenderTarget[0].BlendEnable = TRUE;
                blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

                blendDesc.RenderTarget[0].SrcBlend			= getD3DBlend(srcFact); //SrcBlendAlpha
                blendDesc.RenderTarget[0].DestBlend			= getD3DBlend(dstFact);
                blendDesc.RenderTarget[0].BlendOp			= getD3DBlendOp(blendOp);
				blendDesc.RenderTarget[0].SrcBlendAlpha		= getD3DBlend(srcFactAlpha); //SrcBlendAlpha
				blendDesc.RenderTarget[0].DestBlendAlpha	= getD3DBlend(dstFactAlpha);
				blendDesc.RenderTarget[0].BlendOpAlpha		= getD3DBlendOp(blendOpAlpha);
			}
        }
    }

    virtual void OnUnsetMaterial() 
    {
        if (transparent)
        {
            Driver->ResetBlending = true;
            D3D11_BLEND_DESC& blendDesc = Driver->getBlendDesc();
            blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
            blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
			blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		}
    }

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
    CD3D11MaterialRenderer_SOLID_2_LAYER(IVideoDriver* driver, io::IFileSystem* fSystem)
    : CD3D11FixedFunctionMaterialRenderer(driver, fSystem) {}

    virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
                    bool resetAllRenderstates, IMaterialRendererServices* services)
    {
        // Store current material type
        CurrentMaterial = material;

        // set basic states
        services->setBasicRenderStates( material, lastMaterial, resetAllRenderstates );
            
        D3D11_BLEND_DESC& blendDesc = Driver->getBlendDesc();
            
        blendDesc.RenderTarget[0].BlendEnable = FALSE;
    }
    
};

class CD3D11MaterialRenderer_LIGHTMAP : public CD3D11FixedFunctionMaterialRenderer
{
public:
    CD3D11MaterialRenderer_LIGHTMAP(IVideoDriver* driver, io::IFileSystem* fSystem)
    : CD3D11FixedFunctionMaterialRenderer(driver, fSystem) {}

    virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
                    bool resetAllRenderstates, IMaterialRendererServices* services)
    {
        // Store current material type
        CurrentMaterial = material;

        // set basic states
        services->setBasicRenderStates( material, lastMaterial, resetAllRenderstates );
            
        D3D11_BLEND_DESC& blendDesc = Driver->getBlendDesc();
            
        blendDesc.RenderTarget[0].BlendEnable = FALSE;
    }
    
};
    
class CD3D11MaterialRenderer_DETAIL_MAP : public CD3D11FixedFunctionMaterialRenderer
{
public:
    CD3D11MaterialRenderer_DETAIL_MAP(IVideoDriver* driver, io::IFileSystem* fSystem)
    : CD3D11FixedFunctionMaterialRenderer(driver, fSystem) {}

    virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
                    bool resetAllRenderstates, IMaterialRendererServices* services)
    {
        // Store current material type
        CurrentMaterial = material;

        // set basic states
        services->setBasicRenderStates( material, lastMaterial, resetAllRenderstates );
            
        D3D11_BLEND_DESC& blendDesc = Driver->getBlendDesc();
        D3D11_SAMPLER_DESC* samplerDescs = Driver->getSamplerDescs();
        
        blendDesc.RenderTarget[0].BlendEnable = FALSE;
        samplerDescs[0].AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDescs[0].AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDescs[0].Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        samplerDescs[1].AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDescs[1].AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDescs[1].Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    }
    
};

class CD3D11MaterialRenderer_SPHERE_MAP : public CD3D11FixedFunctionMaterialRenderer
{
public:
    CD3D11MaterialRenderer_SPHERE_MAP(IVideoDriver* driver, io::IFileSystem* fSystem)
    : CD3D11FixedFunctionMaterialRenderer(driver, fSystem) {}

    virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
                    bool resetAllRenderstates, IMaterialRendererServices* services)
    {
        // Store current material type
        CurrentMaterial = material;

        // set basic states
        services->setBasicRenderStates( material, lastMaterial, resetAllRenderstates );
            
        D3D11_BLEND_DESC& blendDesc = Driver->getBlendDesc();
            
        blendDesc.RenderTarget[0].BlendEnable = FALSE;

        // set texture matrix
        BaseRenderer->setVariable("TextureMatrix0", reinterpret_cast<f32*>(&SphereMapMatrixD3D11), sizeof(SphereMapMatrixD3D11));
    }

    virtual void OnUnsetMaterial()
    {
        BaseRenderer->setVariable("TextureMatrix0", reinterpret_cast<f32*>(&UnitMatrixD3D11), sizeof(UnitMatrixD3D11));
    }    
};

class CD3D11MaterialRenderer_REFLECTION_2_LAYER : public CD3D11FixedFunctionMaterialRenderer
{
public:
    CD3D11MaterialRenderer_REFLECTION_2_LAYER(IVideoDriver* driver, io::IFileSystem* fSystem)
    : CD3D11FixedFunctionMaterialRenderer(driver, fSystem) {}

    virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
                    bool resetAllRenderstates, IMaterialRendererServices* services)
    {
        // Store current material type
        CurrentMaterial = material;

        // set basic states
        services->setBasicRenderStates( material, lastMaterial, resetAllRenderstates );
            
        D3D11_BLEND_DESC& blendDesc = Driver->getBlendDesc();
            
        blendDesc.RenderTarget[0].BlendEnable = FALSE;

        // set texture matrix
        BaseRenderer->setVariable("TextureMatrix0", reinterpret_cast<f32*>(&SphereMapMatrixD3D11), sizeof(SphereMapMatrixD3D11));
    }

    virtual void OnUnsetMaterial()
    {
        BaseRenderer->setVariable("TextureMatrix0", reinterpret_cast<f32*>(&UnitMatrixD3D11), sizeof(UnitMatrixD3D11));
    }
};

class CD3D11MaterialRenderer_TRANSPARENT_ADD_COLOR : public CD3D11FixedFunctionMaterialRenderer
{
public:
    CD3D11MaterialRenderer_TRANSPARENT_ADD_COLOR(IVideoDriver* driver, io::IFileSystem* fSystem)
    : CD3D11FixedFunctionMaterialRenderer(driver, fSystem) {}

    virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
                    bool resetAllRenderstates, IMaterialRendererServices* services)
    {
        // Store current material type
        CurrentMaterial = material;

        // set basic states
        services->setBasicRenderStates( material, lastMaterial, resetAllRenderstates );
            
        D3D11_BLEND_DESC& blendDesc = Driver->getBlendDesc();
            
        blendDesc.RenderTarget[0].BlendEnable = TRUE;
        blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR;
        blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    }

    virtual void OnUnsetMaterial()
    {
        Driver->ResetBlending = true;
    }

    virtual bool isTransparent() const { return true; }    
};

class CD3D11MaterialRenderer_TRANSPARENT_ALPHA_CHANNEL : public CD3D11FixedFunctionMaterialRenderer
{
public:
    CD3D11MaterialRenderer_TRANSPARENT_ALPHA_CHANNEL(IVideoDriver* driver, io::IFileSystem* fSystem)
    : CD3D11FixedFunctionMaterialRenderer(driver, fSystem) {}

    virtual void OnSetMaterial(const SMaterial& material, const SMaterial& lastMaterial,
                    bool resetAllRenderstates, IMaterialRendererServices* services)
    {
        // Store current material type
        CurrentMaterial = material;

        // set basic states
        services->setBasicRenderStates( material, lastMaterial, resetAllRenderstates );
            
        D3D11_BLEND_DESC& blendDesc = Driver->getBlendDesc();
            
        blendDesc.RenderTarget[0].BlendEnable = TRUE;
        blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    }

    virtual void OnUnsetMaterial()
    {
        Driver->ResetBlending = true;
    }

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
                    bool resetAllRenderstates, IMaterialRendererServices* services)
    {
        // Store current material type
        CurrentMaterial = material;

        // set basic states
        services->setBasicRenderStates( material, lastMaterial, resetAllRenderstates );
            
        D3D11_BLEND_DESC& blendDesc = Driver->getBlendDesc();
            
        blendDesc.RenderTarget[0].BlendEnable = FALSE;
    }

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
                    bool resetAllRenderstates, IMaterialRendererServices* services)
    {
        // Store current material type
        CurrentMaterial = material;

        // set basic states
        services->setBasicRenderStates( material, lastMaterial, resetAllRenderstates );
            
        D3D11_BLEND_DESC& blendDesc = Driver->getBlendDesc();
            
        blendDesc.RenderTarget[0].BlendEnable = TRUE;
        blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
        blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR;
        blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    }

    virtual void OnUnsetMaterial()
    {
        Driver->ResetBlending = true;
    }

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
                    bool resetAllRenderstates, IMaterialRendererServices* services)
    {
        // Store current material type
        CurrentMaterial = material;

        // set basic states
        services->setBasicRenderStates( material, lastMaterial, resetAllRenderstates );
            
        D3D11_BLEND_DESC& blendDesc = Driver->getBlendDesc();
            
        blendDesc.RenderTarget[0].BlendEnable = TRUE;
        blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    }

    virtual void OnUnsetMaterial()
    {
        Driver->ResetBlending = true;
    }

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
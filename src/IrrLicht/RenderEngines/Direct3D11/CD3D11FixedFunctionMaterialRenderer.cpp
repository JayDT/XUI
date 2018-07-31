#include "CD3D11FixedFunctionMaterialRenderer.h"
#include "CD3D11Shader.h"

#include "System/Uri.h"
#include "System/Resource/ResourceManager.h"
#include "System/Resource/ResourceObject.h"
#include "standard/client/DataSource_Standard.h"

using namespace irr;
using namespace irr::video;

DirectX::XMMATRIX UnitMatrixD3D11;
DirectX::XMMATRIX SphereMapMatrixD3D11;

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

        bool operator==(const Light& other) const
        {
            return memcmp(this, &other, sizeof(Light)) == 0;
        }

        bool operator!=(const Light& other) const
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

        bool operator==(const Material& other) const
        {
            return memcmp(this, &other, sizeof(Material)) == 0;
        }

        bool operator!=(const Material& other) const
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

    ShaderGenericValuesBuffer(video::IShaderDataBuffer::E_UPDATE_TYPE updateType);

    virtual ~ShaderGenericValuesBuffer();

    virtual void InitializeFormShader(video::IShader* gpuProgram, void* Descriptor);

    virtual void UpdateBuffer(video::IShader* gpuProgram, scene::IMeshBuffer* meshBuffer, scene::IMesh* mesh, scene::ISceneNode* node, u32 updateFlags);
};

ShaderGenericValuesBuffer::ShaderGenericValuesBuffer(video::IShaderDataBuffer::E_UPDATE_TYPE updateType) : video::IShaderDataBuffer(updateType)
{
    world = nullptr;
    view = nullptr;
    projection = nullptr;
    nTexture = nullptr;
    AlphaTest = nullptr;
    LightCount = nullptr;
    AlphaRef = nullptr;
    ShaderMaterial = nullptr;
    ShaderLight = nullptr;
}

ShaderGenericValuesBuffer::~ShaderGenericValuesBuffer()
{
}

void ShaderGenericValuesBuffer::InitializeFormShader(video::IShader * gpuProgram, void * Descriptor)
{
#define UNSAFE_ADD_DATABUFFER_ELEMENT(var, type, name) \
    var = new video::ShaderDataBufferElementObject<type>(gpuProgram->GetGPUVariableDesc(name)); \
    AddBufferElement(var);
    UNSAFE_ADD_DATABUFFER_ELEMENT(world, core::matrix4, "worldMatrix");
    UNSAFE_ADD_DATABUFFER_ELEMENT(view, core::matrix4, "viewMatrix");
    UNSAFE_ADD_DATABUFFER_ELEMENT(projection, core::matrix4, "projectionMatrix");

    UNSAFE_ADD_DATABUFFER_ELEMENT(nTexture, int, "g_nTexture");
    UNSAFE_ADD_DATABUFFER_ELEMENT(AlphaTest, int, "g_bAlphaTest");
    UNSAFE_ADD_DATABUFFER_ELEMENT(LightCount, int, "g_iLightCount");
    UNSAFE_ADD_DATABUFFER_ELEMENT(AlphaRef, float, "g_fAlphaRef");

    UNSAFE_ADD_DATABUFFER_ELEMENT(ShaderMaterial, ShaderGenericValuesBuffer::Material, "g_material");

    ShaderLight = new video::ShaderDataBufferElementArray<ShaderGenericValuesBuffer::Light>(gpuProgram->GetGPUVariableDesc("g_lights"));
    AddBufferElement(ShaderLight);
#undef UNSAFE_ADD_DATABUFFER_ELEMENT
}

void ShaderGenericValuesBuffer::UpdateBuffer(video::IShader * gpuProgram, scene::IMeshBuffer * meshBuffer, scene::IMesh * mesh, scene::ISceneNode * node, u32 updateFlags)
{
    CD3D11Driver* Driver = static_cast<CD3D11Driver*>(gpuProgram->getVideoDriver());
    SMaterial& CurrentMaterial = !meshBuffer ? Driver->GetMaterial() : meshBuffer->getMaterial();
    video::IMaterialRenderer* rnd = Driver->getMaterialRenderer(CurrentMaterial.MaterialType);

    *world = Driver->getTransform(ETS_WORLD);
    *view = Driver->getTransform(ETS_VIEW);
    *projection = Driver->getTransform(ETS_PROJECTION);

    *AlphaRef = CurrentMaterial.MaterialType != EMT_ONETEXTURE_BLEND && CurrentMaterial.MaterialTypeParam ? CurrentMaterial.MaterialTypeParam : 0.5f;
    *AlphaTest = Driver->getBlendDesc().RenderTarget[0].BlendEnable || (rnd && rnd->isTransparent());
    int _nTexture = 0;

    for (u8 i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
        if (Driver->getCurrentTexture(i) != nullptr)
            ++_nTexture;

    *nTexture = _nTexture;

    int n = CurrentMaterial.Lighting ? Driver->getDynamicLightCount() : 0;
    *LightCount = n;

    if (n)
    {
        ShaderLight->ResetShaderValues();
        for (int i = 0; i<n; i++)
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


//! Constructor for fixed function effect

irr::video::CD3D11FixedFunctionMaterialRenderer::CD3D11FixedFunctionMaterialRenderer(IVideoDriver * driver, io::IFileSystem * fileSystem)
    : CD3D11MaterialRenderer(static_cast<CD3D11Driver*>(driver))
    , CurrentVertexType(EVT_STANDARD), m_matrixBuffer(nullptr), m_pixelConstantBuffer(nullptr)
{
    IMaterialRenderer* solidRend = Driver->getMaterialRenderer(EMT_SOLID);
    if (solidRend)
    {
        m_defaultShader = static_cast<CD3D11FixedFunctionMaterialRenderer*>(solidRend)->m_defaultShader;
        //Effect->AddRef();
    }
    else
    {
        if (!init(fileSystem))
            return;
    }
}

bool irr::video::CD3D11FixedFunctionMaterialRenderer::OnRender(IMaterialRendererServices * service, E_VERTEX_TYPE vtxtype)
{
    if (!m_defaultShader)
        return false;

    // DX 11 needs a renderer, so configure solid
    Driver->activeGPUProgram(m_defaultShader);

    m_defaultShader->UpdateValues(video::IShaderDataBuffer::EUT_PER_FRAME_PER_MESH, nullptr, nullptr, nullptr, video::IShaderDataBuffer::EUF_COMMIT_VALUES | video::IShaderDataBuffer::EUF_CUSTOM_INPUT_VALUES);
    Driver->SyncShaderConstant();
    return true;
}

bool irr::video::CD3D11FixedFunctionMaterialRenderer::init(io::IFileSystem * fileSystem)
{
    UINT FeatureLevel = Driver->getFeatureLevel();

    auto resource = System::Resource::ResourceManager::Instance()->FindResource(System::URI("pack://application:,,,/Direct3D11;/HLSL/Default/d3d11.hlsl", true));

    m_defaultShader = new irr::video::D3D11HLSLProgram(Driver, EST_HIGH_LEVEL_SHADER);
    if (m_defaultShader)
    {
        const char* ShaderModel = "vs_3_0";     // DirectX 9c
        if (FeatureLevel >= D3D_FEATURE_LEVEL_11_0)
            ShaderModel = "vs_5_0";
        else if (FeatureLevel >= D3D_FEATURE_LEVEL_10_0)
            ShaderModel = "vs_4_0";

        const char* ShaderEntryPoint = "vs_main";

        auto stream = resource->ToMemoryStreamReader();
        m_defaultShader->createVertexShader(Device, stream, ShaderEntryPoint, ShaderModel);
        delete stream;
    }

    if (m_defaultShader)
    {
        const char* ShaderModel = "ps_3_0";     // DirectX 9c
        if (FeatureLevel >= D3D_FEATURE_LEVEL_11_0)
            ShaderModel = "ps_5_0";
        else if (FeatureLevel >= D3D_FEATURE_LEVEL_10_0)
            ShaderModel = "ps_4_0";

        const char* ShaderEntryPoint = "ps_main";

        auto stream = resource->ToMemoryStreamReader();
        m_defaultShader->createPixelShader(Device, stream, ShaderEntryPoint, ShaderModel);
        delete stream;
    }

    //Note: Workaround for (S3DVertex) draw2D3DVertexPrimitiveList need drop this obsolete function
    irr::video::VertexDeclaration* vertDecl = Driver->GetVertexDeclaration(E_VERTEX_TYPE::EVT_STANDARD);
    m_defaultShader->m_inputLayout = static_cast<CD3D11VertexDeclaration*>(vertDecl)->getInputLayout(m_defaultShader);
    m_defaultShader->m_inputLayout->AddRef();

    m_defaultShader->Init();

    if (m_defaultShader)
        m_defaultShader->addDataBuffer(new ShaderGenericValuesBuffer(video::IShaderDataBuffer::EUT_PER_FRAME_PER_MESH), nullptr);

    Driver->m_defaultShader = m_defaultShader;
    return true;
}

void irr::video::CD3D11MaterialRenderer_SPHERE_MAP::OnSetMaterial(const SMaterial & material, const SMaterial & lastMaterial, bool resetAllRenderstates, IMaterialRendererServices * services)
{
    // Store current material type
    //CurrentMaterial = material;

    // set basic states
    services->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

    D3D11_BLEND_DESC& blendDesc = Driver->getBlendDesc();

    if (blendDesc.RenderTarget[0].BlendEnable)
    {
        Driver->InvalidateBlendDesc();
        blendDesc.RenderTarget[0].BlendEnable = FALSE;
    }

    // set texture matrix
    BaseRenderer->setVariable("TextureMatrix0", reinterpret_cast<f32*>(&SphereMapMatrixD3D11), sizeof(SphereMapMatrixD3D11));
}

void irr::video::CD3D11MaterialRenderer_SPHERE_MAP::OnUnsetMaterial()
{
    BaseRenderer->setVariable("TextureMatrix0", reinterpret_cast<f32*>(&UnitMatrixD3D11), sizeof(UnitMatrixD3D11));
}

void irr::video::CD3D11MaterialRenderer_REFLECTION_2_LAYER::OnSetMaterial(const SMaterial & material, const SMaterial & lastMaterial, bool resetAllRenderstates, IMaterialRendererServices * services)
{
    // Store current material type
    //CurrentMaterial = material;

    // set basic states
    services->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

    D3D11_BLEND_DESC& blendDesc = Driver->getBlendDesc();

    if (blendDesc.RenderTarget[0].BlendEnable)
    {
        Driver->InvalidateBlendDesc();
        blendDesc.RenderTarget[0].BlendEnable = FALSE;
    }

    // set texture matrix
    BaseRenderer->setVariable("TextureMatrix0", reinterpret_cast<f32*>(&SphereMapMatrixD3D11), sizeof(SphereMapMatrixD3D11));
}

void irr::video::CD3D11MaterialRenderer_REFLECTION_2_LAYER::OnUnsetMaterial()
{
    BaseRenderer->setVariable("TextureMatrix0", reinterpret_cast<f32*>(&UnitMatrixD3D11), sizeof(UnitMatrixD3D11));
}

void irr::video::CD3D11MaterialRenderer_TRANSPARENT_REFLECTION_2_LAYER::OnSetMaterial(const SMaterial & material, const SMaterial & lastMaterial, bool resetAllRenderstates, IMaterialRendererServices * services)
{
    // Store current material type
    //CurrentMaterial = material;

    // set basic states
    services->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

    D3D11_BLEND_DESC& blendDesc = Driver->getBlendDesc();

    Driver->InvalidateBlendDesc();
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
}

void irr::video::CD3D11MaterialRenderer_TRANSPARENT_REFLECTION_2_LAYER::OnUnsetMaterial()
{
    Driver->ResetBlending = true;
}

void irr::video::CD3D11MaterialRenderer_TRANSPARENT_VERTEX_ALPHA::OnSetMaterial(const SMaterial & material, const SMaterial & lastMaterial, bool resetAllRenderstates, IMaterialRendererServices * services)
{
    // Store current material type
    //CurrentMaterial = material;

    // set basic states
    services->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

    D3D11_BLEND_DESC& blendDesc = Driver->getBlendDesc();

    Driver->InvalidateBlendDesc();
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
}

void irr::video::CD3D11MaterialRenderer_TRANSPARENT_VERTEX_ALPHA::OnUnsetMaterial()
{
    Driver->ResetBlending = true;
}

void irr::video::CD3D11MaterialRenderer_TRANSPARENT_ALPHA_CHANNEL_REF::OnSetMaterial(const SMaterial & material, const SMaterial & lastMaterial, bool resetAllRenderstates, IMaterialRendererServices * services)
{
    // Store current material type
    //CurrentMaterial = material;

    // set basic states
    services->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

    D3D11_BLEND_DESC& blendDesc = Driver->getBlendDesc();

    if (blendDesc.RenderTarget[0].BlendEnable)
    {
        Driver->InvalidateBlendDesc();
        blendDesc.RenderTarget[0].BlendEnable = FALSE;
    }
}

void irr::video::CD3D11MaterialRenderer_TRANSPARENT_ALPHA_CHANNEL::OnSetMaterial(const SMaterial & material, const SMaterial & lastMaterial, bool resetAllRenderstates, IMaterialRendererServices * services)
{
    // Store current material type
    //CurrentMaterial = material;

    // set basic states
    services->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

    D3D11_BLEND_DESC& blendDesc = Driver->getBlendDesc();

    Driver->InvalidateBlendDesc();
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
}

void irr::video::CD3D11MaterialRenderer_TRANSPARENT_ALPHA_CHANNEL::OnUnsetMaterial()
{
    Driver->ResetBlending = true;
}

void irr::video::CD3D11MaterialRenderer_TRANSPARENT_ADD_COLOR::OnSetMaterial(const SMaterial & material, const SMaterial & lastMaterial, bool resetAllRenderstates, IMaterialRendererServices * services)
{
    // Store current material type
    //CurrentMaterial = material;

    // set basic states
    services->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

    D3D11_BLEND_DESC& blendDesc = Driver->getBlendDesc();

    Driver->InvalidateBlendDesc();
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_COLOR;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
}

void irr::video::CD3D11MaterialRenderer_TRANSPARENT_ADD_COLOR::OnUnsetMaterial()
{
    Driver->ResetBlending = true;
}

void irr::video::CD3D11MaterialRenderer_DETAIL_MAP::OnSetMaterial(const SMaterial & material, const SMaterial & lastMaterial, bool resetAllRenderstates, IMaterialRendererServices * services)
{
    // Store current material type
    //CurrentMaterial = material;

    // set basic states
    services->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

    D3D11_BLEND_DESC& blendDesc = Driver->getBlendDesc();
    D3D11_SAMPLER_DESC* samplerDescs = Driver->getSamplerDescs();

    if (blendDesc.RenderTarget[0].BlendEnable)
    {
        Driver->InvalidateBlendDesc();
        blendDesc.RenderTarget[0].BlendEnable = FALSE;
    }

    Driver->InvalidateSamplerDesc(0);
    Driver->InvalidateSamplerDesc(1);

    samplerDescs[0].AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDescs[0].AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDescs[0].Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDescs[1].AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDescs[1].AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDescs[1].Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
}

irr::video::CD3D11MaterialRenderer_LIGHTMAP::CD3D11MaterialRenderer_LIGHTMAP(IVideoDriver * driver, io::IFileSystem * fSystem)
    : CD3D11FixedFunctionMaterialRenderer(driver, fSystem) {}

void irr::video::CD3D11MaterialRenderer_LIGHTMAP::OnSetMaterial(const SMaterial & material, const SMaterial & lastMaterial, bool resetAllRenderstates, IMaterialRendererServices * services)
{
    // Store current material type
    //CurrentMaterial = material;

    // set basic states
    services->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

    D3D11_BLEND_DESC& blendDesc = Driver->getBlendDesc();

    if (blendDesc.RenderTarget[0].BlendEnable)
    {
        Driver->InvalidateBlendDesc();
        blendDesc.RenderTarget[0].BlendEnable = FALSE;
    }
}

irr::video::CD3D11MaterialRenderer_SOLID_2_LAYER::CD3D11MaterialRenderer_SOLID_2_LAYER(IVideoDriver * driver, io::IFileSystem * fSystem)
    : CD3D11FixedFunctionMaterialRenderer(driver, fSystem) {}

void irr::video::CD3D11MaterialRenderer_SOLID_2_LAYER::OnSetMaterial(const SMaterial & material, const SMaterial & lastMaterial, bool resetAllRenderstates, IMaterialRendererServices * services)
{
    // Store current material type
    //CurrentMaterial = material;

    // set basic states
    services->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

    D3D11_BLEND_DESC& blendDesc = Driver->getBlendDesc();

    if (blendDesc.RenderTarget[0].BlendEnable)
    {
        Driver->InvalidateBlendDesc();
        blendDesc.RenderTarget[0].BlendEnable = FALSE;
    }
}

void irr::video::CD3D11MaterialRenderer_ONETEXTURE_BLEND::OnSetMaterial(const SMaterial & material, const SMaterial & lastMaterial, bool resetAllRenderstates, IMaterialRendererServices * services)
{
    services->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

    //if (material.MaterialType != lastMaterial.MaterialType ||
    //	material.uMaterialTypeParam != lastMaterial.uMaterialTypeParam ||
    //    material.BlendOperation != lastMaterial.BlendOperation ||
    //	resetAllRenderstates)
    {

        E_BLEND_FACTOR srcFact, dstFact, srcFactAlpha, dstFactAlpha;
        E_BLEND_OPERATION blendOp, blendOpAlpha;
        E_MODULATE_FUNC modulate;
        u32 alphaSource;
        unpack_textureBlendFunc(srcFact, dstFact, modulate, alphaSource, material.uMaterialTypeParam, &srcFactAlpha, &dstFactAlpha, &blendOp, &blendOpAlpha);

        D3D11_BLEND_DESC& blendDesc = Driver->getBlendDesc();

        if (srcFact == EBF_SRC_COLOR && dstFact == EBF_ZERO)
        {
            transparent = false;

            if (blendDesc.RenderTarget[0].BlendEnable)
            {
                Driver->InvalidateBlendDesc();
                blendDesc.RenderTarget[0].BlendEnable = FALSE;
            }
        }
        else
        {
            transparent = true;

            auto& renderTarget0 = blendDesc.RenderTarget[0];
            auto SrcBlend = getD3DBlend(srcFact); //SrcBlendAlpha
            auto DestBlend = getD3DBlend(dstFact);
            auto BlendOp = getD3DBlendOp(blendOp);
            auto SrcBlendAlpha = getD3DBlend(srcFactAlpha); //SrcBlendAlpha
            auto DestBlendAlpha = getD3DBlend(dstFactAlpha);
            auto BlendOpAlpha = getD3DBlendOp(blendOpAlpha);

            if (!renderTarget0.BlendEnable ||
                renderTarget0.RenderTargetWriteMask != D3D11_COLOR_WRITE_ENABLE_ALL ||
                renderTarget0.SrcBlend != SrcBlend ||
                renderTarget0.DestBlend != DestBlend ||
                renderTarget0.BlendOp != BlendOp ||
                renderTarget0.SrcBlendAlpha != SrcBlendAlpha ||
                renderTarget0.DestBlendAlpha != DestBlendAlpha ||
                renderTarget0.BlendOpAlpha != BlendOpAlpha
                )
            {
                Driver->InvalidateBlendDesc();

                renderTarget0.BlendEnable = TRUE;
                renderTarget0.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

                renderTarget0.SrcBlend = SrcBlend;
                renderTarget0.DestBlend = DestBlend;
                renderTarget0.BlendOp = BlendOp;
                renderTarget0.SrcBlendAlpha = SrcBlendAlpha;
                renderTarget0.DestBlendAlpha = DestBlendAlpha;
                renderTarget0.BlendOpAlpha = BlendOpAlpha;
            }
        }
    }
}

void irr::video::CD3D11MaterialRenderer_ONETEXTURE_BLEND::OnUnsetMaterial()
{
    if (transparent)
    {
        Driver->InvalidateBlendDesc();
        Driver->ResetBlending = true;
        D3D11_BLEND_DESC& blendDesc = Driver->getBlendDesc();
        blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
        blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
        blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
        blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
    }
}

void irr::video::CD3D11MaterialRenderer_RAW::OnSetMaterial(const SMaterial & material, const SMaterial & lastMaterial, bool resetAllRenderstates, IMaterialRendererServices * services)
{
    // Store current material type
    //CurrentMaterial = material;

    // set basic states
    services->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

    D3D11_BLEND_DESC& blendDesc = Driver->getBlendDesc();

    if (blendDesc.RenderTarget[0].BlendEnable)
    {
        Driver->InvalidateBlendDesc();
        blendDesc.RenderTarget[0].BlendEnable = FALSE;
    }
}

void irr::video::CD3D11MaterialRenderer_SOLID::OnSetMaterial(const SMaterial & material, const SMaterial & lastMaterial, bool resetAllRenderstates, IMaterialRendererServices * services)
{
    // Store current material type
    //CurrentMaterial = material;

    // set basic states
    services->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);

    D3D11_BLEND_DESC& blendDesc = Driver->getBlendDesc();

    if (blendDesc.RenderTarget[0].BlendEnable)
    {
        Driver->InvalidateBlendDesc();
        blendDesc.RenderTarget[0].BlendEnable = FALSE;
    }
}

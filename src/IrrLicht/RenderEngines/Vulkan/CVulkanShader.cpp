#include "CVulkanShader.h"
#include "CVulkanDriver.h"
#include "CVulkanHardwareBuffer.h"
#include "CVulkanShaderCompiler.h"
#include "CVulkanDescriptorLayout.h"
#include "CVulkanGpuParams.h"
#include "os.h"
#include "IMaterialRenderer.h"

#include "glslang/Public/ShaderLang.h"
#include "glslang/MachineIndependent/reflection.h"
#include "SPIRV/GlslangToSpv.h"

using namespace irr;
using namespace irr::video;

irr::video::CVulkanGLSLProgram::CVulkanGLSLProgram(video::IVideoDriver * context, E_ShaderTypes type)
    : IShader(context, type)
    , CVulkanDeviceResource(static_cast<CVulkanDriver*>(context))
{
}

irr::video::CVulkanGLSLProgram::~CVulkanGLSLProgram()
{
}

void irr::video::CVulkanGLSLProgram::bind()
{
}

void irr::video::CVulkanGLSLProgram::unbind()
{
}

void irr::video::CVulkanGLSLProgram::Init()
{
    getLayout();
    mParams = new VulkanGpuParams(Driver, this, GpuDeviceFlags::GDF_PRIMARY);
    mParams->initialize();
}

bool irr::video::CVulkanGLSLProgram::enumInputLayout(void *)
{
    return false;
}

void irr::video::CVulkanGLSLProgram::BuildBufferDesc(irr::video::CVulkanGLSLang & compiler, E_ShaderTypes type)
{
}

bool irr::video::CVulkanGLSLProgram::initializeUniforms(irr::video::CVulkanGLSLang& compiler, E_ShaderTypes shaderType)
{
    uint32 num = compiler.Reflection->getNumUniforms();
    for (uint32 i = 0; i < num; ++i)
    {
        const auto& bufferEntry = compiler.Reflection->getUniform(i);

        if (bufferEntry.getType()->getBasicType() == glslang::TBasicType::EbtSampler)
        {
            Textures.push_back(new CbufferDesc);
            CbufferDesc& buffdesc = *Textures.back();
            buffdesc.shaderType = shaderType;
            buffdesc.binding = bufferEntry.getBinding();

            buffdesc.varDesc.m_name = bufferEntry.name;
            buffdesc.varDesc.m_location = i;
            buffdesc.varDesc.m_shaderIndex = ShaderBuffers.size() - 1;
            buffdesc.varDesc.m_type = ESVT_UNIFORM;

            AddShaderVariable(&buffdesc.varDesc);
        }
    }

    return true;
}

void irr::video::CVulkanGLSLProgram::ReflParseStruct(irr::video::CVulkanGLSLProgram::CbufferDesc& buffdesc, irr::video::CVulkanGLSLProgram::ShaderConstantDesc* parent, const glslang::TType* type, std::vector<irr::video::CVulkanGLSLProgram::ShaderConstantDesc>& Variables, std::string namePrefix)
{
    std::string bufferPrefixName = buffdesc.varDesc.m_name + '.';

    uint32 elementNum = type->getStruct()->size();
    Variables.resize(elementNum);
    for (int m = 0; m < elementNum; ++m)
    {
        const auto& pVariable = (*type->getStruct())[m];

        irr::video::CVulkanGLSLProgram::ShaderConstantDesc& structDecl = Variables[m];
        structDecl.BufferDesc = &buffdesc;
        structDecl.Parent = parent;

        assert(pVariable.type->getQualifier().hasOffset()); // ToDo: implement TParseContext::fixBlockUniformOffsets after know layout format
        if (pVariable.type->getQualifier().hasOffset())
            structDecl.offset = pVariable.type->getQualifier().layoutOffset;
        if (pVariable.type->getQualifier().hasAlign())
            structDecl.varDesc.m_size = pVariable.type->getQualifier().layoutAlign;

        structDecl.varDesc.m_name = pVariable.type->getFieldName();
        structDecl.varDesc.m_length = pVariable.type->getArraySizes() ? pVariable.type->getArraySizes()->getDimSize(0) : 1;
        structDecl.varDesc.m_variableType = pVariable.type->getBasicType();
        structDecl.elementSize = 0;

        structDecl.varDesc.m_location = m;                          // Buffer Variable Index
        structDecl.varDesc.m_class = 0;
        structDecl.varDesc.m_size = 0;
        structDecl.varDesc.m_type = ESVT_CONSTANT;
        structDecl.varDesc.m_shaderIndex = buffdesc.varDesc.m_shaderIndex; // Buffer Desc Index

        structDecl.name = bufferPrefixName + namePrefix + pVariable.type->getFieldName().c_str();
        structDecl.varDesc.m_name = namePrefix + pVariable.type->getFieldName().c_str();

        if (pVariable.type->getStruct())
        {
            std::string _namePrefix = namePrefix + pVariable.type->getFieldName().c_str();
            _namePrefix += '.';

            ReflParseStruct(buffdesc, &structDecl, pVariable.type, structDecl.members, _namePrefix);
        }
    }

    std::sort(Variables.begin(), Variables.end(), [](irr::video::CVulkanGLSLProgram::ShaderConstantDesc const& lhs, irr::video::CVulkanGLSLProgram::ShaderConstantDesc const& rhs)
    {
        return lhs.offset < rhs.offset;
    });

    for (int m = 0; m < elementNum - 1; ++m)
    {
        auto& pVariable = Variables[m];
        const auto& pVariableNext = Variables[m + 1];

        uint32 dataSize = pVariableNext.offset - pVariable.offset;
        uint32 elementSize = dataSize / pVariable.varDesc.m_length;

        pVariable.varDesc.m_location = m;
        pVariable.varDesc.m_size = dataSize;
        pVariable.elementSize = elementSize;

        AddShaderVariable(&pVariable.varDesc);
    }

    auto& pVariable = Variables.back();
    uint32 dataSize = parent ? 0 : buffdesc.DataBuffer.size() - pVariable.offset; // ToDo
    uint32 elementSize = dataSize / pVariable.varDesc.m_length;

    pVariable.varDesc.m_location = elementNum - 1;
    pVariable.varDesc.m_size = dataSize;
    pVariable.elementSize = elementSize;

    AddShaderVariable(&pVariable.varDesc);
}

bool irr::video::CVulkanGLSLProgram::initializeConstantBuffers(irr::video::CVulkanGLSLang& compiler, E_ShaderTypes shaderType)
{
    uint32 num = compiler.Reflection->getNumUniformBlocks();
    for (uint32 i = 0; i < num; ++i)
    {
        const auto& bufferEntry = compiler.Reflection->getUniformBlock(i);

        ShaderBuffers.push_back(new CbufferDesc);
        CbufferDesc& buffdesc = *ShaderBuffers.back();

        buffdesc.shaderType = shaderType;

        // Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
        buffdesc.m_constantBuffer = static_cast<CVulkanHardwareBuffer*>(Driver->createHardwareBuffer(E_HARDWARE_BUFFER_TYPE::EHBT_CONSTANTS, E_HARDWARE_BUFFER_ACCESS::EHBA_DEFAULT, bufferEntry.size));

        // init cpu cache
        buffdesc.DataBuffer.set_used(bufferEntry.size);
        memset(buffdesc.DataBuffer.pointer(), 0, buffdesc.DataBuffer.size());
        buffdesc.ChangeId = 0;

        buffdesc.binding = bufferEntry.getBinding();

        buffdesc.varDesc.m_name = bufferEntry.name;
        buffdesc.varDesc.m_location = i;
        buffdesc.varDesc.m_shaderIndex = ShaderBuffers.size() - 1;
        buffdesc.varDesc.m_type = ESVT_CONSTANT;

        ReflParseStruct(buffdesc, nullptr, bufferEntry.getType(), buffdesc.Variables, "");
    }
    return true;
}

bool irr::video::CVulkanGLSLProgram::CreateShaderModul(E_ShaderTypes type, CVulkanDriver * device, System::IO::IFileReader * file, const char * entryPoint)
{
    irr::video::CVulkanGLSLang compiler(Driver);
    bool result = compiler.CompileShader(GetShaderModule(type), type, file, entryPoint);
    if (result)
    {
        mStages[type].first = entryPoint;

        if (type == E_ShaderTypes::EST_VERTEX_SHADER)
        {
            uint32 num = compiler.Reflection->getNumAttributes();
            for (uint32 i = 0; i < num; ++i)
            {
                const auto& attrEntry = compiler.Reflection->getAttribute(i);

                Attributes.emplace_back();
                ShaderVariableDescriptor& desc = Attributes.back();

                desc.m_type = ESVT_ATTRIBUTE;
                desc.m_length = attrEntry.getType()->getArraySizes() ? attrEntry.getType()->getArraySizes()->getDimSize(0) : 1;

                desc.m_name = attrEntry.name;
                desc.m_location = i; // attrEntry.index;
                desc.m_variableType = attrEntry.getType()->getBasicType();
                desc.m_shaderIndex = Attributes.size() - 1;
            }
        }

        initializeConstantBuffers(compiler, type);
        initializeUniforms(compiler, type);
    }
    return result;
}

VulkanDescriptorLayout* irr::video::CVulkanGLSLProgram::getLayout()
{
    if (!mLayout)
    {
        mBindings.resize(ShaderBuffers.size() + Textures.size());

        for (int i = 0; i < ShaderBuffers.size(); ++i)
        {
            const auto& buf = ShaderBuffers[i];
            VkDescriptorSetLayoutBinding& bind = mBindings[i];
            bind.binding = buf->binding;
            bind.stageFlags = GetShaderStageBit(buf->shaderType);
            bind.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; // VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            bind.descriptorCount = 1;
            bind.pImmutableSamplers = nullptr;
        }

        for (int i = 0; i < Textures.size(); ++i)
        {
            const auto& buf = Textures[i];
            VkDescriptorSetLayoutBinding& bind = mBindings[i + ShaderBuffers.size()];
            bind.binding = buf->binding;
            bind.stageFlags = GetShaderStageBit(buf->shaderType);
            bind.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            bind.descriptorCount = 1;
            bind.pImmutableSamplers = nullptr;
        }

        mLayout = new VulkanDescriptorLayout(*Driver->_getPrimaryDevice(), mBindings.data(), mBindings.size());
    }

    return mLayout;
}

VulkanGpuParams * irr::video::CVulkanGLSLProgram::GetDefaultGpuParams()
{
    return mParams;
}

void irr::video::CVulkanGLSLProgram::OnDeviceLost(CVulkanDriver * device)
{
}

void irr::video::CVulkanGLSLProgram::OnDeviceRestored(CVulkanDriver * device)
{
}

VulkanShaderGenericValuesBuffer::VulkanShaderGenericValuesBuffer(video::IShaderDataBuffer::E_UPDATE_TYPE updateType) : video::IShaderDataBuffer(updateType)
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

VulkanShaderGenericValuesBuffer::~VulkanShaderGenericValuesBuffer()
{
}

void VulkanShaderGenericValuesBuffer::InitializeFormShader(video::IShader * gpuProgram, void * Descriptor)
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

    UNSAFE_ADD_DATABUFFER_ELEMENT(ShaderMaterial, VulkanShaderGenericValuesBuffer::Material, "g_material");

    gpuProgram->getVideoDriver()->setShaderMapping(projection->getDescription(), gpuProgram, irr::scene::E_HARDWARE_MAPPING::EHM_DYNAMIC);
    gpuProgram->getVideoDriver()->setShaderMapping(ShaderMaterial->getDescription(), gpuProgram, irr::scene::E_HARDWARE_MAPPING::EHM_DYNAMIC);

    ShaderLight = new video::ShaderDataBufferElementArray<VulkanShaderGenericValuesBuffer::Light>(gpuProgram->GetGPUVariableDesc("g_lights"));
    AddBufferElement(ShaderLight);
#undef UNSAFE_ADD_DATABUFFER_ELEMENT
}

void VulkanShaderGenericValuesBuffer::UpdateBuffer(video::IShader * gpuProgram, scene::IMeshBuffer * meshBuffer, scene::IMesh * mesh, scene::ISceneNode * node, u32 updateFlags)
{
    CVulkanDriver* Driver = static_cast<CVulkanDriver*>(gpuProgram->getVideoDriver());
    SMaterial& CurrentMaterial = !meshBuffer ? Driver->GetMaterial() : meshBuffer->getMaterial();
    video::IMaterialRenderer* rnd = Driver->getMaterialRenderer(CurrentMaterial.MaterialType);

    *world = Driver->getTransform(ETS_WORLD);
    *view = Driver->getTransform(ETS_VIEW);
    *projection = Driver->getTransform(ETS_PROJECTION);

    *AlphaRef = CurrentMaterial.MaterialType != EMT_ONETEXTURE_BLEND && CurrentMaterial.MaterialTypeParam ? CurrentMaterial.MaterialTypeParam : 0.5f;
    *AlphaTest = (rnd && rnd->isTransparent());
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
            VulkanShaderGenericValuesBuffer::Light l;
            memset(&l, 0, sizeof(VulkanShaderGenericValuesBuffer::Light));
            SLight dl = Driver->getDynamicLight(i);
            l.Position = dl.Position;
            l.Atten = dl.Attenuation;
            l.Ambient = dl.AmbientColor;
            l.Diffuse = dl.DiffuseColor;
            l.Specular = dl.SpecularColor;
            *ShaderLight += l;
        }
    }

    VulkanShaderGenericValuesBuffer::Material material;
    memset(&material, 0, sizeof(VulkanShaderGenericValuesBuffer::Material));

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

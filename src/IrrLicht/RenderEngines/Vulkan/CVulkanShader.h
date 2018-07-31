#ifndef __C_VULKAN_SHADER_H_INCLUDED__
#define __C_VULKAN_SHADER_H_INCLUDED__

#include "RenderEngines/General/CIrrVertexDeclaration.h"
#include "RenderEngines/General/CIrrShader.h"
#include "CVulkanUtility.h"
#include "CVulkanVertexDeclaration.h"
#include "CVulkanResources.h"

namespace glslang
{
    class TType;
}

namespace irr
{
    namespace video
    {
        class CVulkanGLSLang;
        class CVulkanHardwareBuffer;
        class VulkanDescriptorLayout;
        class VulkanGraphicsPipelineState;
        class VulkanGpuParams;

        class CVulkanGLSLProgram
            : public IShader
            , public CVulkanDeviceResource
        {
        public:

            struct CbufferDesc;

            struct ShaderConstantDesc
            {
                CbufferDesc* BufferDesc;
                ShaderConstantDesc* Parent = nullptr;
                std::string name;
                ShaderVariableDescriptor varDesc;
                u32 offset;
                u32 elementSize;
                std::vector<ShaderConstantDesc> members;
            };

            struct CbufferDesc
            {
                CbufferDesc() : ChangeId(0), constantMapping(scene::E_HARDWARE_MAPPING::EHM_STATIC), ChangeStartOffset(0xFFFFFFFF), ChangeEndOffset(0)
                {

                }

                u32 binding;
                ShaderVariableDescriptor varDesc;

                std::vector<ShaderConstantDesc> Variables;
                core::array<char> DataBuffer;
                CVulkanHardwareBuffer* m_constantBuffer;
                u32 ChangeId;
                u32 ChangeStartOffset;
                u32 ChangeEndOffset;
                E_ShaderTypes shaderType : 8;
                scene::E_HARDWARE_MAPPING constantMapping : 8;

                CVulkanHardwareBuffer* getHardwareBuffer() const { return m_constantBuffer; }
                scene::E_HARDWARE_MAPPING getHardwareMappingHint() const { return constantMapping; }
                void setHardwareMappingHint(scene::E_HARDWARE_MAPPING value) { constantMapping = value; }
                u32 getChangeId() const { return ChangeId; }
            };

            VkShaderModule* GetShaderModule(E_ShaderTypes type)
            {
                switch (type)
                {
                    case E_ShaderTypes::EST_VERTEX_SHADER:  
                    case E_ShaderTypes::EST_DOMAIN_SHADER:  
                    case E_ShaderTypes::EST_HULL_SHADER:    
                    case E_ShaderTypes::EST_GEOMETRY_SHADER:
                    case E_ShaderTypes::EST_FRAGMENT_SHADER:
                    case E_ShaderTypes::EST_COMPUTE_SHADER:
                        return &mStages[type].second;
                }

                return nullptr;
            }

            bool HasShaderModule(E_ShaderTypes type)
            {
                switch (type)
                {
                    case E_ShaderTypes::EST_VERTEX_SHADER:
                    case E_ShaderTypes::EST_DOMAIN_SHADER:
                    case E_ShaderTypes::EST_HULL_SHADER:
                    case E_ShaderTypes::EST_GEOMETRY_SHADER:
                    case E_ShaderTypes::EST_FRAGMENT_SHADER:
                    case E_ShaderTypes::EST_COMPUTE_SHADER:
                        return mStages[type].second != nullptr ? true : false;
                }

                return false;
            }

            const char* GetShaderEntryPoint(E_ShaderTypes type)
            {
                switch (type)
                {
                    case E_ShaderTypes::EST_VERTEX_SHADER:
                    case E_ShaderTypes::EST_DOMAIN_SHADER:
                    case E_ShaderTypes::EST_HULL_SHADER:
                    case E_ShaderTypes::EST_GEOMETRY_SHADER:
                    case E_ShaderTypes::EST_FRAGMENT_SHADER:
                    case E_ShaderTypes::EST_COMPUTE_SHADER:
                        return mStages[type].first.c_str();
                }

                return nullptr;
            }

            VkShaderStageFlagBits GetShaderStageBit(E_ShaderTypes type)
            {
                switch (type)
                {
                    case E_ShaderTypes::EST_VERTEX_SHADER:      return VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
                    case E_ShaderTypes::EST_DOMAIN_SHADER:      return VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
                    case E_ShaderTypes::EST_HULL_SHADER:        return VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
                    case E_ShaderTypes::EST_GEOMETRY_SHADER:    return VkShaderStageFlagBits::VK_SHADER_STAGE_GEOMETRY_BIT;
                    case E_ShaderTypes::EST_FRAGMENT_SHADER:    return VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
                    case E_ShaderTypes::EST_COMPUTE_SHADER:     return VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT;
                }

                return VkShaderStageFlagBits::VK_SHADER_STAGE_ALL_GRAPHICS;
            }

            void BuildBufferDesc(irr::video::CVulkanGLSLang& compiler, E_ShaderTypes type);

        private:
            friend class VulkanGraphicsPipelineState;
            friend class CVulkanDriver;

            std::vector<CbufferDesc*> ShaderBuffers;
            std::vector<CbufferDesc*> Textures;
            std::vector<ShaderVariableDescriptor> Attributes;

            std::pair<std::string /*entry point*/, VkShaderModule> mStages[E_ShaderTypes::EST_HIGH_LEVEL_SHADER];
            std::vector<VkDescriptorSetLayoutBinding> mBindings;
            VulkanDescriptorLayout* mLayout = nullptr;
            VulkanGpuParams* mParams = nullptr;

        public:
            explicit CVulkanGLSLProgram(video::IVideoDriver* context, E_ShaderTypes type);

            virtual ~CVulkanGLSLProgram();

            virtual E_ShaderVersion getShaderVersion() const { return ESV_GLSL_HIGH_LEVEL; }
            virtual void bind() override;
            virtual void unbind() override;
            virtual void destroy() override {}
            virtual void Init() override;

            bool enumInputLayout(void*);
            bool initializeConstantBuffers(irr::video::CVulkanGLSLang& compiler, E_ShaderTypes shaderType);
            bool initializeUniforms(irr::video::CVulkanGLSLang& compiler, E_ShaderTypes shaderType);

            bool CreateShaderModul(E_ShaderTypes type, CVulkanDriver* device, System::IO::IFileReader* file, const char* entryPoint);

            VulkanDescriptorLayout* getLayout();
            VulkanGpuParams* GetDefaultGpuParams();

            const std::vector<VkDescriptorSetLayoutBinding>& GetBindings() const { return mBindings; }
            const std::vector<CbufferDesc*>& GetBlockBufferInfos() const { return ShaderBuffers; }
            const std::vector<CbufferDesc*> GetTextureInfos() const { return Textures; }
            std::vector<ShaderVariableDescriptor> GetAttributeInfos() const { return Attributes; }

            // Inherited via D3D11DeviceResource
            virtual void OnDeviceLost(CVulkanDriver * device) override;
            virtual void OnDeviceRestored(CVulkanDriver * device) override;
        private:

            void ReflParseStruct(irr::video::CVulkanGLSLProgram::CbufferDesc& buffdesc, irr::video::CVulkanGLSLProgram::ShaderConstantDesc* parent, const glslang::TType* type, std::vector<irr::video::CVulkanGLSLProgram::ShaderConstantDesc>& Variables, std::string namePrefix);
        };

        struct VulkanShaderGenericValuesBuffer : public video::IShaderDataBuffer
        {
            __declspec(align(16)) struct MatrixBufferType
            {
                core::matrix4 world;
                core::matrix4 view;
                core::matrix4 projection;
            };

            __declspec(align(16)) struct Light
            {
                irr::core::vector3df Position;
                irr::core::vector3df Atten;
                irr::video::SColorf Diffuse;
                irr::video::SColorf Specular;
                irr::video::SColorf Ambient;

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
                irr::video::SColorf    Ambient;
                irr::video::SColorf    Diffuse;
                irr::video::SColorf    Specular;
                irr::video::SColorf    Emissive;
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
                Light       lights[4];
            };

            video::ShaderDataBufferElementObject<core::matrix4>* world;
            video::ShaderDataBufferElementObject<core::matrix4>* view;
            video::ShaderDataBufferElementObject<core::matrix4>* projection;

            video::ShaderDataBufferElementObject<int>* nTexture;
            video::ShaderDataBufferElementObject<int>* AlphaTest;
            video::ShaderDataBufferElementObject<int>* LightCount;
            video::ShaderDataBufferElementObject<float>* AlphaRef;
            video::ShaderDataBufferElementObject<VulkanShaderGenericValuesBuffer::Material>* ShaderMaterial;
            video::ShaderDataBufferElementArray<VulkanShaderGenericValuesBuffer::Light>* ShaderLight;

            VulkanShaderGenericValuesBuffer(video::IShaderDataBuffer::E_UPDATE_TYPE updateType);

            virtual ~VulkanShaderGenericValuesBuffer();

            virtual void InitializeFormShader(video::IShader* gpuProgram, void* Descriptor);

            virtual void UpdateBuffer(video::IShader* gpuProgram, scene::IMeshBuffer* meshBuffer, scene::IMesh* mesh, scene::ISceneNode* node, u32 updateFlags);
        };
    }
}

#endif //!__C_DIRECTX11_SHADER_H_INCLUDED__
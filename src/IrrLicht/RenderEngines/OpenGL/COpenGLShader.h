#ifndef __C_VIDEO_OPEN_GL_SHADER_H_INCLUDED__
#define __C_VIDEO_OPEN_GL_SHADER_H_INCLUDED__

#include "IrrCompileConfig.h"

#include "SIrrCreationParameters.h"

namespace irr
{
    class CIrrDeviceWin32;
    class CIrrDeviceLinux;
    class CIrrDeviceSDL;
    class CIrrDeviceMacOSX;
}

#ifdef _IRR_COMPILE_WITH_OPENGL_

#include "RenderEngines/General/CIrrShader.h"
#include "COpenGLExtensionHandler.h"
#include "COpenGLHardwareBuffer.h"

#include <vector>

#ifdef _IRR_COMPILE_WITH_CG_
#include "Cg/cg.h"
#endif

namespace irr
{
    namespace scene
    {
        enum E_HARDWARE_MAPPING;
    };

    namespace video
    {
        class GLArbShader;
        class GLSLGpuShader;
        class COpenGLDriver;
        class COpenGLTexture;
        class COpenGLHardwareBuffer;

        struct CGlslBufferDesc;
        
        struct CGlslVariableDesc
        {
            CGlslBufferDesc* BufferDesc;
            ShaderVariableDescriptor varDesc;
            std::string name;                   // Not qualified name
            std::vector<CGlslVariableDesc> members;
            //u32 type;
            u32 offset;
            u32 blockIndex;
            u32 arrayStride;
            u32 matrixStride;
            u32 elementSize;
            u32 dataSize;
            //SSBO
            u32 toplevel_arraysize;
            u32 toplevel_arraystride;
            bool isRowMajor;
        };

        struct CGlslBufferDesc
        {
            ~CGlslBufferDesc()
            {

            }

            u32 binding;
            u32 dataSize;
            ShaderVariableDescriptor varDesc;
            COpenGLHardwareBuffer* m_constantBuffer = nullptr;
            u32 ChangeId;
            u32 ChangeStartOffset;
            u32 ChangeEndOffset;
            E_ShaderTypes shaderType : 8;
            scene::E_HARDWARE_MAPPING constantMapping : 8;
            std::vector<CGlslVariableDesc> members;
            core::array<char> DataBuffer;

            COpenGLHardwareBuffer* getHardwareBuffer() const { return m_constantBuffer; }
            scene::E_HARDWARE_MAPPING getHardwareMappingHint() const { return constantMapping; }
            void setHardwareMappingHint(scene::E_HARDWARE_MAPPING value) { constantMapping = value; }
            u32 getChangeId() const { return ChangeId; }
        };

        class GLShader : public IShader
        {
        protected:
            u32 programId;
            GLenum glShaderType;

        public:
            GLShader(video::IVideoDriver* context, E_ShaderTypes type, u32 _programId)
                : IShader(context, type)
                , programId(_programId)
            {
            }

            virtual ~GLShader()
            {
            }

            u32 getProgramId() const { return programId; }

            COpenGLDriver* getDriverOGL();

            virtual GLSLGpuShader* ToGLSL() { return nullptr; }
            virtual GLArbShader* ToASM() { return nullptr; }
        };

        class GLArbShader : public GLShader
        {
            GLenum glShaderType;
        public:
            explicit GLArbShader(video::IVideoDriver* context, E_ShaderTypes type, u32 _programId);
            virtual ~GLArbShader()
            {
                destroy();
            }

            void addShaderFile(u32 _glShaderType, const char* pFilename);

            virtual E_ShaderVersion getShaderVersion() const override { return ESV_GLSL_ASM; }
            //virtual void compile() {}
            virtual void bind();
            virtual void unbind();
            virtual void destroy();
            virtual void Init() {}

             GLArbShader* ToASM() override { return this; }
        };

        class GLSLGpuShader : public GLShader
        {
        private:

        public:
            std::vector<CGlslBufferDesc> ShaderAttributes;
            std::vector<CGlslBufferDesc> ShaderBuffers;
            std::vector<CGlslBufferDesc> TextureUnits;

            explicit GLSLGpuShader(video::IVideoDriver* context, E_ShaderTypes type, u32 _programId);
            virtual ~GLSLGpuShader();

            virtual E_ShaderVersion getShaderVersion() const override { return ESV_GLSL_HIGH_LEVEL; }
            //virtual void compile() override;
            virtual void bind();
            virtual void unbind();
            virtual void destroy();
            virtual void Init() {}

            void addShaderFile(GLSLGpuShader*, u32 shaderType, System::IO::IFileReader* pFilename, std::list<u32>& ShaderObjList);
            void compile(GLSLGpuShader*, std::list<u32>& ShaderObjList);
            void buildShaderVariableDescriptor();

            GLSLGpuShader* ToGLSL() override final { return this; }

            const CGlslBufferDesc* getGPUProgramAttributeDesc(u32 index) const
            {
                for (u32 i = 0; i != ShaderAttributes.size(); ++i)
                {
                    if (ShaderAttributes[i].varDesc.m_location == index)
                    {
                        return &ShaderAttributes[i];
                    }
                }
                return nullptr;
            }

            //svirtual void CommitValues(IShaderDataBuffer::E_UPDATE_TYPE updateType);
            //int getProgramParam(int param);

            //virtual void setUniformMatrix4fv(int loc, int count, bool transpose, float const* data);
        };

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
            video::ShaderDataBufferElementObject<ShaderGenericValuesBuffer::Material>* ShaderMaterial;
            video::ShaderDataBufferElementArray<ShaderGenericValuesBuffer::Light>* ShaderLight;

            ShaderGenericValuesBuffer(video::IShaderDataBuffer::E_UPDATE_TYPE updateType);

            virtual ~ShaderGenericValuesBuffer();

            virtual void InitializeFormShader(video::IShader* gpuProgram, void* Descriptor);

            virtual void UpdateBuffer(video::IShader* gpuProgram, scene::IMeshBuffer* meshBuffer, scene::IMesh* mesh, scene::ISceneNode* node, u32 updateFlags);
        };
    }
}

#endif //!_IRR_COMPILE_WITH_OPENGL_
#endif //!__C_VIDEO_OPEN_GL_SHADER_H_INCLUDED__
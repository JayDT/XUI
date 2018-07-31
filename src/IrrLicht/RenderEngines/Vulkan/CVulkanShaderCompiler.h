#ifndef __C_VULKAN_SHADER_LANG_H_INCLUDED__
#define __C_VULKAN_SHADER_LANG_H_INCLUDED__

#include "RenderEngines/General/CIrrShader.h"
#include "CVulkanUtility.h"

#include "glslang/Public/ShaderLang.h"

struct TBuiltInResource;

namespace glslang
{
    class TShader;
    class TProgram;
    class TReflection;
}

namespace irr
{
    namespace video
    {
        class CVulkanGLSLang
        {
            friend class CVulkanDriver;
        private:

            CVulkanDriver * Driver = nullptr;

            void SetMessageOptions(u32& messages);

        public:
            bool isHlsl = false;
            bool isOGL = false;
            bool CompileFailed = false;
            bool LinkFailed = false;
            bool ReflectionFailed = false;

            // Per descriptor-set binding base data
            typedef std::map<unsigned int, unsigned int> TPerSetBaseBinding;

            // keep track of what to free
            std::list<glslang::TShader*> shaders;
            std::vector<std::string> Processes;                     // what should be recorded by OpModuleProcessed, or equivalent

            std::array<std::array<unsigned int, EShLangCount>, glslang::EResCount> baseBinding;
            std::array<std::array<TPerSetBaseBinding, EShLangCount>, glslang::EResCount> baseBindingForSet;
            std::array<std::vector<std::string>, EShLangCount> baseResourceSetBinding;

            TBuiltInResource Resources;
            glslang::TReflection* Reflection = nullptr;
            glslang::TProgram* Program = nullptr;

            explicit CVulkanGLSLang(CVulkanDriver* driver);

            virtual ~CVulkanGLSLang();

            bool CompileShader(VkShaderModule * modul, E_ShaderTypes type, System::IO::IFileReader * file, const char * entryPoint);
        };
    }
}

#endif //!__C_VULKAN_SHADER_LANG_H_INCLUDED__
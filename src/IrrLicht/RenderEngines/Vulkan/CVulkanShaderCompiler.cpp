#include "glslang/MachineIndependent/reflection.h"
#include "SPIRV/GlslangToSpv.h"

#include "CVulkanShaderCompiler.h"
#include "CVulkanDriver.h"
#include "CVulkanDevice.h"

#define SH_EXPORTING

#include <cassert>

#include "glslang/Include/InitializeGlobals.h"
#include "glslang/Public/ShaderLang.h"
#include "glslang/Include/PoolAlloc.h" 

#include "os.h"

using namespace irr;
using namespace irr::video;

int ClientInputSemanticsVersion = 100;                  // maps to, say, #define VULKAN 100
glslang::EShTargetClientVersion VulkanClientVersion =
glslang::EShTargetVulkan_1_0; // would map to, say, Vulkan 1.0
glslang::EShTargetClientVersion OpenGLClientVersion =
glslang::EShTargetOpenGL_450; // doesn't influence anything yet, but maps to OpenGL 4.50
glslang::EShTargetLanguageVersion TargetVersion =
glslang::EShTargetSpv_1_0;    // maps to, say, SPIR-V 1.0

const TBuiltInResource DefaultTBuiltInResource = {
    /* .MaxLights = */ 32,
    /* .MaxClipPlanes = */ 6,
    /* .MaxTextureUnits = */ 32,
    /* .MaxTextureCoords = */ 32,
    /* .MaxVertexAttribs = */ 64,
    /* .MaxVertexUniformComponents = */ 4096,
    /* .MaxVaryingFloats = */ 64,
    /* .MaxVertexTextureImageUnits = */ 32,
    /* .MaxCombinedTextureImageUnits = */ 80,
    /* .MaxTextureImageUnits = */ 32,
    /* .MaxFragmentUniformComponents = */ 4096,
    /* .MaxDrawBuffers = */ 32,
    /* .MaxVertexUniformVectors = */ 128,
    /* .MaxVaryingVectors = */ 8,
    /* .MaxFragmentUniformVectors = */ 16,
    /* .MaxVertexOutputVectors = */ 16,
    /* .MaxFragmentInputVectors = */ 15,
    /* .MinProgramTexelOffset = */ -8,
    /* .MaxProgramTexelOffset = */ 7,
    /* .MaxClipDistances = */ 8,
    /* .MaxComputeWorkGroupCountX = */ 65535,
    /* .MaxComputeWorkGroupCountY = */ 65535,
    /* .MaxComputeWorkGroupCountZ = */ 65535,
    /* .MaxComputeWorkGroupSizeX = */ 1024,
    /* .MaxComputeWorkGroupSizeY = */ 1024,
    /* .MaxComputeWorkGroupSizeZ = */ 64,
    /* .MaxComputeUniformComponents = */ 1024,
    /* .MaxComputeTextureImageUnits = */ 16,
    /* .MaxComputeImageUniforms = */ 8,
    /* .MaxComputeAtomicCounters = */ 8,
    /* .MaxComputeAtomicCounterBuffers = */ 1,
    /* .MaxVaryingComponents = */ 60,
    /* .MaxVertexOutputComponents = */ 64,
    /* .MaxGeometryInputComponents = */ 64,
    /* .MaxGeometryOutputComponents = */ 128,
    /* .MaxFragmentInputComponents = */ 128,
    /* .MaxImageUnits = */ 8,
    /* .MaxCombinedImageUnitsAndFragmentOutputs = */ 8,
    /* .MaxCombinedShaderOutputResources = */ 8,
    /* .MaxImageSamples = */ 0,
    /* .MaxVertexImageUniforms = */ 0,
    /* .MaxTessControlImageUniforms = */ 0,
    /* .MaxTessEvaluationImageUniforms = */ 0,
    /* .MaxGeometryImageUniforms = */ 0,
    /* .MaxFragmentImageUniforms = */ 8,
    /* .MaxCombinedImageUniforms = */ 8,
    /* .MaxGeometryTextureImageUnits = */ 16,
    /* .MaxGeometryOutputVertices = */ 256,
    /* .MaxGeometryTotalOutputComponents = */ 1024,
    /* .MaxGeometryUniformComponents = */ 1024,
    /* .MaxGeometryVaryingComponents = */ 64,
    /* .MaxTessControlInputComponents = */ 128,
    /* .MaxTessControlOutputComponents = */ 128,
    /* .MaxTessControlTextureImageUnits = */ 16,
    /* .MaxTessControlUniformComponents = */ 1024,
    /* .MaxTessControlTotalOutputComponents = */ 4096,
    /* .MaxTessEvaluationInputComponents = */ 128,
    /* .MaxTessEvaluationOutputComponents = */ 128,
    /* .MaxTessEvaluationTextureImageUnits = */ 16,
    /* .MaxTessEvaluationUniformComponents = */ 1024,
    /* .MaxTessPatchComponents = */ 120,
    /* .MaxPatchVertices = */ 32,
    /* .MaxTessGenLevel = */ 64,
    /* .MaxViewports = */ 16,
    /* .MaxVertexAtomicCounters = */ 0,
    /* .MaxTessControlAtomicCounters = */ 0,
    /* .MaxTessEvaluationAtomicCounters = */ 0,
    /* .MaxGeometryAtomicCounters = */ 0,
    /* .MaxFragmentAtomicCounters = */ 8,
    /* .MaxCombinedAtomicCounters = */ 8,
    /* .MaxAtomicCounterBindings = */ 1,
    /* .MaxVertexAtomicCounterBuffers = */ 0,
    /* .MaxTessControlAtomicCounterBuffers = */ 0,
    /* .MaxTessEvaluationAtomicCounterBuffers = */ 0,
    /* .MaxGeometryAtomicCounterBuffers = */ 0,
    /* .MaxFragmentAtomicCounterBuffers = */ 1,
    /* .MaxCombinedAtomicCounterBuffers = */ 1,
    /* .MaxAtomicCounterBufferSize = */ 16384,
    /* .MaxTransformFeedbackBuffers = */ 4,
    /* .MaxTransformFeedbackInterleavedComponents = */ 64,
    /* .MaxCullDistances = */ 8,
    /* .MaxCombinedClipAndCullDistances = */ 8,
    /* .MaxSamples = */ 4,
    /* .limits = */{
    /* .nonInductiveForLoops = */ 1,
    /* .whileLoops = */ 1,
    /* .doWhileLoops = */ 1,
    /* .generalUniformIndexing = */ 1,
    /* .generalAttributeMatrixVectorIndexing = */ 1,
    /* .generalVaryingIndexing = */ 1,
    /* .generalSamplerIndexing = */ 1,
    /* .generalVariableIndexing = */ 1,
    /* .generalConstantMatrixVectorIndexing = */ 1,
} };

class GlslangFileIncluder : public glslang::TShader::Includer
{
    // Inherited via Includer
    virtual void releaseInclude(IncludeResult * result) override
    {
        if (result != nullptr)
        {
            //delete[] static_cast<tUserDataElement*>(result->userData);
            delete result;
        }
    }
};

EShLanguage GetCompileLang(E_ShaderTypes type)
{
    switch (type)
    {
        case E_ShaderTypes::EST_VERTEX_SHADER:      return EShLanguage::EShLangVertex;
        case E_ShaderTypes::EST_DOMAIN_SHADER:      return EShLanguage::EShLangTessEvaluation;
        case E_ShaderTypes::EST_HULL_SHADER:        return EShLanguage::EShLangTessControl;
        case E_ShaderTypes::EST_GEOMETRY_SHADER:    return EShLanguage::EShLangGeometry;
        case E_ShaderTypes::EST_FRAGMENT_SHADER:    return EShLanguage::EShLangFragment;
        case E_ShaderTypes::EST_COMPUTE_SHADER:     return EShLanguage::EShLangCompute;
    }

    return EShLanguage::EShLangCount;
}

//
// Translate the meaningful subset of command-line options to parser-behavior options.
//
void irr::video::CVulkanGLSLang::SetMessageOptions(irr::u32& messages)
{
    //if (Options & EOptionRelaxedErrors)
        messages = (EShMessages)(messages | EShMsgRelaxedErrors);
    //if (Options & EOptionIntermediate)
    //    messages = (EShMessages)(messages | EShMsgAST);
    //if (Options & EOptionSuppressWarnings)
    //    messages = (EShMessages)(messages | EShMsgSuppressWarnings);
    //if (Options & EOptionSpv)
        messages = (EShMessages)(messages | EShMsgSpvRules);
    //if (Options & EOptionVulkanRules)
        messages = (EShMessages)(messages | EShMsgVulkanRules);
    //if (Options & EOptionOutputPreprocessed)
    //    messages = (EShMessages)(messages | EShMsgOnlyPreprocessor);
    if (isHlsl)
        messages = (EShMessages)(messages | EShMsgReadHlsl);
    //if (Options & EOptionCascadingErrors)
    //    messages = (EShMessages)(messages | EShMsgCascadingErrors);
    //if (Options & EOptionKeepUncalled)
    //    messages = (EShMessages)(messages | EShMsgKeepUncalled);
    //if (Options & EOptionHlslOffsets)
    //    messages = (EShMessages)(messages | EShMsgHlslOffsets);
    //if (Options & EOptionDebug)
    //    messages = (EShMessages)(messages | EShMsgDebugInfo);
    //if (HlslEnable16BitTypes)
    //    messages = (EShMessages)(messages | EShMsgHlslEnable16BitTypes);
    //if ((Options & EOptionOptimizeDisable) || !ENABLE_OPT)
    //    messages = (EShMessages)(messages | EShMsgHlslLegalization);
}

irr::video::CVulkanGLSLang::CVulkanGLSLang(CVulkanDriver* driver)
    : Driver(driver)
{
    Resources = ::DefaultTBuiltInResource;

    for (int res = 0; res < glslang::EResCount; ++res)
        baseBinding[res].fill(0);
}

irr::video::CVulkanGLSLang::~CVulkanGLSLang()
{
    if (Program)
        delete Program;
    if (Reflection)
        delete Reflection;
}

bool irr::video::CVulkanGLSLang::CompileShader(VkShaderModule * modul, E_ShaderTypes type, System::IO::IFileReader * file, const char * entryPoint)
{
    EShMessages messages = EShMsgDefault;
    SetMessageOptions(*(u32*)&messages);
    EShLanguage stage = GetCompileLang(type);

    std::string source;
    source.resize(file->Size());
    file->Read((byte*)source.data(), file->Size());

    glslang::TProgram* program = new glslang::TProgram;
    {
        glslang::TShader* shader = new glslang::TShader(stage);
        const char* names = file->FileName.data();
        const char* text = source.data();
        shader->setStringsWithLengthsAndNames(&text, NULL, &names, 1);
        if (entryPoint)
            shader->setEntryPoint(entryPoint);
        //if (sourceEntryPointName)
        //    shader->setSourceEntryPoint(sourceEntryPointName);
        shader->addProcesses(Processes);

        // Set IO mapper binding shift values
        for (int r = 0; r < glslang::EResCount; ++r)
        {
            const glslang::TResourceType res = glslang::TResourceType(r);

            // Set base bindings
            shader->setShiftBinding(res, baseBinding[res][stage]);

            // Set bindings for particular resource sets
            // TODO: use a range based for loop here, when available in all environments.
            for (auto i = baseBindingForSet[res][stage].begin();
                i != baseBindingForSet[res][stage].end(); ++i)
                shader->setShiftBindingForSet(res, i->second, i->first);
        }

        //shader->setFlattenUniformArrays((Options & EOptionFlattenUniformArrays) != 0);
        //shader->setNoStorageFormat((Options & EOptionNoStorageFormat) != 0);
        shader->setResourceSetBinding(baseResourceSetBinding[stage]);

        if (isHlsl)
            shader->setHlslIoMapping(true);

        //shader->setAutoMapBindings(true);
        //shader->setAutoMapLocations(true);

        //if (Options & EOptionInvertY)
        //    shader->setInvertY(true);

        // Set up the environment, some subsettings take precedence over earlier
        // ways of setting things.
        if (!isOGL)
        {
            shader->setEnvInput(isHlsl ? glslang::EShSourceHlsl : glslang::EShSourceGlsl, stage, glslang::EShClientVulkan, ClientInputSemanticsVersion);
            shader->setEnvClient(glslang::EShClientVulkan, VulkanClientVersion);
        }
        else
        {
            shader->setEnvInput(isHlsl ? glslang::EShSourceHlsl : glslang::EShSourceGlsl, stage, glslang::EShClientOpenGL, ClientInputSemanticsVersion);
            shader->setEnvClient(glslang::EShClientOpenGL, OpenGLClientVersion);
        }

        shader->setEnvTarget(glslang::EShTargetSpv, TargetVersion);
        if (isHlsl)
            shader->setEnvTargetHlslFunctionality1();

        shaders.push_back(shader);

        const int defaultVersion = /*Options & EOptionDefaultDesktop ?*/ 110 /*: 100*/;

        GlslangFileIncluder includer;

        //if (Options & EOptionOutputPreprocessed)
        //{
        //    std::string str;
        //    if (shader->preprocess(&Resources, defaultVersion, ENoProfile, false, false,
        //        messages, &str, includer)) {
        //        PutsIfNonEmpty(str.c_str());
        //    }
        //    else {
        //        CompileFailed = true;
        //    }
        //    StderrIfNonEmpty(shader->getInfoLog());
        //    StderrIfNonEmpty(shader->getInfoDebugLog());
        //    continue;
        //}

        if (!shader->parse(&Resources, defaultVersion, false, messages, includer))
            CompileFailed = true;

        program->addShader(shader);

        {
            os::Printer::log(shader->getInfoLog());
            os::Printer::log(shader->getInfoDebugLog());
        }

    }

    if (CompileFailed)
        return false;

    //
    // Program-level processing...
    //

    // Link
    if (!program->link(messages))
        LinkFailed = true;

    if (!program->mapIO())
    {
        LinkFailed = true;
    }

    os::Printer::log(program->getInfoLog());
    os::Printer::log(program->getInfoDebugLog());

    if (LinkFailed)
        return false;

    // Build Reflection
    {
        Reflection = new glslang::TReflection;

        for (int s = 0; s < EShLangCount; ++s)
        {
            if (program->getIntermediate(EShLanguage(s)))
                if (!Reflection->addStage(EShLanguage(s), *program->getIntermediate(EShLanguage(s))))
                {
                    ReflectionFailed = true;
                    break;
                }
        }
    }

    std::vector<unsigned int> spirv;
    spv::SpvBuildLogger logger;

    glslang::SpvOptions spvOptions;
    spvOptions.generateDebugInfo = false;
    spvOptions.disableOptimizer = false;
    spvOptions.optimizeSize = false;
    //if (Options & EOptionDebug)
    //    spvOptions.generateDebugInfo = true;
    //spvOptions.disableOptimizer = (Options & EOptionOptimizeDisable) != 0;
    //spvOptions.optimizeSize = (Options & EOptionOptimizeSize) != 0;


    glslang::GlslangToSpv(*program->getIntermediate(stage), spirv, &logger, &spvOptions);

    printf("%s", logger.getAllMessages().c_str());

    VkShaderModuleCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.codeSize = spirv.size() * 4;
    createInfo.pCode = spirv.data();

    return vkCreateShaderModule(Driver->_getPrimaryDevice()->getLogical(), &createInfo, VulkanDevice::gVulkanAllocator, modul) == VkResult::VK_SUCCESS;
}

namespace glslang
{

    // Workaround for OSDepend includes not install glslang
    //
    // Thread Local Storage Operations
    //
    typedef void* OS_TLSIndex;
#define OS_INVALID_TLS_INDEX ((void*)0)

    OS_TLSIndex OS_AllocTLSIndex();
    bool        OS_SetTLSValue(OS_TLSIndex nIndex, void *lpvValue);
    bool        OS_FreeTLSIndex(OS_TLSIndex nIndex);
    void*       OS_GetTLSValue(OS_TLSIndex nIndex);

    void InitGlobalLock();
    void GetGlobalLock();
    void ReleaseGlobalLock();

    typedef unsigned int(*TThreadEntrypoint)(void*);

    void OS_CleanupThreadData(void);
    bool InitThread();

    OS_TLSIndex ThreadInitializeIndex = OS_INVALID_TLS_INDEX;

    // Per-process initialization.
    // Needs to be called at least once before parsing, etc. is done.
    // Will also do thread initialization for the calling thread; other
    // threads will need to do that explicitly.
    bool InitProcess()
    {
        glslang::GetGlobalLock();

        if (ThreadInitializeIndex != OS_INVALID_TLS_INDEX) {
            //
            // Function is re-entrant.
            //

            glslang::ReleaseGlobalLock();
            return true;
        }

        ThreadInitializeIndex = OS_AllocTLSIndex();

        if (ThreadInitializeIndex == OS_INVALID_TLS_INDEX) {
            assert(0 && "InitProcess(): Failed to allocate TLS area for init flag");

            glslang::ReleaseGlobalLock();
            return false;
        }

        if (!InitializePoolIndex()) {
            assert(0 && "InitProcess(): Failed to initialize global pool");

            glslang::ReleaseGlobalLock();
            return false;
        }

        if (!InitThread()) {
            assert(0 && "InitProcess(): Failed to initialize thread");

            glslang::ReleaseGlobalLock();
            return false;
        }

        glslang::ReleaseGlobalLock();
        return true;
    }

    // Per-thread scoped initialization.
    // Must be called at least once by each new thread sharing the
    // symbol tables, etc., needed to parse.
    bool InitThread()
    {
        //
        // This function is re-entrant
        //
        if (ThreadInitializeIndex == OS_INVALID_TLS_INDEX) {
            assert(0 && "InitThread(): Process hasn't been initalised.");
            return false;
        }

        if (OS_GetTLSValue(ThreadInitializeIndex) != 0)
            return true;

        if (!OS_SetTLSValue(ThreadInitializeIndex, (void *)1)) {
            assert(0 && "InitThread(): Unable to set init flag.");
            return false;
        }

        glslang::SetThreadPoolAllocator(nullptr);

        return true;
    }

    // Not necessary to call this: InitThread() is reentrant, and the need
    // to do per thread tear down has been removed.
    //
    // This is kept, with memory management removed, to satisfy any exiting
    // calls to it that rely on it.
    bool DetachThread()
    {
        bool success = true;

        if (ThreadInitializeIndex == OS_INVALID_TLS_INDEX)
            return true;

        //
        // Function is re-entrant and this thread may not have been initialized.
        //
        if (OS_GetTLSValue(ThreadInitializeIndex) != 0) {
            if (!OS_SetTLSValue(ThreadInitializeIndex, (void *)0)) {
                assert(0 && "DetachThread(): Unable to clear init flag.");
                success = false;
            }
        }

        return success;
    }

    // Not necessary to call this: InitProcess() is reentrant.
    //
    // This is kept, with memory management removed, to satisfy any exiting
    // calls to it that rely on it.
    //
    // Users of glslang should call shFinalize() or glslang::FinalizeProcess() for
    // process-scoped memory tear down.
    bool DetachProcess()
    {
        bool success = true;

        if (ThreadInitializeIndex == OS_INVALID_TLS_INDEX)
            return true;

        success = DetachThread();

        OS_FreeTLSIndex(ThreadInitializeIndex);
        ThreadInitializeIndex = OS_INVALID_TLS_INDEX;

        return success;
    }
} // end namespace glslang  
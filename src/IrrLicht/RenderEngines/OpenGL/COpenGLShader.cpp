#include "COpenGLDriver.h"
#include "COpenGLShader.h"
#include "os.h"
#include "COpenGLSupport.h"
#include "COpenGLExtensionHandler.h"
#include "COpenGLMaterialRenderer.h"

#ifdef _IRR_COMPILE_WITH_OSX_DEVICE_
#include "MacOSX/CIrrDeviceMacOSX.h"
#endif

#ifdef _IRR_COMPILE_WITH_SDL_DEVICE_
#include <SDL/SDL.h>
#endif

using namespace irr;
using namespace irr::video;

static int mBindedProgram = 0;

GLArbShader::GLArbShader(video::IVideoDriver* context, E_ShaderTypes type, u32 _programId) : GLShader(context, type, _programId)
{
    switch (type)
    {
        case EST_VERTEX_SHADER:
            glShaderType = GL_VERTEX_PROGRAM_ARB;
            break;
        case EST_FRAGMENT_SHADER:
            glShaderType = GL_FRAGMENT_PROGRAM_ARB;
            break;
        default:
            throw std::runtime_error("shader not found");
    }
}

void GLArbShader::addShaderFile(u32 _glShaderType, const char *pFileName)
{
    std::string source;
    if (!readFile(pFileName, source))
        throw std::runtime_error("shader not found");

    video::COpenGLDriver* driverOGL = getDriverOGL();

    driverOGL->extGlGenPrograms(1, &programId);
    driverOGL->extGlBindProgram(glShaderType, programId);
    driverOGL->extGlProgramString(glShaderType, GL_PROGRAM_FORMAT_ASCII_ARB, (GLsizei)strlen(source.c_str()), source.c_str());

    auto error = glGetError();
    if (error != GL_NO_ERROR)
    {
        int errpos;
        glGetIntegerv(GL_PROGRAM_ERROR_POSITION_ARB, &errpos);
        printf("Error loading shader: %s\nError position: %d\n", glGetString(GL_PROGRAM_ERROR_STRING_ARB), errpos);
        throw std::runtime_error("shader not found");
    }
}

void GLArbShader::destroy()
{
    if (programId)
    {
        video::COpenGLDriver* driverOGL = getDriverOGL();
        unbind();
        driverOGL->extGlDeleteProgram(programId);
    }
}

COpenGLDriver * irr::video::GLShader::getDriverOGL()
{
    return static_cast<COpenGLDriver*>(getVideoDriver());
}

void GLArbShader::bind()
{
}

void GLArbShader::unbind()
{
}

GLSLGpuShader::GLSLGpuShader(video::IVideoDriver* context, E_ShaderTypes type, u32 _programId) : GLShader(context, type, _programId)
{
    //switch (type)
    //{
    //    case EST_VERTEX_SHADER:
    //        glShaderType = GL_VERTEX_SHADER;
    //        break;
    //    case EST_FRAGMENT_SHADER:
    //        glShaderType = GL_FRAGMENT_SHADER;
    //        break;
    //    case EST_GEOMETRY_SHADER:
    //        glShaderType = GL_GEOMETRY_SHADER;
    //        break;
    //    case EST_TESSELLATION_SHADER:
    //        //glShaderType = GL_TESSELLATION_SHADER;
    //        //break;
    //    case EST_COMPUTE_SHADER:
    //    default:
    //        break;
    //}
}

GLSLGpuShader::~GLSLGpuShader()
{
    destroy();
}

void GLSLGpuShader::destroy()
{
    getVideoDriver()->deleteShader(this);
}


void GLSLGpuShader::addShaderFile(GLSLGpuShader* gpuProgram, u32 shaderType, System::IO::IFileReader *file, std::list<u32>& ShaderObjList)
{
    std::string source;
    source.resize(file->Size());
    file->Read((byte*)source.data(), file->Size());

    GLuint ShaderObj = getDriverOGL()->extGlCreateShader(shaderType);

    if (ShaderObj == 0)
    {
        printf("Error creating shader type %d\n", shaderType);
        throw std::runtime_error("shader not found");
    }

    ShaderObjList.push_back(ShaderObj);

    const GLchar* p[1];
    p[0] = source.c_str();
    GLint Lengths[1] = { (GLint)source.size() };

    getDriverOGL()->extGlShaderSource(ShaderObj, 1, p, Lengths);
    getDriverOGL()->extGlCompileShader(ShaderObj);

    GLint success;
    getDriverOGL()->extGlGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);

    if (success == 0)
    {
        GLchar InfoLog[2048];
        getDriverOGL()->extGlGetShaderInfoLog(ShaderObj, 2048, NULL, InfoLog);
        printf("Error loading shader: %s\nError position: %s\n", InfoLog, file->FileName.c_str());
        throw std::runtime_error("shader not found");
    }

    getDriverOGL()->extGlAttachShader(gpuProgram->getProgramId(), ShaderObj);
    _IRR_DEBUG_BREAK_IF(glGetError() != GL_NO_ERROR);
}

void GLSLGpuShader::compile(GLSLGpuShader* gpuProgram, std::list<u32>& ShaderObjList)
{
    GLint Success = 0;
    GLchar ErrorLog[5000] = { 0 };

    getDriverOGL()->extGlLinkProgram(gpuProgram->getProgramId());

    getDriverOGL()->extGlGetProgramiv(gpuProgram->getProgramId(), GL_LINK_STATUS, &Success);
    if (Success == 0)
    {
        GLint maxLength = 0;
        getDriverOGL()->extGlGetShaderiv(gpuProgram->getProgramId(), GL_INFO_LOG_LENGTH, &maxLength);

        getDriverOGL()->extGlGetProgramInfoLog(gpuProgram->getProgramId(), sizeof(ErrorLog), &maxLength, ErrorLog);
        printf("Error linking shader program: '%s'\n", ErrorLog);
        throw std::runtime_error("shader not found");
    }

    getDriverOGL()->extGLValidateProgram(gpuProgram->getProgramId());
    getDriverOGL()->extGlGetProgramiv(gpuProgram->getProgramId(), GL_VALIDATE_STATUS, &Success);
    if (!Success)
    {
        getDriverOGL()->extGlGetProgramInfoLog(gpuProgram->getProgramId(), sizeof(ErrorLog), NULL, ErrorLog);
        printf("Error linking shader program: '%s'\n", ErrorLog);
        throw std::runtime_error("shader not found");
    }

    // Delete the intermediate shader objects that have been added to the program
    for (auto it : ShaderObjList)
        getDriverOGL()->extGlDeleteShader(it);
    
    ShaderObjList.clear();

    _IRR_DEBUG_BREAK_IF(glGetError() != GL_NO_ERROR);
}

void GLSLReflectUniform(GLSLGpuShader* shader, std::vector<CGlslBufferDesc>& buffers)
{
    GLint num = 0;
    shader->getDriverOGL()->extGlGetProgramiv(shader->getProgramId(), GL_ACTIVE_UNIFORMS, &num);
    if (num == 0)
        return;

    GLint maxlen = 0;
    shader->getDriverOGL()->extGlGetProgramiv(shader->getProgramId(), GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxlen);
    if (maxlen == 0)
    {
        os::Printer::log("GLSL: failed to retrieve uniform information", ELL_ERROR);
        return;
    }

    c8 buf[128];
    shader->TextureUnits.reserve(num);

    for (GLint i = 0; i < num; ++i)
    {
        buffers.emplace_back();
        CGlslBufferDesc& desc = buffers.back();
        memset(buf, 0, maxlen);

        desc.varDesc.m_type = ESVT_UNIFORM;
        desc.varDesc.m_length = 0;

        GLint Elements;
        GLint lenght;
        shader->getDriverOGL()->extGlGetActiveUniform(shader->getProgramId(), i, 127, &lenght, &Elements, &desc.varDesc.m_variableType, reinterpret_cast<GLchar*>(buf));

        //                          0           1             2           3               4               5                   6                 7 
        GLenum memberProps[] = { GL_TYPE, GL_ARRAY_SIZE, GL_OFFSET, GL_BLOCK_INDEX, GL_ARRAY_STRIDE, GL_MATRIX_STRIDE, GL_IS_ROW_MAJOR, GL_LOCATION };

        GLint memberDescParams[50];
        GLint memberParamsDescLength = 0;
        glGetProgramResourceiv(shader->getProgramId(), GL_UNIFORM, i, 8, memberProps, sizeof(memberDescParams), &memberParamsDescLength, memberDescParams);

        //desc.offset = memberDescParams[2];
        //desc.blockIndex = memberDescParams[3];
        //desc.arrayStride = memberDescParams[4];
        //desc.matrixStride = memberDescParams[5];
        //desc.elementSize = 0;
        desc.dataSize = 0; // memberDescParams[8];
        //desc.isRowMajor = memberDescParams[6];
        desc.binding = memberDescParams[7];                       // Buffer Variable Index
        desc.varDesc.m_length = memberDescParams[1];
        desc.varDesc.m_variableType = memberDescParams[0];
        desc.varDesc.m_shaderIndex = shader->ShaderBuffers.size() - 1;
        desc.varDesc.m_location = i;
        desc.varDesc.m_length = Elements;
        desc.varDesc.m_name = buf;
        desc.varDesc.m_location = shader->getDriverOGL()->pGLGetUniformLocation(shader->getProgramId(), buf);

        if (desc.varDesc.m_variableType >= GL_SAMPLER_1D && desc.varDesc.m_variableType <= GL_SAMPLER_2D_SHADOW)
        {
            if (shader->TextureUnits.size() <= i)
                shader->TextureUnits.resize(i + 1);
            shader->TextureUnits[i] = desc;
            shader->AddShaderVariable(&shader->TextureUnits[i].varDesc);
        }
        else
            shader->AddShaderVariable(&desc.varDesc);
    }
}

void GLSLReflectAttributes(GLSLGpuShader* shader, std::vector<CGlslBufferDesc>& buffers)
{
    GLint num = 0;
    shader->getDriverOGL()->extGlGetProgramiv(shader->getProgramId(), GL_ACTIVE_ATTRIBUTES, &num);
    if (num == 0)
        return;

    GLint maxlen = 0;
    shader->getDriverOGL()->extGlGetProgramiv(shader->getProgramId(), GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxlen);
    if (maxlen == 0)
    {
        os::Printer::log("GLSL: failed to retrieve uniform information", ELL_ERROR);
        return;
    }

    c8 buf[128];

    buffers.reserve(num);
    for (GLint i = 0; i < num; ++i)
    {
        buffers.emplace_back();
        CGlslBufferDesc& desc = buffers.back();
        memset(buf, 0, maxlen);

        desc.varDesc.m_type = ESVT_ATTRIBUTE;
        //ui.m_basicVariableLocation = EVertexSemanticType(0);
        desc.varDesc.m_length = 0;

        GLint size;
        shader->getDriverOGL()->pGLGetActiveAttrib(shader->getProgramId(), i, 127, 0, &size, &desc.varDesc.m_variableType, reinterpret_cast<GLchar*>(buf));
        desc.varDesc.m_name = buf;
        desc.varDesc.m_location = shader->getDriverOGL()->pGLGetAttribLocation(shader->getProgramId(), buf);
        desc.varDesc.m_shaderIndex = buffers.size() - 1;

        //shader->AddShaderVariable(&desc.varDesc);
    }
}

void GLSLReflectUniformBlocks(GLSLGpuShader* shader, std::vector<CGlslBufferDesc>& buffers)
{
    GLint num = 0;
    shader->getDriverOGL()->extGlGetProgramiv(shader->getProgramId(), GL_ACTIVE_UNIFORM_BLOCKS, &num);
    if (num == 0)
        return;
    
    GLint maxlen = 0;
    shader->getDriverOGL()->extGlGetProgramiv(shader->getProgramId(), GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &maxlen);
    if (maxlen == 0)
    {
        os::Printer::log("GLSL: failed to retrieve uniform information", ELL_ERROR);
        return;
    }
    
    c8 buf[128];
    
    for (GLint i = 0; i < num; ++i)
    {
        buffers.emplace_back();
        CGlslBufferDesc& desc = buffers.back();
        memset(buf, 0, maxlen);
    
        desc.varDesc.m_type = ESVT_UNIFORM;
        //ui.m_basicVariableLocation = EVertexSemanticType(0);
        desc.varDesc.m_length = 0;
    
        GLint size;
        glGetActiveUniformBlockName(shader->getProgramId(), i, 127, &size, reinterpret_cast<GLchar*>(buf));
        desc.varDesc.m_name = buf;
        desc.varDesc.m_location = shader->getDriverOGL()->pGLGetAttribLocation(shader->getProgramId(), buf);
        desc.varDesc.m_shaderIndex = buffers.size() - 1;
    
        shader->AddShaderVariable(&desc.varDesc);
    }
}

void GLSLReflectStorageBlocks(GLSLGpuShader* shader, std::vector<CGlslBufferDesc>& buffers)
{
    GLint bufferCount = 0;
    glGetProgramInterfaceiv(shader->getProgramId(), GL_SHADER_STORAGE_BLOCK, GL_ACTIVE_RESOURCES, &bufferCount);
    if (bufferCount == 0)
        return;

    GLint maxlen = 0;
    glGetProgramInterfaceiv(shader->getProgramId(), GL_SHADER_STORAGE_BLOCK, GL_MAX_NAME_LENGTH, &maxlen);
    if (maxlen == 0)
    {
        os::Printer::log("GLSL: failed to retrieve uniform information", ELL_ERROR);
        return;
    }

        //GLuint index = glGetProgramResourceIndex(shader->getProgramId(), GL_SHADER_STORAGE_BLOCK, "MatrixBuffer");

    // seems that some implementations use an extra null terminator
    GLint Bufsize;
    c8 buf[128];

    for (GLint i = 0; i < bufferCount; ++i)
    {
        buffers.emplace_back();
        CGlslBufferDesc& desc = buffers.back();
        memset(buf, 0, maxlen);

        glGetProgramResourceName(shader->getProgramId(), GL_SHADER_STORAGE_BLOCK, i, 127, &Bufsize, buf);

        desc.varDesc.m_name = buf;
        desc.varDesc.m_location = i;
        desc.varDesc.m_shaderIndex = buffers.size() - 1;
        desc.varDesc.m_type = ESVT_CONSTANT;

        GLenum props[] = { GL_BUFFER_BINDING, GL_BUFFER_DATA_SIZE, GL_NUM_ACTIVE_VARIABLES, GL_ACTIVE_VARIABLES };

        GLint memberParams[50];
        GLint memberParamsLength = 0;
        glGetProgramResourceiv(shader->getProgramId(), GL_SHADER_STORAGE_BLOCK, i, 4, props, sizeof(memberParams), &memberParamsLength, memberParams);

        desc.binding = memberParams[0];
        desc.dataSize = memberParams[1];

        desc.DataBuffer.set_used(desc.dataSize);
        memset(desc.DataBuffer.pointer(), 0, desc.DataBuffer.size());
        desc.ChangeId = 1;

        if (memberParamsLength >= 3)
        {
            std::map<std::string, std::vector<CGlslVariableDesc*>> structs;
            desc.members.resize(memberParams[2]);
            for (GLint m = 0; m < memberParams[2]; ++m)
            {
                //desc.members.emplace_back();
                CGlslVariableDesc& structDecl = desc.members[m];
                structDecl.BufferDesc = &desc;

                GLint memberId = memberParams[m + 3];

                glGetProgramResourceName(shader->getProgramId(), GL_BUFFER_VARIABLE, memberId, 127, &Bufsize, buf);

                //                          0           1             2           3               4               5                   6                 7                         8
                GLenum memberProps[] = { GL_TYPE, GL_ARRAY_SIZE, GL_OFFSET, GL_BLOCK_INDEX, GL_ARRAY_STRIDE, GL_MATRIX_STRIDE, GL_IS_ROW_MAJOR, GL_TOP_LEVEL_ARRAY_SIZE, GL_TOP_LEVEL_ARRAY_STRIDE };

                GLint memberDescParams[50];
                GLint memberParamsDescLength = 0;
                glGetProgramResourceiv(shader->getProgramId(), GL_BUFFER_VARIABLE, memberId, 9, memberProps, sizeof(memberDescParams), &memberParamsDescLength, memberDescParams);

                structDecl.varDesc.m_name = buf;
                structDecl.offset = memberDescParams[2];
                structDecl.blockIndex = memberDescParams[3];
                structDecl.arrayStride = memberDescParams[4];
                structDecl.matrixStride = memberDescParams[5];
                structDecl.elementSize = 0;
                structDecl.dataSize = 0;
                structDecl.isRowMajor = memberDescParams[6];
                structDecl.toplevel_arraysize = memberDescParams[7];
                structDecl.toplevel_arraystride = memberDescParams[8];
                structDecl.varDesc.m_location = 0;                          // Buffer Variable Index
                structDecl.varDesc.m_length = memberDescParams[1];
                structDecl.varDesc.m_variableType = memberDescParams[0];
                structDecl.varDesc.m_class = 0;
                structDecl.varDesc.m_size = 0;
                structDecl.varDesc.m_type = ESVT_CONSTANT;
                structDecl.varDesc.m_shaderIndex = desc.varDesc.m_shaderIndex; // Buffer Desc Index

                std::string name = desc.varDesc.m_name;
                name += '.';
                name += structDecl.varDesc.m_name;
                structDecl.name = name;
            }

            std::sort(desc.members.begin(), desc.members.end(), [](CGlslVariableDesc const& lhs, CGlslVariableDesc const& rhs)
            {
                return lhs.offset < rhs.offset;
            });

            for (auto& structDecl : desc.members)
            {
                size_t dotIndex = structDecl.varDesc.m_name.find_last_of('.');
                if (dotIndex != std::string::npos)
                {

                    auto structName = structDecl.varDesc.m_name.substr(0, dotIndex);
                    size_t trimmedArraySuffixIndex = structName.find_last_of('[', dotIndex);
                    if (trimmedArraySuffixIndex != std::string::npos)
                        structName = structName.substr(0, trimmedArraySuffixIndex);
                    structs[structName].push_back(&structDecl);
                }
            }

            for (int m = 0; m < desc.members.size() - 1; ++m)
            {
                desc.members[m].varDesc.m_location = m;                          // Buffer Variable Index
                desc.members[m].dataSize = desc.members[m + 1].offset - desc.members[m].offset;
                if (desc.members[m].toplevel_arraysize > 1)  //ToDo: this wrong
                    desc.members[m].elementSize = desc.members[m].toplevel_arraystride;
                else
                    desc.members[m].elementSize = desc.members[m].dataSize / desc.members[m].varDesc.m_length;

                shader->AddShaderVariable(&desc.members[m].varDesc);
            }

            desc.members.back().varDesc.m_location = desc.members.size() - 1;              // Buffer Variable Index
            if (desc.members.back().toplevel_arraysize > 1)
            {
                // ToDo: need implement for intermediate arrays
                int nextArrayElementOffset = desc.dataSize - desc.members.back().toplevel_arraystride * (desc.members.back().toplevel_arraysize - 1);
                desc.members.back().dataSize = desc.members.back().toplevel_arraystride * desc.members.back().toplevel_arraysize;
                desc.members.back().elementSize = nextArrayElementOffset - desc.members.back().offset; //desc.members.back().dataSize / desc.members.back().varDesc.m_length; //ToDo: this wrong
                shader->AddShaderVariable(&desc.members.back().varDesc);
            }
            else
            {
                desc.members.back().dataSize = desc.dataSize - desc.members.back().offset;
                desc.members.back().elementSize = desc.members.back().dataSize / desc.members.back().varDesc.m_length; //ToDo: this wrong
                shader->AddShaderVariable(&desc.members.back().varDesc);
            }

            if (!structs.empty())
            {
                std::vector<CGlslVariableDesc> structEntries;
                for (auto& structEntry : structs)
                {
                    if (structEntry.second.empty())
                        continue;

                    size_t offset = std::numeric_limits<size_t>::max();
                    size_t size = 0;
                    if (structEntry.second.front()->toplevel_arraysize > 1)
                    {
                        size = structEntry.second.front()->toplevel_arraysize * structEntry.second.front()->toplevel_arraystride;
                        for (CGlslVariableDesc* variable : structEntry.second)
                        {
                            offset = std::min(offset, (size_t)variable->offset);
                        }
                    }
                    else
                    {
                        for (CGlslVariableDesc* variable : structEntry.second)
                        {
                            size += variable->dataSize;
                            offset = std::min(offset, (size_t)variable->offset);
                        }
                    }

                    structEntries.emplace_back();
                    CGlslVariableDesc& structDecl = structEntries.back();

                    structDecl.varDesc.m_name = structEntry.first;
                    structDecl.offset = offset;
                    structDecl.blockIndex = 0;
                    structDecl.arrayStride = 0;
                    structDecl.matrixStride = 0;
                    structDecl.elementSize = size / structEntry.second.front()->toplevel_arraysize;
                    structDecl.dataSize = size;
                    structDecl.isRowMajor = 0;
                    structDecl.toplevel_arraysize = structEntry.second.front()->toplevel_arraysize;
                    structDecl.toplevel_arraystride = structEntry.second.front()->toplevel_arraystride;
                    structDecl.varDesc.m_location = 0;                          // Buffer Variable Index
                    structDecl.varDesc.m_length = 0;
                    structDecl.varDesc.m_variableType = 0;
                    structDecl.varDesc.m_class = 0;
                    structDecl.varDesc.m_size = 0;
                    structDecl.varDesc.m_type = ESVT_CONSTANT;
                    structDecl.varDesc.m_shaderIndex = desc.varDesc.m_shaderIndex; // Buffer Desc Index

                    std::string name = desc.varDesc.m_name;
                    name += '.';
                    name += structDecl.varDesc.m_name;
                    structDecl.name = name;
                }

                desc.members.resize(desc.members.size() + structEntries.size());
                for (int i = memberParams[2]; i < (memberParams[2] + structEntries.size()); ++i)
                {
                    desc.members[i] = structEntries[i - memberParams[2]];
                    desc.members[i].varDesc.m_location = i;

                    shader->AddShaderVariable(&desc.members[i].varDesc);
                }
            }
        }

        desc.m_constantBuffer = new COpenGLHardwareBuffer(shader->getDriverOGL(), E_HARDWARE_BUFFER_TYPE::EHBT_CONSTANTS, E_HARDWARE_BUFFER_ACCESS::EHBA_DEFAULT, 0, desc);
    }
}

void GLSLGpuShader::buildShaderVariableDescriptor()
{
    GLint total = 0, num = 0;
    glGetProgramInterfaceiv(getProgramId(), GL_SHADER_STORAGE_BLOCK, GL_ACTIVE_RESOURCES, &num);
    total += num;
    getDriverOGL()->extGlGetProgramiv(getProgramId(), GL_ACTIVE_UNIFORM_BLOCKS, &num);
    total += num;
    getDriverOGL()->extGlGetProgramiv(getProgramId(), GL_ACTIVE_UNIFORMS, &num);
    total += num;
    ShaderBuffers.reserve(total);

    GLSLReflectStorageBlocks(this, ShaderBuffers);
    // Find other legacy binding
    GLSLReflectUniform(this, ShaderBuffers);
    GLSLReflectUniformBlocks(this, ShaderBuffers);

    // Find Attributes
    GLSLReflectAttributes(this, ShaderAttributes);

}

//void GLSLGpuShader::CommitValues(IShaderDataBuffer::E_UPDATE_TYPE updateType)
//{
//    IShader::CommitValues(updateType);
//}

void GLSLGpuShader::bind()
{
    _IRR_DEBUG_BREAK_IF(getDriverOGL()->GetBindedProgramId() && getDriverOGL()->GetBindedProgramId() != getProgramId());
    //if (!mBinded)
    {
        mBinded = true;
        getVideoDriver()->useShader(this);
        for (auto unit : this->TextureUnits)
            glUniform1i(unit.varDesc.m_location, unit.binding);
    }
}

void GLSLGpuShader::unbind()
{
    //if (mBinded)
    {
        mBinded = false;
        _IRR_DEBUG_BREAK_IF(getDriverOGL()->GetBindedProgramId() && getDriverOGL()->GetBindedProgramId() != getProgramId());
        getVideoDriver()->useShader(nullptr);
    }
}

//void GLSLGpuShader::BindBufferData(u32 loc, size_t size, void* data, u32 misc)
//{
//    getDriverOGL()->extGlBindBuffer(GL_ARRAY_BUFFER, loc);
//    getDriverOGL()->extGlBufferData(GL_ARRAY_BUFFER, size, data, misc);
//}

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
    COpenGLDriver* Driver = static_cast<COpenGLDriver*>(gpuProgram->getVideoDriver());
    SMaterial& CurrentMaterial = !meshBuffer ? Driver->GetMaterial() : meshBuffer->getMaterial();
    video::IMaterialRenderer* rnd = Driver->getMaterialRenderer(CurrentMaterial.MaterialType);

    *world = Driver->getTransform(ETS_WORLD);
    *view = Driver->getTransform(ETS_VIEW);
    *projection = Driver->getTransform(ETS_PROJECTION);

    *AlphaRef = CurrentMaterial.MaterialType != EMT_ONETEXTURE_BLEND && CurrentMaterial.MaterialTypeParam ? CurrentMaterial.MaterialTypeParam : 0.5f;
    *AlphaTest =(rnd && rnd->isTransparent());
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
            memset(&l, 0, sizeof(ShaderGenericValuesBuffer::Light));
            SLight dl = Driver->getDynamicLight(i);
            l.Position = dl.Position;
            l.Atten = dl.Attenuation;
            l.Ambient = dl.AmbientColor;
            l.Diffuse = dl.DiffuseColor;
            l.Specular = dl.SpecularColor;
            *ShaderLight += l;
        }
    }

    ShaderGenericValuesBuffer::Material material;
    memset(&material, 0, sizeof(ShaderGenericValuesBuffer::Material));

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

/// OpenGL Driver Shader Spec

IShader* COpenGLDriver::createShader(System::IO::IFileReader* vertexShader, System::IO::IFileReader* fragmentShader, System::IO::IFileReader* geometryShader, System::IO::IFileReader* tesselationShader)
{
    std::list<u32> objectList;
    u32 programId = extGlCreateProgram();

    if (programId == 0)
    {
        printf("Error creating shader program\n");
        throw std::runtime_error("shader not found");
    }

    GLSLGpuShader* gpuProgram = new GLSLGpuShader(this, EST_HIGH_LEVEL_SHADER, programId);
    if (vertexShader)
        gpuProgram->addShaderFile(gpuProgram, GL_VERTEX_SHADER, vertexShader, objectList);

    if (geometryShader)
        gpuProgram->addShaderFile(gpuProgram, GL_GEOMETRY_SHADER, geometryShader, objectList);

    if (fragmentShader)
        gpuProgram->addShaderFile(gpuProgram, GL_FRAGMENT_SHADER, fragmentShader, objectList);

    //addShaderFile(gpuProgram, GL_TESSELLATION_SHADER, tesselationShader, objectList);
    gpuProgram->compile(gpuProgram, objectList);

    gpuProgram->buildShaderVariableDescriptor();
    gpuProgram->Init();
    return gpuProgram;
}

s32 COpenGLDriver::getShaderVariableID(IShader* gpuProgram, const c8* name)
{
    if (gpuProgram->getShaderVersion() == ESV_GLSL_HIGH_LEVEL)
    {
        GLSLGpuShader* glsl = (GLSLGpuShader*)gpuProgram;
        s32 m_location = pGLGetUniformLocation(glsl->getProgramId(), name);
        if (m_location == -1)
            m_location = pGLGetAttribLocation(glsl->getProgramId(), name);
        return m_location;
    }
    return -1;
}


void COpenGLDriver::useShader(IShader* gpuProgram)
{
    if (gpuProgram)
    {
        if (gpuProgram->getShaderVersion() == ESV_GLSL_HIGH_LEVEL)
        {
            GLSLGpuShader* glsl = static_cast<GLSLGpuShader*>(gpuProgram);
            //if (mBindedProgram != glsl->getProgramId())
            {
                _IRR_DEBUG_BREAK_IF(mBindedProgram);
                mBindedProgram = glsl->getProgramId();
                extGlUseProgram(glsl->getProgramId());
            }
        }
        else // ASM Shader
        {
            //_IRR_DEBUG_BREAK_IF(mBindedProgram);
            //mBindedProgram = programId;
            //getDriverOGL()->extGlBindProgram(glShaderType, programId);
            //GetStateCache()->setEnabled(glShaderType, true);

            //_IRR_DEBUG_BREAK_IF(!mBindedProgram || mBindedProgram != programId);
            //mBindedProgram = 0;
            //getDriverOGL()->extGlBindProgram(glShaderType, 0);
            //GetStateCache()->setEnabled(glShaderType, false);
        }
    }
    else
    {
        mBindedProgram = 0;
        extGlUseProgram(0);

        for (auto& buffer : BindedBuffers)
        {
            if (!buffer)
                continue;

            buffer->Unbind();
            buffer = nullptr;
        }
    }
}

void COpenGLDriver::deleteShader(IShader* gpuProgram)
{
    if (gpuProgram->getShaderVersion() == ESV_GLSL_HIGH_LEVEL)
    {
        GLSLGpuShader* glsl = (GLSLGpuShader*)gpuProgram;
        if (glsl->getProgramId())
            if (mBindedProgram == glsl->getProgramId())
                useShader(nullptr);
        extGlDeleteProgram(glsl->getProgramId());
    }
}

core::array<u8> COpenGLDriver::GetShaderVariableMemoryBlock(ShaderVariableDescriptor const* desc, video::IShader* shader)
{
    irr::video::GLSLGpuShader* glsl = static_cast<irr::video::GLSLGpuShader*>(shader);

    irr::video::CGlslBufferDesc& ubuffer = glsl->ShaderBuffers[desc->m_shaderIndex];
    irr::video::CGlslVariableDesc const& constantDecl = ubuffer.members[desc->m_location & 0xFF];

    core::array<u8> result;
    result.set_pointer((u8*)&ubuffer.DataBuffer[constantDecl.offset], constantDecl.dataSize, false, false);
    return result;
}

bool COpenGLDriver::setShaderConstant(ShaderVariableDescriptor const* desc, const void* values, int count, IHardwareBuffer* buffer /*= nullptr*/)
{
    if (!desc || desc->m_location < 0 || !values || !count)
        return false;

    irr::video::GLSLGpuShader* glsl = static_cast<irr::video::GLSLGpuShader*>(getActiveGPUProgram());

    // Attribute never can be modify here
    if (desc->m_type == ESVT_INPUT_STREAM || desc->m_type == ESVT_ATTRIBUTE)
        return false;

    // Modern Buffers (Directx 11 like)
    if (desc->m_type == ESVT_CONSTANT)
    {
        if (!glsl)
            return false;

        irr::video::CGlslBufferDesc& ubuffer = glsl->ShaderBuffers[desc->m_shaderIndex];
        irr::video::CGlslVariableDesc const& constantDecl = ubuffer.members[desc->m_location & 0xFF];

        UINT elementSize = constantDecl.elementSize * count;

        _IRR_DEBUG_BREAK_IF(elementSize > constantDecl.dataSize || (ubuffer.dataSize < (constantDecl.offset + elementSize)));

        if (memcmp(&ubuffer.DataBuffer[constantDecl.offset], values, elementSize))
        {
            if (ubuffer.ChangeStartOffset > constantDecl.offset)
                ubuffer.ChangeStartOffset = constantDecl.offset;

            if (ubuffer.ChangeEndOffset < (constantDecl.offset + elementSize))
                ubuffer.ChangeEndOffset = (constantDecl.offset + elementSize);

            memcpy(&ubuffer.DataBuffer[constantDecl.offset], values, elementSize);
            ++ubuffer.ChangeId;
        }
        return true;
    }

    // Legacy uniform set

    bool status = true;

    switch (desc->m_variableType)
    {
        case GL_FLOAT:
            if (desc->m_type == ESVT_UNIFORM)
                extGlUniform1fv(desc->m_location, count, (float*)values);
            else
                status = false;
            break;
        case GL_FLOAT_VEC2:
            if (desc->m_type == ESVT_UNIFORM)
                extGlUniform2fv(desc->m_location, count, (float*)values);
            else
                status = false;
            break;
        case GL_FLOAT_VEC3:
            if (desc->m_type == ESVT_UNIFORM)
                extGlUniform3fv(desc->m_location, count, (float*)values);
            else
                status = false;
            break;
        case GL_FLOAT_VEC4:
            if (desc->m_type == ESVT_UNIFORM)
                extGlUniform4fv(desc->m_location, count, (float*)values);
            else
                status = false;
            break;
        case GL_FLOAT_MAT2:
            if (desc->m_type == ESVT_UNIFORM)
                extGlUniformMatrix2fv(desc->m_location, count, false, (float*)values);
            else
                status = false;
            break;
        case GL_FLOAT_MAT3:
            if (desc->m_type == ESVT_UNIFORM)
                extGlUniformMatrix3fv(desc->m_location, count, false, (float*)values);
            else
                status = false;
            break;
        case GL_FLOAT_MAT4:
            if (desc->m_type == ESVT_UNIFORM)
                extGlUniformMatrix4fv(desc->m_location, count, false, (float*)values);
            else
                status = false;
            break;
        case GL_INT:
        case GL_SAMPLER_1D:
        case GL_SAMPLER_2D:
        case GL_SAMPLER_3D:
        case GL_SAMPLER_CUBE:
        case GL_SAMPLER_1D_SHADOW:
        case GL_SAMPLER_2D_SHADOW:
            if (desc->m_type == ESVT_UNIFORM)
                extGlUniform1iv(desc->m_location, count, (s32*)values);
            else
                status = false;
            break;
        default:
            status = false;
            break;
    }
    _IRR_DEBUG_BREAK_IF(testGLError());
    return status;
}

bool COpenGLDriver::SyncShaderConstant()
{
    irr::video::GLSLGpuShader* glsl = static_cast<irr::video::GLSLGpuShader*>(getActiveGPUProgram());

    for (u32 ib = 0; ib != glsl->ShaderBuffers.size(); ++ib)
    {
        irr::video::CGlslBufferDesc& ubuffer = glsl->ShaderBuffers[ib];
        if (!ubuffer.getHardwareBuffer())
            continue;

        if (ubuffer.getHardwareMappingHint() == scene::EHM_NEVER)
            ubuffer.setHardwareMappingHint(scene::EHM_STATIC);

        {
            if (ubuffer.getHardwareBuffer()->getChangeID(E_HARDWARE_BUFFER_TYPE::EHBT_CONSTANTS) != ubuffer.getChangeId())
            {
                E_HARDWARE_BUFFER_ACCESS MemoryAccess = E_HARDWARE_BUFFER_ACCESS::EHBA_DEFAULT;
                if (ubuffer.getHardwareMappingHint() == scene::E_HARDWARE_MAPPING::EHM_DYNAMIC)
                    MemoryAccess = E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC;
                else if (ubuffer.getHardwareMappingHint() == scene::E_HARDWARE_MAPPING::EHM_STREAM)
                    MemoryAccess = E_HARDWARE_BUFFER_ACCESS::EHBA_SYSTEM_MEMORY;

                if (ubuffer.getHardwareBuffer()->UpdateBuffer(E_HARDWARE_BUFFER_TYPE::EHBT_CONSTANTS, MemoryAccess, ubuffer.DataBuffer.pointer(),
                    ubuffer.DataBuffer.size()/*, ubuffer.ChangeStartOffset, ubuffer.ChangeEndOffset*/))
                {
                    ubuffer.ChangeStartOffset = ubuffer.DataBuffer.size();
                    ubuffer.ChangeEndOffset = 0;
                    ubuffer.getHardwareBuffer()->setChangeID(E_HARDWARE_BUFFER_TYPE::EHBT_CONSTANTS, ubuffer.ChangeId);
                }
            }
        }

        u32 minSize = std::max(u32(ubuffer.binding), 0u);
        if (BindedBuffers.size() <= (int)minSize)
            BindedBuffers.resize(ubuffer.binding + 1);

        if (BindedBuffers[ubuffer.binding] != ubuffer.getHardwareBuffer())
        {
            //BindedBuffers[ubuffer.binding]->Unbind();
            ubuffer.getHardwareBuffer()->Bind();
            BindedBuffers[ubuffer.binding] = ubuffer.getHardwareBuffer();
        }
    }

    return true;
}

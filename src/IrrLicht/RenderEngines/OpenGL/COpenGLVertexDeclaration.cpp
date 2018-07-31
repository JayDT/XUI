#include "COpenGLVertexDeclaration.h"
#include "COpenGLDriver.h"
#include "COpenGLShader.h"
#include "COpenGLSupport.h"

using namespace irr;
using namespace irr::video;

GLint getFormat(E_VERTEX_ELEMENT_TYPE type)
{
    switch (type)
    {
        case EVET_FLOAT1:
        case EVET_FLOAT2:
        case EVET_FLOAT3:
        case EVET_FLOAT4:
            return GL_FLOAT;

        case EVET_UINT1:
        case EVET_UINT2:
        case EVET_UINT3:
        case EVET_UINT4:
            return GL_UNSIGNED_INT;

        case EVET_SHORT2:
        case EVET_SHORT4:
            return GL_SHORT;

        case EVET_UBYTE4_NORM:
            return GL_UNSIGNED_BYTE;
        case EVET_COLOUR:
            return GL_UNSIGNED_BYTE; ///*Driver->queryOpenGLFeature(COpenGLExtensionHandler::IRR_ARB_vertex_array_bgra) ?*/ GL_BGRA; // : GL_UNSIGNED_BYTE;
        case EVET_UBYTE4:
            return GL_UNSIGNED_BYTE;
    }

    return GL_FLOAT;
}

GLint getFormatBitSize(E_VERTEX_ELEMENT_TYPE type)
{
    switch (type)
    {
        case EVET_FLOAT1:
            return 32;
        case EVET_FLOAT2:
            return 32 * 2;
        case EVET_FLOAT3:
            return 32 * 3;
        case EVET_FLOAT4:
            return 32 * 4;

        case EVET_UINT1:
            return 32;
        case EVET_UINT2:
            return 32 * 2;
        case EVET_UINT3:
            return 32 * 3;
        case EVET_UINT4:
            return 32 * 4;

        case EVET_SHORT2:
            return 16 * 2;
        case EVET_SHORT4:
            return 16 * 4;

        case EVET_UBYTE4_NORM:
        case EVET_COLOUR:
            return 8 * 4;
        case EVET_UBYTE4:
            return 8 * 4;
    }

    return 32;
}

GLint getFormatSize(E_VERTEX_ELEMENT_TYPE type)
{
    switch (type)
    {
        case EVET_FLOAT1:
            return 1;
        case EVET_FLOAT2:
            return 2;
        case EVET_FLOAT3:
            return 3;
        case EVET_FLOAT4:
            return 4;

        case EVET_UINT1:
            return 1;
        case EVET_UINT2:
            return 2;
        case EVET_UINT3:
            return 3;
        case EVET_UINT4:
            return 4;

        case EVET_SHORT2:
            return 2;
        case EVET_SHORT4:
            return 4;

        case EVET_COLOUR:
            return GL_BGRA; //Driver->queryOpenGLFeature(COpenGLExtensionHandler::IRR_ARB_vertex_array_bgra) ? GL_BGRA : 4;
        case EVET_UBYTE4_NORM:
        case EVET_UBYTE4:
            return 4;
    }

    return 4;
}

bool getFormatNormalized(E_VERTEX_ELEMENT_TYPE type)
{
    switch (type)
    {
        case EVET_UBYTE4_NORM:
        case EVET_COLOUR:
            return true;
    }

    return false;
}

irr::video::COpenGLVertexDeclaration::COpenGLVertexDeclaration(COpenGLDriver * driver)
    : Driver(driver)
{
}

irr::video::COpenGLVertexDeclaration::~COpenGLVertexDeclaration()
{
}

core::array<OpenGLVERTEXELEMENT> const & irr::video::COpenGLVertexDeclaration::getVertexDeclaration()
{

//#ifdef GL_BGRA
//    const GLint colorSize = (Driver->queryOpenGLFeature(COpenGLExtensionHandler::IRR_ARB_vertex_array_bgra) || Driver->queryOpenGLFeature(COpenGLExtensionHandler::IRR_EXT_vertex_array_bgra)) ? GL_BGRA : 4;
//#else
//    const GLint colorSize = 4;
//#endif

    if (VertexDeclarationMap.empty())
    {
        OpenGLVERTEXELEMENT desc;
        for (u32 i = 0; i < mVertexElements.size(); i++)
        {
            desc.Usage           = mVertexElements[i].SemanticType;
            desc.Type            = getFormat(mVertexElements[i].Type);
            desc.Size            = getFormatSize(mVertexElements[i].Type);
            desc.Stream          = mVertexElements[i].SlotIndex;
            desc.Offset          = mVertexElements[i].Offset == -1 ? 0xFFFFFFFF : mVertexElements[i].Offset;
            desc.PreInstance     = mVertexElements[i].PerInstance;
            desc.Normalized      = getFormatNormalized(mVertexElements[i].Type);
            desc.InstanceDivisor = std::max(1u, mVertexElements[i].InstanceStep);

            VertexDeclarationMap.push_back(desc);

            if (VertexPitch.size() < desc.Stream + 1)
                VertexPitch.resize(desc.Stream + 1);
            VertexPitch[desc.Stream] += getFormatBitSize(mVertexElements[i].Type) / 8;
        }
    }

    return VertexDeclarationMap;
}

void irr::video::COpenGLVertexDeclaration::createInputLayout(const COpenGLHardwareBuffer * hwBuffer)
{
    core::array<OpenGLVERTEXELEMENT> const& declArray = getVertexDeclaration();

    if (!declArray.empty())
    {
        glBindVertexArray(hwBuffer->GetVAO());
        GLSLGpuShader* glsl = static_cast<GLSLGpuShader*>(hwBuffer && hwBuffer->GetBuffer() ? hwBuffer->GetBuffer()->GetGPUProgram() : nullptr);
        if (!glsl)
            glsl = static_cast<GLSLGpuShader*>(Driver->GetDefaultGPU());

        for (int i = 0; i != declArray.size(); ++i)
        {
            const OpenGLVERTEXELEMENT& decl = declArray[i];
            GLint Size = decl.Size;
        
            if (glsl)
            {
                const CGlslBufferDesc* attributeDesc = glsl->getGPUProgramAttributeDesc(i);
                if (attributeDesc)
                {
                    int attribloc = (int)attributeDesc->varDesc.m_location;
                    {
                        glEnableVertexAttribArray(attribloc);
                        switch (decl.Type)
                        {
                            case GL_INT:
                            case GL_UNSIGNED_INT:
                            case GL_BYTE:
                            case GL_UNSIGNED_BYTE:
                                if (!decl.Normalized)
                                {
                                    glVertexAttribIFormat(attribloc, Size, decl.Type, decl.Offset);
                                    break;
                                }
                            default:
                                glVertexAttribFormat(attribloc, Size, decl.Type, decl.Normalized, decl.Offset);
                                break;
                        }
                        glVertexAttribBinding(attribloc, decl.Stream);
                    }
                }
            }
            //else // client states fully useless
        }
        glBindVertexArray(0);
    }
}

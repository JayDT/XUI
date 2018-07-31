#include "CIrrShader.h"
#include "CIrrVertexDeclaration.h"

using namespace irr;
using namespace irr::video;

void IShaderDataBuffer::setDirty()
{
    for (u32 i = 0; i != m_bufferDataArray.size(); ++i)
        m_bufferDataArray[i]->setDirty();
    ++mChangedID;
}

void IShaderDataBuffer::CommitBuffer(IShader* gpuProgram, IHardwareBuffer* buffer /*= nullptr*/)
{
    for (u32 i = 0; i != m_bufferDataArray.size(); ++i)
    {
        auto bufferData = m_bufferDataArray[i];
        if (bufferData->isChanged() && bufferData->getDescription())
        {
            gpuProgram->getVideoDriver()->setShaderConstant(bufferData->getDescription(), bufferData->getShaderValues(), bufferData->getShaderValueCount(), buffer);
            bufferData->setUpdated();
        }
    }
}


u8 IShader::getContextType() const
{
    return mContext->getDriverType();
}

IShader::IShader(video::IVideoDriver* context, E_ShaderTypes type)
    : mContext(context)
    , mShaderType(type)
    , mBinded(false)
{
    mContextType = getContextType();

    //std::fill_n(m_shaderVariableIndexTable, (s32)EGVAT_MAX_VALUE, -1);

}

IShader::~IShader()
{
    for (u32 bi = 0; bi != IShaderDataBuffer::EUT_MAX_VALUE; ++bi)
    {
        for (u32 i = 0; i != mBuffers[bi].size(); ++i)
            delete mBuffers[bi][i];
        mBuffers[bi].clear();
    }
}

bool IShader::readFile(const char* pFileName, std::string& outFile)
{
    if (!pFileName || !strlen(pFileName))
        return false;

    const char *progtext;
    char *buf = 0;
    FILE *f = fopen(pFileName, "rb");
    if (!f)
        return false;

    fseek(f, 0, SEEK_END);
    size_t len = ftell(f);
    fseek(f, 0, SEEK_SET);

    outFile.resize(len);
    progtext = buf;
    fread(&outFile[0], len, 1, f);
    fclose(f);

    return true;
}

//ShaderVariableDescriptor const* IShader::GetGPUVariableDesc(EVertexSemanticType type) const
//{
//    if (m_shaderVariableIndexTable[type] != -1)
//        return &m_shaderVariableDescArray[m_shaderVariableIndexTable[type]];
//
//    return nullptr;
//}
//
//void IShader::LinkShaderVariable(const char* name, EVertexSemanticType basicVariableLocation /*= EGVAT_NONE*/, u32* rid /*= nullptr*/)
//{
//    u32 pos;
//    ShaderVariableDescriptor* desc = (ShaderVariableDescriptor*)GetGPUVariableDesc(name, &pos);
//    if (desc)
//    {
//        if (rid)
//            *rid = pos;
//
//        if (basicVariableLocation != EGVAT_NONE && (u32)basicVariableLocation < EGVAT_MAX_VALUE)
//        {
//            desc->m_basicVariableLocation = basicVariableLocation;
//            m_shaderVariableIndexTable[(u32)basicVariableLocation] = (s16)pos;
//        }
//    }
//}

ShaderVariableDescriptor const* IShader::GetGPUVariableDesc(const char* name, u32* rid /*= nullptr*/) const
{
    for (u32 i = 0; i != m_shaderVariableDescArray.size(); ++i)
    {
        if (m_shaderVariableDescArray[i].m_name == name)
        {
            if (rid)
                *rid = i;

            return &m_shaderVariableDescArray[i];
        }
    }

    return nullptr;
}

s32 IShader::getGPUProgramAttribLocation(E_VERTEX_ELEMENT_SEMANTIC type) const
{
    //if (ShaderVariableDescriptor const* desc = GetGPUVariableDesc(type))
    //    return desc->m_location;
    return -1;
}

int IShader::getVariableLocation(const char* name)
{
    u32 pos = -1;
    /*ShaderVariableDescriptor const* desc = */ GetGPUVariableDesc(name, &pos);
    return pos;
}

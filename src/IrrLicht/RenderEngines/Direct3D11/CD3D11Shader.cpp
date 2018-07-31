#include "CD3D11Shader.h"
#include "CD3D11Driver.h"
#include "CD3D11HardwareBuffer.h"
#include "os.h"

#include <D3DCompiler.h>

#define Dx11ShaderDebug 0

using namespace irr;
using namespace irr::video;

ID3DBlob* D3D11CompileShader(System::IO::IFileReader* file, const char* entryPoint, const char* shaderModel, ID3DBlob*& errorMessage)
{
    UINT compileFlags = 0;
    //flags |= D3D10_SHADER_ENABLE_BACKWARDS_COMPATIBILITY;
#if	Dx11ShaderDebug
    // These values allow use of PIX and shader debuggers
    compileFlags |= D3D10_SHADER_DEBUG;
    compileFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
    //flags |= D3D10_SHADER_WARNINGS_ARE_ERRORS;
#else
    // These flags allow maximum performance
    compileFlags |= D3D10_SHADER_ENABLE_STRICTNESS;
    compileFlags |= D3D10_SHADER_OPTIMIZATION_LEVEL3;
#endif

    std::string source;
    source.resize(file->Size());
    file->Read((byte*)source.data(), file->Size());
    //if (file->Read((byte*)source.data(), file->Size()) != file->Size())
    //    throw std::runtime_error("shader not found");

    ID3DBlob* ShaderBuffer;

    HRESULT hr = D3DCompile(
        source.c_str(),         // [in] Pointer to the shader in memory. 
        source.size(),          // [in] Size of the shader in memory.  
        file->FileName.c_str(), // [in] Optional. You can use this parameter for strings that specify error messages.
        nullptr,                // [in] Optional. Pointer to a NULL-terminated array of macro definitions. See D3D_SHADER_MACRO. If not used, set this to NULL. 
        nullptr,                // [in] Optional. Pointer to an ID3DInclude Interface interface for handling include files. Setting this to NULL will cause a compile error if a shader contains a #include. 
        entryPoint,             // [in] Name of the shader-entrypoint function where shader execution begins. 
        shaderModel,            // [in] A string that specifies the shader model; can be any profile in shader model 4 or higher. 
        compileFlags,           // [in] Effect compile flags - no D3DCOMPILE_ENABLE_BACKWARDS_COMPATIBILITY at the first try...
        NULL,                   // [in] Effect compile flags
        &ShaderBuffer,          // [out] A pointer to an ID3DBlob Interface which contains the compiled shader, as well as any embedded debug and symbol-table information. 
        &errorMessage           // [out] A pointer to an ID3DBlob Interface which contains a listing of errors and warnings that occurred during compilation. These errors and warnings are identical to the the debug output from a debugger.
    );

    if (FAILED(hr))
    {
        if (errorMessage)
        {
            LPVOID pCompilErrors = errorMessage->GetBufferPointer();
            printf("[HLSL SHADER] Vertex Shader Compile Error: %s \r\n", (const char*)pCompilErrors);
        }

        os::Printer::log("Error, could not open effect file");
        // Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
        SAFE_RELEASE(ShaderBuffer);
        SAFE_RELEASE(errorMessage);
        throw std::runtime_error("invalid shader");
    }

    return ShaderBuffer;
}

void irr::video::D3D11HLSLProgram::OnDeviceLost(CD3D11Driver * device)
{
}

void irr::video::D3D11HLSLProgram::OnDeviceRestored(CD3D11Driver * device)
{
}

irr::video::D3D11HLSLProgram::D3D11HLSLProgram(video::IVideoDriver * context, E_ShaderTypes type)
    : IShader(context, type)
    , D3D11DeviceResource(static_cast<CD3D11Driver*>(context))
    , m_vertexShader(nullptr)
    , m_pixelShader(nullptr)
    , m_hullShader(nullptr)
    , m_domainShader(nullptr)
    , m_computeShader(nullptr)
    , m_geometryShader(nullptr)
    , m_inputLayout(nullptr)
    , m_vertexShaderBytecode(nullptr)
    , pReflectVertexShader(nullptr)
    , pReflectHullShader(nullptr)
    , pReflectDomainShader(nullptr)
    , pReflectComputeShader(nullptr)
    , pReflectGeometryShader(nullptr)
    , pReflectPixelShader(nullptr)
{
}

irr::video::D3D11HLSLProgram::~D3D11HLSLProgram()
{
    SAFE_RELEASE(m_vertexShader)
    SAFE_RELEASE(m_pixelShader)
    SAFE_RELEASE(m_hullShader)
    SAFE_RELEASE(m_domainShader)
    SAFE_RELEASE(m_computeShader)
    SAFE_RELEASE(m_geometryShader)
    SAFE_RELEASE(m_inputLayout)
    SAFE_RELEASE(m_vertexShaderBytecode)
    SAFE_RELEASE(pReflectVertexShader)
    SAFE_RELEASE(pReflectHullShader)
    SAFE_RELEASE(pReflectDomainShader)
    SAFE_RELEASE(pReflectComputeShader)
    SAFE_RELEASE(pReflectGeometryShader)
    SAFE_RELEASE(pReflectPixelShader)

    if (m_inputLayout)
        m_inputLayout->Release();
    m_inputLayout = nullptr;
}

bool irr::video::D3D11HLSLProgram::createVertexShader(ID3D11Device* device, System::IO::IFileReader* file, const char* entryPoint, const char* shaderModel)
{
    SAFE_RELEASE(m_vertexShaderBytecode);

    HRESULT hr = S_OK;
    ID3DBlob* errorMessage;
    m_vertexShaderBytecode = D3D11CompileShader(file, entryPoint, shaderModel, errorMessage);

    // Create the vertex shader from the buffer.
    hr = device->CreateVertexShader(m_vertexShaderBytecode->GetBufferPointer(), m_vertexShaderBytecode->GetBufferSize(), NULL, &m_vertexShader);
    if (FAILED(hr))
    {
        // Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
        SAFE_RELEASE(m_vertexShaderBytecode);
        throw std::runtime_error("invalid shader");
        return false;
    }

    // get description of precompiled shader
    pReflectVertexShader = NULL;
    D3DReflect(
        m_vertexShaderBytecode->GetBufferPointer(),
        m_vertexShaderBytecode->GetBufferSize(),
        IID_ID3D11ShaderReflection,
        (void**)&pReflectVertexShader
    );

    initializeConstantBuffers((CD3D11Driver *)getVideoDriver(), pReflectVertexShader, EST_VERTEX_SHADER);
    hr = enumInputLayout((CD3D11Driver *)getVideoDriver(), pReflectVertexShader);

    if (FAILED(hr))
    {
        // Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
        SAFE_RELEASE(m_vertexShaderBytecode);
        throw std::runtime_error("invalid shader");
        return false;
    }

    return true;
}

bool irr::video::D3D11HLSLProgram::createGeometryShader(ID3D11Device* device, System::IO::IFileReader* file, const char* entryPoint, const char* shaderModel)
{
    HRESULT hr = S_OK;
    ID3DBlob* errorMessage;
    ID3DBlob* ShaderBuffer = D3D11CompileShader(file, entryPoint, shaderModel, errorMessage);

    // Create the vertex shader from the buffer.
    hr = device->CreateGeometryShader(ShaderBuffer->GetBufferPointer(), ShaderBuffer->GetBufferSize(), NULL, &m_geometryShader);
    if (FAILED(hr))
    {
        throw std::runtime_error("invalid shader");
        return false;
    }

    // get description of precompiled shader
    pReflectGeometryShader = NULL;
    D3DReflect(
        ShaderBuffer->GetBufferPointer(),
        ShaderBuffer->GetBufferSize(),
        IID_ID3D11ShaderReflection,
        (void**)&pReflectGeometryShader
    );

    initializeConstantBuffers((CD3D11Driver *)getVideoDriver(), pReflectGeometryShader, EST_GEOMETRY_SHADER);

    // Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
    SAFE_RELEASE(ShaderBuffer);
    return true;
}

bool irr::video::D3D11HLSLProgram::createPixelShader(ID3D11Device* device, System::IO::IFileReader* file, const char* entryPoint, const char* shaderModel)
{
    HRESULT hr = S_OK;
    ID3DBlob* errorMessage;
    ID3DBlob* ShaderBuffer = D3D11CompileShader(file, entryPoint, shaderModel, errorMessage);

    // Create the vertex shader from the buffer.
    hr = device->CreatePixelShader(ShaderBuffer->GetBufferPointer(), ShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
    if (FAILED(hr))
    {
        throw std::runtime_error("invalid shader");
        return false;
    }

    // get description of precompiled shader
    pReflectPixelShader = NULL;
    D3DReflect(
        ShaderBuffer->GetBufferPointer(),
        ShaderBuffer->GetBufferSize(),
        IID_ID3D11ShaderReflection,
        (void**)&pReflectPixelShader
    );

    initializeConstantBuffers((CD3D11Driver *)getVideoDriver(), pReflectPixelShader, EST_FRAGMENT_SHADER);

    // Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
    SAFE_RELEASE(ShaderBuffer);
    return true;
}

void irr::video::D3D11HLSLProgram::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const char* shaderFilename)
{
    char* compileErrors;
    unsigned long bufferSize, i;
    std::ofstream fout;

    // Get a pointer to the error message text buffer.
    compileErrors = (char*)(errorMessage->GetBufferPointer());

    // Get the length of the message.
    bufferSize = errorMessage->GetBufferSize();

    // Open a file to write the error message to.
    fout.open("shader-error.txt");

    // Write out the error message.
    for (i = 0; i<bufferSize; i++)
    {
        fout << compileErrors[i];
    }

    // Close the file.
    fout.close();

    // Release the error message.
    errorMessage->Release();
    errorMessage = 0;

    // Pop a message up on the screen to notify the user to check the text file for compile errors.
    MessageBox(hwnd, "Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);
}

void irr::video::D3D11HLSLProgram::bind()
{
    //_IRR_DEBUG_BREAK_IF(mBindedProgram && mBindedProgram != programId);
    //if (!mBinded)
    {
        mBinded = true;
        getVideoDriver()->useShader(this);
    }
}

void irr::video::D3D11HLSLProgram::unbind()
{
    //if (mBinded)
    {
        //_IRR_DEBUG_BREAK_IF(mBindedProgram && mBindedProgram != programId);
        getVideoDriver()->useShader(nullptr);
    }
}

void irr::video::D3D11HLSLProgram::Init()
{
}

HRESULT irr::video::D3D11HLSLProgram::initializeConstantBuffers(CD3D11Driver * d3dDevice, ID3D11ShaderReflection* pReflector, E_ShaderTypes shaderType)
{
    HRESULT hr = S_OK;

    D3D11_SHADER_DESC desc;
    pReflector->GetDesc(&desc);

    // get description of each constant buffer in the shader
    for (u32 i = 0; i < desc.ConstantBuffers; i++)
    {
        ShaderBuffers.push_back(CbufferDesc());
        CbufferDesc& buffdesc = ShaderBuffers.getLast();

        buffdesc.shaderType = shaderType;

        // get description of constant buffer
        ID3D11ShaderReflectionConstantBuffer * pConstBuffer =
            pReflector->GetConstantBufferByIndex(i);
        pConstBuffer->GetDesc(&buffdesc.shaderBuffer);

        // Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
        //D3D11_BUFFER_DESC BufferDesc;
        //BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        //BufferDesc.ByteWidth = buffdesc.shaderBuffer.Size;
        //BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        //BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        //BufferDesc.MiscFlags = 0;
        //BufferDesc.StructureByteStride = 0;

        // Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
        buffdesc.m_constantBuffer = static_cast<CD3D11HardwareBuffer*>(d3dDevice->createHardwareBuffer(E_HARDWARE_BUFFER_TYPE::EHBT_CONSTANTS, E_HARDWARE_BUFFER_ACCESS::EHBA_DEFAULT, buffdesc.shaderBuffer.Size)); //d3dDevice->GetDxDevice()->CreateBuffer(&BufferDesc, NULL, &buffdesc.m_constantBuffer);
        if (FAILED(hr))
            throw std::runtime_error("invalid buffer");

        // init cpu cache
        buffdesc.DataBuffer.set_used(buffdesc.shaderBuffer.Size);
        ::ZeroMemory(buffdesc.DataBuffer.pointer(), buffdesc.DataBuffer.size());
        buffdesc.ChangeId = 0;

        // get description of each variable in constant buffer
        for (u32 j = 0; j < buffdesc.shaderBuffer.Variables; ++j)
        {
            ShaderConstantDesc constant;
            constant.BufferDesc = &buffdesc;

            // description of variable
            ID3D11ShaderReflectionVariable * pVariable =
                pConstBuffer->GetVariableByIndex(j);
            pVariable->GetDesc(&constant.VariableDesc);

            // type of variable
            ID3D11ShaderReflectionType * pType = pVariable->GetType();
            pType->GetDesc(&constant.varType);

            constant.elementSize = constant.VariableDesc.Size;
            if (constant.varType.Elements > 1)
            {
                _IRR_DEBUG_BREAK_IF(constant.elementSize % constant.varType.Elements);
                constant.elementSize /= constant.varType.Elements;
            }

            if (constant.varType.Members)
            {
                for (unsigned mj = 0; mj < constant.varType.Members; ++mj)
                {
                    ShaderStructMember structDecl;
                    structDecl.name = pType->GetMemberTypeName(mj);

                    ID3D11ShaderReflectionType* memberType = pType->GetMemberTypeByIndex(mj);
                    memberType->GetDesc(&structDecl.varType);

                    std::string name = constant.VariableDesc.Name;
                    name += '.';
                    name += structDecl.name;
                    structDecl.name = name;
                    constant.members.push_back(structDecl);

                    _IRR_DEBUG_BREAK_IF(structDecl.varType.Class == D3D_SVC_STRUCT); // Implement multiple depth struct buffer

                                                                                     // Generic Descriptor
                    ShaderVariableDescriptor ui;

                    //constant.VariableDesc.Size

                    ui.m_location = (j & 0xFF) | (mj << 8);                          // Buffer Variable Index
                    ui.m_length = structDecl.varType.Elements;
                    ui.m_variableType = structDecl.varType.Type;
                    ui.m_class = structDecl.varType.Class;
                    ui.m_name = name;
                    ui.m_size = 0;
                    ui.m_type = ESVT_CONSTANT;
                    ui.m_shaderIndex = ShaderBuffers.size() - 1; // Buffer Desc Index

                    AddShaderVariable(&ui);
                }
            }

            {
                // Generic Descriptor
                ShaderVariableDescriptor ui;

                ui.m_location = j & 0xFF;                          // Buffer Variable Index
                ui.m_length = constant.varType.Elements;
                ui.m_variableType = constant.varType.Type;
                ui.m_class = constant.varType.Class;
                ui.m_name = constant.VariableDesc.Name;
                ui.m_size = constant.VariableDesc.Size;
                ui.m_type = ESVT_CONSTANT;
                ui.m_shaderIndex = ShaderBuffers.size() - 1; // Buffer Desc Index

                AddShaderVariable(&ui);
            }

            buffdesc.Variables.push_back(constant);
        }

        //S_GPU_SHADER_VARIABLE_DEFAULT_LINK const* mLinkPtr = sDefaultShaderVariableLink;
        //do
        //{
        //    LinkShaderVariable(mLinkPtr->name, mLinkPtr->id);
        //} while ((++mLinkPtr)->id != EGVAT_MAX_VALUE);

        // get binding description for constant buffer
        hr = pReflector->GetResourceBindingDescByName(buffdesc.shaderBuffer.Name, &buffdesc.bindingDesc);
    }

    ///////////////////////////////////////////////////
    // Save description of textures and samplers
    for (u32 i = 0; i != desc.BoundResources; ++i)
    {
        D3D11_SHADER_INPUT_BIND_DESC inputBindDesc;
        pReflector->GetResourceBindingDesc(i, &inputBindDesc);
        if (inputBindDesc.Type == D3D_SIT_TEXTURE || inputBindDesc.Type == D3D_SIT_SAMPLER)
            BoundResources.push_back(inputBindDesc);
    }

    return S_OK;
}

HRESULT irr::video::D3D11HLSLProgram::enumInputLayout(CD3D11Driver * d3dDevice, ID3D11ShaderReflection* vertReflect)
{
    // http://www.sunandblackcat.com/tipFullView.php?l=eng&topicid=26
    HRESULT hr = S_OK;

    D3D11_SHADER_DESC descVertex;
    vertReflect->GetDesc(&descVertex);

    // save description of input parameters (attributes of vertex shader)
    core::array<D3D11_INPUT_ELEMENT_DESC> inputLayoutArray;
    u32 byteOffset = 0;
    D3D11_SIGNATURE_PARAMETER_DESC input_desc;
    for (u32 i = 0; i < descVertex.InputParameters; ++i)
    {
        // get description of input parameter
        vertReflect->GetInputParameterDesc(i, &input_desc);

        // fill element description to create input layout later
        D3D11_INPUT_ELEMENT_DESC ie;
        ie.SemanticName = input_desc.SemanticName;
        ie.SemanticIndex = input_desc.SemanticIndex;
        ie.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        ie.InputSlot = 0; // can't reflect
        ie.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;  // can't reflect
        ie.InstanceDataStepRate = 0;  // can't reflect
        ie.AlignedByteOffset = byteOffset;

        u32 componentCount = core::math::NumberOfSetBits(input_desc.Mask);
        byteOffset += (4 * componentCount);

        // determine correct format of input parameter and offset
        if (componentCount == 1)
        {
            if (input_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
            {
                ie.Format = DXGI_FORMAT_R32_UINT;
            }
            else if (input_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
            {
                ie.Format = DXGI_FORMAT_R32_SINT;
            }
            else if (input_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
            {
                ie.Format = DXGI_FORMAT_R32_FLOAT;
            }
        }
        else if (componentCount == 2)
        {
            if (input_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
            {
                ie.Format = DXGI_FORMAT_R32G32_UINT;
            }
            else if (input_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
            {
                ie.Format = DXGI_FORMAT_R32G32_SINT;
            }
            else if (input_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
            {
                ie.Format = DXGI_FORMAT_R32G32_FLOAT;
            }
        }
        else if (componentCount == 3)
        {
            if (input_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
            {
                ie.Format = DXGI_FORMAT_R32G32B32_UINT;
            }
            else if (input_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
            {
                ie.Format = DXGI_FORMAT_R32G32B32_SINT;
            }
            else if (input_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
            {
                ie.Format = DXGI_FORMAT_R32G32B32_FLOAT;
            }
        }
        else if (componentCount == 4)
        {
            if (input_desc.ComponentType == D3D_REGISTER_COMPONENT_UINT32)
            {
                ie.Format = DXGI_FORMAT_R32G32B32A32_UINT;
            }
            else if (input_desc.ComponentType == D3D_REGISTER_COMPONENT_SINT32)
            {
                ie.Format = DXGI_FORMAT_R32G32B32A32_SINT;
            }
            else if (input_desc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
            {
                ie.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
            }
        }

        inputLayoutArray.push_back(ie);
    }

    return hr;
}

#ifndef __C_DIRECTX11_SHADER_H_INCLUDED__
#define __C_DIRECTX11_SHADER_H_INCLUDED__

#include "RenderEngines/General/CIrrVertexDeclaration.h"
#include "RenderEngines/General/CIrrShader.h"
#include "CD3D11VertexDeclaration.h"
#include "CD3D11Resources.h"
#include <d3d11_1.h>
#include <D3DCompiler.h>

namespace irr
{
    namespace video
    {
        class CD3D11HardwareBuffer;

        class D3D11HLSLProgram
            : public IShader
            , protected D3D11DeviceResource
        {
        public:

            struct CbufferDesc;

            struct ShaderStructMember
            {
                std::string name;
                D3D11_SHADER_TYPE_DESC varType;
            };

            struct ShaderConstantDesc
            {
                CbufferDesc* BufferDesc;
                D3D11_SHADER_VARIABLE_DESC VariableDesc;
                D3D11_SHADER_TYPE_DESC varType;
                UINT elementSize;
                std::vector<ShaderStructMember> members;
            };

            struct CbufferDesc
            {
                CbufferDesc() : ChangeId(0), constantMapping(scene::E_HARDWARE_MAPPING::EHM_STATIC), ChangeStartOffset(0xFFFFFFFF), ChangeEndOffset(0)
                {

                }

                D3D11_SHADER_BUFFER_DESC shaderBuffer;
                D3D11_SHADER_INPUT_BIND_DESC bindingDesc;
                std::vector<ShaderConstantDesc> Variables;
                core::array<char> DataBuffer;
                CD3D11HardwareBuffer* m_constantBuffer;
                u32 ChangeId;
                u32 ChangeStartOffset;
                u32 ChangeEndOffset;
                E_ShaderTypes shaderType : 8;
                scene::E_HARDWARE_MAPPING constantMapping : 8;

                CD3D11HardwareBuffer* getHardwareBuffer() const { return m_constantBuffer; }
                scene::E_HARDWARE_MAPPING getHardwareMappingHint() const { return constantMapping; }
                void setHardwareMappingHint(scene::E_HARDWARE_MAPPING value) { constantMapping = value; }
                u32 getChangeId() const { return ChangeId; }
            };

            core::array<D3D11_SHADER_INPUT_BIND_DESC> BoundResources;

        private:
            friend class CD3D11FixedFunctionMaterialRenderer;
            friend class CD3D11MaterialRenderer_ONETEXTURE_BLEND;
            friend class CD3D11Driver;

            ID3D11VertexShader*     m_vertexShader;
            ID3D11HullShader*       m_hullShader;
            ID3D11DomainShader*     m_domainShader;
            ID3D11ComputeShader*    m_computeShader;
            ID3D11GeometryShader*   m_geometryShader;
            ID3D11PixelShader*      m_pixelShader;
            ID3D11InputLayout*      m_inputLayout;          // Temporary for old style ui draw
            ID3DBlob*               m_vertexShaderBytecode;

            ID3D11ShaderReflection* pReflectVertexShader;
            ID3D11ShaderReflection* pReflectHullShader;
            ID3D11ShaderReflection* pReflectDomainShader;
            ID3D11ShaderReflection* pReflectComputeShader;
            ID3D11ShaderReflection* pReflectGeometryShader;
            ID3D11ShaderReflection* pReflectPixelShader;

            core::array<CbufferDesc> ShaderBuffers;

        public:
            explicit D3D11HLSLProgram(video::IVideoDriver* context, E_ShaderTypes type);

            virtual ~D3D11HLSLProgram();

            virtual E_ShaderVersion getShaderVersion() const { return ESV_HLSL_HIGH_LEVEL; }
            virtual void bind();
            virtual void unbind();
            virtual void destroy() {}
            virtual void Init();

            void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const char* shaderFilename);
            virtual bool createVertexShader(ID3D11Device* device, System::IO::IFileReader* file, const char* main, const char* shaderModel);
            virtual bool createGeometryShader(ID3D11Device* device, System::IO::IFileReader* file, const char* main, const char* shaderModel);
            virtual bool createPixelShader(ID3D11Device* device, System::IO::IFileReader* file, const char* main, const char* shaderModel);
            HRESULT enumInputLayout(CD3D11Driver * d3dDevice, ID3D11ShaderReflection*);
            HRESULT initializeConstantBuffers(CD3D11Driver * d3dDevice, ID3D11ShaderReflection*, E_ShaderTypes shaderType);

            ID3D11InputLayout*       GetInputLayout() { return m_inputLayout; }

            // Inherited via D3D11DeviceResource
            virtual void OnDeviceLost(CD3D11Driver * device) override;
            virtual void OnDeviceRestored(CD3D11Driver * device) override;

            ID3DBlob* GetVertexShaderByteCode() const { return m_vertexShaderBytecode; }
        };
    }
}

#endif //!__C_DIRECTX11_SHADER_H_INCLUDED__
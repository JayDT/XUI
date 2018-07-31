#ifndef __C_DIRECTX11_VERTEX_DECLARATION_H_INCLUDED__
#define __C_DIRECTX11_VERTEX_DECLARATION_H_INCLUDED__

#include "RenderEngines/General/CIrrVertexDeclaration.h"
#include "CD3D11Resources.h"
#include <d3d11_1.h>

namespace irr
{
    namespace video
    {
        class D3D11HLSLProgram;

        class CD3D11VertexDeclaration
            : public VertexDeclaration
            , protected D3D11DeviceResource
        {
            mutable core::array<D3D11_INPUT_ELEMENT_DESC> VertexDeclarationMap;
            mutable core::array<D3D11_SO_DECLARATION_ENTRY> VertexOutputDeclarationMap;
            std::map<const D3D11HLSLProgram *, ID3D11InputLayout*> InputLayouts;
            std::vector<irr::u32> VertexPitch;

        public:
            CD3D11VertexDeclaration(CD3D11Driver* driver);
            virtual ~CD3D11VertexDeclaration();

            core::array<D3D11_INPUT_ELEMENT_DESC> const& getVertexDeclaration();
            const core::array<D3D11_SO_DECLARATION_ENTRY>& getStreamOutputDeclaration() const;
            ID3D11InputLayout* getInputLayout(const D3D11HLSLProgram * shader);
            irr::u32 GetVertexPitch(irr::u8 inputSlot) const { return VertexPitch[inputSlot]; }

            // Inherited via D3D11DeviceResource
            virtual void OnDeviceLost(CD3D11Driver * device) override;
            virtual void OnDeviceRestored(CD3D11Driver * device) override;
        };
    }
}

#endif
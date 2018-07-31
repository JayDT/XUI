
#ifndef __C_D3D11_MATERIAL_RENDERER_H_INCLUDED__
#define __C_D3D11_MATERIAL_RENDERER_H_INCLUDED__

#include "IrrCompileConfig.h"
#ifdef _IRR_WINDOWS_
#define WIN32_LEAN_AND_MEAN
#endif

#ifdef _IRR_COMPILE_WITH_DIRECT3D_11_

#include "IMaterialRenderer.h"
#include <DirectXMath.h>

namespace irr
{

namespace video
{
class CD3D11Driver;

class CD3D11MaterialRenderer : public IMaterialRenderer
{
public:
	CD3D11MaterialRenderer(IVideoDriver* driver)
	: Driver(static_cast<CD3D11Driver*>(driver))
	, Device(0)
	, ImmediateContext(0)
	, BaseRenderer(0)
	{
		Device = Driver->getExposedVideoData().D3D11.D3DDev11;

		if (Device)
		{
			Device->AddRef();
		}
		Device->GetImmediateContext( &ImmediateContext );
	}

	~CD3D11MaterialRenderer()
	{
		if (BaseRenderer)
			BaseRenderer->drop();

		SAFE_RELEASE(ImmediateContext);
		SAFE_RELEASE(Device);
	}

	//! sets a variable in the shader.
	//! \param name: Name of the variable
	//! \param floats: Pointer to array of floats
	//! \param count: Amount of floats in array.
	virtual bool setVariable(const c8* name, const f32* floats, int count)
	{
		return false;
	}

	virtual bool OnRender(IMaterialRendererServices* service, E_VERTEX_TYPE vtxtype)
	{
		return false;
	}

	//! get shader signature
	virtual void* getShaderByteCode() const = 0;

	//! get shader signature size
	virtual u32 getShaderByteCodeSize() const = 0;

	inline DirectX::XMVECTOR colorToD3DXVECTOR4(video::SColor& clr)
	{
		const float cf255 = 255.0f;
		const float r = (float)clr.getRed() / cf255;
		const float g = (float)clr.getGreen() / cf255;
		const float b = (float)clr.getBlue() / cf255;
		const float a = (float)clr.getAlpha() / cf255;
		return DirectX::XMVectorSet(r,g,b,a);
	}

	inline DirectX::XMVECTOR colorToD3DXVECTOR4(const video::SColor& clr)
	{
		const float cf255 = 255.0f;
		const float r = (float)clr.getRed() / cf255;
		const float g = (float)clr.getGreen() / cf255;
		const float b = (float)clr.getBlue() / cf255;
		const float a = (float)clr.getAlpha() / cf255;
		return DirectX::XMVectorSet(r,g,b,a);
	}

    CD3D11Driver* GetDriver() { return Driver; }

protected:
	// DX 11 objects
	ID3D11Device* Device;
	ID3D11DeviceContext* ImmediateContext;

	// Irrlicht objects
	CD3D11Driver* Driver;
	CD3D11MaterialRenderer* BaseRenderer;

	//SMaterial CurrentMaterial;
};

}
}

#endif
#endif



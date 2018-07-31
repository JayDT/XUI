#pragma once

#ifndef __C_VIDEO_DIRECTX_11_H_INCLUDED__
#define __C_VIDEO_DIRECTX_11_H_INCLUDED__

#include "IrrCompileConfig.h"
#include "SIrrCreationParameters.h"

#ifdef _IRR_COMPILE_WITH_DIRECT3D_11_

#include <unordered_map>
#include <set>
#include <string>
#include <map>
#include <algorithm>

#include "CNullDriver.h"
#include "IMaterialRendererServices.h"
#include "CD3D11Resources.h"

#include <d3d11_1.h>
#include <d2d1_2.h>
//#include <dwrite.h>
#include <wincodec.h>

#if _DEBUG
#define DEVEL_LOG(msg,code)	os::Printer::log(msg, code)	
#endif

#define SAFE_RELEASE(x) \
    if(x)				\
        x->Release(); 	\
    x = 0;

namespace irr
{
namespace video
{
    class CD3D11FixedFunctionMaterialRenderer;
    class CD3D11MaterialRenderer_ONETEXTURE_BLEND;
    class CD3D11VertexDeclaration;
    class CD3D11HardwareBuffer;

    struct SDepthSurface11 : public IReferenceCounted
    {
        SDepthSurface11() : Surface(0)
        {
            #ifdef _DEBUG
            setDebugName("SDepthSurface");
            #endif
        }
        virtual ~SDepthSurface11()
        {
            if (Surface)
                Surface->Release();
        }

        ID3D11DepthStencilView* Surface;
        core::dimension2du Size;
    };

    class CD3D11Driver : public CNullDriver, IMaterialRendererServices
    {
    public:
        friend class CD3D11FixedFunctionMaterialRenderer;
        friend class CD3D11Texture;
        friend class D3D11HLSLProgram;

        //! constructor
        CD3D11Driver(const SIrrlichtCreationParameters& params, io::IFileSystem* io, const bool pureSoftware = false);

        //! destructor
        virtual ~CD3D11Driver();

        void ReleaseDriver();

        bool _getErrorsFromQueue() const;

        //! applications must call this method before performing any rendering. returns false if failed.
        virtual bool beginScene(bool backBuffer = true, bool zBuffer = true,
            SColor color = SColor(255, 0, 0, 0),
            const SExposedVideoData& videoData = SExposedVideoData(),
            core::rect<s32>* sourceRect = 0);

        //! applications must call this method after performing any rendering. returns false if failed.
        virtual bool endScene();

        //! queries the features of the driver, returns true if feature is available
        virtual bool queryFeature(E_VIDEO_DRIVER_FEATURE feature) const;

        //! sets transformation
        virtual void setTransform(E_TRANSFORMATION_STATE state, const core::matrix4& mat);

        //! sets a material
        virtual void setMaterial(const SMaterial& material) override;

        //! sets a render target
        virtual bool setRenderTarget(video::ITexture* texture,
            bool clearBackBuffer = true, bool clearZBuffer = true,
            SColor color = video::SColor(0, 0, 0, 0));

        //! Sets multiple render targets
        virtual bool setRenderTarget(const core::array<video::IRenderTarget>& texture,
            bool clearBackBuffer = true, bool clearZBuffer = true,
            SColor color = video::SColor(0, 0, 0, 0));

        //! sets a viewport
        virtual void setViewPort(const core::rect<s32>& area);

        //! gets the area of the current viewport
        virtual const core::rect<s32>& getViewPort() const;

        bool updateVertexHardwareBuffer(CD3D11HardwareBuffer *HWBuffer, E_HARDWARE_BUFFER_TYPE Type);
        bool updateIndexHardwareBuffer(CD3D11HardwareBuffer *HWBuffer);

        //! updates hardware buffer if needed
        virtual bool updateHardwareBuffer(IHardwareBuffer *HWBuffer);

        //! Create hardware buffer from mesh
        virtual IHardwareBuffer *createHardwareBuffer(const scene::IMeshBuffer* mb);

        //! Delete hardware buffer (only some drivers can)
        virtual void deleteHardwareBuffer(IHardwareBuffer *HWBuffer);

        //! is vbo recommended on this mesh? for DirectX 11 ALWAYS YES!!!!!!!!!!!
        // DirectX 11 doesn't use methods like drawPrimitiveUp (DX9) or glVertex (OpenGL)
        virtual bool isHardwareBufferRecommend(const scene::IMeshBuffer* mb) { return true; }

        void InitDrawStates(const scene::IMeshBuffer * mb, scene::E_PRIMITIVE_TYPE pType = scene::E_PRIMITIVE_TYPE::EPT_TRIANGLES);

        void drawMeshBuffer(const scene::IMeshBuffer* mb, scene::IMesh* mesh/* = nullptr*/, scene::ISceneNode* node/* = nullptr*/) override;

        //! Draw hardware buffer
        virtual void drawHardwareBuffer(IHardwareBuffer *HWBuffer, scene::IMesh* mesh = nullptr, scene::ISceneNode* node = nullptr);

        //! draw
        virtual void drawHardwareBuffer(IHardwareBuffer* vertices,
            IHardwareBuffer* indices, E_VERTEX_TYPE vType = EVT_STANDARD,
            scene::E_PRIMITIVE_TYPE pType = scene::EPT_TRIANGLES,
            E_INDEX_TYPE iType = EIT_16BIT, u32 numInstances = 0);

        //! Draw automatically using stream output buffers
        virtual void drawAuto(IHardwareBuffer* vertices, E_VERTEX_TYPE vType = EVT_STANDARD,
            scene::E_PRIMITIVE_TYPE pType = scene::EPT_TRIANGLES);

        //! draws a vertex primitive list
        virtual void drawVertexPrimitiveList(const void* vertices, u32 vertexCount,
            const void* indexList, u32 primitiveCount,
            E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType,
            E_INDEX_TYPE iType);

        //! draws a vertex primitive list in 2d
        virtual void draw2DVertexPrimitiveList(const void* vertices, u32 vertexCount,
            const void* indexList, u32 primitiveCount,
            E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType,
            E_INDEX_TYPE iType);

        //! draws an 2d image, using a color (if color is other then Color(255,255,255,255)) and the alpha channel of the texture if wanted.
        //virtual void draw2DImage(const video::ITexture* texture, const core::position2d<s32>& destPos,
        //    const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect = 0,
        //    SColor color = SColor(255, 255, 255, 255), bool useAlphaChannelOfTexture = false);

        ////! Draws a part of the texture into the rectangle.
        //virtual void draw2DImage(const video::ITexture* texture, const core::rect<s32>& destRect,
        //    const core::rect<s32>& sourceRect, const core::rect<s32>* clipRect = 0,
        //    const video::SColor* const colors = 0, bool useAlphaChannelOfTexture = false);

        //! Draws a set of 2d images, using a color and the alpha channel of the texture.
        //virtual void draw2DImageBatch(const video::ITexture* texture,
        //    const core::array<core::position2d<s32> >& positions,
        //    const core::array<core::rect<s32> >& sourceRects,
        //    const core::rect<s32>* clipRect = 0,
        //    SColor color = SColor(255, 255, 255, 255),
        //    bool useAlphaChannelOfTexture = false);

        ////!Draws an 2d rectangle with a gradient.
        //virtual void draw2DRectangle(const core::rect<s32>& pos,
        //    SColor colorLeftUp, SColor colorRightUp, SColor colorLeftDown, SColor colorRightDown,
        //    const core::rect<s32>* clip, bool filled = true);

        ////! Draws a 2d line.
        //virtual void draw2DLine(const core::position2d<s32>& start,
        //    const core::position2d<s32>& end,
        //    SColor color = SColor(255, 255, 255, 255));

        //! Draws a pixel.
        virtual void drawPixel(u32 x, u32 y, const SColor & color);

        ////! Draws a 3d line.
        //virtual void draw3DLine(const core::vector3df& start,
        //    const core::vector3df& end, SColor color = SColor(255, 255, 255, 255));

        //! initialises the Direct3D API
        bool initDriver(HWND hwnd, bool pureSoftware);
        bool initDriver2D(HWND hwnd, bool pureSoftware);
        void CreateDeviceIndependentResources();
        void InitAdapters(void);
        bool initOutput(HWND hwnd, bool pureSoftware);

        //! \return Returns the name of the video driver. Example: In case of the DIRECT3D8
        //! driver, it would return "Direct3D8.1".
        virtual const wchar_t* getName() const;

        //! deletes all dynamic lights there are
        virtual void deleteAllDynamicLights();

        //! adds a dynamic light, returning an index to the light
        //! \param light: the light data to use to create the light
        //! \return An index to the light, or -1 if an error occurs
        virtual s32 addDynamicLight(const SLight& light);

        //! Turns a dynamic light on or off
        //! \param lightIndex: the index returned by addDynamicLight
        //! \param turnOn: true to turn the light on, false to turn it off
        virtual void turnLightOn(s32 lightIndex, bool turnOn);

        //! returns the maximal amount of dynamic lights the device can handle
        virtual u32 getMaximalDynamicLightAmount() const;

        //! Sets the dynamic ambient light color. The default color is
        //! (0,0,0,0) which means it is dark.
        //! \param color: New color of the ambient light.
        virtual void setAmbientLight(const SColorf& color);

        //! Draws a shadow volume into the stencil buffer.
        virtual void drawStencilShadowVolume(const core::array<core::vector3df>& triangles, bool zfail = true, u32 debugDataVisible = 0);

        //! Fills the stencil shadow with color.
        virtual void drawStencilShadow(bool clearStencilBuffer = false,
            video::SColor leftUpEdge = video::SColor(0, 0, 0, 0),
            video::SColor rightUpEdge = video::SColor(0, 0, 0, 0),
            video::SColor leftDownEdge = video::SColor(0, 0, 0, 0),
            video::SColor rightDownEdge = video::SColor(0, 0, 0, 0));

        //! Returns the maximum amount of primitives (mostly vertices) which
        //! the device is able to render with one drawIndexedTriangleList
        //! call.
        virtual u32 getMaximalPrimitiveCount() const;

        //! Enables or disables a texture creation flag.
        virtual void setTextureCreationFlag(E_TEXTURE_CREATION_FLAG flag, bool enabled);

        //! Sets the fog mode.
        virtual void setFog(SColor color, E_FOG_TYPE fogType, f32 start,
            f32 end, f32 density, bool pixelFog, bool rangeFog);

        //! Only used by the internal engine. Used to notify the driver that
        //! the window was resized.
        virtual void OnResize(const core::dimension2d<u32>& size);

        //! Can be called by an IMaterialRenderer to make its work easier.
        virtual void setBasicRenderStates(const SMaterial& material, const SMaterial& lastMaterial,
            bool resetAllRenderstates);

        //! Returns type of video driver
        virtual E_DRIVER_TYPE getDriverType() const;

        //! Returns the transformation set by setTransform
        virtual const core::matrix4& getTransform(E_TRANSFORMATION_STATE state) const;

        //! Sets a vertex shader constant.
        virtual void setVertexShaderConstant(const f32* data, s32 startRegister, s32 constantAmount = 1);

        //! Sets a pixel shader constant.
        virtual void setPixelShaderConstant(const f32* data, s32 startRegister, s32 constantAmount = 1);

        //! Sets a constant for the vertex shader based on a name.
        virtual bool setVertexShaderConstant(const c8* name, const f32* floats, int count);

        //! Sets a constant for the pixel shader based on a name.
        virtual bool setPixelShaderConstant(const c8* name, const f32* floats, int count);

        //! Bool interface for the above.
        virtual bool setVertexShaderConstant(const c8* name, const bool* bools, int count)
        {
            return false;
        }

        //! Int interface for the above.
        virtual bool setVertexShaderConstant(const c8* name, const s32* ints, int count)
        {
            return false;
        }

        //! Bool interface for the above.
        virtual bool setPixelShaderConstant(const c8* name, const bool* bools, int count)
        {
            return false;
        }

        //! Int interface for the above.
        virtual bool setPixelShaderConstant(const c8* name, const s32* ints, int count)
        {
            return false;
        }

        //! Set hardware buffer for stream output stage
        virtual bool setStreamOutputBuffer(IHardwareBuffer* buffer);

        //! Returns a pointer to the IVideoDriver interface. (Implementation for
        //! IMaterialRendererServices)
        virtual IVideoDriver* getVideoDriver();

        //! Creates a render target texture.
        virtual ITexture* addRenderTargetTexture(const core::dimension2d<u32>& size,
            const io::path& name, const ECOLOR_FORMAT format = ECF_UNKNOWN);

        //! Clears the ZBuffer.
        virtual void clearZBuffer();

        //! Returns an image created from the last rendered frame.
        virtual IImage* createScreenShot(video::ECOLOR_FORMAT format = video::ECF_UNKNOWN, video::E_RENDER_TARGET target = video::ERT_FRAME_BUFFER);

        //! Set/unset a clipping plane.
        virtual bool setClipPlane(u32 index, const core::plane3df& plane, bool enable = false);

        //! Enable/disable a clipping plane.
        virtual void enableClipPlane(u32 index, bool enable);

        //! Used by CD3D11MaterialRenderer to get clip plane and status
        virtual void getClipPlane(u32 index, core::plane3df& plane, bool& enable);

        //! Returns the graphics card vendor name.
        virtual core::stringc getVendorInfo() { return VendorName; }

        //! Enable the 2d override material
        virtual void enableMaterial2D(bool enable = true);

        //! Get the current color format of the color buffer
        /** \return Color format of the color buffer. */
        virtual ECOLOR_FORMAT getColorFormat() const;

        //! Get the current color format of the color buffer
        /** \return Color format of the color buffer as D3D color value. */
        DXGI_FORMAT getD3DColorFormat() const;

        //! Returns the maximum texture size supported.
        virtual core::dimension2du getMaxTextureSize() const;

        //! Get D3D color format from Irrlicht color format.
        DXGI_FORMAT getD3DFormatFromColorFormat(ECOLOR_FORMAT format) const;

        //! Get Irrlicht color format from D3D color format.
        ECOLOR_FORMAT getColorFormatFromD3DFormat(DXGI_FORMAT format) const;

        //! Get index format from type
        DXGI_FORMAT getIndexType(E_INDEX_TYPE iType) const;

        //! query support for color format
        bool querySupportForColorFormat(DXGI_FORMAT format, D3D11_FORMAT_SUPPORT support);

        //! return feature level
        //! this is needed for material renderers to select correct shader profiles
        D3D_FEATURE_LEVEL getFeatureLevel() { return FeatureLevel; }

        //! Get primitive topology
        D3D_PRIMITIVE_TOPOLOGY getTopology(scene::E_PRIMITIVE_TYPE primType) const;

        //! Get number of bits per pixel
        u32 getBitsPerPixel(DXGI_FORMAT format) const;

        //! Get number of components
        u32 getNumberOfComponents(DXGI_FORMAT format) const;

        //! Get number of indices
        u32 getIndexAmount(scene::E_PRIMITIVE_TYPE primType, u32 primitiveCount) const;

        //! Get depth function
        D3D11_COMPARISON_FUNC getDepthFunction(E_COMPARISON_FUNC func) const;

        //! get color write enable
        D3D11_COLOR_WRITE_ENABLE getColorWriteEnable(E_COLOR_PLANE plane) const;

        bool isHardware() const { return DriverType == D3D_DRIVER_TYPE_HARDWARE; }

        // Return references to state descriptions for material renderers
        D3D11_BLEND_DESC& getBlendDesc() { return BlendDesc; }
        D3D11_RASTERIZER_DESC& getRasterizerDesc() { return RasterizerDesc; }
        D3D11_DEPTH_STENCIL_DESC& getDepthStencilDesc() { return DepthStencilDesc; }
        D3D11_SAMPLER_DESC* getSamplerDescs() { return SamplerDesc; }
        const ITexture* getCurrentTexture(irr::u8 idx) { return CurrentTexture[idx]; }

        inline void InvalidateBlendDesc() { if (!BlendStateChanged) BlendStateChanged = true; }
        inline void InvalidateRasterizerDesc() { if (!RasterizerStateChanged) RasterizerStateChanged = true; }
        inline void InvalidateDepthStencilDesc() { if (!DepthStencilStateChanged) DepthStencilStateChanged = true; }
        inline void InvalidateSamplerDesc(char idx) { if (!SamplerStateChanged[idx]) SamplerStateChanged[idx] = true; }

        //! Create a implementation dependant hardware buffer
        virtual IHardwareBuffer* createHardwareBuffer(E_HARDWARE_BUFFER_TYPE type, E_HARDWARE_BUFFER_ACCESS accessType,
            u32 size, u32 flags = 0, const void* initialData = 0);

        video::VertexDeclaration* createVertexDeclaration();

        //! Register a new vertex type
        virtual E_VERTEX_TYPE registerVertexType(core::array<SVertexElement>& elements);

        //! Check multisample quality levels
        virtual u32 queryMultisampleLevels(ECOLOR_FORMAT format, u32 numSamples) const;
        virtual bool setShaderConstant(ShaderVariableDescriptor const* desc, const void* values, int count, IHardwareBuffer* buffer /*= nullptr*/);
        core::array<u8> GetShaderVariableMemoryBlock(ShaderVariableDescriptor const* desc, video::IShader* shader) override final;
        virtual bool setShaderMapping(ShaderVariableDescriptor const* desc, IShader* shader, scene::E_HARDWARE_MAPPING constantMapping);
        bool SyncShaderConstant();
        virtual IShader* createShader(System::IO::IFileReader* vertexShader, System::IO::IFileReader* fragmentShader, System::IO::IFileReader* geometryShader, System::IO::IFileReader* tesselationShader);
        virtual void useShader(IShader* gpuProgram);

        ID3D11Device* GetDxDevice() { return Device; }

        SMaterial& GetMaterial() { return Material; }
        SMaterial& GetLastMaterial() { return LastMaterial; }

        void AddResourceObject(D3D11DeviceResource* res)
        {
            ResourceList.insert(res);
        }

        void RemoveResourceObject(D3D11DeviceResource* res)
        {
            ResourceList.erase(res);
        }

        void HandleDeviceLost();

    private:

        // enumeration for rendering modes such as 2d and 3d for minizing the switching of renderStates.
        enum E_RENDER_MODE
        {
            ERM_2D = 0,		// 2d drawing rendermode
            ERM_3D,			// 3d rendering mode
        };

        E_RENDER_MODE CurrentRenderMode;

        SMaterial Material, LastMaterial;
        IImage* blankImage;
        ITexture* blankTexture;
        const ITexture* CurrentTexture[MATERIAL_MAX_TEXTURES];
        core::matrix4 Matrices[ETS_COUNT];			// matrizes of the 3d mode we need to restore when we switch back from the 2d mode.
        std::wstring DriverAndFeatureName;

        // Direct3D 11 objects

        // Libraries (dxgi library is loaded automatically)
        HINSTANCE D3DLibrary;

        // DirectX Core Objects. Required for 2D and 3D.
        // DXGI objects
        DXGI_SWAP_CHAIN_DESC1 present;
        IDXGISwapChain1* SwapChain;
        IDXGIAdapter* Adapter;
        IDXGIOutput* Output;
        IDXGIFactory2* DXGIFactory;
        IDXGIDevice1* DXGIDevice;
        ID3D11InfoQueue* m_infoQueue;

        // D3D 11 Device objects
        D3D_DRIVER_TYPE DriverType;
        D3D_FEATURE_LEVEL FeatureLevel;
        ID3D11Device1* Device;
        ID3D11DeviceContext1* ImmediateContext;

        // Direct2D Rendering Objects. Required for 2D.
        core::vector2df         m_DPI;
        IDWriteFactory*         DwriteFactory;
        ID2D1Device*            D2DDevice;
        ID2D1DeviceContext*     D2DContext;
        ID2D1Factory1*          D2DFactory;
        ID2D1Bitmap1*           D2DTargetBitmap;
        IWICImagingFactory2*    WicFactory;
        ID2D1DrawingStateBlock* m_stateBlock;


        // Direct3D Rendering Objects. Required for 3D.
        // Back and depth buffers
        ID3D11RenderTargetView* DefaultBackBuffer;
        ID3D11DepthStencilView* DefaultDepthBuffer;
        ID3D11RenderTargetView* CurrentBackBuffer;
        ID3D11DepthStencilView* CurrentDepthBuffer;

        // Buffers for dynamic data
        ID3D11Buffer* DynVertexBuffer;
        ID3D11Buffer* DynIndexBuffer;
        u32 DynVertexBufferSize;
        u32 DynIndexBufferSize;

        // Input layout
        ID3D11InputLayout* CurrentInputLayout;		// store current to prevent set multiple times the same layout
        D3D_PRIMITIVE_TOPOLOGY lastTopology = D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
        ID3D11DepthStencilState* LastDepthStencilState = nullptr;
        ID3D11BlendState* LastBlendState = nullptr;
        ID3D11RasterizerState* LastRasterizerState = nullptr;

        // Storing Resources
        std::set<D3D11DeviceResource*> ResourceList;

        //// Depricate
        //struct LayoutKey
        //{
        //    D3D11_INPUT_ELEMENT_DESC* elementDesc;
        //    u32 pInputSignature;
        //
        //    // shall define these operators for this structure
        //    inline bool operator==(const LayoutKey& other) const
        //    {
        //        return ((elementDesc == other.elementDesc) &&
        //            (pInputSignature == other.pInputSignature));
        //    }
        //
        //    inline bool operator!=(const LayoutKey& other) const
        //    {
        //        return ((elementDesc != other.elementDesc) ||
        //            (pInputSignature != other.pInputSignature));
        //    }
        //
        //    inline bool operator<(const LayoutKey& other) const
        //    {
        //        return ((elementDesc < other.elementDesc) ||
        //            ((elementDesc == other.elementDesc) && (pInputSignature < other.pInputSignature)));
        //    }
        //};

        //typedef std::map<LayoutKey, ID3D11InputLayout*> InputLayoutStateMap;
        //InputLayoutStateMap InputLayoutMap;

        // States map: due to need of comparison operators, will use wrappers for comparison

        // Blend
        struct SD3D11_BLEND_DESC : public D3D11_BLEND_DESC
        {
            SD3D11_BLEND_DESC()
            {
                reset();
            }

            inline bool operator==(const SD3D11_BLEND_DESC& other) const
            {
                return memcmp(this, &other, sizeof(SD3D11_BLEND_DESC)) == 0;
            }

            inline bool operator!=(const SD3D11_BLEND_DESC& other) const
            {
                return memcmp(this, &other, sizeof(SD3D11_BLEND_DESC)) != 0;
            }

            inline bool operator<(const SD3D11_BLEND_DESC& other) const
            {
                return memcmp(this, &other, sizeof(SD3D11_BLEND_DESC)) < 0;
            }

            inline void reset()
            {
                AlphaToCoverageEnable = false;
                IndependentBlendEnable = false;
                RenderTarget[0].BlendEnable = false;
                RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
                RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
                RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
                RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
                RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
                RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
                RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
                memcpy(&RenderTarget[1], &RenderTarget[0], sizeof(D3D11_RENDER_TARGET_BLEND_DESC));
                memcpy(&RenderTarget[2], &RenderTarget[0], sizeof(D3D11_RENDER_TARGET_BLEND_DESC));
                memcpy(&RenderTarget[3], &RenderTarget[0], sizeof(D3D11_RENDER_TARGET_BLEND_DESC));
                memcpy(&RenderTarget[4], &RenderTarget[0], sizeof(D3D11_RENDER_TARGET_BLEND_DESC));
                memcpy(&RenderTarget[5], &RenderTarget[0], sizeof(D3D11_RENDER_TARGET_BLEND_DESC));
                memcpy(&RenderTarget[6], &RenderTarget[0], sizeof(D3D11_RENDER_TARGET_BLEND_DESC));
                memcpy(&RenderTarget[7], &RenderTarget[0], sizeof(D3D11_RENDER_TARGET_BLEND_DESC));
            }
        };

        ID3D11BlendState* BlendState;
        SD3D11_BLEND_DESC BlendDesc;
        //SD3D11_BLEND_DESC lastBlendDesc;
        //std::map<SD3D11_BLEND_DESC, ID3D11BlendState*> BlendStateCache;
        bool BlendStateChanged = false;

        // Rasterizer
        struct SD3D11_RASTERIZER_DESC : public D3D11_RASTERIZER_DESC
        {
            SD3D11_RASTERIZER_DESC()
            {
                reset();
            }

            inline bool operator==(const SD3D11_RASTERIZER_DESC& other) const
            {
                return memcmp(this, &other, sizeof(SD3D11_RASTERIZER_DESC)) == 0;
            }

            inline bool operator!=(const SD3D11_RASTERIZER_DESC& other) const
            {
                return memcmp(this, &other, sizeof(SD3D11_RASTERIZER_DESC)) != 0;
            }

            inline bool operator<(const SD3D11_RASTERIZER_DESC& other) const
            {
                return memcmp(this, &other, sizeof(SD3D11_RASTERIZER_DESC)) < 0;
            }

            inline void reset()
            {
                FillMode = D3D11_FILL_SOLID;
                CullMode = D3D11_CULL_BACK;
                FrontCounterClockwise = false;
                DepthBias = 0;
                DepthBiasClamp = 0;
                SlopeScaledDepthBias = 0;
                DepthClipEnable = true;
                ScissorEnable = false;
                MultisampleEnable = false;
                AntialiasedLineEnable = false;
            }
        };

        ID3D11RasterizerState* RasterizerState;
        SD3D11_RASTERIZER_DESC RasterizerDesc;
        //SD3D11_RASTERIZER_DESC lastRasterizerDesc;
        //std::map<SD3D11_RASTERIZER_DESC, ID3D11RasterizerState*> RasterizerStateCache;
        bool RasterizerStateChanged = false;

        //// Depth stencil
        struct SD3D11_DEPTH_STENCIL_DESC : public D3D11_DEPTH_STENCIL_DESC
        {
            SD3D11_DEPTH_STENCIL_DESC()
            {
                reset();
            }

            inline bool operator==(const SD3D11_DEPTH_STENCIL_DESC& other) const
            {
                return memcmp(this, &other, sizeof(SD3D11_DEPTH_STENCIL_DESC)) == 0;
            }

            inline bool operator!=(const SD3D11_DEPTH_STENCIL_DESC& other) const
            {
                return memcmp(this, &other, sizeof(SD3D11_DEPTH_STENCIL_DESC)) != 0;
            }

            inline bool operator<(const SD3D11_DEPTH_STENCIL_DESC& other) const
            {
                return memcmp(this, &other, sizeof(SD3D11_DEPTH_STENCIL_DESC)) < 0;
            }

            inline void reset()
            {
                DepthEnable = true;
                DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
                DepthFunc = D3D11_COMPARISON_LESS;
                StencilEnable = false;
                StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
                StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
                FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
                FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
                FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
                FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
                BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
                BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
                BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
                BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
            }
        };

        ID3D11DepthStencilState* DepthStencilState;
        SD3D11_DEPTH_STENCIL_DESC DepthStencilDesc;
        //SD3D11_DEPTH_STENCIL_DESC lastDepthStencilDesc;

        bool DepthStencilStateChanged = false;

        //std::map<SD3D11_DEPTH_STENCIL_DESC, ID3D11DepthStencilState*> DepthStencilStateCache;
        ID3D11DeviceChild* shaderPtr[EST_HIGH_LEVEL_SHADER] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
        ID3D11SamplerState* samplers[MATERIAL_MAX_TEXTURES] = { nullptr, nullptr, nullptr, nullptr, nullptr };
        ID3D11ShaderResourceView* views[MATERIAL_MAX_TEXTURES] = { nullptr, nullptr, nullptr, nullptr, nullptr };

        bool SamplerStateChanged[MATERIAL_MAX_TEXTURES] = { false, false, false, false, false };

        //// Samplers
        struct SD3D11_SAMPLER_DESC : public D3D11_SAMPLER_DESC
        {
            SD3D11_SAMPLER_DESC()
            {
                reset();
            }

            inline bool operator==(const SD3D11_SAMPLER_DESC& other) const
            {
                return memcmp(this, &other, sizeof(SD3D11_SAMPLER_DESC)) == 0;
            }

            inline bool operator!=(const SD3D11_SAMPLER_DESC& other) const
            {
                return memcmp(this, &other, sizeof(SD3D11_SAMPLER_DESC)) != 0;
            }

            inline bool operator<(const SD3D11_SAMPLER_DESC& other) const
            {
                return memcmp(this, &other, sizeof(SD3D11_SAMPLER_DESC)) < 0;
            }

            inline void reset()
            {
                Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
                AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
                AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
                AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
                MipLODBias = 0;
                MaxAnisotropy = 16;
                ComparisonFunc = D3D11_COMPARISON_NEVER;
                MinLOD = 0.0f;
                MaxLOD = D3D11_FLOAT32_MAX;
            }
        };

        ID3D11SamplerState* SamplerState[MATERIAL_MAX_TEXTURES];
        SD3D11_SAMPLER_DESC SamplerDesc[MATERIAL_MAX_TEXTURES];

        SIrrlichtCreationParameters Params;

        core::dimension2d<u32> CurrentRendertargetSize;

        // Just one clip plane for now
        core::array<core::plane3df> ClipPlanes;

        core::rect<s32>* SceneSourceRect;

        E_VERTEX_TYPE LastVertexType;

        D3D11HLSLProgram* m_defaultShader;

        //! All the lights that have been requested; a hardware limited
        //! number of them will be used at once.
        struct RequestedLight
        {
            RequestedLight(SLight const & lightData)
                : LightData(lightData), HardwareLightIndex(-1), DesireToBeOn(true) { }

            SLight	LightData;
            s32	HardwareLightIndex; // GL_LIGHT0 - GL_LIGHT7
            bool	DesireToBeOn;
        };
        core::array<RequestedLight> RequestedLights;
        SColorf AmbientLight;
        u32 MaxActiveLights;

        core::stringc VendorName;
        u16 VendorID;

        core::array<SDepthSurface11*> DepthBuffers;

        core::array<ID3D11Buffer*> BindedBuffers[EST_HIGH_LEVEL_SHADER];

        u32 MaxTextureUnits;
        u32 MaxUserClipPlanes;
        f32 MaxLightDistance;
        s32 LastSetLight;

        ECOLOR_FORMAT ColorFormat;
        DXGI_FORMAT D3DColorFormat;
        DXGI_FORMAT DepthStencilFormat;     // Best format for depth stencil
        bool ClipPlaneEnabled[3];
        bool ResetRenderStates : 1;         // bool to make all renderstates be reseted if set.
        bool Transformation3DChanged : 1;
        bool AlphaToCoverageSupport : 1;
        bool Block2DRenderStateChange : 1;

    public:
        bool ResetBlending : 1;

    protected:

        void createMaterialRenderers();

        void draw2D3DVertexPrimitiveList(const void* vertices,
            u32 vertexCount, const void* indexList, u32 primitiveCount,
            E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType,
            E_INDEX_TYPE iType, bool is3D);

        D3D11_TEXTURE_ADDRESS_MODE getTextureWrapMode(const u8 clamp);

        //! sets the needed renderstates
        bool setRenderStates3DMode();

        //! sets the needed renderstates
        void setRenderStates2DMode(bool alpha, bool texture, bool alphaChannel) override;

        bool setActiveTexture(u32 stage, const video::ITexture* texture);

        //! returns a device dependent texture from a software surface (IImage)
        //! THIS METHOD HAS TO BE OVERRIDDEN BY DERIVED DRIVERS WITH OWN TEXTURES
        virtual video::ITexture* createDeviceDependentTexture(IImage* surface, const io::path& name, void* mipmapData = 0);

        //! Check if a proper depth buffer for the RTT is available, otherwise create it.
        void checkDepthBuffer(ITexture* tex);

        // removes the depth struct from the DepthSurface array
        void removeDepthSurface(SDepthSurface11* depth);

        // creates a depth buffer view
        ID3D11DepthStencilView* createDepthStencilView(core::dimension2d<u32> size);

        //! returns the current size of the screen or rendertarget
        virtual const core::dimension2d<u32>& getCurrentRenderTargetSize() const;

        //! set input vertex layout
        //virtual bool setInputLayout( video::E_VERTEX_TYPE vertexType, IMaterialRenderer* renderer );

        //! bind states to pipeline (description was adjusted in other methods)
        ID3D11BlendState* getBlendState();
        ID3D11RasterizerState* getRasterizerState();
        ID3D11DepthStencilState* getDepthStencilState();
        ID3D11SamplerState* getSamplerState(u32 idx);

        //! reallocate dynamic buffers
        virtual bool reallocateDynamicBuffers(u32 vertexBufferSize, u32 indexBufferSize);

        //! upload dynamic vertex and index data to GPU
        virtual bool uploadVertexData(const void* vertices, u32 vertexCount,
            const void* indexList, u32 indexCount,
            E_VERTEX_TYPE vType, E_INDEX_TYPE iType);

        //! handle screen resize
        void reset();
    };
}
}

#endif // _IRR_COMPILE_WITH_DIRECT3D_11_
#endif // __C_VIDEO_DIRECTX_11_H_INCLUDED__
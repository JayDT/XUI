#define _IRR_DONT_DO_MEMORY_DEBUGGING_HERE
#include "CD3D11Driver.h"

#ifdef _IRR_COMPILE_WITH_DIRECT3D_11_

#define _WIN32_WINNT 0x600
#include <stdio.h>
#include <fstream>
#include <comdef.h>  // you will need this

#include <DirectXMath.h>
#include "os.h"
#include "S3DVertex.h"
#include "CD3D11Texture.h"
#include "CD3D11MaterialRenderer.h"
#include "CD3D11FixedFunctionMaterialRenderer.h"
#include "CD3D11CustomMaterialRenderer.h"
#include "CD3D11NormalMapRenderer.h"
#include "CD3D11VertexDeclaration.h"
#include "CD3D11HardwareBuffer.h"
#include "standard/client/DataSource_Standard.h"

#include "DirectXDefines.h"

//#include "wincodec.h"

//#include <d2d1_1helper.h>
//#include <d2dbasetypes.h>

#undef min
#undef max

static DWORD STATHREADiD = 0;

/////////////
// LINKING //
/////////////
#pragma comment(lib, "Dwrite.lib")
#pragma comment(lib, "D2d1.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")

//extern irr::video::S_GPU_SHADER_VARIABLE_DEFAULT_LINK sDefaultShaderVariableLink[irr::video::EGVAT_MAX_VALUE];

#define Dx11VisualStudioDebug 0
#define Dx11ShaderDebug (Dx11VisualStudioDebug | 0)

#define SAFE_RELEASE(x)     \
    if(x)                   \
        x->Release();       \
    x = 0;

#define RELEASE_ALL(x)            \
    ULONG rest = x->Release();    \
    while( rest > 0 )             \
        rest = x->Release();      \
    x = 0;

extern bool InitializeModulResourceDirect3D11();

namespace irr
{

namespace video
{

    namespace
    {
        inline DWORD F2DW(FLOAT f)
        {
            return *((DWORD*)&f);
        }
    }

//! constructor
CD3D11Driver::CD3D11Driver(const SIrrlichtCreationParameters& params, io::IFileSystem* io, const bool pureSoftware /*= false*/)
    : CNullDriver(io, params.WindowSize),
    //CurrentRenderMode(ERM_NONE),
    ResetRenderStates(true), ResetBlending(false), Transformation3DChanged(false), Block2DRenderStateChange(false),
    DriverType(D3D_DRIVER_TYPE_HARDWARE),
    D3DLibrary(0), Device(0), ImmediateContext(0), SwapChain(0), Adapter(0), DXGIFactory(0), DXGIDevice(0),
    Output(0), CurrentInputLayout(0), DefaultBackBuffer(0), DefaultDepthBuffer(0),
    DynVertexBuffer(0), DynIndexBuffer(0), DynVertexBufferSize(0), DynIndexBufferSize(0),
    SceneSourceRect(0), LastVertexType((video::E_VERTEX_TYPE)-1), VendorID(0), ColorFormat(ECF_A8R8G8B8),
    CurrentRenderMode(ERM_3D), MaxActiveLights(8), AlphaToCoverageSupport(true), m_defaultShader(nullptr),
    DepthStencilFormat(DXGI_FORMAT_UNKNOWN), D3DColorFormat(DXGI_FORMAT_R8G8B8A8_UNORM),
    Params(params), CurrentDepthBuffer(nullptr), CurrentBackBuffer(nullptr),
    // DirectX 11 can handle much more than this value, but keep compatibility
    MaxTextureUnits(MATERIAL_MAX_TEXTURES) 

{
    #ifdef _DEBUG
    setDebugName("CD3D11Driver");
    #endif

    enableMaterial2D(true);

    blockgpuprogramchange = false;

    blankImage = nullptr;
    blankTexture = nullptr;
    D2DDevice = nullptr;
    D2DContext = nullptr;
    D2DFactory = nullptr;
    D2DTargetBitmap = nullptr;

    BlendState = nullptr;
    RasterizerState = nullptr;
    DepthStencilState = nullptr;
    memset(SamplerState, 0, sizeof(SamplerState));

    printVersion();

    for(u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
    {
        CurrentTexture[i] = 0;
    }

    // init clip planes
    ClipPlanes.push_back( core::plane3df() );
    ClipPlanes.push_back( core::plane3df() );
    ClipPlanes.push_back( core::plane3df() );
    ClipPlaneEnabled[0] = ClipPlaneEnabled[1] = ClipPlaneEnabled[2] = false;

    // create sphere map matrix
    SphereMapMatrixD3D11 = DirectX::XMMatrixSet(0.5f, 0.0f, 0.0f, 0.0f,
                                        0.0f, -0.5f, 0.0f, 0.0f,
                                        0.0f, 0.0f, 1.0f, 0.0f,
                                        0.5f, 0.5f, 0.0f, 1.0f);

    core::matrix4 mat;
    UnitMatrixD3D11 = *(DirectX::XMMATRIX*)((void*)mat.pointer());

    // TODO: 1x1 texture
    blankImage = createImage(ECF_A1R5G5B5, getCurrentRenderTargetSize());
    // Size.Height * Pitch
    memset(blankImage->lock(), 0, (blankImage->getPitch() * blankImage->getDimension().Height));
}

CD3D11Driver::~CD3D11Driver()
{
    // Unbound all shader resources
    ID3D11ShaderResourceView* views[1] = { NULL };
    ImmediateContext->VSSetShaderResources(0, 1, views);
    ImmediateContext->HSSetShaderResources(0, 1, views);
    ImmediateContext->DSSetShaderResources(0, 1, views);
    ImmediateContext->GSSetShaderResources(0, 1, views);
    ImmediateContext->PSSetShaderResources(0, 1, views);

    // Set windowed mode before release swap chain
    SwapChain->SetFullscreenState( FALSE, NULL );

    // Delete renderers and textures
    deleteMaterialRenders();
    deleteAllTextures();

    // clear state
    if (ImmediateContext)
    {
        ImmediateContext->ClearState();
        ImmediateContext->Flush();
    }

    if ( BlendState )
        BlendState->Release();

    if ( RasterizerState )
        RasterizerState->Release();

    if ( DepthStencilState )
        DepthStencilState->Release();

    for ( u32 i = 0; i != MATERIAL_MAX_TEXTURES; ++i )
        if ( SamplerState[i] )
            SamplerState[i]->Release();


    // release Direct3D objects

    // clear vertex declarations
    //DeclarationIterator it = declarationMap.getIterator();
    //while(!it.atEnd())
    //{
    //    it->getValue()->drop();
    //    it++;
    //}
    //declarationMap.clear();

    // release blend states
    //BlendStateMap::Iterator bldIt = BlendMap.getIterator();
    //while (!bldIt.atEnd())
    //{
    //    if (bldIt->getValue()) bldIt->getValue()->Release();
    //    bldIt++;
    //}
    //BlendMap.clear();

    // release rasterizer states
    //RasterizerStateMap::Iterator rasIt = RasterizerMap.getIterator();
    //while (!rasIt.atEnd())
    //{
    //    if (rasIt->getValue()) rasIt->getValue()->Release();
    //    rasIt++;
    //}
    //RasterizerMap.clear();
    //
    //// release depth stencil states
    //DepthStencilStateMap::Iterator dsIt = DepthStencilMap.getIterator();
    //while (!dsIt.atEnd())
    //{
    //    if (dsIt->getValue()) dsIt->getValue()->Release();
    //    dsIt++;
    //}
    //DepthStencilMap.clear();
    //
    //// release sampler states
    //SamplerStateMap::Iterator samIt = SamplerMap.getIterator();
    //while (!samIt.atEnd())
    //{
    //    if (samIt->getValue()) samIt->getValue()->Release();
    //    samIt++;
    //}
    //SamplerMap.clear();

    SAFE_RELEASE(DynIndexBuffer);
    SAFE_RELEASE(DynVertexBuffer);
    SAFE_RELEASE(SwapChain);
    SAFE_RELEASE(DefaultDepthBuffer);
    SAFE_RELEASE(DefaultBackBuffer);
    SAFE_RELEASE(Output);
    SAFE_RELEASE(Adapter);
    SAFE_RELEASE(ImmediateContext);
    SAFE_RELEASE(DXGIFactory);
    SAFE_RELEASE(DXGIDevice);

    SAFE_RELEASE(D2DDevice);
    SAFE_RELEASE(D2DContext);
    SAFE_RELEASE(D2DFactory);
    SAFE_RELEASE(D2DTargetBitmap);

    RELEASE_ALL(Device);

    FreeLibrary(D3DLibrary);
}

void CD3D11Driver::createMaterialRenderers()
{
    // create D3D11 material renderers
    /*
    CD3D11MaterialRenderer* renderer = new CD3D11FixedFunctionMaterialRenderer(this, FileSystem);
    addMaterialRenderer( renderer );
    addMaterialRenderer( renderer ); // video::EMT_SOLID_2_LAYER;

    addMaterialRenderer( renderer ); // video::EMT_LIGHTMAP
    addMaterialRenderer( renderer ); // video::EMT_LIGHTMAP_ADD
    addMaterialRenderer( renderer ); // video::EMT_LIGHTMAP_M2
    addMaterialRenderer( renderer ); // video::EMT_LIGHTMAP_M4
    addMaterialRenderer( renderer ); // video::EMT_LIGHTMAP_LIGHTING
    addMaterialRenderer( renderer ); // video::EMT_LIGHTMAP_LIGHTING_M2
    addMaterialRenderer( renderer ); // video::EMT_LIGHTMAP_LIGHTING_M4

    addMaterialRenderer( renderer ); // video::EMT_DETAIL_MAP
    addMaterialRenderer( renderer ); // video::EMT_SPHERE_MAP
    addMaterialRenderer( renderer ); // video::EMT_REFLECTION_2_LAYER
    addMaterialRenderer( renderer ); // video::EMT_TRANSPARENT_ADD_COLOR
    addMaterialRenderer( renderer ); // video::EMT_TRANSPARENT_ALPHA_CHANNEL
    addMaterialRenderer( renderer ); // video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF
    addMaterialRenderer( renderer ); // video::EMT_TRANSPARENT_VERTEX_ALPHA
    addMaterialRenderer( renderer ); // video::EMT_TRANSPARENT_REFLECTION_2_LAYER
    */

    CD3D11MaterialRenderer_SOLID* renderer = new CD3D11MaterialRenderer_SOLID(this, FileSystem);
    addMaterialRenderer( renderer );
    addAndDropMaterialRenderer( new CD3D11MaterialRenderer_SOLID_2_LAYER(this, FileSystem) ); // video::EMT_SOLID_2_LAYER;

    CD3D11MaterialRenderer_LIGHTMAP* lmr = new CD3D11MaterialRenderer_LIGHTMAP(this, FileSystem);
    addMaterialRenderer( lmr ); // video::EMT_LIGHTMAP
    addMaterialRenderer( lmr ); // video::EMT_LIGHTMAP_ADD
    addMaterialRenderer( lmr ); // video::EMT_LIGHTMAP_M2
    addMaterialRenderer( lmr ); // video::EMT_LIGHTMAP_M4
    addMaterialRenderer( lmr ); // video::EMT_LIGHTMAP_LIGHTING
    addMaterialRenderer( lmr ); // video::EMT_LIGHTMAP_LIGHTING_M2
    addMaterialRenderer( lmr ); // video::EMT_LIGHTMAP_LIGHTING_M4
    lmr->drop();

    addAndDropMaterialRenderer( new CD3D11MaterialRenderer_DETAIL_MAP(this, FileSystem) ); // video::EMT_DETAIL_MAP
    addAndDropMaterialRenderer( new CD3D11MaterialRenderer_SPHERE_MAP(this, FileSystem) ); // video::EMT_SPHERE_MAP
    addAndDropMaterialRenderer( new CD3D11MaterialRenderer_REFLECTION_2_LAYER(this, FileSystem) ); // video::EMT_REFLECTION_2_LAYER
    addAndDropMaterialRenderer( new CD3D11MaterialRenderer_TRANSPARENT_ADD_COLOR(this, FileSystem) ); // video::EMT_TRANSPARENT_ADD_COLOR
    addAndDropMaterialRenderer( new CD3D11MaterialRenderer_TRANSPARENT_ALPHA_CHANNEL(this, FileSystem) ); // video::EMT_TRANSPARENT_ALPHA_CHANNEL
    addAndDropMaterialRenderer( new CD3D11MaterialRenderer_TRANSPARENT_ALPHA_CHANNEL_REF(this, FileSystem) ); // video::EMT_TRANSPARENT_ALPHA_CHANNEL_REF
    addAndDropMaterialRenderer( new CD3D11MaterialRenderer_TRANSPARENT_VERTEX_ALPHA(this, FileSystem) ); // video::EMT_TRANSPARENT_VERTEX_ALPHA
    addAndDropMaterialRenderer( new CD3D11MaterialRenderer_TRANSPARENT_REFLECTION_2_LAYER(this, FileSystem) ); // video::EMT_TRANSPARENT_REFLECTION_2_LAYER

    // add normal map renderers
    s32 tmp = 0;
    //video::IMaterialRenderer* matRenderer = 0;
    //
    //matRenderer = new CD3D11NormalMapRenderer(Device, this, FileSystem, tmp,
    //    MaterialRenderers[EMT_SOLID].Renderer);
    //matRenderer->drop();
    //
    //matRenderer = new CD3D11NormalMapRenderer(Device, this, FileSystem, tmp,
    //    MaterialRenderers[EMT_TRANSPARENT_ADD_COLOR].Renderer);
    //matRenderer->drop();
    //
    //matRenderer = new CD3D11NormalMapRenderer(Device, this, FileSystem, tmp,
    //    MaterialRenderers[EMT_TRANSPARENT_VERTEX_ALPHA].Renderer);
    //matRenderer->drop();
    addMaterialRenderer(renderer);
    addMaterialRenderer(renderer);
    addMaterialRenderer(renderer);

    // The following shall be changed to parallax in future
    addMaterialRenderer( renderer );
    addMaterialRenderer( renderer );
    addMaterialRenderer( renderer );
    renderer->drop();

    addMaterialRenderer(new CD3D11MaterialRenderer_ONETEXTURE_BLEND(this, FileSystem));
    addAndDropMaterialRenderer(new CD3D11MaterialRenderer_RAW(this, FileSystem));
}

void CD3D11Driver::InitAdapters(void)
{
    UINT pSelectedAdapter = 0;
    std::vector <IDXGIAdapter*> vAdapters;
    std::vector <DXGI_ADAPTER_DESC> vAdaptersDesc;

    if (!DXGIFactory)
    {
        // Create a DXGIFactory object.
        if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory2), (void**)&DXGIFactory)))
        {
            throw std::runtime_error("Critical Error");
        }
    }


    for (UINT i = 0;
    DXGIFactory->EnumAdapters(i, &Adapter) != DXGI_ERROR_NOT_FOUND;
        ++i)
    {
        vAdapters.push_back(Adapter);
        DXGI_ADAPTER_DESC adapDesc;
        ::ZeroMemory(&adapDesc, sizeof(DXGI_ADAPTER_DESC));
        // Get the adapter (video card) description.
        if (FAILED(Adapter->GetDesc(&adapDesc)))
        {
            throw std::runtime_error("Critical Error");
        }

        vAdaptersDesc.push_back(adapDesc);

        // Select best performance adapter
        if (vAdaptersDesc[i].DedicatedVideoMemory > vAdaptersDesc[pSelectedAdapter].DedicatedVideoMemory)
            pSelectedAdapter = i;
    }

    for (auto iAdapter : vAdapters)
        if (iAdapter != vAdapters[pSelectedAdapter])
            iAdapter->Release();

    Adapter = vAdapters[pSelectedAdapter];
}

bool CD3D11Driver::initOutput(HWND hwnd, bool pureSoftware)
{
    HRESULT hr;

    // Get adapter used by this device and query informations

    if (SUCCEEDED(DXGIDevice->GetAdapter(&Adapter)))
    {
        DXGI_ADAPTER_DESC adapDesc;
        ::ZeroMemory(&adapDesc, sizeof(DXGI_ADAPTER_DESC));

        Adapter->GetDesc(&adapDesc);

        s32 Product = HIWORD(adapDesc.AdapterLuid.HighPart);
        s32 Version = LOWORD(adapDesc.AdapterLuid.HighPart);
        s32 SubVersion = HIWORD(adapDesc.AdapterLuid.LowPart);
        s32 Build = LOWORD(adapDesc.AdapterLuid.LowPart);

        char description[120];
        wcstombs(description, adapDesc.Description, wcslen(adapDesc.Description) + 1);    // convert from wchar_t* to char*
        char tmp[512];
        snprintf(tmp, 512, "%s, Revision %d", description, adapDesc.Revision);
        os::Printer::log(tmp, ELL_INFORMATION);
        os::Printer::log("Currently available Video Memory (kB)", core::stringc((int)(adapDesc.DedicatedVideoMemory / 1024)).c_str());

        // Assign vendor name based on vendor id.
        VendorID = static_cast<u16>(adapDesc.VendorId);
        switch (VendorID)
        {
        case 0x1002: VendorName = "ATI Technologies Inc."; break;
        case 0x10DE: VendorName = "NVIDIA Corporation"; break;
        case 0x102B: VendorName = "Matrox Electronic Systems Ltd."; break;
        case 0x121A: VendorName = "3dfx Interactive Inc"; break;
        case 0x5333: VendorName = "S3 Graphics Co., Ltd."; break;
        case 0x8086: VendorName = "Intel Corporation"; break;
        default: VendorName = "Unknown VendorId: "; VendorName += (u32)adapDesc.VendorId; break;
        }

        const WCHAR* wc = adapDesc.Description;
        _bstr_t b(wc);

        ZeroMemory(tmp, 512);
        snprintf(tmp, 512, "DirectX %s Adapter: %s",
#ifdef _WIN32_WINNT_WIN10
            (FeatureLevel == D3D_FEATURE_LEVEL_12_1) ? "12.1" :
            (FeatureLevel == D3D_FEATURE_LEVEL_12_0) ? "12.0" :
#endif
#ifdef _WIN32_WINNT_WINBLUE
            (FeatureLevel == D3D_FEATURE_LEVEL_11_1) ? "11.1" :
#endif
            (FeatureLevel == D3D_FEATURE_LEVEL_11_0) ? "11.0" :
            (FeatureLevel == D3D_FEATURE_LEVEL_10_1) ? "10.1" : "10.0", b.operator const char*());

        size_t length = 512;
        std::wstring out(length, L'#');
        mbstowcs(&out[0], tmp, length);

        DriverAndFeatureName = out;
    }

    if ( FAILED(Adapter->EnumOutputs(0, &Output)) )
    {
        UINT i = 0;
        IDXGIOutput * pOutput;
        std::vector<IDXGIOutput*> vOutputs;
        while (Adapter->EnumOutputs(i, &pOutput) != DXGI_ERROR_NOT_FOUND)
        {
            vOutputs.push_back(pOutput);
            ++i;
        }

        os::Printer::log("Error, could not get monitor", ELL_INFORMATION);
        //throw std::runtime_error("Critical Error");
    }

    INT selectedMode = -1;
    DXGI_MODE_DESC* displayModes = nullptr;

    if (Output)
    {
        UINT numModes = 0;

        // Get the number of elements
        hr = Output->GetDisplayModeList(D3DColorFormat, 0, &numModes, NULL);
        if (FAILED(hr))
        {
            throw std::runtime_error("Critical Error");
        }

        displayModes = new DXGI_MODE_DESC[numModes];

        // Get the list
        hr = Output->GetDisplayModeList(D3DColorFormat, 0, &numModes, displayModes);
        if (FAILED(hr))
        {
            throw std::runtime_error("Critical Error");
        }

        for (u32 i = 0; i != numModes; ++i)
        {
            if (displayModes[i].Height == Params.WindowSize.Height &&
                displayModes[i].Width == Params.WindowSize.Width &&
                displayModes[i].Format == D3DColorFormat)
                selectedMode = i;
        }

        if (selectedMode == -1)
        {
            selectedMode = numModes - 1;
            //throw std::runtime_error("Critical Error");
        }
    }

    // Initialize the swap chain description.
    ZeroMemory(&present, sizeof(present));

    // Set to a single back buffer.
    present.BufferCount = 2;

    // Set the width and height of the back buffer.
    //present.BufferDesc.Width  = displayModes[selectedMode].Width;
    //present.BufferDesc.Height = displayModes[selectedMode].Height;
    //
    //// Set regular 32-bit surface for the back buffer.
    //present.BufferDesc.Format = displayModes[selectedMode].Format;
    //
    //// Set the refresh rate of the back buffer.
    //if ( this->Params.Vsync )
    //    present.BufferDesc.RefreshRate = displayModes[selectedMode].RefreshRate;
    //else
    //{
    //    present.BufferDesc.RefreshRate.Numerator = 0;
    //    present.BufferDesc.RefreshRate.Denominator = 1;
    //}

    // Set the handle for the window to render to.
    //present.OutputWindow = hwnd;

    // Set the usage of the back buffer.
    present.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    if (displayModes)
    {
        present.Width = displayModes[selectedMode].Width;
        present.Height = displayModes[selectedMode].Height;
    }
    else
    {
        present.Width  = 0;                                     // Use automatic sizing.
        present.Height = 0;
    }
    present.Format = DXGI_FORMAT_B8G8R8A8_UNORM;           // This is the most common swap chain format.

    // Turn multisampling off.
    present.SampleDesc.Count = 1;
    present.SampleDesc.Quality = 0;

    // enable anti alias if possible and desired
    if (Params.AntiAlias > 0)
    {
        if (Params.AntiAlias > 32)
            Params.AntiAlias = 32;

        UINT qualityLevels = 0;

        while (Params.AntiAlias > 0)
        {
            if (SUCCEEDED(Device->CheckMultisampleQualityLevels(DXGI_FORMAT_B8G8R8A8_UNORM, (UINT)Params.AntiAlias, &qualityLevels)))
            {
                if (qualityLevels)
                {
                    // Turn multisampling on.
                    present.SampleDesc.Count = Params.AntiAlias;
                    present.SampleDesc.Quality = qualityLevels - 1;
                    break;
                }
            }
            --Params.AntiAlias;
        }

        if (Params.AntiAlias == 0)
        {
            os::Printer::log("Anti aliasing disabled because hardware/driver lacks necessary caps.", ELL_WARNING);
        }
    }
    present.Stereo = false;
    present.Scaling = DXGI_SCALING_STRETCH;
    present.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // All Windows Store apps must use this SwapEffect.

    // Set to full screen or windowed mode.
    //present.Windowed = !Params.Fullscreen;

    // Set the scan line ordering and scaling to unspecified.
    //present.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    //present.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    // Discard the back buffer contents after presenting.
    //present.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    //present.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // All Windows Store apps must use this SwapEffect.

    // Don't set the advanced flags.
    present.Flags = 0; // DXGI_SWAP_CHAIN_FLAG_DISPLAY_ONLY | DXGI_SWAP_CHAIN_FLAG_RESTRICTED_CONTENT; // DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenPresent;
    fullscreenPresent.RefreshRate.Numerator = 0; // displayModes[selectedMode].RefreshRate;
    fullscreenPresent.RefreshRate.Denominator = 1;
    fullscreenPresent.Scaling = DXGI_MODE_SCALING_STRETCHED;
    fullscreenPresent.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    fullscreenPresent.Windowed = !Params.Fullscreen;


    //if ( Params.Fullscreen )
    //{
    //    present.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
    //    present.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST;
    //    present.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    //    present.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    //
    //    // Optimization: query a display mode closest to requested.
    //    // This will make surface flip, instead of blit
    //    if ( Output )
    //    {
    //        DXGI_MODE_DESC* request = &(present.BufferDesc);
    //        DXGI_MODE_DESC match;
    //        ZeroMemory(&match, sizeof(DXGI_MODE_DESC));
    //        hr = Output->FindClosestMatchingMode(request, &match, Device);
    //        if ( SUCCEEDED(hr) )
    //        {
    //            present.BufferDesc.Format = match.Format;
    //            present.BufferDesc.Height = match.Height;
    //            present.BufferDesc.RefreshRate = match.RefreshRate;
    //            present.BufferDesc.Scaling = match.Scaling;
    //            present.BufferDesc.ScanlineOrdering = match.ScanlineOrdering;
    //            present.BufferDesc.Width = match.Width;
    //
    //            // Adjust screen size accordingly
    //            ScreenSize.Width = match.Width;
    //            ScreenSize.Height = match.Height;
    //        }
    //    }
    //}

    delete[] displayModes;


    hr = DXGIFactory->CreateSwapChainForHwnd(Device, hwnd, &present, &fullscreenPresent, Output, &SwapChain);
    if (FAILED(hr))
    {
        throw std::runtime_error("Critical Error");
    }

    if (FAILED(DXGIFactory->MakeWindowAssociation(hwnd, 0
        | DXGI_MWA_NO_WINDOW_CHANGES
        | DXGI_MWA_NO_ALT_ENTER)))
        return false;


    hr = Device->QueryInterface(IID_ID3D11InfoQueue, (void**)&m_infoQueue);
    if (SUCCEEDED(hr))
    {
        m_infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
        m_infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);
        m_infoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_WARNING, false);

        D3D11_INFO_QUEUE_FILTER filter;
        memset(&filter, 0, sizeof(filter));

        D3D11_MESSAGE_CATEGORY catlist[] =
        {
            D3D11_MESSAGE_CATEGORY_STATE_SETTING,
            D3D11_MESSAGE_CATEGORY_EXECUTION,
        };
        //filter.DenyList.NumCategories = COUNTOF(catlist);
        filter.DenyList.pCategoryList = catlist;
        m_infoQueue->PushStorageFilter(&filter);

        SAFE_RELEASE(m_infoQueue);
    }

    // Ensure that DXGI does not queue more than one frame at a time. This both reduces latency and
    // ensures that the application will only render after each VSync, minimizing power consumption.
    DX::ThrowIfFailed(
        DXGIDevice->SetMaximumFrameLatency(1)
        );

    return true;
}

bool getIntelExtensions(ID3D11Device* _device)
{
    uint8_t temp[28];

    D3D11_BUFFER_DESC desc;
    desc.ByteWidth = sizeof(temp);
    desc.Usage = D3D11_USAGE_STAGING;
    desc.BindFlags = 0;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = &temp;
    initData.SysMemPitch = sizeof(temp);
    initData.SysMemSlicePitch = 0;

    //bx::StaticMemoryBlockWriter writer(&temp, sizeof(temp));
    //bx::write(&writer, "INTCEXTNCAPSFUNC", 16);
    //bx::write(&writer, UINT32_C(0x00010000));
    //bx::write(&writer, UINT32_C(0));
    //bx::write(&writer, UINT32_C(0));
    //
    //ID3D11Buffer* buffer;
    //HRESULT hr = _device->CreateBuffer(&desc, &initData, &buffer);
    //
    //if (SUCCEEDED(hr))
    //{
    //    buffer->Release();
    //
    //    bx::MemoryReader reader(&temp, sizeof(temp));
    //    bx::skip(&reader, 16);
    //
    //    uint32_t version;
    //    bx::read(&reader, version);
    //
    //    uint32_t driverVersion;
    //    bx::read(&reader, driverVersion);
    //
    //    return version <= driverVersion;
    //}

    return false;
};

//! initialises the Direct3D API
bool CD3D11Driver::initDriver(HWND hwnd, bool pureSoftware)
{
    InitializeModulResourceDirect3D11();
    System::Resource::ResourceManager::Instance()->Initialize();

    HRESULT hr;

    D3DLibrary = LoadLibrary( __TEXT("d3d11.dll") );
    if (!D3DLibrary)
    {
        os::Printer::log("Error, could not load d3d11.dll.", ELL_ERROR);
        return false;
    }

    //CreateDeviceIndependentResources();
//#if Dx11VisualStudioDebug==0
    InitAdapters();
//#endif

    // Device flags
    // This flag adds support for surfaces with a different color channel ordering
    // than the API default. It is required for compatibility with Direct2D.
    UINT deviceFlags = 0; // D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    if (!Params.DriverMultithreaded)
        deviceFlags |= D3D11_CREATE_DEVICE_SINGLETHREADED;
#if Dx11VisualStudioDebug
    deviceFlags |= D3D11_CREATE_DEVICE_DEBUG; // | D3D11_CREATE_DEVICE_DEBUGGABLE | D3D11_CREATE_DEVICE_SINGLETHREADED;
#endif

    // Get function address
    PFN_D3D11_CREATE_DEVICE CreateDeviceFunc = (PFN_D3D11_CREATE_DEVICE) GetProcAddress( D3DLibrary, "D3D11CreateDevice" );
    if( !CreateDeviceFunc )
    {
        os::Printer::log("Error, could not get D3D11CreateDevice function.", ELL_ERROR);
        return false;
    }

    // Try creating hardware device
    static D3D_FEATURE_LEVEL RequestedLevels[] = {
#ifdef _WIN32_WINNT_WIN10
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
#endif
#ifdef _WIN32_WINNT_WINBLUE
        D3D_FEATURE_LEVEL_11_1,
#endif
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };

    ID3D11Device* device;
    ID3D11DeviceContext* context;

    u32 RequestedLevelsSize = sizeof( RequestedLevels ) / sizeof( RequestedLevels[0] );
#if	Dx11VisualStudioDebug
        //DriverType = D3D_DRIVER_TYPE_WARP;
        hr = CreateDeviceFunc(NULL/*Adapter*/, D3D_DRIVER_TYPE_WARP, NULL, deviceFlags, RequestedLevels, RequestedLevelsSize, D3D11_SDK_VERSION, &device, &FeatureLevel, &context);
#else
        hr = CreateDeviceFunc(Adapter, D3D_DRIVER_TYPE_UNKNOWN/*DriverType*/, NULL, deviceFlags, RequestedLevels, RequestedLevelsSize, D3D11_SDK_VERSION, &device, &FeatureLevel, &context);
#endif
    if (FAILED(hr))
    {
        //printf("[HLSL SHADER] Error: %s error description: %s\n",
        //    DXGetErrorString(hr), DXGetErrorDescription(hr));
        os::Printer::log("Error, could not create device.", ELL_ERROR);
        throw std::runtime_error("Error, could not create device.");
        return false;
    }

    
    DX::ThrowIfFailed( device->QueryInterface(__uuidof(ID3D11Device1), reinterpret_cast<void**>(&Device)) );
    DX::ThrowIfFailed( context->QueryInterface(__uuidof(ID3D11DeviceContext1), reinterpret_cast<void**>(&ImmediateContext)) );
    DX::ThrowIfFailed( Device->QueryInterface(__uuidof(IDXGIDevice1), reinterpret_cast<void**>(&DXGIDevice)) );

    // Printing type of Direct3D 11 device and feature level
    char msg[512];
    sprintf(msg, "Using %s device, feature level %s",
                 (DriverType == D3D_DRIVER_TYPE_HARDWARE) ? "Hardware" : "WARP",
#ifdef _WIN32_WINNT_WIN10
                    (FeatureLevel == D3D_FEATURE_LEVEL_12_1) ? "12.1" :
                    (FeatureLevel == D3D_FEATURE_LEVEL_12_0) ? "12.0" :
#endif
#ifdef _WIN32_WINNT_WINBLUE
                    (FeatureLevel == D3D_FEATURE_LEVEL_11_1) ? "11.1" :
#endif
                    (FeatureLevel == D3D_FEATURE_LEVEL_11_0) ? "11.0" :
                    (FeatureLevel == D3D_FEATURE_LEVEL_10_1) ? "10.1" : "10.0");
    os::Printer::log(msg, ELL_INFORMATION);

    //initDriver2D(hwnd, pureSoftware);

    initOutput(hwnd, pureSoftware);
    //getIntelExtensions(Device);

    // Get default render target
    ID3D11Texture2D* backBuffer = NULL;
    hr = SwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast<void**>( &backBuffer ) );
    if( FAILED(hr))
    {
        os::Printer::log("Error, could not get back buffer.", ELL_ERROR);
        return false;
    }
    hr = Device->CreateRenderTargetView( backBuffer, NULL, &DefaultBackBuffer );
    if( FAILED(hr))
    {
        os::Printer::log("Error, could not create render target view.", ELL_ERROR);
        return false;
    }
    backBuffer->Release();

    // creating depth buffer
    DefaultDepthBuffer = createDepthStencilView(Params.WindowSize);

    // set exposed data
    ExposedData.D3D11.D3DDev11  = Device;
    ExposedData.D3D11.SwapChain = SwapChain;
    ExposedData.D3D11.HWnd      = hwnd;

    // register vertex types
    for ( u32 i = 0; i != EVT_MAX_VERTEX_TYPE; ++i )
    {
        VertexDeclarationMap[i].clear();
        {
            VertexDeclarationMap[i].push_back(D3D11_INPUT_ELEMENT_DESC() = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(S3DVertex, S3DVertex::Pos),     D3D11_INPUT_PER_VERTEX_DATA, 0 });
            VertexDeclarationMap[i].push_back(D3D11_INPUT_ELEMENT_DESC() = { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(S3DVertex, S3DVertex::Normal),  D3D11_INPUT_PER_VERTEX_DATA, 0 });
            VertexDeclarationMap[i].push_back(D3D11_INPUT_ELEMENT_DESC() = { "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM,  0, offsetof(S3DVertex, S3DVertex::Color),   D3D11_INPUT_PER_VERTEX_DATA, 0 });
            VertexDeclarationMap[i].push_back(D3D11_INPUT_ELEMENT_DESC() = { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, offsetof(S3DVertex, S3DVertex::TCoords), D3D11_INPUT_PER_VERTEX_DATA, 0 });

            if ( i == EVT_2TCOORDS || i == EVT_TERRAIN || i == EVT_SKINNING )
                VertexDeclarationMap[i].push_back(D3D11_INPUT_ELEMENT_DESC() = { "TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(S3DVertex2TCoords, S3DVertex2TCoords::TCoords2), D3D11_INPUT_PER_VERTEX_DATA, 0 });

            if ( i == EVT_TANGENTS )
            {
                VertexDeclarationMap[i].push_back(D3D11_INPUT_ELEMENT_DESC() = { "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT,  0, offsetof(S3DVertexTangents, S3DVertexTangents::Binormal), D3D11_INPUT_PER_VERTEX_DATA, 0 });
                VertexDeclarationMap[i].push_back(D3D11_INPUT_ELEMENT_DESC() = { "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,  0, offsetof(S3DVertexTangents, S3DVertexTangents::Tangent),  D3D11_INPUT_PER_VERTEX_DATA, 0 });
                //throw std::runtime_error("not implement yet!");
            }

            if ( i == EVT_TERRAIN )
                VertexDeclarationMap[i].push_back(D3D11_INPUT_ELEMENT_DESC() = { "COLOR",    1, DXGI_FORMAT_R8G8B8A8_UNORM, 0, offsetof(S3DChunkVertex, S3DChunkVertex::VertexShading), D3D11_INPUT_PER_VERTEX_DATA, 0 });

            if ( i == EVT_SKINNING )
            {
                VertexDeclarationMap[i].push_back(D3D11_INPUT_ELEMENT_DESC() = { "BLENDINDICES", 0, DXGI_FORMAT_R8G8B8A8_UINT,  0, offsetof(S3DSkinningVertex, S3DSkinningVertex::Bones),   D3D11_INPUT_PER_VERTEX_DATA, 0 });
                VertexDeclarationMap[i].push_back(D3D11_INPUT_ELEMENT_DESC() = { "BLENDWEIGHT",  0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, offsetof(S3DSkinningVertex, S3DSkinningVertex::Weights), D3D11_INPUT_PER_VERTEX_DATA, 0 });
            }
        }

        VertexDeclarationMap[i].push_back(D3D11_INPUT_ELEMENT_DESC() = { "TEXCOORD", 2, DXGI_FORMAT_R32G32B32_FLOAT, 1, offsetof(S3DInstance, S3DInstance::WorldView),                               D3D11_INPUT_PER_INSTANCE_DATA, 1 });
        VertexDeclarationMap[i].push_back(D3D11_INPUT_ELEMENT_DESC() = { "TEXCOORD", 3, DXGI_FORMAT_R32G32B32_FLOAT, 1, offsetof(S3DInstance, S3DInstance::WorldView) + (sizeof(float) * 3),         D3D11_INPUT_PER_INSTANCE_DATA, 1 });
        VertexDeclarationMap[i].push_back(D3D11_INPUT_ELEMENT_DESC() = { "TEXCOORD", 4, DXGI_FORMAT_R32G32B32_FLOAT, 1, offsetof(S3DInstance, S3DInstance::WorldView) + (sizeof(float) * 3 * 2),     D3D11_INPUT_PER_INSTANCE_DATA, 1 });
        VertexDeclarationMap[i].push_back(D3D11_INPUT_ELEMENT_DESC() = { "TEXCOORD", 5, DXGI_FORMAT_R32G32B32_FLOAT, 1, offsetof(S3DInstance, S3DInstance::WorldView) + (sizeof(float) * 3 * 3),     D3D11_INPUT_PER_INSTANCE_DATA, 1 });
        VertexDeclarationMap[i].push_back(D3D11_INPUT_ELEMENT_DESC() = { "TEXCOORD", 6, DXGI_FORMAT_R32G32B32_FLOAT, 1, offsetof(S3DInstance, S3DInstance::InstaceFloatData),                        D3D11_INPUT_PER_INSTANCE_DATA, 1 });
        VertexDeclarationMap[i].push_back(D3D11_INPUT_ELEMENT_DESC() = { "TEXCOORD", 7, DXGI_FORMAT_R32G32B32_UINT,  1, offsetof(S3DInstance, S3DInstance::InstaceUIntData),                         D3D11_INPUT_PER_INSTANCE_DATA, 1 });

        if ( i == EVT_PARTICLE )
        {
            VertexDeclarationMap[i].push_back(D3D11_INPUT_ELEMENT_DESC() = { "COLOR",    2, DXGI_FORMAT_R8G8B8A8_UNORM, 1, offsetof(S3DTextureInstance, S3DTextureInstance::Color),                     D3D11_INPUT_PER_INSTANCE_DATA, 1 });
            VertexDeclarationMap[i].push_back(D3D11_INPUT_ELEMENT_DESC() = { "TEXCOORD", 8, DXGI_FORMAT_R32G32_FLOAT,   1, offsetof(S3DTextureInstance, S3DTextureInstance::UVposMod),                  D3D11_INPUT_PER_INSTANCE_DATA, 1 });
        }
    }

    // init states description
    BlendDesc.reset();
    RasterizerDesc.reset();
    DepthStencilDesc.reset();
    for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; i++)
        SamplerDesc[i].reset();

    // Init dynamic buffers
    reallocateDynamicBuffers( 100 * sizeof(S3DVertex2TCoords), 100 * sizeof(short) );

    // With all DX 11 objects created, init driver states

    // Only enable multisampling if needed
    disableFeature(EVDF_TEXTURE_MULTISAMPLING, true);

    // Set render targets
    setRenderTarget(0, true, true);

    // set fog mode
    //setFog(FogColor, FogType, FogStart, FogEnd, FogDensity, PixelFog, RangeFog);
    setFog(FogColor, (E_FOG_TYPE)0, FogStart, FogEnd, FogDensity, PixelFog, RangeFog);

    ResetRenderStates = true;

    // create materials
    createMaterialRenderers();

    // clear textures
    for (u16 i = 0; i < MATERIAL_MAX_TEXTURES; i++)
        setActiveTexture(i, 0);

    blankImage->grab();
    blankTexture = createTexture(blankImage, "internal_null_texture");
    return true;
}

bool CD3D11Driver::initDriver2D(HWND hwnd, bool pureSoftware)
{
    // Create the Direct2D device object and a corresponding context.
    DX::ThrowIfFailed(
        D2DFactory->CreateDevice(DXGIDevice, &D2DDevice)
        );
    
    DX::ThrowIfFailed(
        D2DDevice->CreateDeviceContext(
            D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
            &D2DContext
            )
        );

    DX::ThrowIfFailed(
        D2DFactory->CreateDrawingStateBlock(&m_stateBlock)
        );

    return true;
}

// These are the resources required independent of the device.
void CD3D11Driver::CreateDeviceIndependentResources()
{
    D2D1_FACTORY_OPTIONS options;
    ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));

#if defined(_DEBUG)
    // If the project is in a debug build, enable Direct2D debugging via SDK Layers.
    options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

    DX::ThrowIfFailed(
        D2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED,
            options,
            &D2DFactory
            )
        );

    //DX::ThrowIfFailed(
    //    DWriteCreateFactory(
    //        DWRITE_FACTORY_TYPE_SHARED,
    //        __uuidof(IDWriteFactory),
    //        (IUnknown**)&DwriteFactory
    //        )
    //    );

    DX::ThrowIfFailed(
        CoCreateInstance(
            CLSID_WICImagingFactory,
            nullptr,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&WicFactory)
            )
        );
}


bool ShaderHLSLD11::createVertexShader(ID3D11Device* device, System::IO::IFileReader* file, const char* entryPoint, const char* shaderModel)
{
    HRESULT hr = S_OK;
    
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

    ID3DBlob* errorMessage;
    ID3DBlob* ShaderBuffer;

    hr = D3DCompile(
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
        throw std::runtime_error("invalid shader");
        return false;
    }

    // Create the vertex shader from the buffer.
    hr = device->CreateVertexShader(ShaderBuffer->GetBufferPointer(), ShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
    if (FAILED(hr))
    {
        // Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
        SAFE_RELEASE(ShaderBuffer);
        throw std::runtime_error("invalid shader");
        return false;
    }

    initializeConstantBuffers((CD3D11Driver *)getVideoDriver(), ShaderBuffer, EST_VERTEX_SHADER);
    hr = enumInputLayout((CD3D11Driver *)getVideoDriver(), ShaderBuffer);
    if (FAILED(hr))
    {
        // Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
        SAFE_RELEASE(ShaderBuffer);
        throw std::runtime_error("invalid shader");
        return false;
    }
    SAFE_RELEASE(ShaderBuffer);

    return true;
}

bool ShaderHLSLD11::createGeometryShader(ID3D11Device* device, System::IO::IFileReader* file, const char* entryPoint, const char* shaderModel)
{
    HRESULT hr = S_OK;

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
    if (file->Read((byte*)source.data(), file->Size()) != file->Size())
        throw std::runtime_error("shader not found");

    ID3DBlob* errorMessage;
    ID3DBlob* ShaderBuffer;

    hr = D3DCompile(
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

    if ( FAILED(hr) )
    {
        if ( errorMessage )
        {
            LPVOID pCompilErrors = errorMessage->GetBufferPointer();
            printf("[HLSL SHADER] Vertex Shader Compile Error: %s \r\n", (const char*)pCompilErrors);
        }
        os::Printer::log("Error, could not open effect file");
        // Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
        SAFE_RELEASE(ShaderBuffer);
        throw std::runtime_error("invalid shader");
        return false;
    }

    // Create the vertex shader from the buffer.
    hr = device->CreateGeometryShader(ShaderBuffer->GetBufferPointer(), ShaderBuffer->GetBufferSize(), NULL, &m_geometryShader);
    if ( FAILED(hr) )
    {
        throw std::runtime_error("invalid shader");
        return false;
    }

    initializeConstantBuffers((CD3D11Driver *)getVideoDriver(), ShaderBuffer, EST_GEOMETRY_SHADER);

    // Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
    SAFE_RELEASE(ShaderBuffer);
    return true;
}

bool ShaderHLSLD11::createPixelShader(ID3D11Device* device, System::IO::IFileReader* file, const char* entryPoint, const char* shaderModel)
{
    HRESULT hr = S_OK;

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

    ID3DBlob* errorMessage;
    ID3DBlob* ShaderBuffer;

    hr = D3DCompile(
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
        throw std::runtime_error("invalid shader");
        return false;
    }

    // Create the vertex shader from the buffer.
    hr = device->CreatePixelShader(ShaderBuffer->GetBufferPointer(), ShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
    if (FAILED(hr))
    {
        throw std::runtime_error("invalid shader");
        return false;
    }

    initializeConstantBuffers((CD3D11Driver *)getVideoDriver(), ShaderBuffer, EST_FRAGMENT_SHADER);

    // Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
    SAFE_RELEASE(ShaderBuffer);
    return true;
}

void ShaderHLSLD11::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, const char* shaderFilename)
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

    return;
}

IShader* CD3D11Driver::createShader(System::IO::IFileReader* vertexShader, System::IO::IFileReader* fragmentShader, System::IO::IFileReader* geometryShader, System::IO::IFileReader* tesselationShader)
{
    ShaderHLSLD11* gpuProgram = new ShaderHLSLD11(this, EST_HIGH_LEVEL_SHADER);
    if (vertexShader)
    {
        const char* ShaderModel = "vs_3_0";     // DirectX 9c
        if (FeatureLevel >= D3D_FEATURE_LEVEL_11_0)
            ShaderModel = "vs_5_0";
        else if (FeatureLevel >= D3D_FEATURE_LEVEL_10_0)
            ShaderModel = "vs_4_0";

        const char* ShaderEntryPoint = "vs_main";
        
        //std::string path(vertexShader);
        //int pos = path.find_last_of('/');
        //std::string shaderPath;
        //shaderPath += path.substr(0, pos);
        //shaderPath += "/HLSL/";
        //shaderPath += ShaderModel;
        //shaderPath += path.substr(pos, path.size() - pos);
        //shaderPath += ".hlsl";

        gpuProgram->createVertexShader(Device, vertexShader, ShaderEntryPoint, ShaderModel);
    }

    if (geometryShader)
    {
        const char* ShaderModel = "gs_3_0";     // DirectX 9c
        if (FeatureLevel >= D3D_FEATURE_LEVEL_11_0)
            ShaderModel = "gs_5_0";
        else if (FeatureLevel >= D3D_FEATURE_LEVEL_10_0)
            ShaderModel = "gs_4_0";

        const char* ShaderEntryPoint = "gs_main";

        //std::string path(geometryShader);
        //int pos = path.find_last_of('/');
        //std::string shaderPath;
        //shaderPath += path.substr(0, pos);
        //shaderPath += "/HLSL/";
        //shaderPath += ShaderModel;
        //shaderPath += path.substr(pos, path.size() - pos);
        //shaderPath += ".hlsl";

        gpuProgram->createGeometryShader(Device, geometryShader, ShaderEntryPoint, ShaderModel);
    }

    if (fragmentShader)
    {
        const char* ShaderModel = "ps_3_0";     // DirectX 9c
        if (FeatureLevel >= D3D_FEATURE_LEVEL_11_0)
            ShaderModel = "ps_5_0";
        else if (FeatureLevel >= D3D_FEATURE_LEVEL_10_0)
            ShaderModel = "ps_4_0";

        const char* ShaderEntryPoint = "ps_main";

        //std::string path(fragmentShader);
        //int pos = path.find_last_of('/');
        //std::string shaderPath;
        //shaderPath += path.substr(0, pos);
        //shaderPath += "/HLSL/";
        //shaderPath += ShaderModel;
        //shaderPath += path.substr(pos, path.size() - pos);
        //shaderPath += ".hlsl";

        gpuProgram->createPixelShader(Device, fragmentShader, ShaderEntryPoint, ShaderModel);
    }

    gpuProgram->Init();
    return gpuProgram;
}

HRESULT ShaderHLSLD11::initializeConstantBuffers(CD3D11Driver * d3dDevice, ID3DBlob * blob, E_ShaderTypes shaderType)
{
    HRESULT hr = S_OK;

    // get description of precompiled shader
    ID3D11ShaderReflection* pReflector = NULL;
    D3DReflect(
        blob->GetBufferPointer(),
        blob->GetBufferSize(),
        IID_ID3D11ShaderReflection,
        (void**)&pReflector
        );

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
        D3D11_BUFFER_DESC BufferDesc;
        BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        BufferDesc.ByteWidth = buffdesc.shaderBuffer.Size;
        BufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        BufferDesc.MiscFlags = 0;
        BufferDesc.StructureByteStride = 0;

                                                             // Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
        hr = d3dDevice->GetDxDevice()->CreateBuffer(&BufferDesc, NULL, &buffdesc.m_constantBuffer);
        if (FAILED(hr))
            throw std::runtime_error("invalid buffer");

        // init cpu cache
        buffdesc.DataBuffer.set_used(buffdesc.shaderBuffer.Size);
        ::ZeroMemory(buffdesc.DataBuffer.pointer(), buffdesc.DataBuffer.size());
        buffdesc.DataBufferChanged = false;

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

                    ui.m_location       = (j & 0xFF) | (mj << 8);                          // Buffer Variable Index
                    ui.m_length         = structDecl.varType.Elements;
                    ui.m_variableType   = structDecl.varType.Type;
                    ui.m_class          = structDecl.varType.Class;
                    ui.m_name           = name;
                    ui.m_size           = 0;
                    ui.m_type           = ESVT_CONSTANT;
                    ui.m_shaderIndex    = ShaderBuffers.size() - 1; // Buffer Desc Index

                    AddShaderVariable(&ui);
                }
            }

            {
                // Generic Descriptor
                ShaderVariableDescriptor ui;

                ui.m_location       = j & 0xFF;                          // Buffer Variable Index
                ui.m_length         = constant.varType.Elements;
                ui.m_variableType   = constant.varType.Type;
                ui.m_class          = constant.varType.Class;
                ui.m_name           = constant.VariableDesc.Name;
                ui.m_size           = constant.VariableDesc.Size;
                ui.m_type           = ESVT_CONSTANT;
                ui.m_shaderIndex    = ShaderBuffers.size() - 1; // Buffer Desc Index

                AddShaderVariable(&ui);
            }

            buffdesc.Variables.push_back(constant);
        }

        S_GPU_SHADER_VARIABLE_DEFAULT_LINK const* mLinkPtr = sDefaultShaderVariableLink;
        do
        {
            LinkShaderVariable(mLinkPtr->name, mLinkPtr->id);
        } while ((++mLinkPtr)->id != EGVAT_MAX_VALUE);

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

HRESULT ShaderHLSLD11::enumInputLayout(CD3D11Driver * d3dDevice, ID3DBlob * VSBlob)
{
    // http://www.sunandblackcat.com/tipFullView.php?l=eng&topicid=26
    HRESULT hr = S_OK;

    // Get description from precompiled shader
    ID3D11ShaderReflection* vertReflect;
    D3DReflect(
        VSBlob->GetBufferPointer(),
        VSBlob->GetBufferSize(),
        IID_ID3D11ShaderReflection,
        (void**)&vertReflect
        );

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

    u32 it = 0;
    for (; it != EVT_MAX_VERTEX_TYPE; ++it)
    {
        mVertexDeclartion.clear();
        core::array<D3D11_INPUT_ELEMENT_DESC> const& decl = d3dDevice->getVertexDeclaration((E_VERTEX_TYPE)it);

        bool match = decl.size() != 0;
        for (u32 itr = 0; itr < inputLayoutArray.size(); ++itr)
        {
            bool found = false;
            for (u32 ite = 0; ite < decl.size(); ++ite)
            {
                if (decl[ite].SemanticIndex == inputLayoutArray[itr].SemanticIndex && !strcmp(decl[ite].SemanticName, inputLayoutArray[itr].SemanticName))
                {
                    mVertexDeclartion.push_back(decl[ite]);
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                match = false;
                break;
            }
        }

        if (match && decl.size() >= inputLayoutArray.size())
            break;
    }

    _IRR_DEBUG_BREAK_IF(it == EVT_MAX_VERTEX_TYPE);

    CD3D11Driver::LayoutKey iKey;

    iKey.elementDesc = mVertexDeclartion.pointer();
    iKey.pInputSignature = inputLayoutArray.size();

    CD3D11Driver::InputLayoutStateMap::iterator iLayout = d3dDevice->InputLayoutMap.find(iKey);
    if ( iLayout == d3dDevice->InputLayoutMap.end() )
    {
        // create input layout from previosly created description
        hr = d3dDevice->getExposedVideoData().D3D11.D3DDev11->CreateInputLayout(
            mVertexDeclartion.pointer(), //d3dDevice->getVertexDeclaration((E_VERTEX_TYPE)it).const_pointer(),
            inputLayoutArray.size(),
            VSBlob->GetBufferPointer(),
            VSBlob->GetBufferSize(),
            &m_layout
            );

        if ( FAILED(hr) )
        {
            // impossible to create input layout
            return hr;
        }

        d3dDevice->InputLayoutMap[iKey] = m_layout;
    }
    else
    {
        m_layout = iLayout->second;
    }

    return hr;
}

bool CD3D11Driver::setShaderConstant(ShaderVariableDescriptor const* desc, const void* values, int count, IHardwareBuffer* buffer /*= nullptr*/)
{
    ShaderHLSLD11* hlsl = (ShaderHLSLD11*)getActiveGPUProgram();
    
    if (desc->m_type == ESVT_INPUT_STREAM)
        return false;

    ShaderHLSLD11::CbufferDesc& cbuffer = hlsl->ShaderBuffers[desc->m_shaderIndex];
    ShaderHLSLD11::ShaderConstantDesc const& constantDecl = cbuffer.Variables[desc->m_location & 0xFF];

    UINT elementSize = constantDecl.elementSize * count;

    _IRR_DEBUG_BREAK_IF(elementSize > constantDecl.VariableDesc.Size || (cbuffer.DataBuffer.size() < (constantDecl.VariableDesc.StartOffset + elementSize)));

    if ( memcmp(&cbuffer.DataBuffer[constantDecl.VariableDesc.StartOffset], values, elementSize) )
    {
        memcpy(&cbuffer.DataBuffer[constantDecl.VariableDesc.StartOffset], values, elementSize);
        cbuffer.DataBufferChanged = true;
    }
    return true;
}

bool CD3D11Driver::FinalizeShaderConstant()
{
    bool needUpdateResource[EST_HIGH_LEVEL_SHADER];
    ::ZeroMemory(needUpdateResource, sizeof(needUpdateResource));

    bool haveResource[EST_HIGH_LEVEL_SHADER];
    ::ZeroMemory(haveResource, sizeof(haveResource));

    ShaderHLSLD11* hlsl = (ShaderHLSLD11*)getActiveGPUProgram();
    _IRR_DEBUG_BREAK_IF(!hlsl);

    for (u32 ib = 0; ib != hlsl->ShaderBuffers.size(); ++ib)
    {
        ShaderHLSLD11::CbufferDesc& cbuffer = hlsl->ShaderBuffers[ib];
        if ( cbuffer.DataBufferChanged )
        {
            //needUpdateResource[cbuffer.shaderType] = true;
            cbuffer.DataBufferChanged = false;
            _IRR_DEBUG_BREAK_IF(!cbuffer.m_constantBuffer);

            D3D11_MAPPED_SUBRESOURCE mappedResource;
            HRESULT hr = ImmediateContext->Map(cbuffer.m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
            _IRR_DEBUG_BREAK_IF(FAILED(hr));

            memcpy(mappedResource.pData, cbuffer.DataBuffer.pointer(), cbuffer.DataBuffer.size());

            ImmediateContext->Unmap(cbuffer.m_constantBuffer, 0);
        }

        u32 minSize = std::max(cbuffer.bindingDesc.BindPoint, (u32)0);
        if ( BindedBuffers[cbuffer.shaderType].size() <= (int)minSize )
        {
            for ( s32 i = 0; i <= (((s32)minSize - (s32)BindedBuffers[cbuffer.shaderType].size()) + 1); ++i )
                BindedBuffers[cbuffer.shaderType].push_back(nullptr);
        }

        haveResource[cbuffer.shaderType] = true;
        if ( BindedBuffers[cbuffer.shaderType][cbuffer.bindingDesc.BindPoint] != cbuffer.m_constantBuffer )
        {
            needUpdateResource[cbuffer.shaderType] = true;
            BindedBuffers[cbuffer.shaderType][cbuffer.bindingDesc.BindPoint] = cbuffer.m_constantBuffer;
        }
    }

    if ( needUpdateResource[EST_VERTEX_SHADER] && BindedBuffers[EST_VERTEX_SHADER].size() )
        ImmediateContext->VSSetConstantBuffers(0, BindedBuffers[EST_VERTEX_SHADER].size(), BindedBuffers[EST_VERTEX_SHADER].pointer());
    else if ( !haveResource[EST_VERTEX_SHADER] && BindedBuffers[EST_VERTEX_SHADER].size())
    {
        ::ZeroMemory(BindedBuffers[EST_VERTEX_SHADER].pointer(), BindedBuffers[EST_VERTEX_SHADER].size() * sizeof(ID3D11Buffer*));
        ImmediateContext->VSSetConstantBuffers(0, BindedBuffers[EST_VERTEX_SHADER].size(), BindedBuffers[EST_VERTEX_SHADER].pointer());
        BindedBuffers[EST_VERTEX_SHADER].set_used(0);
    }
    if ( needUpdateResource[EST_GEOMETRY_SHADER] && BindedBuffers[EST_GEOMETRY_SHADER].size() )
        ImmediateContext->GSSetConstantBuffers(0, BindedBuffers[EST_GEOMETRY_SHADER].size(), BindedBuffers[EST_GEOMETRY_SHADER].pointer());
    else if ( !haveResource[EST_GEOMETRY_SHADER] && BindedBuffers[EST_GEOMETRY_SHADER].size() )
    {
        ::ZeroMemory(BindedBuffers[EST_GEOMETRY_SHADER].pointer(), BindedBuffers[EST_GEOMETRY_SHADER].size() * sizeof(ID3D11Buffer*));
        ImmediateContext->GSSetConstantBuffers(0, BindedBuffers[EST_GEOMETRY_SHADER].size(), BindedBuffers[EST_GEOMETRY_SHADER].pointer());
        BindedBuffers[EST_GEOMETRY_SHADER].set_used(0);
    }
    if ( needUpdateResource[EST_FRAGMENT_SHADER] && BindedBuffers[EST_FRAGMENT_SHADER].size() )
        ImmediateContext->PSSetConstantBuffers(0, BindedBuffers[EST_FRAGMENT_SHADER].size(), BindedBuffers[EST_FRAGMENT_SHADER].pointer());
    else if ( !haveResource[EST_FRAGMENT_SHADER] && BindedBuffers[EST_FRAGMENT_SHADER].size() )
    {
        ::ZeroMemory(BindedBuffers[EST_FRAGMENT_SHADER].pointer(), BindedBuffers[EST_FRAGMENT_SHADER].size() * sizeof(ID3D11Buffer*));
        ImmediateContext->PSSetConstantBuffers(0, BindedBuffers[EST_FRAGMENT_SHADER].size(), BindedBuffers[EST_FRAGMENT_SHADER].pointer());
        BindedBuffers[EST_FRAGMENT_SHADER].set_used(0);
    }
    return true;
}

void ShaderHLSLD11::bind()
{
    //_IRR_DEBUG_BREAK_IF(mBindedProgram && mBindedProgram != programId);
    //if (!mBinded)
    {
        mBinded = true;
        getVideoDriver()->useShader(this);
    }
}

void ShaderHLSLD11::unbind()
{
    //if (mBinded)
    {
        //_IRR_DEBUG_BREAK_IF(mBindedProgram && mBindedProgram != programId);
        getVideoDriver()->useShader(nullptr);
    }
}

void CD3D11Driver::useShader(IShader* gpuProgram)
{
    if (gpuProgram)
    {
        if ( gpuProgram->getShaderVersion() == ESV_HLSL_HIGH_LEVEL )
        {
            ShaderHLSLD11* hlsl = (ShaderHLSLD11*)gpuProgram;
            // Set the vertex input layout.
            if ( hlsl->m_layout != CurrentInputLayout )
            {
                CurrentInputLayout = hlsl->m_layout;
                ImmediateContext->IASetInputLayout(hlsl->m_layout);
            }

            if ( hlsl->m_vertexShader )
            {
                shaderPtr[EST_VERTEX_SHADER] = hlsl->m_vertexShader;
                ImmediateContext->VSSetShader(hlsl->m_vertexShader, nullptr, 0);
            }
            else if ( shaderPtr[EST_VERTEX_SHADER] )
            {
                shaderPtr[EST_VERTEX_SHADER] = nullptr;
                ImmediateContext->VSSetShader(nullptr, nullptr, 0);
            }

            if ( hlsl->m_hullShader )
            {
                shaderPtr[EST_HULL_SHADER] = hlsl->m_hullShader;
                ImmediateContext->HSSetShader(hlsl->m_hullShader, nullptr, 0);
            }
            else if ( shaderPtr[EST_HULL_SHADER] )
            {
                shaderPtr[EST_HULL_SHADER] = nullptr;
                ImmediateContext->HSSetShader(nullptr, nullptr, 0);
            }

            if ( hlsl->m_domainShader )
            {
                shaderPtr[EST_DOMAIN_SHADER] = hlsl->m_domainShader;
                ImmediateContext->DSSetShader(hlsl->m_domainShader, nullptr, 0);
            }
            else if ( shaderPtr[EST_DOMAIN_SHADER] )
            {
                shaderPtr[EST_DOMAIN_SHADER] = nullptr;
                ImmediateContext->DSSetShader(nullptr, nullptr, 0);
            }

            if ( hlsl->m_geometryShader )
            {
                shaderPtr[EST_GEOMETRY_SHADER] = hlsl->m_geometryShader;
                ImmediateContext->GSSetShader(hlsl->m_geometryShader, nullptr, 0);
            }
            else if ( shaderPtr[EST_GEOMETRY_SHADER] )
            {
                shaderPtr[EST_GEOMETRY_SHADER] = nullptr;
                ImmediateContext->GSSetShader(nullptr, nullptr, 0);
            }

            if ( hlsl->m_pixelShader )
            {
                shaderPtr[EST_FRAGMENT_SHADER] = hlsl->m_pixelShader;
                ImmediateContext->PSSetShader(hlsl->m_pixelShader, nullptr, 0);
            }
            else if ( shaderPtr[EST_FRAGMENT_SHADER] )
            {
                shaderPtr[EST_FRAGMENT_SHADER] = nullptr;
                ImmediateContext->PSSetShader(nullptr, nullptr, 0);
            }
        }
        else
        {
            _IRR_DEBUG_BREAK_IF(true);
        }
    }
    else
    {
        //ImmediateContext->IASetInputLayout(nullptr);
        //ImmediateContext->VSSetShader(nullptr, nullptr, 0);
        //ImmediateContext->DSSetShader(nullptr, nullptr, 0);
        //ImmediateContext->HSSetShader(nullptr, nullptr, 0);
        //ImmediateContext->GSSetShader(nullptr, nullptr, 0);
        //ImmediateContext->PSSetShader(nullptr, nullptr, 0);
    }
}

ID3D11BlendState* CD3D11Driver::getBlendState()
{
    // Blend state
    if (!BlendState || lastBlendDesc != BlendDesc )
    {
        auto iBlend = BlendStateCache.find(BlendDesc);
        if ( iBlend != BlendStateCache.end() )
        {
            BlendState = iBlend->second;
            lastBlendDesc = BlendDesc;
        }
        else
        {
            HRESULT hr = Device->CreateBlendState(&BlendDesc, &BlendState);
            if ( FAILED(hr) )
            {
                //printf("[HLSL SHADER] Error: %s error description: %s\n",
                //    DXGetErrorString(hr), DXGetErrorDescription(hr));
                os::Printer::log("Error, failed to create blend state", ELL_ERROR);
                throw std::runtime_error("Error, failed to create blend state");
            }

            BlendStateCache[BlendDesc] = BlendState;
            lastBlendDesc = BlendDesc;
        }
    }

    return BlendState;
}

ID3D11RasterizerState* CD3D11Driver::getRasterizerState()
{
    // Rasterizer state
    if (!RasterizerState || lastRasterizerDesc != RasterizerDesc )
    {
        auto iBlend = RasterizerStateCache.find(RasterizerDesc);
        if ( iBlend != RasterizerStateCache.end() )
        {
            RasterizerState = iBlend->second;
            lastRasterizerDesc = RasterizerDesc;
        }
        else
        {
            HRESULT hr = Device->CreateRasterizerState(&RasterizerDesc, &RasterizerState);
            if ( FAILED(hr) )
            {
                //printf("[Stencil] Error: %s error description: %s\n",
                //    DXGetErrorString(hr), DXGetErrorDescription(hr));
                os::Printer::log("Error, failed to create rasterizer state", ELL_ERROR);
                throw std::runtime_error("Error, failed to create rasterizer state");
            }

            RasterizerStateCache[RasterizerDesc] = RasterizerState;
            lastRasterizerDesc = RasterizerDesc;
        }
    }

    return RasterizerState;
}

ID3D11DepthStencilState* CD3D11Driver::getDepthStencilState()
{
    // Depth stencil state
    if ( !DepthStencilState || lastDepthStencilDesc != DepthStencilDesc )
    {
        auto iDepthStencil = DepthStencilStateCache.find(DepthStencilDesc);
        if ( iDepthStencil != DepthStencilStateCache.end() )
        {
            DepthStencilState = iDepthStencil->second;
            lastDepthStencilDesc = DepthStencilDesc;
        }
        else
        {
            HRESULT hr = Device->CreateDepthStencilState(&DepthStencilDesc, &DepthStencilState);
            if ( FAILED(hr) )
            {
                os::Printer::log("Error, failed to create depth stencil state", ELL_ERROR);
                throw std::runtime_error("Error, failed to create depth stencil state");
            }

            DepthStencilStateCache[DepthStencilDesc] = DepthStencilState;
            lastDepthStencilDesc = DepthStencilDesc;
        }
    }

    return DepthStencilState;
}

ID3D11SamplerState* CD3D11Driver::getSamplerState(u32 idx)
{
    if ( !SamplerState[idx] || lastSamplerDesc[idx] != SamplerDesc[idx] )
    {
        auto iSamplerState = SamplerStateCache.find(SamplerDesc[idx]);
        if ( iSamplerState != SamplerStateCache.end() )
        {
            SamplerState[idx] = iSamplerState->second;
            lastSamplerDesc[idx] = SamplerDesc[idx];
        }
        else
        {
            HRESULT hr = Device->CreateSamplerState(&SamplerDesc[idx], &SamplerState[idx]);
            if ( FAILED(hr) )
            {
                //printf("[Sampler] Error: %s error description: %s\n",
                //    DXGetErrorString(hr), DXGetErrorDescription(hr));
                os::Printer::log("Error, failed to create sampler state", ELL_ERROR);
                throw std::runtime_error("Error, failed to create sampler state");
            }

            SamplerStateCache[SamplerDesc[idx]] = SamplerState[idx];
            lastSamplerDesc[idx] = SamplerDesc[idx];
        }
    }

    return SamplerState[idx];
}

//! applications must call this method before performing any rendering. returns false if failed.
bool CD3D11Driver::beginScene(bool backBuffer, bool zBuffer, SColor color,
                              const SExposedVideoData& videoData, core::rect<s32>* sourceRect)
{
    CNullDriver::beginScene(backBuffer, zBuffer, color, videoData, sourceRect);

    if( backBuffer )
    {
        SColorf fCol( color );
        FLOAT c[4] = { fCol.r, fCol.g, fCol.b, fCol.a };        // don't swizzle clear color for default back buffer
        ImmediateContext->ClearRenderTargetView( DefaultBackBuffer, c );
    }
    if( zBuffer && DefaultDepthBuffer )
    {
        ImmediateContext->ClearDepthStencilView( DefaultDepthBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );
    }

    // For safety, unset all shader resource views
    // If some is bound to shader stages, can cause error when bound to Output Merger too
    //ID3D11ShaderResourceView* views[1] = { NULL };
    //ImmediateContext->VSSetShaderResources( 0, 1, views );
    //ImmediateContext->GSSetShaderResources( 0, 1, views );
    //ImmediateContext->PSSetShaderResources( 0, 1, views );

    return true;
}

//! applications must call this method after performing any rendering. returns false if failed.
bool CD3D11Driver::endScene()
{
    CNullDriver::endScene();

    // The application may optionally specify "dirty" or "scroll" rects to improve efficiency
    // in certain scenarios.  In this sample, however, we do not utilize those features.
    DXGI_PRESENT_PARAMETERS parameters = { 0 };
    parameters.DirtyRectsCount = 0;
    parameters.pDirtyRects = nullptr;
    parameters.pScrollRect = nullptr;
    parameters.pScrollOffset = nullptr;

    HRESULT hr = S_OK;
    
    hr = SwapChain->Present1( Params.Vsync ? 1 : 0, 0 , &parameters);

    // Discard the contents of the render target.
    // This is a valid operation only when the existing contents will be entirely
    // overwritten. If dirty or scroll rects are used, this call should be removed.
    if (CurrentBackBuffer)
        ImmediateContext->DiscardView(CurrentBackBuffer);

    // Discard the contents of the depth stencil.
    if (CurrentDepthBuffer)
        ImmediateContext->DiscardView(CurrentDepthBuffer);

    // If the device was removed either by a disconnect or a driver upgrade, we
    // must recreate all device resources.
    if (hr == DXGI_ERROR_DEVICE_REMOVED)
    {
        //HandleDeviceLost();
        _IRR_DEBUG_BREAK_IF(true);
    }
    else if( FAILED( hr ) )
    {
        DX::ThrowIfFailed(hr);
    }

    return true;
}

bool CD3D11Driver::queryFeature(E_VIDEO_DRIVER_FEATURE feature) const
{
    switch( feature )
    {
    case EVDF_TEXTURE_COMPRESSION:
        return true;

    case EVDF_MULTIPLE_RENDER_TARGETS:
        return DriverType == D3D_DRIVER_TYPE_HARDWARE;

    case EVDF_MRT_BLEND:
    case EVDF_ALPHA_TO_COVERAGE:
    case EVDF_MRT_BLEND_FUNC:    
    case EVDF_TEXTURE_NSQUARE:
    case EVDF_VERTEX_BUFFER_OBJECT:
    case EVDF_COLOR_MASK:
    case EVDF_RENDER_TO_TARGET:
    case EVDF_MULTITEXTURE:
    case EVDF_BILINEAR_FILTER:
    case EVDF_TEXTURE_NPOT:
    case EVDF_STENCIL_BUFFER:
    case EVDF_HLSL:
    case EVDF_MIP_MAP:
    case EVDF_MIP_MAP_AUTO_UPDATE:
    case EVDF_TEXTURE_MULTISAMPLING:
        return true;

    case EVDF_VERTEX_SHADER_4_0:
    case EVDF_PIXEL_SHADER_4_0:
    case EVDF_GEOMETRY_SHADER_4_0:
    case EVDF_STREAM_OUTPUT_4_0:
        return FeatureLevel == D3D_FEATURE_LEVEL_10_0 ||
                FeatureLevel == D3D_FEATURE_LEVEL_10_1 ||
                FeatureLevel == D3D_FEATURE_LEVEL_11_0;

    case EVDF_VERTEX_SHADER_4_1:
    case EVDF_PIXEL_SHADER_4_1:
    case EVDF_GEOMETRY_SHADER_4_1:
    case EVDF_STREAM_OUTPUT_4_1:
        return FeatureLevel == D3D_FEATURE_LEVEL_10_1 ||
               FeatureLevel == D3D_FEATURE_LEVEL_11_0;

    case EVDF_VERTEX_SHADER_5_0:
    case EVDF_PIXEL_SHADER_5_0:
    case EVDF_GEOMETRY_SHADER_5_0:
    case EVDF_STREAM_OUTPUT_5_0:
    case EVDF_TESSELATION_SHADER_5_0:
        return FeatureLevel == D3D_FEATURE_LEVEL_11_0;
    
    case EVDF_COMPUTING_SHADER_4_0:
    case EVDF_COMPUTING_SHADER_4_1:
        {
            D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS opts;
            if (SUCCEEDED( Device->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, 
                        &opts, sizeof(D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS)) ))
            {
                return opts.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x != 0;
            }
            return false;
        }

    case EVDF_COMPUTING_SHADER_5_0:
        return FeatureLevel == D3D_FEATURE_LEVEL_11_0;
    }

    return false;
}

//! sets transformation
void CD3D11Driver::setTransform(E_TRANSFORMATION_STATE state, const core::matrix4& mat)
{
    Transformation3DChanged = true;
    Matrices[state] = mat;

    if ( state == ETS_PROJECTION || state == ETS_VIEW)
        Matrices[ETS_PROJECTION_VIEW] = Matrices[ETS_PROJECTION] * Matrices[ETS_VIEW];
}

bool CD3D11Driver::setActiveTexture(u32 stage, const video::ITexture* texture)
{
    if (texture && texture->getDriverType() != EDT_DIRECT3D11)
    {
        os::Printer::log("Fatal Error: Tried to set a texture not owned by this driver.", ELL_ERROR);
        return false;
    }

    // For first stage, set NullTexture if texture is NULL
    CurrentTexture[stage] = (stage == 0 && texture == 0) ? nullptr : texture;

    return true;
}

//! sets a material
void CD3D11Driver::setMaterial(const SMaterial& material)
{
    Material = material;
    OverrideMaterial.apply(Material);

    // set textures
    for(u16 i = 0; i < MATERIAL_MAX_TEXTURES; i ++)
    {    
        setActiveTexture(i, Material.getTexture(i));
        setTransform((E_TRANSFORMATION_STATE) ( ETS_TEXTURE_0 + i ), material.getTextureMatrix(i));
    }
}

bool CD3D11Driver::setRenderTarget(video::ITexture* texture,
            bool clearBackBuffer, bool clearZBuffer, SColor color)
{
    // check for right driver type
    if (texture && texture->getDriverType() != EDT_DIRECT3D11)
    {
        os::Printer::log("Fatal Error: Tried to set a texture not owned by this driver.", ELL_ERROR);
        return false;
    }

    // check for valid render target
    if (texture && !texture->isRenderTarget())
    {
        os::Printer::log("Fatal Error: Tried to set a non render target texture as render target.", ELL_ERROR);
        return false;
    }

    CD3D11Texture* tex = static_cast<CD3D11Texture*>(texture);

    // Discard the contents of the depth stencil.
    if (DefaultBackBuffer)
        ImmediateContext->DiscardView(DefaultBackBuffer);

    // check if we should set the previous RT back
    if (tex == 0)
    {
        CurrentBackBuffer = DefaultBackBuffer;
        CurrentDepthBuffer = DefaultDepthBuffer;
        ImmediateContext->OMSetRenderTargets( 1, &CurrentBackBuffer, CurrentDepthBuffer );
        CurrentRendertargetSize = core::dimension2d<u32>(0,0);
    }
    else
    {
        CurrentBackBuffer = tex->getRenderTargetView();
        CurrentDepthBuffer = tex->DepthSurface->Surface;
        ImmediateContext->OMSetRenderTargets( 1, &CurrentBackBuffer, CurrentDepthBuffer);
        CurrentRendertargetSize = tex->getSize();
    }

    // clear views
    if (clearBackBuffer)
    {
        SColorf fCol( color );
        // swizzle clear color is texture is passed
        // i.e.: is is default render target, don't swizzle
        FLOAT c[4] = { tex == 0 ? fCol.r : fCol.b, 
                       fCol.g, 
                       tex == 0 ? fCol.b :fCol.r,
                       fCol.a };
        ImmediateContext->ClearRenderTargetView( CurrentBackBuffer, c );        
    }

    // clear depth
    if(clearZBuffer )
        this->clearZBuffer();

    // set blend
    BlendDesc.IndependentBlendEnable = TRUE;
    //BlendDesc.IndependentBlendEnable = TRUE;

    // don't forget to set viewport
    setViewPort(core::rect<s32>(0, 0, getCurrentRenderTargetSize().Width, getCurrentRenderTargetSize().Height));

    return true;
}

bool CD3D11Driver::setRenderTarget(const core::array<video::IRenderTarget>& targets,
            bool clearBackBuffer, bool clearZBuffer, SColor color)
{
    u32 i;
    // If no targets, set default render target
    if (targets.size()==0)
    {        
        // set default render target
        return setRenderTarget(0, clearBackBuffer, clearZBuffer, color);
    }

    // max number of render targets is 8 for DX11 feature level
    u32 maxMultipleRTTs = core::min_((u32)D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, targets.size());
        
    // validation
    for (i = 0; i < maxMultipleRTTs; ++i)
    {
        if (targets[i].TargetType != ERT_RENDER_TEXTURE || !targets[i].RenderTexture)
        {
            maxMultipleRTTs = i;
            os::Printer::log("Missing texture for MRT.", ELL_WARNING);
            break;
        }

        // check for right driver type
        if (targets[i].RenderTexture->getDriverType() != EDT_DIRECT3D11)
        {
            maxMultipleRTTs = i;
            os::Printer::log("Tried to set a texture not owned by this driver.", ELL_WARNING);
            break;
        }

        // check for valid render target
        if (!targets[i].RenderTexture->isRenderTarget())
        {
            maxMultipleRTTs = i;
            os::Printer::log("Tried to set a non render target texture as render target.", ELL_WARNING);
            break;
        }

        // check for valid size
        if (targets[0].RenderTexture->getSize() != targets[i].RenderTexture->getSize())
        {
            maxMultipleRTTs = i;
            os::Printer::log("Render target texture has wrong size.", ELL_WARNING);
            break;
        }
    }
    if (maxMultipleRTTs==0)
    {
        os::Printer::log("Fatal Error: No valid MRT found.", ELL_ERROR);
        return false;
    }

    CD3D11Texture* tex = static_cast<CD3D11Texture*>(targets[0].RenderTexture);

    ID3D11RenderTargetView* RTViews[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];

    // parse clear color
    SColorf fCol( color );
    // swizzle clear color is texture is passed
    // i.e.: is is default render target, don't swizzle
    FLOAT c[4] = { tex == 0 ? fCol.r : fCol.b, 
                    fCol.g, 
                    tex == 0 ? fCol.b :fCol.r,
                    fCol.a };

    // zero blend description
    ::ZeroMemory( &BlendDesc, sizeof( BlendDesc ) );
    BlendDesc.IndependentBlendEnable = TRUE;

    // set blend based on render target configuration
    // set source blend
    for(i = 0; i < maxMultipleRTTs; i++)
    {
        E_BLEND_FACTOR blendFac = targets[i].BlendFuncSrc;
        
        BlendDesc.RenderTarget[i].SrcBlend = BlendDesc.RenderTarget[i].SrcBlendAlpha =
            blendFac == EBF_ZERO ? D3D11_BLEND_ZERO :
            blendFac == EBF_ONE  ? D3D11_BLEND_ONE :
            blendFac == EBF_DST_COLOR ? D3D11_BLEND_DEST_COLOR :
            blendFac == EBF_ONE_MINUS_DST_COLOR ? D3D11_BLEND_INV_DEST_COLOR :
            blendFac == EBF_SRC_COLOR ? D3D11_BLEND_SRC_COLOR :
            blendFac == EBF_ONE_MINUS_SRC_COLOR ? D3D11_BLEND_INV_SRC_COLOR:
            blendFac == EBF_SRC_ALPHA ? D3D11_BLEND_SRC_ALPHA :
            blendFac == EBF_ONE_MINUS_SRC_ALPHA ? D3D11_BLEND_INV_SRC_ALPHA :
            blendFac == EBF_DST_ALPHA ? D3D11_BLEND_DEST_ALPHA :
            blendFac == EBF_ONE_MINUS_DST_ALPHA ? D3D11_BLEND_INV_DEST_ALPHA :
                                                  D3D11_BLEND_SRC_ALPHA_SAT ;

        // set destination blend
        blendFac = targets[i].BlendFuncDst;
        BlendDesc.RenderTarget[i].DestBlend = BlendDesc.RenderTarget[i].DestBlendAlpha =
            blendFac == EBF_ZERO ? D3D11_BLEND_ZERO :
            blendFac == EBF_ONE  ? D3D11_BLEND_ONE :
            blendFac == EBF_DST_COLOR ? D3D11_BLEND_DEST_COLOR :
            blendFac == EBF_ONE_MINUS_DST_COLOR ? D3D11_BLEND_INV_DEST_COLOR :
            blendFac == EBF_SRC_COLOR ? D3D11_BLEND_SRC_COLOR :
            blendFac == EBF_ONE_MINUS_SRC_COLOR ? D3D11_BLEND_INV_SRC_COLOR:
            blendFac == EBF_SRC_ALPHA ? D3D11_BLEND_SRC_ALPHA :
            blendFac == EBF_ONE_MINUS_SRC_ALPHA ? D3D11_BLEND_INV_SRC_ALPHA :
            blendFac == EBF_DST_ALPHA ? D3D11_BLEND_DEST_ALPHA :
            blendFac == EBF_ONE_MINUS_DST_ALPHA ? D3D11_BLEND_INV_DEST_ALPHA :
                                                  D3D11_BLEND_SRC_ALPHA_SAT ;

        // set blend operation
        BlendDesc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
        BlendDesc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        BlendDesc.RenderTarget[i].BlendEnable = targets[i].BlendOp != EBO_NONE;

        // set blend based on render target information
        if ( targets[i].BlendOp != EBO_NONE )
        {
            // set color mask
            BlendDesc.RenderTarget[i].RenderTargetWriteMask = getColorWriteEnable(targets[i].ColorMask);
        }

        // store render target view in array
        RTViews[i] = static_cast<CD3D11Texture*>(targets[i].RenderTexture)->getRenderTargetView();
        
        // clear render target view
        if( clearBackBuffer )
            ImmediateContext->ClearRenderTargetView( RTViews[i], c );
    }

    // set depth buffer
    CurrentDepthBuffer = tex->DepthSurface->Surface;
    if (clearZBuffer)
        this->clearZBuffer();

    // set targets
    ImmediateContext->OMSetRenderTargets( maxMultipleRTTs, RTViews, CurrentDepthBuffer );

    // don't forget to set viewport
    CurrentRendertargetSize = tex->getSize();
    setViewPort(core::rect<s32>(0, 0, getCurrentRenderTargetSize().Width, getCurrentRenderTargetSize().Height));

    return true;
}

//---------------------------------------------------------------------
bool CD3D11Driver::_getErrorsFromQueue() const
{
    if (m_infoQueue != NULL)
    {
        UINT64 numStoredMessages = m_infoQueue->GetNumStoredMessages();

        for (UINT64 i = 0; i < numStoredMessages; i++)
        {
            // Get the size of the message
            SIZE_T messageLength = 0;
            m_infoQueue->GetMessage(i, NULL, &messageLength);
            // Allocate space and get the message
            D3D11_MESSAGE * pMessage = (D3D11_MESSAGE*)malloc(messageLength);
            m_infoQueue->GetMessage(i, pMessage, &messageLength);

            bool res = false;
            switch (pMessage->Severity)
            {
            case D3D11_MESSAGE_SEVERITY_CORRUPTION:
                //if (D3D_CORRUPTION == mExceptionsErrorLevel)
                //{
                //    res = true;
                //}
                break;
            case D3D11_MESSAGE_SEVERITY_ERROR:
                //switch (mExceptionsErrorLevel)
                //{
                //case D3D_INFO:
                //case D3D_WARNING:
                //case D3D_ERROR:
                //    res = true;
                //}
                break;
            case D3D11_MESSAGE_SEVERITY_WARNING:
                //switch (mExceptionsErrorLevel)
                //{
                //case D3D_INFO:
                //case D3D_WARNING:
                //    res = true;
                //}
                break;
            }

            free(pMessage);
            if (res)
            {
                // we don't need to loop anymore...
                return true;
            }

        }

        //clearStoredErrorMessages();

        return false;

    }
    else
    {
        return false;
    }
}

void CD3D11Driver::setViewPort(const core::rect<s32>& area)
{
    core::rect<s32> vp = area;
    core::rect<s32> rendert(0,0, getCurrentRenderTargetSize().Width, getCurrentRenderTargetSize().Height);
    vp.clipAgainst(rendert);

    D3D11_VIEWPORT viewPort;
    viewPort.TopLeftX = (FLOAT)vp.UpperLeftCorner.X;
    viewPort.TopLeftY = (FLOAT)vp.UpperLeftCorner.Y;
    viewPort.Width = (FLOAT)vp.getWidth();
    viewPort.Height = (FLOAT)vp.getHeight();
    //if (requiresTextureFlipping())
    //{
    //    // Convert "top-left" to "bottom-left"
    //    viewPort.TopLeftY = target->getHeight() - viewPort.Height - viewPort.TopLeftY;
    //}
    viewPort.MinDepth = 0.0f;
    viewPort.MaxDepth = 1.0f;

    ImmediateContext->RSSetViewports( 1, &viewPort );

    this->ViewPort = vp;
}

const core::rect<s32>& CD3D11Driver::getViewPort() const
{
    return ViewPort;
}

bool CD3D11Driver::updateVertexHardwareBuffer(CD3D11HardwareBuffer *hwBuffer, E_HARDWARE_BUFFER_TYPE Type)
{
    if ( !hwBuffer )
        return false;

    E_HARDWARE_BUFFER_ACCESS MemoryAccess = E_HARDWARE_BUFFER_ACCESS::EHBA_DEFAULT;
    if ( hwBuffer->GetBuffer()->getHardwareMappingHint_Vertex() != scene::EHM_STATIC )
        MemoryAccess = E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC;

    const scene::IMeshBuffer* mb = hwBuffer->GetBuffer();
    const E_VERTEX_TYPE vType = mb->getVertexType();

    if ( Type == E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX )
    {
        const void* vertices = mb->getVertices();
        const u32 vertexCount = mb->getVertexCount();
        const u32 vertexSize = hwBuffer->GetBuffer()->GetVertexStride() ? hwBuffer->GetBuffer()->GetVertexStride() : getVertexPitchFromType(vType);
        const u32 bufSize = vertexSize * vertexCount;

        hwBuffer->UpdateBuffer(Type, MemoryAccess, vertices, bufSize);
    }
    else if ( Type == E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX_INSTANCE_STREAM )
    {
        IShaderDataBufferElement* variable = hwBuffer->GetInstanceBuffer()->m_bufferDataArray[0];
        //TODO: Optimalization workaround Terrain or Liquid (need correct way)
        if ( vType != EVT_TERRAIN && vType != EVT_STANDARD )
            MemoryAccess = E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC;
        hwBuffer->UpdateBuffer(Type, MemoryAccess, variable->getShaderValues(), variable->getValueSizeOf() * variable->getShaderValueCount());
    }
    return true;
}

bool CD3D11Driver::updateIndexHardwareBuffer(CD3D11HardwareBuffer* hwBuffer)
{
    if ( !hwBuffer )
        return false;

    const scene::IMeshBuffer* mb = hwBuffer->GetBuffer();
    const u16* indices = mb->getIndices();
    const u32 indexCount = mb->getIndexCount();
    const u32 indexStride = video::getIndexSize(mb->getIndexType());
    const u32 bufSize = indexStride * indexCount;

    E_HARDWARE_BUFFER_ACCESS MemoryAccess = E_HARDWARE_BUFFER_ACCESS::EHBA_DEFAULT;
    if ( hwBuffer->GetBuffer()->getHardwareMappingHint_Index() != scene::EHM_STATIC )
        MemoryAccess = E_HARDWARE_BUFFER_ACCESS::EHBA_DYNAMIC;

    hwBuffer->UpdateBuffer(E_HARDWARE_BUFFER_TYPE::EHBT_INDEX, MemoryAccess, indices, bufSize);
    return true;
}

bool CD3D11Driver::updateHardwareBuffer(IHardwareBuffer *hwBuffer)
{
    if ( !hwBuffer )
        return false;

    if ( !hwBuffer->GetBuffer()->GetGPUProgram() )
    {
        _IRR_DEBUG_BREAK_IF(!m_defaultShader);
        hwBuffer->GetBuffer()->SetGPUProgram(m_defaultShader);
    }

    if ( hwBuffer->GetBuffer()->getHardwareMappingHint_Vertex() == scene::EHM_NEVER || hwBuffer->GetBuffer()->getHardwareMappingHint_Index() == scene::EHM_NEVER )
        hwBuffer->GetBuffer()->setHardwareMappingHint(scene::EHM_STATIC);

    {
        if ( hwBuffer->getChangeID(E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX) != hwBuffer->GetBuffer()->getChangedID_Vertex() )
        {
            if ( !updateVertexHardwareBuffer((CD3D11HardwareBuffer*)hwBuffer, E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX) )
                return false;

            hwBuffer->setChangeID(E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX, hwBuffer->GetBuffer()->getChangedID_Vertex());
        }

        if ( hwBuffer->GetInstanceBuffer() && hwBuffer->getChangeID(E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX_INSTANCE_STREAM) != hwBuffer->GetInstanceBuffer()->getChangedID() )
        {
            if ( !updateVertexHardwareBuffer((CD3D11HardwareBuffer*)hwBuffer, E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX_INSTANCE_STREAM) )
                return false;

            hwBuffer->setChangeID(E_HARDWARE_BUFFER_TYPE::EHBT_VERTEX_INSTANCE_STREAM, hwBuffer->GetInstanceBuffer()->getChangedID());
        }
    }

    //if (  )
    {
        if ( hwBuffer->getChangeID(E_HARDWARE_BUFFER_TYPE::EHBT_INDEX) != hwBuffer->GetBuffer()->getChangedID_Index() )
        {
            if ( !updateIndexHardwareBuffer((CD3D11HardwareBuffer*)hwBuffer) )
                return false;

            hwBuffer->setChangeID(E_HARDWARE_BUFFER_TYPE::EHBT_INDEX, hwBuffer->GetBuffer()->getChangedID_Vertex());
        }
    }

    return true;
}

IHardwareBuffer* CD3D11Driver::createHardwareBuffer(const scene::IMeshBuffer* mb)
{
    if (!STATHREADiD)
        STATHREADiD = GetCurrentThreadId();
    assert(STATHREADiD == GetCurrentThreadId());

    // DirectX 11 ALWAYS uses hardware buffer
    if ( !mb /*|| (mb->getHardwareMappingHint_Index() == scene::EHM_NEVER || mb->getHardwareMappingHint_Vertex() == scene::EHM_NEVER)*/ )
        return 0;

    CD3D11HardwareBuffer *hwBuffer = new CD3D11HardwareBuffer(this, (scene::IMeshBuffer*)mb, ((scene::IMeshBuffer*)mb)->GetHWInstanceBuffer(),
        mb->getIndexType() == video::EIT_16BIT ?
        (u32)E_HARDWARE_BUFFER_FLAGS::EHBF_INDEX_16_BITS : (u32)E_HARDWARE_BUFFER_FLAGS::EHBF_INDEX_32_BITS, mb->getVertexType());

    //add to map
    //HWBufferMap.insert(hwBuffer->MeshBuffer, hwBuffer);

    if ( !updateHardwareBuffer(hwBuffer) )
    {
        deleteHardwareBuffer(hwBuffer);
        return 0;
    }

    hwBuffer->Initialize();
    return hwBuffer;
}

void CD3D11Driver::deleteHardwareBuffer(IHardwareBuffer *_HWBuffer)
{
    if (!_HWBuffer)
        return;
    
    CNullDriver::deleteHardwareBuffer(_HWBuffer);
}

void CD3D11Driver::InitDrawStates(scene::E_PRIMITIVE_TYPE pType)
{
    // Bind textures and samplers
    bool viewChanged = false;
    bool stateChanged = false;
    for (int i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
    {
        if (CurrentTexture[i])
        {
            ID3D11ShaderResourceView* view = ((CD3D11Texture*)CurrentTexture[i])->getShaderResourceView();
            viewChanged |= views[i] != view;
            if (viewChanged)
                views[i] = view;

            ID3D11SamplerState* state = getSamplerState(i);
            stateChanged |= samplers[i] != state;
            if ( stateChanged )
                samplers[i] = state;
        }
        else
        {
            ID3D11ShaderResourceView* view = ((CD3D11Texture*)blankTexture)->getShaderResourceView();
            viewChanged |= views[i] != view; //viewChanged |= views[i] != 0;
            if (viewChanged)
                views[i] = view; //views[i] = 0;

            ID3D11SamplerState* state = getSamplerState(i);
            stateChanged |= samplers[i] != state;
            if (stateChanged)
                samplers[i] = state;

            //stateChanged |= samplers[i] != 0;
            //if ( stateChanged )
            //    samplers[i] = 0;
        }
    }

    if (viewChanged)
        ImmediateContext->PSSetShaderResources(0, MATERIAL_MAX_TEXTURES, views);

    if ( stateChanged )
        ImmediateContext->PSSetSamplers(0, MATERIAL_MAX_TEXTURES, samplers);

    // Bind depth-stencil view
    ID3D11DepthStencilState* DepthStencilState = getDepthStencilState();
    if ( LastDepthStencilState != DepthStencilState )
    {
        LastDepthStencilState = DepthStencilState;
        ImmediateContext->OMSetDepthStencilState(DepthStencilState, 1);
    }

    // Bind blend state
    ID3D11BlendState* BlendStateState = getBlendState();
    if ( LastBlendState != BlendStateState )
    {
        LastBlendState = BlendStateState;
        ImmediateContext->OMSetBlendState(BlendStateState, 0, 0xffffffff);
    }

    // Bind rasterizer state
    ID3D11RasterizerState* RasterizerState = getRasterizerState();
    if ( LastRasterizerState != RasterizerState )
    {
        LastRasterizerState = RasterizerState;
        ImmediateContext->RSSetState(RasterizerState);
    }

    // finally, draw
    D3D_PRIMITIVE_TOPOLOGY Topology = getTopology(pType);
    if ( lastTopology != Topology )
    {
        lastTopology = Topology;
        ImmediateContext->IASetPrimitiveTopology(Topology);
    }
}

void CD3D11Driver::drawHardwareBuffer(IHardwareBuffer* _HWBuffer, scene::IMesh* mesh/* = nullptr*/, scene::ISceneNode* node/* = nullptr*/)
{
    if (!_HWBuffer)
        return;

    if (!STATHREADiD)
        STATHREADiD = GetCurrentThreadId();
    assert(STATHREADiD == GetCurrentThreadId());

    const scene::IMeshBuffer* mb = _HWBuffer->GetBuffer();

    _IRR_DEBUG_BREAK_IF(!getActiveGPUProgram());

    scene::E_PRIMITIVE_TYPE PrimitiveTopology = mb->getRenderPrimitive();

    if (PrimitiveTopology == scene::EPT_TRIANGLE_FAN)
        return;

    if (mb->Is3DMode())
    {
        if (!setRenderStates3DMode(mb->getVertexType()))
            return;
    }
    else
    {
        if (Material.MaterialType == EMT_ONETEXTURE_BLEND)
        {
            E_BLEND_FACTOR srcFact;
            E_BLEND_FACTOR dstFact;
            E_MODULATE_FUNC modulo;
            u32 alphaSource;
            unpack_textureBlendFunc(srcFact, dstFact, modulo, alphaSource, Material.MaterialTypeParam);
            setRenderStates2DMode(alphaSource & video::EAS_VERTEX_COLOR, (Material.getTexture(0) != 0), (alphaSource&video::EAS_TEXTURE) != 0);
        }
        else
            setRenderStates2DMode(Material.MaterialType == EMT_TRANSPARENT_VERTEX_ALPHA, (Material.getTexture(0) != 0), Material.MaterialType == EMT_TRANSPARENT_ALPHA_CHANNEL);
    }

    blockgpuprogramchange = true;
    _IRR_DEBUG_BREAK_IF(!getActiveGPUProgram());

    if (getActiveGPUProgram())
        getActiveGPUProgram()->UpdateValues(video::IShaderDataBuffer::EUT_PER_FRAME_PER_MATERIAL, (scene::IMeshBuffer*)mb, mesh, node, video::IShaderDataBuffer::EUF_COMMIT_VALUES);

    if (_HWBuffer->GetInstanceBuffer())
        _HWBuffer->GetInstanceBuffer()->UpdateBuffer(getActiveGPUProgram(), _HWBuffer->GetBuffer(), mesh, node, 0);
    updateHardwareBuffer(_HWBuffer); //check if update is needed

    if (!_HWBuffer->IsBinded() || !_HWBuffer->IsManualBind())
    {
        if (getActiveGPUProgram())
            getActiveGPUProgram()->UpdateValues(video::IShaderDataBuffer::EUT_PER_FRAME_PER_MESH, (scene::IMeshBuffer*)mb, mesh, node, video::IShaderDataBuffer::EUF_COMMIT_VALUES);
        _HWBuffer->Bind();
    }

    _IRR_DEBUG_BREAK_IF(!getActiveGPUProgram());

    blockgpuprogramchange = false;

    if (!checkPrimitiveCount(mb->getVertexCount()))
    {
        if (!_HWBuffer->IsManualBind())
            _HWBuffer->Unbind();
        return;
    }

    FinalizeShaderConstant();
    InitDrawStates(PrimitiveTopology);

    u32 instanceCount = (!mesh || mesh->IsInstanceModeAvailable()) && _HWBuffer->GetInstanceBuffer() ? _HWBuffer->GetInstanceBuffer()->getInstanceCount() : 0;

    if (instanceCount > 1)
    {
        if (mb->GetSubBufferCount())
            ImmediateContext->DrawIndexedInstanced(mb->GetIndexRangeCount(), instanceCount, mb->GetIndexRangeStart(), 0, 0);
        else
            ImmediateContext->DrawIndexedInstanced(mb->getIndexCount(), instanceCount, 0, 0, 0);
    }
    else
    {
        if (mb->GetSubBufferCount())
            ImmediateContext->DrawIndexed(mb->GetIndexRangeCount(), mb->GetIndexRangeStart(), 0);
        else
            ImmediateContext->DrawIndexed(mb->getIndexCount(), 0, 0);
    }

    if (!_HWBuffer->IsManualBind())
        _HWBuffer->Unbind();
}

void CD3D11Driver::drawHardwareBuffer(IHardwareBuffer* vertices,
                IHardwareBuffer* indices, E_VERTEX_TYPE vType, 
                scene::E_PRIMITIVE_TYPE pType, E_INDEX_TYPE iType, u32 numInstances)
{
    //CNullDriver::drawHardwareBuffer(vertices, indices, vType, pType, iType);

    // Get vertex declaration
    //DeclarationNode declNode = declarationMap.find(vType);
    //if (!declNode)
    //{
    //    os::Printer::log("Error, vertex type not registered", ELL_ERROR);
    //    return;
    //}
    //
    //// Set render states
    //if (!setRenderStates3DMode(vType))
    //    return;
    //
    //CD3D11HardwareBuffer* vertexBuffer = static_cast<CD3D11HardwareBuffer*>(vertices);
    //const u32 stride = declNode->getValue()->getVertexPitch();
    //const u32 vertexCount = vertexBuffer->size() / stride;
    //UINT offset = 0;
    //
    //// set vertex buffer
    //ID3D11Buffer* buffers[1] = { vertexBuffer->getBufferResource() };
    //ImmediateContext->IASetVertexBuffers( 0, 1, buffers, &stride, &offset );
    //
    //// Bind textures and samplers
    //ID3D11SamplerState* samplers[MATERIAL_MAX_TEXTURES];
    //ID3D11ShaderResourceView* views[MATERIAL_MAX_TEXTURES];
    //for(int i = 0; i < MATERIAL_MAX_TEXTURES; i++)
    //{
    //    if( CurrentTexture[i] )
    //    {
    //        views[i] = ((CD3D11Texture*)CurrentTexture[i])->getShaderResourceView();
    //        samplers[i] = getSamplerState(i);
    //    }
    //    else
    //    {
    //        views[i] = 0;
    //        samplers[i] = 0;
    //    }
    //}
    //ImmediateContext->PSSetShaderResources( 0, MATERIAL_MAX_TEXTURES, views );
    //ImmediateContext->PSSetSamplers( 0, MATERIAL_MAX_TEXTURES, samplers );
    //ImmediateContext->GSSetShaderResources( 0, MATERIAL_MAX_TEXTURES, views );    // Test for recirculate
    //ImmediateContext->GSSetSamplers( 0, MATERIAL_MAX_TEXTURES, samplers );
    //
    //// Bind depth-stencil view
    //ImmediateContext->OMSetDepthStencilState( getDepthStencilState(), 1 );
    //
    //// Bind blend state
    //ImmediateContext->OMSetBlendState( getBlendState(), 0, 0xffffffff );
    //
    //// Bind rasterizer state
    //ImmediateContext->RSSetState( getRasterizerState() );
    //
    //// Bind input layout
    //IMaterialRenderer* renderer = MaterialRenderers[Material.MaterialType].Renderer;
    //ImmediateContext->IASetInputLayout( declNode->getValue()->getInputLayout(renderer) );
    //
    //// Set topology
    //ImmediateContext->IASetPrimitiveTopology( getTopology(pType) );
    //
    //// finally, draw
    //if (indices)
    //{
    //    // Using indices
    //    CD3D11HardwareBuffer* indexBuffer = static_cast<CD3D11HardwareBuffer*>(indices);
    //    const DXGI_FORMAT idxType = getIndexType(iType);
    //    const u32 idxSize = getIndexSize(iType);
    //    const u32 indexCount = indexBuffer->size() / idxSize;
    //    
    //    ImmediateContext->IASetIndexBuffer( indexBuffer->getBufferResource(), idxType, 0 );
    //
    //    if (numInstances > 0)
    //        ImmediateContext->DrawIndexedInstanced( indexCount, numInstances, 0, 0, 0 );
    //    else
    //        ImmediateContext->DrawIndexed( indexCount, 0, 0 );
    //}
    //else
    //{
    //    if (numInstances > 0)
    //        ImmediateContext->DrawInstanced( vertexCount, numInstances, 0, 0 );
    //    else
    //        ImmediateContext->Draw( vertexCount, 0 );
    //}
    //
    //// unset buffers
    //buffers[0] = 0;
    //ImmediateContext->IASetVertexBuffers( 0, 1, buffers, &stride, &offset );
    //ImmediateContext->IASetIndexBuffer( 0, DXGI_FORMAT_R32_UINT, 0 );
    //ImmediateContext->SOSetTargets( 1, buffers, &offset );
}

void CD3D11Driver::drawAuto(IHardwareBuffer* vertices, E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType)
{
    //CNullDriver::drawAuto(vertices, vType, pType);

    //// Get vertex declaration
    //DeclarationNode declNode = declarationMap.find(vType);
    //if (!declNode)
    //{
    //    os::Printer::log("Error, vertex type not registered", ELL_ERROR);
    //    return;
    //}
    //
    //// Set render states
    //if (!setRenderStates3DMode(vType))
    //    return;
    //
    //CD3D11HardwareBuffer* vertexBuffer = static_cast<CD3D11HardwareBuffer*>(vertices);
    //const u32 stride = declNode->getValue()->getVertexPitch();
    //const u32 offset = 0;
    //
    //// set vertex buffer
    //ID3D11Buffer* buffers[1] = { vertexBuffer->getBufferResource() };
    //ImmediateContext->IASetVertexBuffers( 0, 1, buffers, &stride, &offset );
    //
    //// Bind textures and samplers
    //ID3D11SamplerState* samplers[MATERIAL_MAX_TEXTURES];
    //ID3D11ShaderResourceView* views[MATERIAL_MAX_TEXTURES];
    //for(int i = 0; i < MATERIAL_MAX_TEXTURES; i++)
    //{
    //    if( CurrentTexture[i] )
    //    {
    //        views[i] = ((CD3D11Texture*)CurrentTexture[i])->getShaderResourceView();
    //        samplers[i] = getSamplerState(i);
    //    }
    //    else
    //    {
    //        views[i] = 0;
    //        samplers[i] = 0;
    //    }
    //}
    //ImmediateContext->PSSetShaderResources( 0, MATERIAL_MAX_TEXTURES, views );
    //ImmediateContext->PSSetSamplers( 0, MATERIAL_MAX_TEXTURES, samplers );    
    //ImmediateContext->GSSetShaderResources( 0, MATERIAL_MAX_TEXTURES, views );        // Test for recirculate
    //ImmediateContext->GSSetSamplers( 0, MATERIAL_MAX_TEXTURES, samplers );
    //
    //// Bind input layout
    //IMaterialRenderer* renderer = MaterialRenderers[Material.MaterialType].Renderer;
    //ImmediateContext->IASetInputLayout( declNode->getValue()->getInputLayout(renderer) );
    //
    //// Bind depth-stencil view
    //ImmediateContext->OMSetDepthStencilState( getDepthStencilState(), 1 );
    //
    //// Bind blend state
    //ImmediateContext->OMSetBlendState( getBlendState(), 0, 0xffffffff );
    //
    //// Bind rasterizer state
    //ImmediateContext->RSSetState( getRasterizerState() );
    //
    //// Bind topology
    //ImmediateContext->IASetPrimitiveTopology( getTopology(pType) );
    //
    //// Finally, draw
    //ImmediateContext->DrawAuto();
    //
    //// Unset buffers, including stream output
    //buffers[0] = 0;
    //ImmediateContext->IASetVertexBuffers( 0, 1, buffers, &stride, &offset );
    //ImmediateContext->IASetIndexBuffer( 0, DXGI_FORMAT_R32_UINT, 0 );
    //ImmediateContext->SOSetTargets( 1, buffers, &offset );
}

void CD3D11Driver::drawVertexPrimitiveList(const void* vertices, u32 vertexCount,
                const void* indexList, u32 primitiveCount,
                E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType,
                E_INDEX_TYPE iType)
{
    if (!checkPrimitiveCount(primitiveCount))
        return;

    CNullDriver::drawVertexPrimitiveList(vertices, vertexCount, indexList, primitiveCount, vType, pType,iType);

    if (vertexCount || primitiveCount)
        draw2D3DVertexPrimitiveList(vertices, vertexCount, indexList, primitiveCount,
            vType, pType, iType, true);
}

void CD3D11Driver::draw2DVertexPrimitiveList(const void* vertices, u32 vertexCount,
                const void* indexList, u32 primitiveCount,
                E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType,
                E_INDEX_TYPE iType)
{
    if (!checkPrimitiveCount(primitiveCount))
        return;

    CNullDriver::draw2DVertexPrimitiveList(vertices, vertexCount, indexList, primitiveCount, vType, pType,iType);

    if (vertexCount || primitiveCount)
        draw2D3DVertexPrimitiveList(vertices, vertexCount, indexList, primitiveCount,
            vType, pType, iType, false);
}

void CD3D11Driver::draw2D3DVertexPrimitiveList(const void* vertices,
        u32 vertexCount, const void* indexList, u32 primitiveCount,
        E_VERTEX_TYPE vType, scene::E_PRIMITIVE_TYPE pType,
        E_INDEX_TYPE iType, bool is3D)
{
    if (pType == scene::EPT_TRIANGLE_FAN)
        return;

    if (is3D)
    {
        if (!setRenderStates3DMode(vType))
            return;
    }
    //else
    //{
    //    if (Material.MaterialType==EMT_ONETEXTURE_BLEND)
    //    {
    //        E_BLEND_FACTOR srcFact;
    //        E_BLEND_FACTOR dstFact;
    //        E_MODULATE_FUNC modulo;
    //        u32 alphaSource;
    //        unpack_textureBlendFunc ( srcFact, dstFact, modulo, alphaSource, Material.MaterialTypeParam);
    //        setRenderStates2DMode(alphaSource & video::EAS_VERTEX_COLOR, (Material.getTexture(0) != 0), (alphaSource&video::EAS_TEXTURE) != 0);
    //    }
    //    else
    //        setRenderStates2DMode(Material.MaterialType==EMT_TRANSPARENT_VERTEX_ALPHA, (Material.getTexture(0) != 0), Material.MaterialType==EMT_TRANSPARENT_ALPHA_CHANNEL);
    //}
    
    bool shaderOK = MaterialRenderers[Material.MaterialType].Renderer->OnRender(this, vType);
    _IRR_DEBUG_BREAK_IF(!shaderOK);

    InitDrawStates(pType);

    // copy vertices to dynamic buffers, if needed
    if(vertices || indexList)
        uploadVertexData(vertices, vertexCount, indexList, getIndexAmount(pType, primitiveCount), vType, iType);

    if (pType == scene::EPT_POINTS || pType == scene::EPT_POINT_SPRITES)
        ImmediateContext->Draw( vertexCount, 0 );
    else if (vertexCount == -1)
        ImmediateContext->DrawAuto();
    else
        ImmediateContext->DrawIndexed( getIndexAmount(pType, primitiveCount), 0, 0 );
}

void CD3D11Driver::draw2DImage(const video::ITexture* texture, 
            const core::rect<s32>& destRect,
            const core::rect<s32>& sourceRect, 
            const core::rect<s32>* clipRect,
            const video::SColor* const colors, 
            bool useAlphaChannelOfTexture)
{
    if(!texture || texture->getDriverType() != EDT_DIRECT3D11)
        return;
    
    const core::dimension2d<u32>& ss = texture->getOriginalSize();

    // clip source and destination rects to prevent draw pixels outside draw area.
    core::rect<f32> tcoords;
    tcoords.UpperLeftCorner.X = (f32)sourceRect.UpperLeftCorner.X / (f32)ss.Width;
    tcoords.UpperLeftCorner.Y = (f32)sourceRect.UpperLeftCorner.Y / (f32)ss.Height;
    tcoords.LowerRightCorner.X = (f32)sourceRect.LowerRightCorner.X / (f32)ss.Width;
    tcoords.LowerRightCorner.Y = (f32)sourceRect.LowerRightCorner.Y / (f32)ss.Height;

    const video::SColor temp[4] =
    {
        0xFFFFFFFF,
        0xFFFFFFFF,
        0xFFFFFFFF,
        0xFFFFFFFF
    };
    
    const video::SColor* const useColor = colors ? colors : temp;

    S3DVertex vtx[4]; // clock wise
    vtx[0] = S3DVertex((f32)destRect.UpperLeftCorner.X, (f32)destRect.UpperLeftCorner.Y, 0.0f,
            0.0f, 0.0f, 0.0f, useColor[0],
            tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y);
    vtx[1] = S3DVertex((f32)destRect.LowerRightCorner.X, (f32)destRect.UpperLeftCorner.Y, 0.0f,
            0.0f, 0.0f, 0.0f, useColor[3],
            tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y);
    vtx[2] = S3DVertex((f32)destRect.LowerRightCorner.X, (f32)destRect.LowerRightCorner.Y, 0.0f,
            0.0f, 0.0f, 0.0f, useColor[2],
            tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y);
    vtx[3] = S3DVertex((f32)destRect.UpperLeftCorner.X, (f32)destRect.LowerRightCorner.Y, 0.0f,
            0.0f, 0.0f, 0.0f, useColor[1],
            tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y);

    s32 verticesSize = sizeof(vtx)/sizeof(vtx[0]);

    s16 indices[6] = {0,1,2,0,2,3};
    s32 indicesSize = 4;

    SMaterial m;
    m.setTexture(0, const_cast<video::ITexture*>(texture));
    setMaterial(m);

    setRenderStates2DMode(useColor[0].getAlpha()<255 || useColor[1].getAlpha()<255 ||
        useColor[2].getAlpha()<255 || useColor[3].getAlpha()<255,
        true, useAlphaChannelOfTexture);

    // DX 11 needs a renderer, so configure solid
    //MaterialRenderers[video::EMT_SOLID].Renderer->OnSetMaterial(Material, LastMaterial, ResetRenderStates, this);

    //Block2DRenderStateChange = true;
    // Draw
    draw2DVertexPrimitiveList(vtx, verticesSize, indices, 2, video::EVT_STANDARD, scene::EPT_TRIANGLES, video::EIT_16BIT);

    //Block2DRenderStateChange = false;
}

void CD3D11Driver::draw2DImage(const video::ITexture* texture, 
            const core::position2d<s32>& pos,
            const core::rect<s32>& sourceRect, 
            const core::rect<s32>* clipRect,
            SColor color, 
            bool useAlphaChannelOfTexture)
{
    if (!texture)
        return;

    if (!sourceRect.isValid())
        return;

    SMaterial m;
    m.setTexture(0, const_cast<video::ITexture*>(texture));
    setMaterial(m);

    core::position2d<s32> targetPos = pos;
    core::position2d<s32> sourcePos = sourceRect.UpperLeftCorner;
    // This needs to be signed as it may go negative.
    core::dimension2d<s32> sourceSize(sourceRect.getSize());

    if (clipRect)
    {
        if (targetPos.X < clipRect->UpperLeftCorner.X)
        {
            sourceSize.Width += targetPos.X - clipRect->UpperLeftCorner.X;
            if (sourceSize.Width <= 0)
                return;

            sourcePos.X -= targetPos.X - clipRect->UpperLeftCorner.X;
            targetPos.X = clipRect->UpperLeftCorner.X;
        }

        if (targetPos.X + (s32)sourceSize.Width > clipRect->LowerRightCorner.X)
        {
            sourceSize.Width -= (targetPos.X + sourceSize.Width) - clipRect->LowerRightCorner.X;
            if (sourceSize.Width <= 0)
                return;
        }

        if (targetPos.Y < clipRect->UpperLeftCorner.Y)
        {
            sourceSize.Height += targetPos.Y - clipRect->UpperLeftCorner.Y;
            if (sourceSize.Height <= 0)
                return;

            sourcePos.Y -= targetPos.Y - clipRect->UpperLeftCorner.Y;
            targetPos.Y = clipRect->UpperLeftCorner.Y;
        }

        if (targetPos.Y + (s32)sourceSize.Height > clipRect->LowerRightCorner.Y)
        {
            sourceSize.Height -= (targetPos.Y + sourceSize.Height) - clipRect->LowerRightCorner.Y;
            if (sourceSize.Height <= 0)
                return;
        }
    }

    // clip these coordinates

    //if (targetPos.X<0)
    //{
    //    sourceSize.Width += targetPos.X;
    //    if (sourceSize.Width <= 0)
    //        return;
    //
    //    sourcePos.X -= targetPos.X;
    //    targetPos.X = 0;
    //}

    //const core::dimension2d<u32>& renderTargetSize = getCurrentRenderTargetSize();
    //
    //if (targetPos.X + sourceSize.Width > (s32)renderTargetSize.Width)
    //{
    //    sourceSize.Width -= (targetPos.X + sourceSize.Width) - renderTargetSize.Width;
    //    if (sourceSize.Width <= 0)
    //        return;
    //}

    //if (targetPos.Y<0)
    //{
    //    sourceSize.Height += targetPos.Y;
    //    if (sourceSize.Height <= 0)
    //        return;
    //
    //    sourcePos.Y -= targetPos.Y;
    //    targetPos.Y = 0;
    //}

    //if (targetPos.Y + sourceSize.Height > (s32)renderTargetSize.Height)
    //{
    //    sourceSize.Height -= (targetPos.Y + sourceSize.Height) - renderTargetSize.Height;
    //    if (sourceSize.Height <= 0)
    //        return;
    //}

    // ok, we've clipped everything.
    // now draw it.

    core::rect<f32> tcoords;
    tcoords.UpperLeftCorner.X = (((f32)sourcePos.X)) / texture->getOriginalSize().Width ;
    tcoords.UpperLeftCorner.Y = (((f32)sourcePos.Y)) / texture->getOriginalSize().Height;
    tcoords.LowerRightCorner.X = tcoords.UpperLeftCorner.X + ((f32)(sourceSize.Width) / texture->getOriginalSize().Width);
    tcoords.LowerRightCorner.Y = tcoords.UpperLeftCorner.Y + ((f32)(sourceSize.Height) / texture->getOriginalSize().Height);

    const core::rect<s32> poss(targetPos, sourceSize);

    setRenderStates2DMode(color.getAlpha()<255, true, useAlphaChannelOfTexture);

    S3DVertex vtx[4];
    vtx[0] = S3DVertex((f32)poss.UpperLeftCorner.X, (f32)poss.UpperLeftCorner.Y, 0.0f,
            0.0f, 0.0f, 0.0f, color,
            tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y);
    vtx[1] = S3DVertex((f32)poss.LowerRightCorner.X, (f32)poss.UpperLeftCorner.Y, 0.0f,
            0.0f, 0.0f, 0.0f, color,
            tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y);
    vtx[2] = S3DVertex((f32)poss.LowerRightCorner.X, (f32)poss.LowerRightCorner.Y, 0.0f,
            0.0f, 0.0f, 0.0f, color,
            tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y);
    vtx[3] = S3DVertex((f32)poss.UpperLeftCorner.X, (f32)poss.LowerRightCorner.Y, 0.0f,
            0.0f, 0.0f, 0.0f, color,
            tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y);

    s32 verticesSize = 4;

    s16 indices[6] = {0,1,2,0,2,3};
    s32 indicesSize = sizeof(indices)/sizeof(indices[0]);

    // DX 11 needs a renderer, so configure solid
    //MaterialRenderers[video::EMT_SOLID].Renderer->OnSetMaterial(Material, LastMaterial, ResetRenderStates, this);

    //Block2DRenderStateChange = true;
    // Draw
    draw2DVertexPrimitiveList(vtx, verticesSize, indices, indicesSize / 3, video::EVT_STANDARD, scene::EPT_TRIANGLES, video::EIT_16BIT);
    //Block2DRenderStateChange = false;
}

void CD3D11Driver::draw2DImageBatch(const video::ITexture* texture,
                const core::array<core::position2d<s32> >& positions,
                const core::array<core::rect<s32> >& sourceRects,
                const core::rect<s32>* clipRect,
                SColor color, bool useAlphaChannelOfTexture)
{
    if (!texture)
        return;

    SMaterial m;
    m.setTexture(0, const_cast<video::ITexture*>(texture));
    setMaterial(m);

    setRenderStates2DMode(color.getAlpha()<255, true, useAlphaChannelOfTexture);

    const irr::u32 drawCount = core::min_<u32>(positions.size(), sourceRects.size());
    const core::dimension2d<u32>& renderTargetSize = getCurrentRenderTargetSize();

    core::array<S3DVertex> vtx(drawCount * 4);
    core::array<u16> indices(drawCount * 6);

    for(u32 i = 0;i < drawCount; ++i)
    {
        core::position2d<s32> targetPos = positions[i];
        core::position2d<s32> sourcePos = sourceRects[i].UpperLeftCorner;
        // This needs to be signed as it may go negative.
        core::dimension2d<s32> sourceSize(sourceRects[i].getSize());

        if (clipRect)
        {
            if (targetPos.X < clipRect->UpperLeftCorner.X)
            {
                sourceSize.Width += targetPos.X - clipRect->UpperLeftCorner.X;
                if (sourceSize.Width <= 0)
                    continue;

                sourcePos.X -= targetPos.X - clipRect->UpperLeftCorner.X;
                targetPos.X = clipRect->UpperLeftCorner.X;
            }

            if (targetPos.X + (s32)sourceSize.Width > clipRect->LowerRightCorner.X)
            {
                sourceSize.Width -= (targetPos.X + sourceSize.Width) - clipRect->LowerRightCorner.X;
                if (sourceSize.Width <= 0)
                    continue;
            }

            if (targetPos.Y < clipRect->UpperLeftCorner.Y)
            {
                sourceSize.Height += targetPos.Y - clipRect->UpperLeftCorner.Y;
                if (sourceSize.Height <= 0)
                    continue;

                sourcePos.Y -= targetPos.Y - clipRect->UpperLeftCorner.Y;
                targetPos.Y = clipRect->UpperLeftCorner.Y;
            }

            if (targetPos.Y + (s32)sourceSize.Height > clipRect->LowerRightCorner.Y)
            {
                sourceSize.Height -= (targetPos.Y + sourceSize.Height) - clipRect->LowerRightCorner.Y;
                if (sourceSize.Height <= 0)
                    continue;
            }
        }

        // clip these coordinates

        if (targetPos.X<0)
        {
            sourceSize.Width += targetPos.X;
            if (sourceSize.Width <= 0)
                continue;

            sourcePos.X -= targetPos.X;
            targetPos.X = 0;
        }

        if (targetPos.X + sourceSize.Width > (s32)renderTargetSize.Width)
        {
            sourceSize.Width -= (targetPos.X + sourceSize.Width) - renderTargetSize.Width;
            if (sourceSize.Width <= 0)
                continue;
        }

        if (targetPos.Y<0)
        {
            sourceSize.Height += targetPos.Y;
            if (sourceSize.Height <= 0)
                continue;

            sourcePos.Y -= targetPos.Y;
            targetPos.Y = 0;
        }

        if (targetPos.Y + sourceSize.Height > (s32)renderTargetSize.Height)
        {
            sourceSize.Height -= (targetPos.Y + sourceSize.Height) - renderTargetSize.Height;
            if (sourceSize.Height <= 0)
                continue;
        }

        // ok, we've clipped everything.
        // now draw it.

        core::rect<f32> tcoords;
        tcoords.UpperLeftCorner.X = (((f32)sourcePos.X)) / texture->getOriginalSize().Width ;
        tcoords.UpperLeftCorner.Y = (((f32)sourcePos.Y)) / texture->getOriginalSize().Height;
        tcoords.LowerRightCorner.X = tcoords.UpperLeftCorner.X + ((f32)(sourceSize.Width) / texture->getOriginalSize().Width);
        tcoords.LowerRightCorner.Y = tcoords.UpperLeftCorner.Y + ((f32)(sourceSize.Height) / texture->getOriginalSize().Height);

        const core::rect<s32> poss(targetPos, sourceSize);

        vtx.push_back(S3DVertex((f32)poss.UpperLeftCorner.X, (f32)poss.UpperLeftCorner.Y, 0.0f,
                0.0f, 0.0f, 0.0f, color,
                tcoords.UpperLeftCorner.X, tcoords.UpperLeftCorner.Y));
        vtx.push_back(S3DVertex((f32)poss.LowerRightCorner.X, (f32)poss.UpperLeftCorner.Y, 0.0f,
                0.0f, 0.0f, 0.0f, color,
                tcoords.LowerRightCorner.X, tcoords.UpperLeftCorner.Y));
        vtx.push_back(S3DVertex((f32)poss.LowerRightCorner.X, (f32)poss.LowerRightCorner.Y, 0.0f,
                0.0f, 0.0f, 0.0f, color,
                tcoords.LowerRightCorner.X, tcoords.LowerRightCorner.Y));
        vtx.push_back(S3DVertex((f32)poss.UpperLeftCorner.X, (f32)poss.LowerRightCorner.Y, 0.0f,
                0.0f, 0.0f, 0.0f, color,
                tcoords.UpperLeftCorner.X, tcoords.LowerRightCorner.Y));

        const u32 curPos = vtx.size()-4;
        indices.push_back(0+curPos);
        indices.push_back(1+curPos);
        indices.push_back(2+curPos);

        indices.push_back(0+curPos);
        indices.push_back(2+curPos);
        indices.push_back(3+curPos);
    }

    if (vtx.size())
    {        
        // DX 11 needs a renderer, so configure solid
        //MaterialRenderers[video::EMT_SOLID].Renderer->OnSetMaterial(Material, LastMaterial, ResetRenderStates, this);

        //Block2DRenderStateChange = true;
        // Draw
        draw2DVertexPrimitiveList(vtx.pointer(), vtx.size(), indices.pointer(), indices.size() / 3, video::EVT_STANDARD, scene::EPT_TRIANGLES, video::EIT_16BIT);
        //Block2DRenderStateChange = false;
    }
}

void CD3D11Driver::draw2DRectangle(const core::rect<s32>& position,
            SColor colorLeftUp, SColor colorRightUp, SColor colorLeftDown, SColor colorRightDown,
            const core::rect<s32>* clip, bool filled /*= true*/)
{
    core::rect<s32> pos(position);
    const core::dimension2d<u32>& renderTargetSize = getCurrentRenderTargetSize();

    if (clip)
        pos.clipAgainst(*clip);

    if (!pos.isValid())
        return;

    S3DVertex vtx[4];
    vtx[0] = S3DVertex((f32)pos.UpperLeftCorner.X + 0.375f, (f32)pos.UpperLeftCorner.Y + 0.375f, 0.0f,
            0.0f, 0.0f, 0.0f, colorLeftUp, 0.0f, 0.0f);
    vtx[1] = S3DVertex((f32)pos.LowerRightCorner.X + 0.375f, (f32)pos.UpperLeftCorner.Y + 0.375f, 0.0f,
            0.0f, 0.0f, 0.0f, colorRightUp, 0.0f, 1.0f);
    vtx[2] = S3DVertex((f32)pos.LowerRightCorner.X + 0.375f, (f32)pos.LowerRightCorner.Y + 0.375f, 0.0f,
            0.0f, 0.0f, 0.0f, colorRightDown, 1.0f, 0.0f);
    vtx[3] = S3DVertex((f32)pos.UpperLeftCorner.X + 0.375f, (f32)pos.LowerRightCorner.Y + 0.375f, 0.0f,
            0.0f, 0.0f, 0.0f, colorLeftDown, 1.0f, 1.0f);

    //if (!filled)
    //{
    //    vtx[0].Pos.X += 0.375f;
    //    vtx[0].Pos.Y += 0.375f;
    //    vtx[1].Pos.X += 0.375f;
    //    vtx[1].Pos.Y += 0.375f;
    //    vtx[2].Pos.X += 0.375f;
    //    vtx[2].Pos.Y += 0.375f;
    //    vtx[3].Pos.X += 0.375f;
    //    vtx[3].Pos.Y += 0.375f;
    //}

    static s16 fillIndices[6] = {0,1,2,0,2,3};
    static s16 unfillIndices[8] = { 0,1,1,2,2,3,3,0 };

    static SMaterial _m;
    setMaterial(_m);

    setRenderStates2DMode(
        colorLeftUp.getAlpha() < 255 ||
        colorRightUp.getAlpha() < 255 ||
        colorLeftDown.getAlpha() < 255 ||
        colorRightDown.getAlpha() < 255, false, false);

    //D2DContext->SaveDrawingState(m_stateBlock);
    //
    //D2DContext->BeginDraw();
    //D2DContext->SetTransform(D2D1::Matrix3x2F::Identity());
    //
    //D2D1_BRUSH_PROPERTIES brushDesc;
    //brushDesc.opacity = colorLeftUp.getAlpha() / 255.f;
    ////brushDesc.transform;
    //
    //ID2D1SolidColorBrush* m_Brush;
    //
    //DX::ThrowIfFailed(
    //    D2DContext->CreateSolidColorBrush(D2D1::ColorF(colorLeftUp.color), brushDesc, &m_Brush)
    //    //D2DContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), &m_Brush)
    //    );
    //
    //D2D1_RECT_F rec;
    //rec.top     = (float)position.UpperLeftCorner.Y;
    //rec.left    = (float)position.UpperLeftCorner.X;
    //rec.bottom  = (float)position.LowerRightCorner.Y;
    //rec.right   = (float)position.LowerRightCorner.X;
    //
    //D2DContext->FillRectangle(rec, m_Brush);
    ////D2DContext->DrawRectangle( rec, m_Brush, 0.5f );
    //
    //// We ignore D2DERR_RECREATE_TARGET here. This error indicates that the device
    //// is lost. It will be handled during the next call to Present.
    //HRESULT hr = D2DContext->EndDraw();
    //if (hr != D2DERR_RECREATE_TARGET)
    //{
    //    DX::ThrowIfFailed(hr);
    //}
    //
    //D2DContext->RestoreDrawingState(m_stateBlock);

    //MaterialRenderers[video::EMT_SOLID].Renderer->OnSetMaterial(Material, LastMaterial, ResetRenderStates, this);
    //Block2DRenderStateChange = true;
    // Draw
    if (filled)
        draw2DVertexPrimitiveList(vtx, 4, fillIndices, 2, video::EVT_STANDARD, scene::EPT_TRIANGLES, video::EIT_16BIT);
    else
        draw2DVertexPrimitiveList(vtx, 4, unfillIndices, 4, video::EVT_STANDARD, scene::EPT_LINES, video::EIT_16BIT);
    //Block2DRenderStateChange = false;
}

void CD3D11Driver::draw2DLine(const core::position2d<s32>& start,
                    const core::position2d<s32>& end, SColor color)
{
    if (start == end)
        drawPixel(start.X, start.Y, color);
    else
    {
        // thanks to Vash TheStampede who sent in his implementation
        S3DVertex vtx[2];
        vtx[0] = S3DVertex((f32)start.X + 0.375f, (f32)start.Y + 0.375f, 0.0f,
                           0.0f, 0.0f, 0.0f, color, 0.0f, 0.0f);
        vtx[1] = S3DVertex((f32)end.X + 0.375f, (f32)end.Y + 0.375f, 0.0f,
                           0.0f, 0.0f, 0.0f, color, 0.0f, 1.0f);

        static const u16 indices[2] = {0, 1};

        static SMaterial _m;
        setMaterial(_m);

        setRenderStates2DMode(color.getAlpha() < 255, false, false);

        //setRenderStates2DMode(color.getAlpha() < 255, false, false);
        //setActiveTexture(0, 0);

        //Block2DRenderStateChange = true;
        // Draw
        draw2DVertexPrimitiveList(vtx, 2, indices, 1, video::EVT_STANDARD, scene::EPT_LINES, video::EIT_16BIT);
        //Block2DRenderStateChange = false;
    }
}

void CD3D11Driver::drawPixel(u32 x, u32 y, const SColor & color)
{
    os::Printer::log("\"drawPixel\" method not supported in Direct3D 11", ELL_ERROR);
}

void CD3D11Driver::draw3DLine(const core::vector3df& start,
            const core::vector3df& end, SColor color)
{
    setRenderStates3DMode(video::EVT_STANDARD);
    video::S3DVertex v[2];
    v[0].Color = color;
    v[1].Color = color;
    v[0].Pos = start;
    v[1].Pos = end;

    static const u16 indices[2] = { 0, 1 };

    //Block2DRenderStateChange = true;
    // Draw
    draw2D3DVertexPrimitiveList(v, 2, indices, 1, video::EVT_STANDARD, scene::EPT_LINES, video::EIT_16BIT, true);
    //draw2DVertexPrimitiveList(v, 2, indices, 1, video::EVT_STANDARD, scene::EPT_LINES, video::EIT_16BIT);
    //Block2DRenderStateChange = false;
}

const wchar_t* CD3D11Driver::getName() const
{
    return DriverAndFeatureName.c_str(); //L"Direct3D 11.0";
}

void CD3D11Driver::deleteAllDynamicLights()
{
    RequestedLights.clear();

    CNullDriver::deleteAllDynamicLights();
}

s32 CD3D11Driver::addDynamicLight(const SLight& light)
{
    CNullDriver::addDynamicLight(light);

    RequestedLights.push_back(RequestedLight(light));

    return (s32)(RequestedLights.size() - 1);
}

void CD3D11Driver::turnLightOn(s32 lightIndex, bool turnOn)
{
    if(lightIndex < 0 || lightIndex >= (s32)RequestedLights.size())
        return;

    RequestedLight & requestedLight = RequestedLights[lightIndex];
    requestedLight.DesireToBeOn = turnOn;
}

u32 CD3D11Driver::getMaximalDynamicLightAmount() const
{
    return MaxActiveLights;
}

void CD3D11Driver::setAmbientLight(const SColorf& color)
{
    AmbientLight = color;
}

void CD3D11Driver::drawStencilShadowVolume(const core::array<core::vector3df>& triangles, bool zfail, u32 debugDataVisible)
{

}

void CD3D11Driver::drawStencilShadow(bool clearStencilBuffer,
            video::SColor leftUpEdge, video::SColor rightUpEdge,
            video::SColor leftDownEdge, video::SColor rightDownEdge)
{

}

u32 CD3D11Driver::getMaximalPrimitiveCount() const
{
    return 0x7fffffff;
}


void CD3D11Driver::setTextureCreationFlag(E_TEXTURE_CREATION_FLAG flag, bool enabled)
{
    if (flag == video::ETCF_CREATE_MIP_MAPS && !queryFeature(EVDF_MIP_MAP))
        enabled = false;

    CNullDriver::setTextureCreationFlag(flag, enabled);
}


void CD3D11Driver::setFog(SColor color, E_FOG_TYPE fogType, f32 start,
            f32 end, f32 density, bool pixelFog, bool rangeFog)
{
    // just forward method, configurations will be applyed in the material renderer
    CNullDriver::setFog(color, fogType, start, end, density, pixelFog, rangeFog);


}


void CD3D11Driver::OnResize(const core::dimension2d<u32>& size)
{
    if (!Device || !SwapChain)
        return;

    CNullDriver::OnResize(size);
    present.Width  = size.Width;
    present.Height = size.Height;

    //present.BufferDesc.Width = size.Width;
    //present.BufferDesc.Height = size.Height;
    ScreenSize = size;

    reset();
}

//! sets the needed renderstates
bool CD3D11Driver::setRenderStates3DMode(E_VERTEX_TYPE vType)
{
    if (!STATHREADiD)
        STATHREADiD = GetCurrentThreadId();
    assert(STATHREADiD == GetCurrentThreadId());

    if (!Device)
        return false;

    LastVertexType = vType;

    if (CurrentRenderMode != ERM_3D)
    {
        ResetRenderStates = true;
    }

    if (ResetRenderStates || LastMaterial != Material)
    {
        // unset old material
        if (CurrentRenderMode == ERM_3D &&
            LastMaterial.MaterialType != Material.MaterialType &&
            LastMaterial.MaterialType >= 0 && LastMaterial.MaterialType < (s32)MaterialRenderers.size())
            MaterialRenderers[LastMaterial.MaterialType].Renderer->OnUnsetMaterial();

        // set new material.
        if (Material.MaterialType >= 0 && Material.MaterialType < (s32)MaterialRenderers.size())
            MaterialRenderers[Material.MaterialType].Renderer->OnSetMaterial(Material, LastMaterial, ResetRenderStates, this);
    }

    //bool shaderOK = true;
    //if (Material.MaterialType >= 0 && Material.MaterialType < (s32)MaterialRenderers.size())
    //    shaderOK = MaterialRenderers[Material.MaterialType].Renderer->OnRender(this, LastVertexType);

    LastMaterial = Material;

    ResetRenderStates = false;

    CurrentRenderMode = ERM_3D;

    return true;
}

bool CD3D11Driver::setRenderStates2DMode(bool alpha, bool texture, bool alphaChannel)
{
    if (Block2DRenderStateChange)
        return true;

    if (CurrentRenderMode != ERM_2D || Transformation3DChanged)
    {
        // unset last 3d material
        if (CurrentRenderMode == ERM_3D)
        {
            if (static_cast<u32>(LastMaterial.MaterialType) < MaterialRenderers.size())
                MaterialRenderers[LastMaterial.MaterialType].Renderer->OnUnsetMaterial();
        }

        if(!OverrideMaterial2DEnabled)
        {
            setBasicRenderStates(InitMaterial2D, LastMaterial, true);
            LastMaterial = InitMaterial2D;

            DepthStencilDesc.StencilEnable = FALSE;
        }

        // Set world to identity
        core::matrix4 m;
        setTransform(ETS_WORLD, m);

        // Set view to translate a little forward
        //m.setTranslation(core::vector3df(-0.5f, -0.5f, 0)ü);
        setTransform(ETS_VIEW, getTransform(ETS_VIEW_2D));

        // Adjust projection
        const core::dimension2d<u32>& renderTargetSize = getCurrentRenderTargetSize();
        m.buildProjectionMatrixOrthoLH(f32(renderTargetSize.Width), f32(-(s32)(renderTargetSize.Height)), -1.0, 1.0);
        m.setTranslation(core::vector3df(-1, 1, 0));
        setTransform(ETS_PROJECTION, m);

        Transformation3DChanged = false;
    }

    // no alphaChannel without texture
    alphaChannel &= texture;

    if (alpha || alphaChannel)
    {
        BlendDesc.RenderTarget[0].BlendEnable = TRUE;
        BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

        BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    }
    else
        BlendDesc.RenderTarget[0].BlendEnable = FALSE;

    if (texture)
    {
        setTransform(ETS_TEXTURE_0, core::IdentityMatrix);
        // Due to the transformation change, the previous line would call a reset each frame
        // but we can safely reset the variable as it was false before
        Transformation3DChanged = false;
    }

    if (OverrideMaterial2DEnabled)
    {
        OverrideMaterial2D.Lighting=false;
        OverrideMaterial2D.ZBuffer=ECFN_NEVER;
        OverrideMaterial2D.ZWriteEnable=false;
        setBasicRenderStates(OverrideMaterial2D, LastMaterial, false);
        LastMaterial = OverrideMaterial2D;
    }

    // handle alpha

    CurrentRenderMode = ERM_2D;
    return true;
}

void CD3D11Driver::setBasicRenderStates(const SMaterial& material, const SMaterial& lastMaterial,
            bool resetAllRenderstates)
{
    // init states description
    //BlendDesc.reset();
    //RasterizerDesc.reset();
    //DepthStencilDesc.reset();
    //for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; i++)
    //    SamplerDesc[i].reset();

    // This needs only to be updated onresets
    //if ( Params.HandleSRGB && resetAllRenderstates )
    //    //TODO: no information

    // fillmode
    if ( resetAllRenderstates || lastMaterial.Wireframe != material.Wireframe || lastMaterial.PointCloud != material.PointCloud )
    {
        if ( material.Wireframe )
            RasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
        else
            RasterizerDesc.FillMode = D3D11_FILL_SOLID;
    }

    // multisample
    //if (FeatureEnabled[EVDF_TEXTURE_MULTISAMPLING])
    //    RasterizerDesc.MultisampleEnable = true;

    // material properties (ambient, diffuse, etc, and lighthing)
    // handled in MaterialRenderers

    // shademode (Flat or Gouraud)
    // handled in MaterialRenderers

    // lighting 
    // handled in MaterialRenderers

    // fog
    // handled in MaterialRenderers

    // specular highlights
    // handled in MaterialRenderers

    // normalization
    // handled in MaterialRenderers (shader)

    // thickness
    // handled in MaterialRenderers

    // zbuffer
    if ( resetAllRenderstates || (lastMaterial.ZBuffer != material.ZBuffer) )
    {
        DepthStencilDesc.DepthEnable = (material.ZBuffer == ECFN_NEVER) ? false : true;
        DepthStencilDesc.DepthFunc = getDepthFunction((E_COMPARISON_FUNC)material.ZBuffer);
    }

    // zwrite
    if ( resetAllRenderstates || (lastMaterial.ZWriteEnable != material.ZWriteEnable) )
    {
        // zwrite
        if ( material.ZWriteEnable && (AllowZWriteOnTransparent || !material.isTransparent()) )
            DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        else
            DepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    }

    // back face culling
    if ( resetAllRenderstates || (lastMaterial.FrontfaceCulling != material.FrontfaceCulling) || (lastMaterial.BackfaceCulling != material.BackfaceCulling) )
    {
        if ( material.FrontfaceCulling && !material.Wireframe )
            RasterizerDesc.CullMode = D3D11_CULL_FRONT;
        else if ( material.BackfaceCulling && !material.Wireframe )
            RasterizerDesc.CullMode = D3D11_CULL_BACK;
        else
            RasterizerDesc.CullMode = D3D11_CULL_NONE;
    }

    // Color Mask
    if ( queryFeature(EVDF_COLOR_MASK) &&
        (resetAllRenderstates || ResetBlending || lastMaterial.ColorMask != material.ColorMask) )
    {
        // multiple color masks and independent blend will be handled in setRenderTarget
        const UINT8 flag =
            ((material.ColorMask & ECP_RED) ? D3D11_COLOR_WRITE_ENABLE_RED : 0) |
            ((material.ColorMask & ECP_GREEN) ? D3D11_COLOR_WRITE_ENABLE_GREEN : 0) |
            ((material.ColorMask & ECP_BLUE) ? D3D11_COLOR_WRITE_ENABLE_BLUE : 0) |
            ((material.ColorMask & ECP_ALPHA) ? D3D11_COLOR_WRITE_ENABLE_ALPHA : 0);
        BlendDesc.RenderTarget[0].RenderTargetWriteMask = flag;
    }

    // Anti Aliasing
    if ( AlphaToCoverageSupport && (resetAllRenderstates || (lastMaterial.AntiAliasing != material.AntiAliasing)) )
    {
        if ( (material.AntiAliasing & EAAM_ALPHA_TO_COVERAGE) )
            BlendDesc.AlphaToCoverageEnable = TRUE;
        else if ( (lastMaterial.AntiAliasing & EAAM_ALPHA_TO_COVERAGE) )
            BlendDesc.AlphaToCoverageEnable = FALSE;
    }

    if ( queryFeature(EVDF_BLEND_OPERATIONS) &&
        (resetAllRenderstates || ResetBlending || lastMaterial.BlendOperation != material.BlendOperation) )
    {
        ResetBlending = false;

        if ( material.BlendOperation == EBO_NONE )
            BlendDesc.RenderTarget[0].BlendEnable = FALSE;
        else
        {
            BlendDesc.RenderTarget[0].BlendEnable = TRUE;
            //BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

            switch ( material.BlendOperation )
            {
                case EBO_SUBTRACT:
                    BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_SUBTRACT;
                    break;
                case EBO_REVSUBTRACT:
                    BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
                    break;
                case EBO_MIN:
                case EBO_MIN_FACTOR:
                case EBO_MIN_ALPHA:
                    BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MIN;
                    break;
                case EBO_MAX:
                case EBO_MAX_FACTOR:
                case EBO_MAX_ALPHA:
                    BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_MAX;
                    break;
                default:
                    BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
                    break;
            }
        }
    }

    // Polygon offset
    if ( queryFeature(EVDF_POLYGON_OFFSET) && (resetAllRenderstates ||
        lastMaterial.PolygonOffsetDirection != material.PolygonOffsetDirection ||
        lastMaterial.PolygonOffsetFactor != material.PolygonOffsetFactor) )
    {
        if ( material.PolygonOffsetFactor )
        {
            if ( material.PolygonOffsetDirection == EPO_BACK )
            {
                RasterizerDesc.SlopeScaledDepthBias = 1.f;
                RasterizerDesc.DepthBias = F2DW((FLOAT)material.PolygonOffsetFactor);
            }
            else
            {
                RasterizerDesc.SlopeScaledDepthBias = -1.f;
                RasterizerDesc.DepthBias = F2DW((FLOAT)-material.PolygonOffsetFactor);

            }
        }
        else
        {
            RasterizerDesc.SlopeScaledDepthBias = 0;
            RasterizerDesc.DepthBias = 0;
        }
    }

    // enable antialiasing
    if ( resetAllRenderstates || lastMaterial.AntiAliasing != material.AntiAliasing )
    {
        //RasterizerDesc.ada
        if ( Params.AntiAlias )
        {
            if ( material.AntiAliasing & (EAAM_SIMPLE | EAAM_QUALITY) )
                RasterizerDesc.MultisampleEnable = TRUE;
            else if ( lastMaterial.AntiAliasing & (EAAM_SIMPLE | EAAM_QUALITY) )
                RasterizerDesc.MultisampleEnable = FALSE;

            if ( material.AntiAliasing & (EAAM_LINE_SMOOTH) )
                RasterizerDesc.AntialiasedLineEnable = TRUE;
            else if ( lastMaterial.AntiAliasing & (EAAM_LINE_SMOOTH) )
                RasterizerDesc.AntialiasedLineEnable = FALSE;
        }
    }

    // texture address mode
    for (u32 st=0; st<MaxTextureUnits; ++st)
    {
        //if ( resetAllRenderstates && Params.HandleSRGB )
        // TODO:

        if ( resetAllRenderstates || lastMaterial.TextureLayer[st].LODBias != material.TextureLayer[st].LODBias )
        {
            SamplerDesc[st].MinLOD = 0;
            SamplerDesc[st].MaxLOD = D3D11_FLOAT32_MAX;

            const float tmp = material.TextureLayer[st].LODBias * 0.125f;
            SamplerDesc[st].MipLODBias = tmp;
        }

        // Describe the Sample State
        SamplerDesc[st].ComparisonFunc = D3D11_COMPARISON_NEVER;
        SamplerDesc[st].AddressW = getTextureWrapMode(material.TextureLayer[st].TextureWrapU);

        if ( resetAllRenderstates || lastMaterial.TextureLayer[st].TextureWrapU != material.TextureLayer[st].TextureWrapU )
            SamplerDesc[st].AddressU = getTextureWrapMode(material.TextureLayer[st].TextureWrapU);
        if ( resetAllRenderstates || lastMaterial.TextureLayer[st].TextureWrapV != material.TextureLayer[st].TextureWrapV )
            SamplerDesc[st].AddressV = getTextureWrapMode(material.TextureLayer[st].TextureWrapV);

        // Bilinear, trilinear, and anisotropic filter
        if ( resetAllRenderstates ||
            lastMaterial.TextureLayer[st].BilinearFilter != material.TextureLayer[st].BilinearFilter ||
            lastMaterial.TextureLayer[st].TrilinearFilter != material.TextureLayer[st].TrilinearFilter ||
            lastMaterial.TextureLayer[st].AnisotropicFilter != material.TextureLayer[st].AnisotropicFilter ||
            lastMaterial.UseMipMaps != material.UseMipMaps )
        {
            if ( material.TextureLayer[st].BilinearFilter || material.TextureLayer[st].TrilinearFilter || material.TextureLayer[st].AnisotropicFilter )
            {
                D3D11_FILTER filter = ( Params.AntiAlias &&
                    material.TextureLayer[st].AnisotropicFilter) ? D3D11_FILTER_ANISOTROPIC : 
                    (material.TextureLayer[st].TrilinearFilter && lastMaterial.UseMipMaps ? 
                        D3D11_FILTER_MIN_MAG_MIP_LINEAR : D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT);

                if ( filter == D3D11_FILTER_ANISOTROPIC )
                    SamplerDesc[st].MaxAnisotropy = (DWORD)material.TextureLayer[st].AnisotropicFilter;
                else
                    SamplerDesc[st].MaxAnisotropy = 0;

                SamplerDesc[st].Filter = filter;
            }
            else
            {
                SamplerDesc[st].Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
            }
        }
    }
}


E_DRIVER_TYPE CD3D11Driver::getDriverType() const
{
    return EDT_DIRECT3D11;
}


const core::matrix4& CD3D11Driver::getTransform(E_TRANSFORMATION_STATE state) const
{
    return Matrices[state];
}
//! Creates a render target texture.
ITexture* CD3D11Driver::addRenderTargetTexture(const core::dimension2d<u32>& size,
                const io::path& name, const ECOLOR_FORMAT format)
{
    ITexture* tex = new CD3D11Texture(this, size, name, format, 1, present.SampleDesc.Count, present.SampleDesc.Quality);
    if (tex)
    {
        checkDepthBuffer(tex);
        addTexture(tex);
        tex->drop();
    }
    return tex;
}

video::ITexture* CD3D11Driver::createDeviceDependentTexture(IImage* surface, const io::path& name, void* mipmapData)
{
    return new CD3D11Texture(surface, this, TextureCreationFlags, name, 1, mipmapData, 1, 0);
}

IHardwareBuffer* CD3D11Driver::createHardwareBuffer(E_HARDWARE_BUFFER_TYPE type, 
                                                    E_HARDWARE_BUFFER_ACCESS accessType, 
                                                    u32 size, u32 flags, const void* initialData)
{
    _IRR_DEBUG_BREAK_IF(true);
    return nullptr; // new CD3D11HardwareBuffer(this, type, accessType, size, flags, initialData);
}

E_VERTEX_TYPE CD3D11Driver::registerVertexType(core::array<SVertexElement>& elements)
{
    //CD3D11VertexDeclaration* decl = new CD3D11VertexDeclaration(this, elements, (E_VERTEX_TYPE)declarationMap.size());
    //declarationMap.insert(decl->getType(), decl);
    //
    //return decl->getType();
    return E_VERTEX_TYPE(0);
}

u32 CD3D11Driver::queryMultisampleLevels(ECOLOR_FORMAT format, u32 numSamples) const
{
    UINT quality = 0;
    if (SUCCEEDED(Device->CheckMultisampleQualityLevels(this->getD3DFormatFromColorFormat(format),
                                                        4, &quality)))
    {
        return quality;
    }

    return 0;
}

void CD3D11Driver::clearZBuffer()
{
    if( CurrentDepthBuffer )
        ImmediateContext->ClearDepthStencilView( CurrentDepthBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0 );
}

IImage* CD3D11Driver::createScreenShot(video::ECOLOR_FORMAT format, video::E_RENDER_TARGET target)
{
    u32 width = getCurrentRenderTargetSize().Width;
    u32 height = getCurrentRenderTargetSize().Height;
    ID3D11Texture2D* bufftex = 0;
    D3D11_MAPPED_SUBRESOURCE mappedData;
    CD3D11_TEXTURE2D_DESC tex_desc(DXGI_FORMAT_R8G8B8A8_UNORM, width, height, 1, 1, 0, D3D11_USAGE_STAGING, D3D11_CPU_ACCESS_READ|D3D11_CPU_ACCESS_WRITE);

    IImage* img = createImage(ECF_A8R8G8B8, getCurrentRenderTargetSize());

    if( !img )
        return NULL;
    
    // First, create a texture buffer
    HRESULT hr = Device->CreateTexture2D( &tex_desc, NULL, &bufftex );
    if( FAILED( hr ) )
        return NULL;

    // Copy back buffer to our texture buffer
    ID3D11Resource* backBuffer = 0;
    DefaultBackBuffer->GetResource( &backBuffer );
    ImmediateContext->CopyResource( bufftex, backBuffer );
    backBuffer->Release();

    // Get buffer data
    if( FAILED( ImmediateContext->Map( bufftex, 0, D3D11_MAP_READ_WRITE, 0, &mappedData ) ) )
        return NULL;

    // Process data in place, handling alpha channel
    for (u32 y = 0; y < height; ++y)
    {
        u8* ptr = (u8*)mappedData.pData + y * mappedData.RowPitch + 3;
        for (u32 x = 0; x < width; ++x)
        {
            *ptr = 0xFF;
            ptr += 4;
        }
    }
    
    // Copy data to image
    memcpy( img->lock(), mappedData.pData, width * height * 4 );
    
    // Unlock image and texture
    img->unlock();
    ImmediateContext->Unmap( bufftex, 0 );

    // Release texture buffer
    bufftex->Release();

    // Return image
    return img;
}

bool CD3D11Driver::setClipPlane(u32 index, const core::plane3df& plane, bool enable)
{
    if(index > 3)
        return false;

    ClipPlanes[index] = plane;
    
    enableClipPlane(index, enable);

    return true;
}

void CD3D11Driver::enableClipPlane(u32 index, bool enable)
{
    ClipPlaneEnabled[index] = enable;
}

void CD3D11Driver::getClipPlane(u32 index, core::plane3df& plane, bool& enable)
{
    plane = ClipPlanes[index];
    enable = ClipPlaneEnabled[index];
}

void CD3D11Driver::enableMaterial2D(bool enable)
{
    CNullDriver::enableMaterial2D(enable);
} 

ECOLOR_FORMAT CD3D11Driver::getColorFormat() const
{
    return ColorFormat;
}

DXGI_FORMAT CD3D11Driver::getD3DColorFormat() const
{
    return D3DColorFormat;
}

core::dimension2du CD3D11Driver::getMaxTextureSize() const
{
    // Maximal value depends of driver type.
    switch( FeatureLevel )
    {
    case D3D_FEATURE_LEVEL_11_0:
        return core::dimension2du( 16384, 16384 );

    case D3D10_FEATURE_LEVEL_10_1:
    case D3D10_FEATURE_LEVEL_10_0:
        return core::dimension2du( 8192, 8192 );

    case D3D10_FEATURE_LEVEL_9_3:
        return core::dimension2du( 4096, 4096 );
    
    case D3D10_FEATURE_LEVEL_9_2:
    case D3D10_FEATURE_LEVEL_9_1:
        return core::dimension2du( 2048, 2048 );
    }

    return core::dimension2du( 16384, 16384 );
}

DXGI_FORMAT CD3D11Driver::getD3DFormatFromColorFormat(ECOLOR_FORMAT format) const
{
    switch(format)
    {
    case ECF_RGBA_S3TC_DXT1:
        return DXGI_FORMAT_BC1_UNORM;

    case ECF_RGBA_S3TC_DXT3:
        return DXGI_FORMAT_BC2_UNORM;

    case ECF_RGBA_S3TC_DXT5:
        return DXGI_FORMAT_BC3_UNORM;

    case ECF_A1R5G5B5:
        return DXGI_FORMAT_B5G5R5A1_UNORM;

    case ECF_R5G6B5:
        return DXGI_FORMAT_B5G6R5_UNORM;

    case ECF_R8G8B8:
        return DXGI_FORMAT_R8G8B8A8_UNORM;

    case ECF_RGBA8:
    case ECF_A8R8G8B8:
        return DXGI_FORMAT_R8G8B8A8_UNORM;

    case ECF_R16F:
        return DXGI_FORMAT_R16_FLOAT;

    case ECF_G16R16F:
        return DXGI_FORMAT_R16G16_FLOAT;

    case ECF_A16B16G16R16F:
        return DXGI_FORMAT_R16G16B16A16_FLOAT;

    case ECF_R32F:
        return DXGI_FORMAT_R32_FLOAT;

    case ECF_G32R32F:
        return DXGI_FORMAT_R32G32_FLOAT;

    case ECF_A32B32G32R32F:
        return DXGI_FORMAT_R32G32B32A32_FLOAT;
    }

    return DXGI_FORMAT_UNKNOWN;
}

ECOLOR_FORMAT CD3D11Driver::getColorFormatFromD3DFormat(DXGI_FORMAT format) const
{
    switch(format)
    {
    case DXGI_FORMAT_BC1_UNORM:
        return ECF_RGBA_S3TC_DXT1;

    case DXGI_FORMAT_BC2_UNORM:
        return ECF_RGBA_S3TC_DXT3;

    case DXGI_FORMAT_BC3_UNORM:
        return ECF_RGBA_S3TC_DXT5;

    case DXGI_FORMAT_B5G5R5A1_UNORM:
        return ECF_A1R5G5B5;

    case DXGI_FORMAT_B5G6R5_UNORM:
        return ECF_R5G6B5;

    case DXGI_FORMAT_R8G8B8A8_UNORM:
        return ECF_A8R8G8B8;

    case DXGI_FORMAT_R16_FLOAT:
        return ECF_R16F;

    case DXGI_FORMAT_R16G16_FLOAT :
        return ECF_G16R16F;

    case DXGI_FORMAT_R16G16B16A16_FLOAT :
        return ECF_A16B16G16R16F;

    case DXGI_FORMAT_R32_FLOAT :
        return ECF_R32F;

    case DXGI_FORMAT_R32G32_FLOAT :
        return ECF_G32R32F;

    case DXGI_FORMAT_R32G32B32A32_FLOAT :
        return ECF_A32B32G32R32F;
    }

    return (ECOLOR_FORMAT)0;
}

D3D_PRIMITIVE_TOPOLOGY CD3D11Driver::getTopology(scene::E_PRIMITIVE_TYPE primType) const
{
    switch( primType )
    {
        case scene::EPT_POINT_SPRITES:
        case scene::EPT_POINTS:
            return D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

        case scene::EPT_LINE_STRIP:
            return  D3D_PRIMITIVE_TOPOLOGY_LINESTRIP;

        case scene::EPT_LINE_LOOP:
        case scene::EPT_LINES:
            return D3D_PRIMITIVE_TOPOLOGY_LINELIST;

        case scene::EPT_TRIANGLE_STRIP:
            return D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;

        case scene::EPT_TRIANGLES:
            return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

        case scene::EPT_TRIANGLE_FAN:
        default:
            return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
    }
}

u32 CD3D11Driver::getBitsPerPixel(DXGI_FORMAT format) const
{
    switch( format )
    {
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R32G32B32A32_SINT:
        return 128;

    case DXGI_FORMAT_R32G32B32_TYPELESS:
    case DXGI_FORMAT_R32G32B32_FLOAT:
    case DXGI_FORMAT_R32G32B32_UINT:
    case DXGI_FORMAT_R32G32B32_SINT:
        return 96;

    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_UNORM:
    case DXGI_FORMAT_R16G16B16A16_UINT:
    case DXGI_FORMAT_R16G16B16A16_SNORM:
    case DXGI_FORMAT_R16G16B16A16_SINT:
    case DXGI_FORMAT_R32G32_TYPELESS:
    case DXGI_FORMAT_R32G32_FLOAT:
    case DXGI_FORMAT_R32G32_UINT:
    case DXGI_FORMAT_R32G32_SINT:
    case DXGI_FORMAT_R32G8X24_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
        return 64;

    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R10G10B10A2_UINT:
    case DXGI_FORMAT_R11G11B10_FLOAT:
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R8G8B8A8_SNORM:
    case DXGI_FORMAT_R8G8B8A8_SINT:
    case DXGI_FORMAT_R16G16_TYPELESS:
    case DXGI_FORMAT_R16G16_FLOAT:
    case DXGI_FORMAT_R16G16_UNORM:
    case DXGI_FORMAT_R16G16_UINT:
    case DXGI_FORMAT_R16G16_SNORM:
    case DXGI_FORMAT_R16G16_SINT:
    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R32_UINT:
    case DXGI_FORMAT_R32_SINT:
    case DXGI_FORMAT_R24G8_TYPELESS:
    case DXGI_FORMAT_D24_UNORM_S8_UINT:
    case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
    case DXGI_FORMAT_X24_TYPELESS_G8_UINT:
    case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
    case DXGI_FORMAT_R8G8_B8G8_UNORM:
    case DXGI_FORMAT_G8R8_G8B8_UNORM:
    case DXGI_FORMAT_B8G8R8A8_UNORM:
    case DXGI_FORMAT_B8G8R8X8_UNORM:
    case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        return 32;

    case DXGI_FORMAT_R8G8_TYPELESS:
    case DXGI_FORMAT_R8G8_UNORM:
    case DXGI_FORMAT_R8G8_UINT:
    case DXGI_FORMAT_R8G8_SNORM:
    case DXGI_FORMAT_R8G8_SINT:
    case DXGI_FORMAT_R16_TYPELESS:
    case DXGI_FORMAT_R16_FLOAT:
    case DXGI_FORMAT_D16_UNORM:
    case DXGI_FORMAT_R16_UNORM:
    case DXGI_FORMAT_R16_UINT:
    case DXGI_FORMAT_R16_SNORM:
    case DXGI_FORMAT_R16_SINT:
    case DXGI_FORMAT_B5G6R5_UNORM:
    case DXGI_FORMAT_B5G5R5A1_UNORM:
        return 16;

    case DXGI_FORMAT_R8_TYPELESS:
    case DXGI_FORMAT_R8_UNORM:
    case DXGI_FORMAT_R8_UINT:
    case DXGI_FORMAT_R8_SNORM:
    case DXGI_FORMAT_R8_SINT:
    case DXGI_FORMAT_A8_UNORM:
        return 8;

    case DXGI_FORMAT_R1_UNORM:
        return 1;

    case DXGI_FORMAT_BC1_TYPELESS:
    case DXGI_FORMAT_BC1_UNORM:
    case DXGI_FORMAT_BC1_UNORM_SRGB:
        return 4;

    case DXGI_FORMAT_BC2_TYPELESS:
    case DXGI_FORMAT_BC2_UNORM:
    case DXGI_FORMAT_BC2_UNORM_SRGB:
    case DXGI_FORMAT_BC3_TYPELESS:
    case DXGI_FORMAT_BC3_UNORM:
    case DXGI_FORMAT_BC3_UNORM_SRGB:
    case DXGI_FORMAT_BC4_TYPELESS:
    case DXGI_FORMAT_BC4_UNORM:
    case DXGI_FORMAT_BC4_SNORM:
    case DXGI_FORMAT_BC5_TYPELESS:
    case DXGI_FORMAT_BC5_UNORM:
    case DXGI_FORMAT_BC5_SNORM:
    case DXGI_FORMAT_BC6H_TYPELESS:
    case DXGI_FORMAT_BC6H_UF16:
    case DXGI_FORMAT_BC6H_SF16:
    case DXGI_FORMAT_BC7_TYPELESS:
    case DXGI_FORMAT_BC7_UNORM:
    case DXGI_FORMAT_BC7_UNORM_SRGB:
        return 8;

#if defined(_XBOX_ONE) && defined(_TITLE)

    case DXGI_FORMAT_R10G10B10_7E3_A2_FLOAT:
    case DXGI_FORMAT_R10G10B10_6E4_A2_FLOAT:
        return 32;

    case DXGI_FORMAT_D16_UNORM_S8_UINT:
    case DXGI_FORMAT_R16_UNORM_X8_TYPELESS:
    case DXGI_FORMAT_X16_TYPELESS_G8_UINT:
        return 24;

#endif // _XBOX_ONE && _TITLE
    default:
        return 0;
    }
}

u32 CD3D11Driver::getNumberOfComponents(DXGI_FORMAT format) const
{
    switch( format )
    {
    case DXGI_FORMAT_R32G32B32A32_TYPELESS:
    case DXGI_FORMAT_R32G32B32A32_FLOAT:
    case DXGI_FORMAT_R32G32B32A32_UINT:
    case DXGI_FORMAT_R32G32B32A32_SINT:
    case DXGI_FORMAT_R16G16B16A16_TYPELESS:
    case DXGI_FORMAT_R16G16B16A16_FLOAT:
    case DXGI_FORMAT_R16G16B16A16_UNORM:
    case DXGI_FORMAT_R16G16B16A16_UINT:
    case DXGI_FORMAT_R16G16B16A16_SNORM:
    case DXGI_FORMAT_R16G16B16A16_SINT:
    case DXGI_FORMAT_R10G10B10A2_TYPELESS:
    case DXGI_FORMAT_R10G10B10A2_UNORM:
    case DXGI_FORMAT_R10G10B10A2_UINT:
    case DXGI_FORMAT_R8G8B8A8_TYPELESS:
    case DXGI_FORMAT_R8G8B8A8_UNORM:
    case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
    case DXGI_FORMAT_R8G8B8A8_UINT:
    case DXGI_FORMAT_R8G8B8A8_SNORM:
    case DXGI_FORMAT_R8G8B8A8_SINT:
    case DXGI_FORMAT_B8G8R8A8_TYPELESS:
    case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
    case DXGI_FORMAT_B8G8R8X8_TYPELESS:
    case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
        return 4;

    case DXGI_FORMAT_R32G32B32_TYPELESS:
    case DXGI_FORMAT_R32G32B32_FLOAT:
    case DXGI_FORMAT_R32G32B32_UINT:
    case DXGI_FORMAT_R32G32B32_SINT:
    case DXGI_FORMAT_R11G11B10_FLOAT:
        return 3;

    case DXGI_FORMAT_R32G32_TYPELESS:
    case DXGI_FORMAT_R32G32_FLOAT:
    case DXGI_FORMAT_R32G32_UINT:
    case DXGI_FORMAT_R32G32_SINT:
    case DXGI_FORMAT_R16G16_TYPELESS:
    case DXGI_FORMAT_R16G16_FLOAT:
    case DXGI_FORMAT_R16G16_UNORM:
    case DXGI_FORMAT_R16G16_UINT:
    case DXGI_FORMAT_R16G16_SNORM:
    case DXGI_FORMAT_R16G16_SINT:
    case DXGI_FORMAT_R8G8_TYPELESS:
    case DXGI_FORMAT_R8G8_UNORM:
    case DXGI_FORMAT_R8G8_UINT:
    case DXGI_FORMAT_R8G8_SNORM:
    case DXGI_FORMAT_R8G8_SINT:
        return 2;

    case DXGI_FORMAT_R32G8X24_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
    case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
    case DXGI_FORMAT_X32_TYPELESS_G8X24_UINT:
    case DXGI_FORMAT_R32_TYPELESS:
    case DXGI_FORMAT_D32_FLOAT:
    case DXGI_FORMAT_R32_FLOAT:
    case DXGI_FORMAT_R32_UINT:
    case DXGI_FORMAT_R32_SINT:
    case DXGI_FORMAT_R16_TYPELESS:
    case DXGI_FORMAT_R16_FLOAT:
    case DXGI_FORMAT_D16_UNORM:
    case DXGI_FORMAT_R16_UNORM:
    case DXGI_FORMAT_R16_UINT:
    case DXGI_FORMAT_R16_SNORM:
    case DXGI_FORMAT_R16_SINT:
    case DXGI_FORMAT_R8_TYPELESS:
    case DXGI_FORMAT_R8_UNORM:
    case DXGI_FORMAT_R8_UINT:
    case DXGI_FORMAT_R8_SNORM:
    case DXGI_FORMAT_R8_SINT:
    case DXGI_FORMAT_A8_UNORM:
    case DXGI_FORMAT_R1_UNORM:
        return 1;
    
    default:
        return 0;
    }
}

u32 CD3D11Driver::getIndexAmount(scene::E_PRIMITIVE_TYPE primType, u32 primitiveCount) const
{
    switch( primType )
    {
        case scene::EPT_LINE_STRIP:
            return primitiveCount * 2;

        case scene::EPT_LINE_LOOP:
        case scene::EPT_LINES:
            return primitiveCount * 2;

        case scene::EPT_TRIANGLE_STRIP:
            return primitiveCount + 2;

        case scene::EPT_TRIANGLES:
            return primitiveCount * 3;

        default:
            return 0;
    }
}

D3D11_TEXTURE_ADDRESS_MODE CD3D11Driver::getTextureWrapMode(const u8 clamp)
{
    switch (clamp)
    {
        case ETC_REPEAT:
            return D3D11_TEXTURE_ADDRESS_WRAP;
        case ETC_CLAMP:
        case ETC_CLAMP_TO_EDGE:
            return D3D11_TEXTURE_ADDRESS_CLAMP;
        case ETC_MIRROR:
            return D3D11_TEXTURE_ADDRESS_MIRROR;
        case ETC_CLAMP_TO_BORDER:
            return D3D11_TEXTURE_ADDRESS_BORDER;
        case ETC_MIRROR_CLAMP:
        case ETC_MIRROR_CLAMP_TO_EDGE:
        case ETC_MIRROR_CLAMP_TO_BORDER:
            return D3D11_TEXTURE_ADDRESS_MIRROR_ONCE;
        default:
            return D3D11_TEXTURE_ADDRESS_WRAP;
    }
}

D3D11_COMPARISON_FUNC CD3D11Driver::getDepthFunction(E_COMPARISON_FUNC func) const
{
    switch (func)
    {
    case ECFN_LESSEQUAL:
        return D3D11_COMPARISON_LESS_EQUAL;
    case ECFN_EQUAL:
        return D3D11_COMPARISON_EQUAL;
    case ECFN_LESS:
        return D3D11_COMPARISON_LESS;
    case ECFN_NOTEQUAL:
        return D3D11_COMPARISON_NOT_EQUAL;
    case ECFN_GREATEREQUAL:
        return D3D11_COMPARISON_GREATER_EQUAL;
    case ECFN_GREATER:
        return D3D11_COMPARISON_GREATER;
    case ECFN_ALWAYS:
        return D3D11_COMPARISON_ALWAYS;
    case ECFN_NEVER:
    default:
        return D3D11_COMPARISON_NEVER;
    }
}

D3D11_COLOR_WRITE_ENABLE CD3D11Driver::getColorWriteEnable(E_COLOR_PLANE plane) const
{
    return (D3D11_COLOR_WRITE_ENABLE)
            (
                ( (plane & ECP_RED) ?     D3D11_COLOR_WRITE_ENABLE_RED : 0)         |
                ( (plane & ECP_GREEN) ?     D3D11_COLOR_WRITE_ENABLE_GREEN : 0)     |
                ( (plane & ECP_BLUE) ?     D3D11_COLOR_WRITE_ENABLE_BLUE : 0)         |
                ( (plane & ECP_ALPHA) ?     D3D11_COLOR_WRITE_ENABLE_ALPHA : 0)
            );
}

DXGI_FORMAT CD3D11Driver::getIndexType(E_INDEX_TYPE iType) const
{
    return (iType == video::EIT_16BIT) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
}

core::array<D3D11_INPUT_ELEMENT_DESC> const& CD3D11Driver::getVertexDeclaration(E_VERTEX_TYPE vType) const
{
    return VertexDeclarationMap[vType % EVT_MAX_VERTEX_TYPE];
}

u32 CD3D11Driver::getVertexDeclarationStride(E_VERTEX_TYPE vType, u32 inputSlot) const
{
    core::array<D3D11_INPUT_ELEMENT_DESC> const& vDecl = getVertexDeclaration(vType);
    u32 stride = 0;
    for ( u32 i = 0; i != vDecl.size(); ++i )
    {
        if (vDecl[i].InputSlot == inputSlot)
            stride += getBitsPerPixel(vDecl[i].Format) / 8;
    }
    return stride;
}

bool CD3D11Driver::querySupportForColorFormat(DXGI_FORMAT format, D3D11_FORMAT_SUPPORT support)
{
    UINT values = 0;
    if( FAILED( Device->CheckFormatSupport(format, &values) ) )
        return false;

    if( values && support )
        return true;

    return false;
}

void CD3D11Driver::checkDepthBuffer(ITexture* tex)
{
    if (!tex)
        return;

    const core::dimension2du optSize = tex->getSize().getOptimalSize(
            !queryFeature(EVDF_TEXTURE_NPOT),
            !queryFeature(EVDF_TEXTURE_NSQUARE), true);
    SDepthSurface11* depth=0;
    core::dimension2du destSize(0x7fffffff, 0x7fffffff);
    for (u32 i=0; i<DepthBuffers.size(); ++i)
    {
        if ((DepthBuffers[i]->Size.Width>=optSize.Width) &&
            (DepthBuffers[i]->Size.Height>=optSize.Height))
        {
            if ((DepthBuffers[i]->Size.Width<destSize.Width) &&
                (DepthBuffers[i]->Size.Height<destSize.Height))
            {
                depth = DepthBuffers[i];
                destSize=DepthBuffers[i]->Size;
            }
        }
    }
    
    // Create a depth buffer for this texture
    if (!depth)
    {
        // create depth buffer
        DepthBuffers.push_back(new SDepthSurface11());
        DepthBuffers.getLast()->Surface = createDepthStencilView(optSize);
        if ( DepthBuffers.getLast()->Surface )
        {
            depth=DepthBuffers.getLast();
            depth->Size.set(optSize.Width, optSize.Height);
        }
        else
        {
            char buffer[128];
            sprintf(buffer,"Could not create DepthBuffer of %ix%i",optSize.Width,optSize.Height);
            os::Printer::log(buffer,ELL_ERROR);
            DepthBuffers.erase(DepthBuffers.size()-1);
        }
    }
    else
        depth->grab();

    static_cast<CD3D11Texture*>(tex)->DepthSurface=depth;
}

void CD3D11Driver::removeDepthSurface(SDepthSurface11* depth)
{
    for (u32 i=0; i<DepthBuffers.size(); ++i)
    {
        if (DepthBuffers[i]==depth)
        {
            DepthBuffers.erase(i);
            return;
        }
    }
}

bool CD3D11Driver::uploadVertexData(const void* vertices, u32 vertexCount,
                                    const void* indexList, u32 indexCount,
                                    E_VERTEX_TYPE vType, E_INDEX_TYPE iType)
{
    // Parse information about buffers
    const u32 vertexStride = getVertexPitchFromType(vType);
    const u32 indexStride = video::getIndexSize(iType); // == video::EIT_16BIT ? 2 : 4;
    const DXGI_FORMAT indexFormat = iType == video::EIT_16BIT ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
    const UINT offset = 0;

    // reallocated if needed
    if( !reallocateDynamicBuffers( vertexCount * vertexStride, indexCount * indexStride ) )
        return false;

    HRESULT hr = S_OK;
    if(vertices)
    {        
        D3D11_MAPPED_SUBRESOURCE mappedData;
        ZeroMemory(&mappedData, sizeof(D3D11_MAPPED_SUBRESOURCE));

        hr = ImmediateContext->Map( DynVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData );
        if(SUCCEEDED(hr))
        {
            memcpy(mappedData.pData, vertices, vertexCount * vertexStride);
            ImmediateContext->Unmap( DynVertexBuffer, 0 );
        }
        else
        {
            os::Printer::log("Error, could not upload dynamic vertex data to GPU", ELL_ERROR);
            return false;
        }

        // set vertex buffer
        ImmediateContext->IASetVertexBuffers( 0, 1, &DynVertexBuffer, &vertexStride, &offset );
    }
    if(indexList)
    {
        D3D11_MAPPED_SUBRESOURCE mappedData;
        ZeroMemory(&mappedData, sizeof(D3D11_MAPPED_SUBRESOURCE));

        hr = ImmediateContext->Map( DynIndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData );
        if(SUCCEEDED(hr))
        {
            memcpy(mappedData.pData, indexList, indexCount * indexStride);
            ImmediateContext->Unmap( DynIndexBuffer, 0 );
        }
        else
        {
            os::Printer::log("Error, could not upload dynamic index data to GPU", ELL_ERROR);
            return false;
        }

        // set index buffer
        ImmediateContext->IASetIndexBuffer( DynIndexBuffer, indexFormat, 0 );
    }

    return true;
}

bool CD3D11Driver::reallocateDynamicBuffers( u32 vertexBufferSize, u32 indexBufferSize )
{
    HRESULT hr = S_OK;

    if( !DynVertexBuffer || DynVertexBufferSize < vertexBufferSize )
    {
        D3D11_BUFFER_DESC desc;
        ::ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

        // Release old buffer if small
        if( DynVertexBufferSize < vertexBufferSize )
            SAFE_RELEASE(DynVertexBuffer);

        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.ByteWidth = vertexBufferSize;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0;
        hr = Device->CreateBuffer( &desc, NULL, &DynVertexBuffer);
        if(FAILED(hr))
        {
            os::Printer::log("Error, failed to create dynamic vertex buffer", ELL_ERROR);
            return false;
        }

        DynVertexBufferSize = vertexBufferSize;
    }

    if( !DynIndexBuffer || DynIndexBufferSize < indexBufferSize )
    {
        D3D11_BUFFER_DESC desc;
        ::ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

        // Release old buffer if small
        if( DynIndexBufferSize < indexBufferSize ) 
            SAFE_RELEASE(DynIndexBuffer);

        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.ByteWidth = indexBufferSize;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0;
        hr = Device->CreateBuffer( &desc, NULL, &DynIndexBuffer);
        if(FAILED(hr))
        {
            os::Printer::log("Error, failed to create dynamic index buffer", ELL_ERROR);
            return false;
        }

        DynIndexBufferSize = indexBufferSize;
    }

    return true;
}

ID3D11DepthStencilView* CD3D11Driver::createDepthStencilView(core::dimension2d<u32> size)
{
    // check stencil buffer format
    if( DepthStencilFormat == DXGI_FORMAT_UNKNOWN )
    {
        DepthStencilFormat = DXGI_FORMAT_D32_FLOAT_S8X24_UINT;
        UINT formatSupport = 0;
        if( Params.Stencilbuffer )
        {
            Device->CheckFormatSupport( DepthStencilFormat, &formatSupport );
            if( ( formatSupport && D3D11_FORMAT_SUPPORT_DEPTH_STENCIL ) == 0 )
            {
                DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
                Device->CheckFormatSupport( DepthStencilFormat, &formatSupport );
                if( ( formatSupport && D3D11_FORMAT_SUPPORT_DEPTH_STENCIL ) == 0 )
                {
                    os::Printer::log("Device does not support stencilbuffer, disabling stencil buffer.", ELL_WARNING);
                    Params.Stencilbuffer = false;
                }
            }        
        }
        if (!Params.Stencilbuffer)    // do not use else here to cope with flag change in previous block
        {
            DepthStencilFormat = DXGI_FORMAT_D32_FLOAT;
            Device->CheckFormatSupport( DepthStencilFormat, &formatSupport );
            if( ( formatSupport && D3D11_FORMAT_SUPPORT_DEPTH_STENCIL ) == 0 )
            {
                DepthStencilFormat = DXGI_FORMAT_D16_UNORM;
                Device->CheckFormatSupport( DepthStencilFormat, &formatSupport );
                if( ( formatSupport && D3D11_FORMAT_SUPPORT_DEPTH_STENCIL ) == 0 )
                {
                    os::Printer::log("Device does not support required depth buffer.", ELL_WARNING);
                    return NULL;
                }
            }
        }
    }

    // create depth buffer
    ID3D11DepthStencilView* dsView = NULL;
    ID3D11Texture2D* depthTexture = NULL;
    D3D11_TEXTURE2D_DESC dsTexDesc;
    ::ZeroMemory( &dsTexDesc, sizeof( dsTexDesc ) );
    dsTexDesc.ArraySize = 1;
    dsTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    //// If we tell we want to use it as a Shader Resource when in MSAA, we will fail
    //// This is a recomandation from NVidia.
    if (FeatureLevel >= D3D_FEATURE_LEVEL_10_0 && present.SampleDesc.Count == 1)
        dsTexDesc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
    dsTexDesc.CPUAccessFlags = 0;
    if (FeatureLevel < D3D_FEATURE_LEVEL_10_0)
        dsTexDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    else
        dsTexDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    //dsTexDesc.Format = DepthStencilFormat;
    dsTexDesc.MipLevels = 1;
    dsTexDesc.MiscFlags = 0;
    dsTexDesc.Usage = D3D11_USAGE_DEFAULT;
    dsTexDesc.SampleDesc = present.SampleDesc;
    dsTexDesc.Width = (UINT)size.Width;
    dsTexDesc.Height = (UINT)size.Height;

    if (dsTexDesc.ArraySize == 6)
    {
        dsTexDesc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
    }

    HRESULT hr = Device->CreateTexture2D( &dsTexDesc, NULL, &depthTexture );
    if(FAILED(hr))
    {
        os::Printer::log("Error, could not create depth texture.", ELL_WARNING);
        return NULL;
    }

    //
    // Create the View of the texture
    // If MSAA is used, we cannot do this
    //
    //if (FeatureLevel >= D3D_FEATURE_LEVEL_10_0 && present.SampleDesc.Count == 1)
    //{
    //    ID3D11ShaderResourceView* mDSTResView = nullptr;
    //    D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
    //    viewDesc.Format = DXGI_FORMAT_R32_FLOAT;
    //    viewDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
    //    viewDesc.Texture2D.MostDetailedMip = 0;
    //    viewDesc.Texture2D.MipLevels = 1;
    //    SAFE_RELEASE(mDSTResView);
    //    HRESULT hr = Device->CreateShaderResourceView(depthTexture, &viewDesc, &mDSTResView);
    //    if (FAILED(hr))
    //    {
    //    }
    //}

    D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc;
    ::ZeroMemory( &dsDesc, sizeof( dsDesc ) );

    if (FeatureLevel < D3D_FEATURE_LEVEL_10_0)
        dsDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    else
        dsDesc.Format = DXGI_FORMAT_D32_FLOAT;
    //dsDesc.Format = DepthStencilFormat;
    dsDesc.Flags = 0;  /* D3D11_DSV_READ_ONLY_DEPTH | D3D11_DSV_READ_ONLY_STENCIL */;    // TODO: Allows bind depth buffer as depth view AND texture simultaneously.
                                                                                         // TODO: Decide how to expose this feature
    dsDesc.ViewDimension = (present.SampleDesc.Count > 1) ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
    dsDesc.Texture2D.MipSlice = 0;
    hr = Device->CreateDepthStencilView( depthTexture, &dsDesc, &dsView );
    depthTexture->Release();
    if(FAILED(hr))
    {
        os::Printer::log("Error, could not create depth stencil view.", ELL_WARNING);
        return NULL;
    }

    return dsView;
}

void CD3D11Driver::reset()
{
//    u32 i;
    os::Printer::log("Resetting D3D11 device.", ELL_INFORMATION);

    ImmediateContext->OMSetRenderTargets(0, 0, 0);
    
    HRESULT hr = S_OK;

    for (auto iTex : Textures)
    {
        if ( iTex.second->isRenderTarget())
        {
            ID3D11Resource* tex = ((CD3D11Texture*)(iTex.second))->getTextureResource();
            if (tex)
                tex->Release();
        }
    }

    // clear textures
    for (u16 i = 0; i < MATERIAL_MAX_TEXTURES; i++)
        setActiveTexture(i, 0);

    // unbind render targets
    ID3D11RenderTargetView* views[] = { NULL };
    ImmediateContext->OMSetRenderTargets(1, views, NULL);
    activeGPUProgram(nullptr);

    // Unbound all shader resources
    ID3D11ShaderResourceView* tviews[1] = { NULL };
    ImmediateContext->VSSetShaderResources(0, 1, tviews);
    ImmediateContext->HSSetShaderResources(0, 1, tviews);
    ImmediateContext->DSSetShaderResources(0, 1, tviews);
    ImmediateContext->GSSetShaderResources(0, 1, tviews);
    ImmediateContext->PSSetShaderResources(0, 1, tviews);

    SAFE_RELEASE(DefaultBackBuffer);
    SAFE_RELEASE(DefaultDepthBuffer);

    // If fullscreen, do it
    if (Params.Fullscreen)
    {
        SwapChain->SetFullscreenState(TRUE, Output);
    }
    else
    {
        SwapChain->SetFullscreenState(FALSE, NULL);
    }

    // Preserve the existing buffer count and format.
    // Automatically choose the width and height to match the client rect for HWNDs.
    hr = SwapChain->ResizeBuffers(2, 0, 0, DXGI_FORMAT_B8G8R8A8_UNORM, 0);
    if(FAILED(hr))
    {
        os::Printer::log("Error resizing back buffer", ELL_ERROR);
        return;
    }


    ID3D11Texture2D* backBuffer = NULL;
    hr = SwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), reinterpret_cast<void**>( &backBuffer ) );
    if( FAILED(hr))
    {
        os::Printer::log("Error, could not get back buffer.", ELL_ERROR);
        return;
    }

    // Get default render target
    //D2DContext->SetDpi(0, 0);
    //D2DContext->SetTarget(nullptr);
    //
    //D2DContext->GetDpi(&m_DPI.X, &m_DPI.Y);

    hr = Device->CreateRenderTargetView( backBuffer, NULL, &DefaultBackBuffer );
    if( FAILED(hr))
    {
        os::Printer::log("Error, could not create render target view.", ELL_ERROR);
        return;
    }
    SAFE_RELEASE(backBuffer);

    // create depth buffer
    DefaultDepthBuffer = createDepthStencilView(ScreenSize);

    // Set render targets
    setRenderTarget(0, true, true);

    CurrentDepthBuffer = nullptr;
    CurrentBackBuffer = nullptr;

    //IDXGISurface* dxgiBackBuffer;
    //DX::ThrowIfFailed(
    //    SwapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer))
    //    );
    //
    ////// Create a Direct2D target bitmap associated with the
    //// swap chain back buffer and set it as the current target.
    //D2D1_BITMAP_PROPERTIES1 bitmapProperties =
    //    D2D1::BitmapProperties1(
    //        D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
    //        D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED)
    //        , m_DPI.X, m_DPI.Y
    //        );
    //
    //DX::ThrowIfFailed(
    //    D2DContext->CreateBitmapFromDxgiSurface(
    //        dxgiBackBuffer,
    //        bitmapProperties,
    //        &D2DTargetBitmap
    //        )
    //    );
    //
    //D2DContext->SetTarget(D2DTargetBitmap);
    //
    //// Grayscale text anti-aliasing is recommended for all Windows Store apps.
    //D2DContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

    for (u32 i = 0; i<MATERIAL_MAX_TEXTURES; ++i)
        CurrentTexture[i] = 0;

    // init states description
    BlendDesc.reset();
    RasterizerDesc.reset();
    DepthStencilDesc.reset();
    for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; i++)
        SamplerDesc[i].reset();

    // set fog mode
    setFog(FogColor, (E_FOG_TYPE)0, FogStart, FogEnd, FogDensity, PixelFog, RangeFog);

    ResetRenderStates = true;

    setRenderStates2DMode(true, false, true);
    setRenderStates3DMode(EVT_STANDARD);
    setFog(FogColor, FogType, FogStart, FogEnd, FogDensity, PixelFog, RangeFog);
    setAmbientLight(AmbientLight);
}

// returns the current size of the screen or rendertarget
const core::dimension2d<u32>& CD3D11Driver::getCurrentRenderTargetSize() const
{
    if ( CurrentRendertargetSize.Width == 0 )
        return ScreenSize;
    else
        return CurrentRendertargetSize;
}

////////////// IGPUProgrammingServices methods start ////////////////////////////////////////////

//s32 CD3D11Driver::addHighLevelShaderMaterial(
//            const c8* vertexShaderProgram,
//            const c8* vertexShaderEntryPointName,
//            E_VERTEX_SHADER_TYPE vsCompileTarget,
//            const c8* pixelShaderProgram,
//            const c8* pixelShaderEntryPointName,
//            E_PIXEL_SHADER_TYPE psCompileTarget,
//            const c8* geometryShaderProgram,
//            const c8* geometryShaderEntryPointName,
//            E_GEOMETRY_SHADER_TYPE gsCompileTarget,
//            scene::E_PRIMITIVE_TYPE inType,
//            scene::E_PRIMITIVE_TYPE outType,
//            u32 verticesOut,
//            E_VERTEX_TYPE vertexTypeOut,
//            IShaderConstantSetCallBack* callback,
//            E_MATERIAL_TYPE baseMaterial,
//            s32 userData)
//{
//    s32 id = 0;
//    //CD3D11CustomMaterialRenderer* rend = 
//    //        new CD3D11CustomMaterialRenderer(Device, this,
//    //                                         id,
//    //                                         vertexShaderProgram,
//    //                                         vertexShaderEntryPointName,
//    //                                         vsCompileTarget,
//    //                                         pixelShaderProgram,
//    //                                         pixelShaderEntryPointName,
//    //                                         psCompileTarget,
//    //                                         geometryShaderProgram,
//    //                                         geometryShaderEntryPointName,
//    //                                         gsCompileTarget,
//    //                                         inType,
//    //                                         outType,
//    //                                         verticesOut,
//    //                                         vertexTypeOut,
//    //                                         callback,
//    //                                         MaterialRenderers[baseMaterial].Renderer,
//    //                                         userData);
//    return id;
//}

////////////// IGPUProgrammingServices methods end ////////////////////////////////////////////

////////////// IMaterialRenderer methods start ////////////////////////////////////////////

void CD3D11Driver::setVertexShaderConstant(const f32* data, s32 startRegister, s32 constantAmount)
{
    os::Printer::log("\"setVertexShaderConstant\" with offset is not supported", ELL_ERROR);
}

void CD3D11Driver::setPixelShaderConstant(const f32* data, s32 startRegister, s32 constantAmount)
{
    os::Printer::log("\"setPixelShaderConstant\" with offset is not supported", ELL_ERROR);
}

bool CD3D11Driver::setVertexShaderConstant(const c8* name, const f32* floats, int count)
{
    static_cast<CD3D11MaterialRenderer*>(MaterialRenderers[Material.MaterialType].Renderer)->setVariable(name, floats, count);
    return true;
}

bool CD3D11Driver::setPixelShaderConstant(const c8* name, const f32* floats, int count)
{
    static_cast<CD3D11MaterialRenderer*>(MaterialRenderers[Material.MaterialType].Renderer)->setVariable(name, floats, count);
    return true;
}

bool CD3D11Driver::setStreamOutputBuffer(IHardwareBuffer* buffer)
{
    ID3D11Buffer* buffers = 0;
    UINT offset = 0;

    // If buffer is null, remove from pipeline
    if (!buffer)
    {
        ImmediateContext->SOSetTargets(1, &buffers, &offset);
        return true;
    }

    // Validate buffer
    if (buffer->getDriverType() != EDT_DIRECT3D11)
    {
        os::Printer::log("Fatal Error: Tried to set a buffer not owned by this driver.", ELL_ERROR);
        return false;
    }
    if (buffer->getType(E_HARDWARE_BUFFER_TYPE::EHBT_STREAM_OUTPUT) != E_HARDWARE_BUFFER_TYPE::EHBT_STREAM_OUTPUT)
    {
        os::Printer::log("Fatal Error: Tried to set a buffer that is not for stream output.", ELL_ERROR);
        return false;
    }

    // Set stream output buffer
    buffers = static_cast<CD3D11HardwareBuffer*>(buffer)->getBufferResource(E_HARDWARE_BUFFER_TYPE::EHBT_STREAM_OUTPUT);
    ImmediateContext->SOSetTargets(1, &buffers, &offset);

    return true;
}

IVideoDriver* CD3D11Driver::getVideoDriver()
{
    return this;
}

////////////// IMaterialRenderer methods end ////////////////////////////////////////////

} // end namespace video
} // end namespace irr

#endif


namespace irr
{
namespace video
{

#ifdef _IRR_COMPILE_WITH_DIRECT3D_11_
//! creates a video driver
IVideoDriver* createDirectX11Driver(const SIrrlichtCreationParameters& params,
    io::IFileSystem* io, HWND hwnd)
{
    bool pureSoftware = false;
    CD3D11Driver* dx11 = new CD3D11Driver(params, io, pureSoftware);
    if (!dx11->initDriver(hwnd, pureSoftware))
    {
        dx11->drop();
        dx11 = 0;
    }

    return dx11;
}
#endif // _IRR_COMPILE_WITH_DIRECT3D_11_

} // end namespace video
} // end namespace irr
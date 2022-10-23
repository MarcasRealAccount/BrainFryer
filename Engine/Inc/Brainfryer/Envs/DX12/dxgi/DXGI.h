#pragma once

#include "Brainfryer/Envs/DX12/ComHelper.h"
#include "Brainfryer/Envs/Windows/Types.h"
#include "Brainfryer/Utils/Core.h"
#include "DXGIFormat.h"

namespace Brainfryer::DX12
{
	using namespace Brainfryer::Windows::Types;
	extern "C"
	{
		static constexpr UINT DXGI_CPU_ACCESS_NONE       = 0;
		static constexpr UINT DXGI_CPU_ACCESS_DYNAMIC    = 1;
		static constexpr UINT DXGI_CPU_ACCESS_READ_WRITE = 2;
		static constexpr UINT DXGI_CPU_ACCESS_SCRATCH    = 3;
		static constexpr UINT DXGI_CPU_ACCESS_FIELD      = 15;

		struct DXGI_JPEG_DC_HUFFMAN_TABLE
		{
			BYTE CodeCounts[12];
			BYTE CodeValues[12];
		};

		struct DXGI_JPEG_AC_HUFFMAN_TABLE
		{
			BYTE CodeCounts[16];
			BYTE CodeValues[162];
		};

		struct DXGI_JPEG_QUANTIZATION_TABLE
		{
			BYTE Elements[64];
		};

		struct DXGI_RATIONAL
		{
			UINT Numerator;
			UINT Denominator;
		};

		// The following values are used with DXGI_SAMPLE_DESC::Quality:
		static constexpr UINT DXGI_STANDARD_MULTISAMPLE_QUALITY_PATTERN = 0xffffffff;
		static constexpr UINT DXGI_CENTER_MULTISAMPLE_QUALITY_PATTERN   = 0xfffffffe;

		struct DXGI_SAMPLE_DESC
		{
			UINT Count;
			UINT Quality;
		};

		enum DXGI_COLOR_SPACE_TYPE
		{
			DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P709           = 0,
			DXGI_COLOR_SPACE_RGB_FULL_G10_NONE_P709           = 1,
			DXGI_COLOR_SPACE_RGB_STUDIO_G22_NONE_P709         = 2,
			DXGI_COLOR_SPACE_RGB_STUDIO_G22_NONE_P2020        = 3,
			DXGI_COLOR_SPACE_RESERVED                         = 4,
			DXGI_COLOR_SPACE_YCBCR_FULL_G22_NONE_P709_X601    = 5,
			DXGI_COLOR_SPACE_YCBCR_STUDIO_G22_LEFT_P601       = 6,
			DXGI_COLOR_SPACE_YCBCR_FULL_G22_LEFT_P601         = 7,
			DXGI_COLOR_SPACE_YCBCR_STUDIO_G22_LEFT_P709       = 8,
			DXGI_COLOR_SPACE_YCBCR_FULL_G22_LEFT_P709         = 9,
			DXGI_COLOR_SPACE_YCBCR_STUDIO_G22_LEFT_P2020      = 10,
			DXGI_COLOR_SPACE_YCBCR_FULL_G22_LEFT_P2020        = 11,
			DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020        = 12,
			DXGI_COLOR_SPACE_YCBCR_STUDIO_G2084_LEFT_P2020    = 13,
			DXGI_COLOR_SPACE_RGB_STUDIO_G2084_NONE_P2020      = 14,
			DXGI_COLOR_SPACE_YCBCR_STUDIO_G22_TOPLEFT_P2020   = 15,
			DXGI_COLOR_SPACE_YCBCR_STUDIO_G2084_TOPLEFT_P2020 = 16,
			DXGI_COLOR_SPACE_RGB_FULL_G22_NONE_P2020          = 17,
			DXGI_COLOR_SPACE_YCBCR_STUDIO_GHLG_TOPLEFT_P2020  = 18,
			DXGI_COLOR_SPACE_YCBCR_FULL_GHLG_TOPLEFT_P2020    = 19,
			DXGI_COLOR_SPACE_RGB_STUDIO_G24_NONE_P709         = 20,
			DXGI_COLOR_SPACE_RGB_STUDIO_G24_NONE_P2020        = 21,
			DXGI_COLOR_SPACE_YCBCR_STUDIO_G24_LEFT_P709       = 22,
			DXGI_COLOR_SPACE_YCBCR_STUDIO_G24_LEFT_P2020      = 23,
			DXGI_COLOR_SPACE_YCBCR_STUDIO_G24_TOPLEFT_P2020   = 24,
			DXGI_COLOR_SPACE_CUSTOM                           = 0xFFFFFFFF
		};

		struct DXGI_RGB
		{
			float Red;
			float Green;
			float Blue;
		};

		using D3DCOLORVALUE = struct _D3DCOLORVALUE
		{
			float r;
			float g;
			float b;
			float a;
		};

		using DXGI_RGBA = D3DCOLORVALUE;

		struct DXGI_GAMMA_CONTROL
		{
			DXGI_RGB Scale;
			DXGI_RGB Offset;
			DXGI_RGB GammaCurve[1025];
		};

		struct DXGI_GAMMA_CONTROL_CAPABILITIES
		{
			BOOL  ScaleAndOffsetSupported;
			float MaxConvertedValue;
			float MinConvertedValue;
			UINT  NumGammaControlPoints;
			float ControlPointPositions[1025];
		};

		enum DXGI_MODE_SCANLINE_ORDER
		{
			DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED       = 0,
			DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE       = 1,
			DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST = 2,
			DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST = 3
		};

		enum DXGI_MODE_SCALING
		{
			DXGI_MODE_SCALING_UNSPECIFIED = 0,
			DXGI_MODE_SCALING_CENTERED    = 1,
			DXGI_MODE_SCALING_STRETCHED   = 2
		};

		enum DXGI_MODE_ROTATION
		{
			DXGI_MODE_ROTATION_UNSPECIFIED = 0,
			DXGI_MODE_ROTATION_IDENTITY    = 1,
			DXGI_MODE_ROTATION_ROTATE90    = 2,
			DXGI_MODE_ROTATION_ROTATE180   = 3,
			DXGI_MODE_ROTATION_ROTATE270   = 4
		};

		struct DXGI_MODE_DESC
		{
			UINT                     Width;
			UINT                     Height;
			DXGI_RATIONAL            RefreshRate;
			DXGI_FORMAT              Format;
			DXGI_MODE_SCANLINE_ORDER ScanlineOrdering;
			DXGI_MODE_SCALING        Scaling;
		};

		using DXGI_USAGE                                            = UINT;
		static constexpr DXGI_USAGE DXGI_USAGE_SHADER_INPUT         = 0x010;
		static constexpr DXGI_USAGE DXGI_USAGE_RENDER_TARGET_OUTPUT = 0x020;
		static constexpr DXGI_USAGE DXGI_USAGE_BACK_BUFFER          = 0x040;
		static constexpr DXGI_USAGE DXGI_USAGE_SHARED               = 0x080;
		static constexpr DXGI_USAGE DXGI_USAGE_READ_ONLY            = 0x100;
		static constexpr DXGI_USAGE DXGI_USAGE_DISCARD_ON_PRESENT   = 0x200;
		static constexpr DXGI_USAGE DXGI_USAGE_UNORDERED_ACCESS     = 0x400;

		struct DXGI_FRAME_STATISTICS
		{
			UINT          PresentCount;
			UINT          PresentRefreshCount;
			UINT          SyncRefreshCount;
			LARGE_INTEGER SyncQPCTime;
			LARGE_INTEGER SyncGPUTime;
		};

		struct DXGI_MAPPED_RECT
		{
			INT   Pitch;
			BYTE* pBits;
		};

		struct DXGI_ADAPTER_DESC
		{
			wchar_t Description[128];
			UINT    VendorId;
			UINT    DeviceId;
			UINT    SubSysId;
			UINT    Revision;
			SIZE_T  DedicatedVideoMemory;
			SIZE_T  DedicatedSystemMemory;
			SIZE_T  SharedSystemMemory;
			LUID    AdapterLuid;
		};

		struct DXGI_OUTPUT_DESC
		{
			wchar_t            DeviceName[32];
			RECT               DesktopCoordinates;
			BOOL               AttachedToDesktop;
			DXGI_MODE_ROTATION Rotation;
			HMONITOR           Monitor;
		};

		struct DXGI_SHARED_RESOURCE
		{
			HANDLE Handle;
		};

		static constexpr UINT DXGI_RESOURCE_PRIORITY_MINIMUM  = 0x28000000;
		static constexpr UINT DXGI_RESOURCE_PRIORITY_LOW      = 0x50000000;
		static constexpr UINT DXGI_RESOURCE_PRIORITY_NORMAL   = 0x78000000;
		static constexpr UINT DXGI_RESOURCE_PRIORITY_HIGH     = 0xa0000000;
		static constexpr UINT DXGI_RESOURCE_PRIORITY_MAXMIMUM = 0xc8000000;

		enum DXGI_RESIDENCY
		{
			DXGI_RESIDENCY_FULLY_RESIDENT            = 1,
			DXGI_RESIDENCY_RESIDENT_IN_SHARED_MEMORY = 2,
			DXGI_RESIDENCY_EVICTED_TO_DISK           = 3
		};

		struct DXGI_SURFACE_DESC
		{
			UINT             Width;
			UINT             Height;
			DXGI_FORMAT      Format;
			DXGI_SAMPLE_DESC SampleDesc;
		};

		enum DXGI_SWAP_EFFECT
		{
			DXGI_SWAP_EFFECT_DISCARD         = 0,
			DXGI_SWAP_EFFECT_SEQUENTIAL      = 1,
			DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL = 3,
			DXGI_SWAP_EFFECT_FLIP_DISCARD    = 4
		};

		enum DXGI_SWAP_CHAIN_FLAG
		{
			DXGI_SWAP_CHAIN_FLAG_NONPREROTATED                          = 1,
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH                      = 2,
			DXGI_SWAP_CHAIN_FLAG_GDI_COMPATIBLE                         = 4,
			DXGI_SWAP_CHAIN_FLAG_RESTRICTED_CONTENT                     = 8,
			DXGI_SWAP_CHAIN_FLAG_RESTRICT_SHARED_RESOURCE_DRIVER        = 16,
			DXGI_SWAP_CHAIN_FLAG_DISPLAY_ONLY                           = 32,
			DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT          = 64,
			DXGI_SWAP_CHAIN_FLAG_FOREGROUND_LAYER                       = 128,
			DXGI_SWAP_CHAIN_FLAG_FULLSCREEN_VIDEO                       = 256,
			DXGI_SWAP_CHAIN_FLAG_YUV_VIDEO                              = 512,
			DXGI_SWAP_CHAIN_FLAG_HW_PROTECTED                           = 1024,
			DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING                          = 2048,
			DXGI_SWAP_CHAIN_FLAG_RESTRICTED_TO_ALL_HOLOGRAPHIC_DISPLAYS = 4096
		};

		struct DXGI_SWAP_CHAIN_DESC
		{
			DXGI_MODE_DESC   BufferDesc;
			DXGI_SAMPLE_DESC SampleDesc;
			DXGI_USAGE       BufferUsage;
			UINT             BufferCount;
			HWND             OutputWindow;
			BOOL             Windowed;
			DXGI_SWAP_EFFECT SwapEffect;
			UINT             Flags;
		};

		MIDL_INTERFACE("aec22fb8-76f3-4639-9be0-28eb43a67a2e")
		IDXGIObject : public IUnknown
		{
		public:
			virtual HRESULT SetPrivateData(const GUID& Name, UINT DataSize, const void* pData)  = 0;
			virtual HRESULT SetPrivateDataInterface(const GUID& Name, const IUnknown* pUnknown) = 0;
			virtual HRESULT GetPrivateData(const GUID& Name, UINT* pDataSize, void* pData)      = 0;

			virtual HRESULT GetParent(const GUID& riid, void** ppParent) = 0;
		};

		MIDL_INTERFACE("3d3e0379-f9de-4d58-bb6c-18d62992f1a6")
		IDXGIDeviceSubObject : public IDXGIObject
		{
		public:
			virtual HRESULT GetDevice(const GUID& riid, void** ppDevice) = 0;
		};

		MIDL_INTERFACE("035f3ab4-482e-4e50-b41f-8a7f8bd8960b")
		IDXGIResource : public IDXGIDeviceSubObject
		{
		public:
			virtual HRESULT GetSharedHandle(HANDLE * pSharedHandle) = 0;

			virtual HRESULT GetUsage(DXGI_USAGE * pUsage) = 0;

			virtual HRESULT SetEvictionPriority(UINT EvictionPriority)    = 0;
			virtual HRESULT GetEvictionPriority(UINT * pEvictionPriority) = 0;
		};

		MIDL_INTERFACE("9d8e1289-d7b3-465f-8126-250e349af85d")
		IDXGIKeyedMutex : public IDXGIDeviceSubObject
		{
		public:
			virtual HRESULT AcquireSync(std::uint64_t Key, DWORD dwMilliseconds) = 0;

			virtual HRESULT ReleaseSync(std::uint64_t Key) = 0;
		};

		static constexpr UINT DXGI_MAP_READ    = 1;
		static constexpr UINT DXGI_MAP_WRITE   = 2;
		static constexpr UINT DXGI_MAP_DISCARD = 4;

		MIDL_INTERFACE("cafcb56c-6ac3-4889-bf47-9e23bbd260ec")
		IDXGISurface : public IDXGIDeviceSubObject
		{
		public:
			virtual HRESULT GetDesc(DXGI_SURFACE_DESC * pDesc) = 0;

			virtual HRESULT Map(DXGI_MAPPED_RECT * pLockedRect, UINT MapFlags) = 0;
			virtual HRESULT Unmap()                                            = 0;
		};

		MIDL_INTERFACE("4AE63092-6327-4c1b-80AE-BFE12EA32B86")
		IDXGISurface1 : public IDXGISurface
		{
		public:
			virtual HRESULT GetDC(BOOL Discard, HDC* phdc) = 0;

			virtual HRESULT ReleaseDC(RECT * pDirtyRect) = 0;
		};

		MIDL_INTERFACE("2411e7e1-12ac-4ccf-bd14-9798e8534dc0")
		IDXGIAdapter : public IDXGIObject
		{
		public:
			virtual HRESULT EnumOutputs(UINT Output, struct IDXGIOutput * *ppOutput) = 0;

			virtual HRESULT GetDesc(DXGI_ADAPTER_DESC * pDesc) = 0;

			virtual HRESULT CheckInterfaceSupport(const GUID& InterfaceName, LARGE_INTEGER* pUMDVersion) = 0;
		};

		static constexpr UINT DXGI_ENUM_MODES_INTERLACED = 1;
		static constexpr UINT DXGI_ENUM_MODES_SCALING    = 2;

		MIDL_INTERFACE("ae02eedb-c735-4690-8d52-5a8dc20213aa")
		IDXGIOutput : public IDXGIObject
		{
		public:
			virtual HRESULT GetDesc(DXGI_OUTPUT_DESC * pDesc) = 0;

			virtual HRESULT GetDisplayModeList(DXGI_FORMAT EnumFormat, UINT Flags, UINT * pNumModes, DXGI_MODE_DESC * pDesc) = 0;

			virtual HRESULT FindClosestMatchingMode(const DXGI_MODE_DESC* pModeToMatch, DXGI_MODE_DESC* pClosestMatch, IUnknown* pConcernedDevice) = 0;

			virtual HRESULT WaitForVBlank() = 0;

			virtual HRESULT TakeOwnership(IUnknown * pDevice, BOOL Exclusive) = 0;
			virtual void    ReleaseOwnership()                                = 0;

			virtual HRESULT GetGammaControlCapabilities(DXGI_GAMMA_CONTROL_CAPABILITIES * pGammaCaps) = 0;
			virtual HRESULT SetGammaControl(const DXGI_GAMMA_CONTROL* pArray)                         = 0;
			virtual HRESULT GetGammaControl(DXGI_GAMMA_CONTROL * pArray)                              = 0;

			virtual HRESULT SetDisplaySurface(IDXGISurface * pScanoutSurface)  = 0;
			virtual HRESULT GetDisplaySurfaceData(IDXGISurface * pDestination) = 0;

			virtual HRESULT GetFrameStatistics(DXGI_FRAME_STATISTICS * pStats) = 0;
		};

		static constexpr UINT DXGI_MAP_SWAP_CHAIN_BUFFERS        = 16;
		static constexpr UINT DXGI_PRESENT_TEST                  = 0x00000001UL;
		static constexpr UINT DXGI_PRESENT_DO_NOT_SEQUENCE       = 0x00000002UL;
		static constexpr UINT DXGI_PRESENT_RESTART               = 0x00000004UL;
		static constexpr UINT DXGI_PRESENT_DO_NOT_WAIT           = 0x00000008UL;
		static constexpr UINT DXGI_PRESENT_STEREO_PREFER_RIGHT   = 0x000000010UL;
		static constexpr UINT DXGI_PRESENT_STEREO_TEMPORARY_MONO = 0x00000020UL;
		static constexpr UINT DXGI_PRESENT_RESTRICT_TO_OUTPUT    = 0x00000040UL;
		static constexpr UINT DXGI_PRESENT_USE_DURATION          = 0x00000100UL;
		static constexpr UINT DXGI_PRESENT_ALLOW_TEARING         = 0x00000200UL;

		MIDL_INTERFACE("310d36a0-d2e7-4c0a-aa04-6a9d23b8886a")
		IDXGISwapChain : public IDXGIDeviceSubObject
		{
		public:
			virtual HRESULT Present(UINT SyncInterval, UINT Flags) = 0;

			virtual HRESULT GetBuffer(UINT Buffer, const GUID& riid, void** ppSurface) = 0;

			virtual HRESULT SetFullscreenState(BOOL Fullscreen, IDXGIOutput* pTarget)     = 0;
			virtual HRESULT GetFullscreenState(BOOL* pFullscreen, IDXGIOutput** ppTarget) = 0;

			virtual HRESULT GetDesc(DXGI_SWAP_CHAIN_DESC * pDesc) = 0;

			virtual HRESULT ResizeBuffers(UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT NewFormat, UINT SwapChainFlags) = 0;

			virtual HRESULT ResizeTarget(const DXGI_MODE_DESC* pNewTargetParameters) = 0;

			virtual HRESULT GetContainingOutput(IDXGIOutput * *ppOutput) = 0;

			virtual HRESULT GetFrameStatistics(DXGI_FRAME_STATISTICS * pStats) = 0;

			virtual HRESULT GetLastPresentCount(UINT * pLastPresentCount) = 0;
		};

		static constexpr UINT DXGI_MWA_NO_WINDOW_CHANGES = 1;
		static constexpr UINT DXGI_MWA_NO_ALT_ENTER      = 2;
		static constexpr UINT DXGI_MWA_NO_PRINT_SCREEN   = 4;
		static constexpr UINT DXGI_MWA_VALID             = 0x7;

		MIDL_INTERFACE("7b7166ec-21c7-44ae-b21a-c9ae321ae369")
		IDXGIFactory : public IDXGIObject
		{
		public:
			virtual HRESULT EnumAdapters(UINT Adapter, IDXGIAdapter * *ppAdapter) = 0;

			virtual HRESULT MakeWindowAssociation(HWND WindowHandle, UINT Flags) = 0;
			virtual HRESULT GetWindowAssociation(HWND * pWindowHandle)           = 0;

			virtual HRESULT CreateSwapChain(IUnknown * pDevice, DXGI_SWAP_CHAIN_DESC * pDesc, IDXGISwapChain * *ppSwapChain) = 0;

			virtual HRESULT CreateSoftwareAdapter(HMODULE Module, IDXGIAdapter * *ppAdapter) = 0;
		};

		HRESULT CreateDXGIFactory(const GUID& riid, void** ppFactory);
		HRESULT CreateDXGIFactory1(const GUID& riid, void** ppFactory);

		MIDL_INTERFACE("54ec77fa-1377-44e6-8c32-88fd5f44c84c")
		IDXGIDevice : public IDXGIObject
		{
		public:
			virtual HRESULT GetAdapter(IDXGIAdapter * *pAdapter) = 0;

			virtual HRESULT CreateSurface(const DXGI_SURFACE_DESC* pDesc, UINT NumSurfaces, DXGI_USAGE Usage, const DXGI_SHARED_RESOURCE* pSharedResource, IDXGISurface** ppSurface) = 0;

			virtual HRESULT QueryResourceResidency(IUnknown* const* ppResources, DXGI_RESIDENCY* pResidencyStatus, UINT NumResources) = 0;

			virtual HRESULT SetGPUThreadPriority(INT Priority)    = 0;
			virtual HRESULT GetGPUThreadPriority(INT * pPriority) = 0;
		};

		enum DXGI_ADAPTER_FLAG
		{
			DXGI_ADAPTER_FLAG_NONE        = 0,
			DXGI_ADAPTER_FLAG_REMOTE      = 1,
			DXGI_ADAPTER_FLAG_SOFTWARE    = 2,
			DXGI_ADAPTER_FLAG_FORCE_DWORD = 0xffffffff
		};

		struct DXGI_ADAPTER_DESC1
		{
			wchar_t Description[128];
			UINT    VendorId;
			UINT    DeviceId;
			UINT    SubSysId;
			UINT    Revision;
			SIZE_T  DedicatedVideoMemory;
			SIZE_T  DedicatedSystemMemory;
			SIZE_T  SharedSystemMemory;
			LUID    AdapterLuid;
			UINT    Flags;
		};

		struct DXGI_DISPLAY_COLOR_SPACE
		{
			FLOAT PrimaryCoordinates[8][2];
			FLOAT WhitePoints[16][2];
		};

		MIDL_INTERFACE("770aae78-f26f-4dba-a829-253c83d1b387")
		IDXGIFactory1 : public IDXGIFactory
		{
		public:
			virtual HRESULT EnumAdapters1(UINT Adapter, struct IDXGIAdapter1 * *ppAdapter) = 0;

			virtual BOOL IsCurrent() = 0;
		};

		MIDL_INTERFACE("29038f61-3839-4626-91fd-086879011a05")
		IDXGIAdapter1 : public IDXGIAdapter
		{
		public:
			virtual HRESULT GetDesc1(DXGI_ADAPTER_DESC1 * pDesc) = 0;
		};

		MIDL_INTERFACE("77db970f-6276-48ba-ba28-070143b4392c")
		IDXGIDevice1 : public IDXGIDevice
		{
		public:
			virtual HRESULT SetMaximumFrameLatency(UINT MaxLatency) = 0;

			virtual HRESULT GetMaximumFrameLatency(UINT * pMaxLatency) = 0;
		};
	}
} // namespace Brainfryer::DX12
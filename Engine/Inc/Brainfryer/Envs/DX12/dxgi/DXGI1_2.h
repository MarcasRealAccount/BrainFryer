#pragma once

#include "DXGI.h"

namespace Brainfryer::DX12
{
	extern "C"
	{
		MIDL_INTERFACE("ea9dbf1a-c88e-4486-854a-98aa0138f30c")
		IDXGIDisplayControl : public IUnknown
		{
		public:
			virtual bool IsStereoEnabled()              = 0;
			virtual void SetStereoEnabled(bool enabled) = 0;
		};

		struct DXGI_OUTDUPL_MOVE_RECT
		{
			POINT SourcePoint;
			RECT  DestinationRect;
		};

		struct DXGI_OUTDUPL_DESC
		{
			DXGI_MODE_DESC     ModeDesc;
			DXGI_MODE_ROTATION Rotation;
			bool               DesktopImageInSystemMemory;
		};

		struct DXGI_OUTDUPL_POINTER_POSITION
		{
			POINT Position;
			bool  Visible;
		};

		enum DXGI_OUTDUPL_POINTER_SHAPE_TYPE
		{
			DXGI_OUTDUPL_POINTER_SHAPE_TYPE_MONOCHROME   = 0x1,
			DXGI_OUTDUPL_POINTER_SHAPE_TYPE_COLOR        = 0x2,
			DXGI_OUTDUPL_POINTER_SHAPE_TYPE_MASKED_COLOR = 0x4
		};

		struct DXGI_OUTDUPL_POINTER_SHAPE_INFO
		{
			UINT  Type;
			UINT  Width;
			UINT  Height;
			UINT  Pitch;
			POINT HotSpot;
		};

		struct DXGI_OUTDUPL_FRAME_INFO
		{
			LARGE_INTEGER                 LastPresentTime;
			LARGE_INTEGER                 LastMouseUpdateTime;
			UINT                          AccumulatedFrames;
			bool                          RectsCoalesced;
			bool                          ProtectedContentMaskedOut;
			DXGI_OUTDUPL_POINTER_POSITION PointerPosition;
			UINT                          TotalMetadataBufferSize;
			UINT                          PointerShapeBufferSize;
		};

		MIDL_INTERFACE("191cfac3-a341-470d-b26e-a864f428319c")
		IDXGIOutputDuplication : public IDXGIObject
		{
		public:
			virtual void GetDesc(DXGI_OUTDUPL_DESC * pDesc) = 0;

			virtual HRESULT AcquireNextFrame(UINT TimeoutInMilliseconds, DXGI_OUTDUPL_FRAME_INFO * pFrameInfo, IDXGIResource * *ppDesktopResource) = 0;

			virtual HRESULT GetFrameDirtyRects(UINT DirtyRectsBufferSize, RECT * pDirtyRectsBuffer, UINT * pDirtyRectsBufferSizeRequired) = 0;

			virtual HRESULT GetFrameMoveRects(UINT MoveRectsBufferSize, DXGI_OUTDUPL_MOVE_RECT * pMoveRectBuffer, UINT * pMoveRectsBufferSizeRequired) = 0;

			virtual HRESULT GetFramePointerShape(UINT PointerShapeBufferSize, void* pPointerShapeBuffer, UINT* pPointerShapeBufferSizeRequired, DXGI_OUTDUPL_POINTER_SHAPE_INFO* pPointerShapeInfo) = 0;

			virtual HRESULT MapDesktopSurface(DXGI_MAPPED_RECT * pLockedRect) = 0;
			virtual HRESULT UnMapDesktopSurface()                             = 0;

			virtual HRESULT ReleaseFrame() = 0;
		};

		enum DXGI_ALPHA_MODE
		{
			DXGI_ALPHA_MODE_UNSPECIFIED   = 0,
			DXGI_ALPHA_MODE_PREMULTIPLIED = 1,
			DXGI_ALPHA_MODE_STRAIGHT      = 2,
			DXGI_ALPHA_MODE_IGNORE        = 3,
			DXGI_ALPHA_MODE_FORCE_DWORD   = 0xffffffff
		};

		MIDL_INTERFACE("aba496dd-b617-4cb8-a866-bc44d7eb1fa2")
		IDXGISurface2 : public IDXGISurface1
		{
		public:
			virtual HRESULT GetResource(const GUID& riid, void** ppParentResource, UINT* pSubresourceIndex) = 0;
		};

		MIDL_INTERFACE("30961379-4609-4a41-998e-54fe567ee0c1")
		IDXGIResource1 : public IDXGIResource
		{
		public:
			virtual HRESULT CreateSubresourceSurface(UINT index, IDXGISurface2 * *ppSurface) = 0;

			virtual HRESULT CreateSharedHandle(const SECURITY_ATTRIBUTES* pAttributes, DWORD dwAccess, LPCWSTR lpName, HANDLE* pHandle) = 0;
		};

		using DXGI_OFFER_RESOURCE_PRIORITY = enum _DXGI_OFFER_RESOURCE_PRIORITY {
			DXGI_OFFER_RESOURCE_PRIORITY_LOW    = 1,
			DXGI_OFFER_RESOURCE_PRIORITY_NORMAL = (DXGI_OFFER_RESOURCE_PRIORITY_LOW + 1),
			DXGI_OFFER_RESOURCE_PRIORITY_HIGH   = (DXGI_OFFER_RESOURCE_PRIORITY_NORMAL + 1)
		};

		MIDL_INTERFACE("05008617-fbfd-4051-a790-144884b4f6a9")
		IDXGIDevice2 : public IDXGIDevice1
		{
		public:
			virtual HRESULT OfferResources(UINT NumResources, IDXGIResource* const* ppResources, DXGI_OFFER_RESOURCE_PRIORITY Priority) = 0;

			virtual HRESULT ReclaimResources(UINT NumResources, IDXGIResource* const* ppResources, bool* pDiscarded) = 0;

			virtual HRESULT EnqueueSetEvent(HANDLE hEvent) = 0;
		};

		static constexpr UINT DXGI_ENUM_MODES_STEREO          = 4;
		static constexpr UINT DXGI_ENUM_MODES_DISABLED_STEREO = 8;
		static constexpr UINT DXGI_SHARED_RESOURCE_READ       = 0x8000'0000;
		static constexpr UINT DXGI_SHARED_RESOURCE_WRITE      = 1;

		struct DXGI_MODE_DESC1
		{
			UINT                     Width;
			UINT                     Height;
			DXGI_RATIONAL            RefreshRate;
			DXGI_FORMAT              Format;
			DXGI_MODE_SCANLINE_ORDER ScanlineOrdering;
			DXGI_MODE_SCALING        Scaling;
			bool                     Stereo;
		};

		enum DXGI_SCALING
		{
			DXGI_SCALING_STRETCH              = 0,
			DXGI_SCALING_NONE                 = 1,
			DXGI_SCALING_ASPECT_RATIO_STRETCH = 2
		};

		struct DXGI_SWAP_CHAIN_DESC1
		{
			UINT             Width;
			UINT             Height;
			DXGI_FORMAT      Format;
			bool             Stereo;
			DXGI_SAMPLE_DESC SampleDesc;
			DXGI_USAGE       BufferUsage;
			UINT             BufferCount;
			DXGI_SCALING     Scaling;
			DXGI_SWAP_EFFECT SwapEffect;
			DXGI_ALPHA_MODE  AlphaMode;
			UINT             Flags;
		};

		struct DXGI_SWAP_CHAIN_FULLSCREEN_DESC
		{
			DXGI_RATIONAL            RefreshRate;
			DXGI_MODE_SCANLINE_ORDER ScanlineOrdering;
			DXGI_MODE_SCALING        Scaling;
			bool                     Windowed;
		};

		struct DXGI_PRESENT_PARAMETERS
		{
			UINT   DirtyRectsCount;
			RECT*  pDirtyRects;
			RECT*  pScrollRect;
			POINT* pScrollOffset;
		};

		MIDL_INTERFACE("790a45f7-0d42-4876-983a-0a55cfe6f4aa")
		IDXGISwapChain1 : public IDXGISwapChain
		{
		public:
			virtual HRESULT GetDesc1(DXGI_SWAP_CHAIN_DESC1 * pDesc) = 0;

			virtual HRESULT GetFullscreenDesc(DXGI_SWAP_CHAIN_FULLSCREEN_DESC * pDesc) = 0;

			virtual HRESULT GetHwnd(HWND * pHwnd) = 0;

			virtual HRESULT GetCoreWindow(const GUID& refiid, void** ppUnk) = 0;

			virtual HRESULT Present1(UINT SyncInterval, UINT PresentFlags, const DXGI_PRESENT_PARAMETERS* pPresentParameters) = 0;

			virtual bool IsTemporaryMonoSupported() = 0;

			virtual HRESULT GetRestrictToOutput(IDXGIOutput * *ppRestrictToOutput) = 0;

			virtual HRESULT SetBackgroundColor(const DXGI_RGBA* pColor) = 0;
			virtual HRESULT GetBackgroundColor(DXGI_RGBA * pColor)      = 0;

			virtual HRESULT SetRotation(DXGI_MODE_ROTATION Rotation)    = 0;
			virtual HRESULT GetRotation(DXGI_MODE_ROTATION * pRotation) = 0;
		};

		MIDL_INTERFACE("50c83a1c-e072-4c48-87b0-3630fa36a6d0")
		IDXGIFactory2 : public IDXGIFactory1
		{
		public:
			virtual bool IsWindowedStereoEnabled() = 0;

			virtual HRESULT CreateSwapChainForHwnd(IUnknown * pDevice, HWND hWnd, const DXGI_SWAP_CHAIN_DESC1* pDesc, const DXGI_SWAP_CHAIN_FULLSCREEN_DESC* pFullscreenDesc, IDXGIOutput* pRestrictToOutput, IDXGISwapChain1** ppSwapChain) = 0;

			virtual HRESULT CreateSwapChainForCoreWindow(IUnknown * pDevice, IUnknown * pWindow, const DXGI_SWAP_CHAIN_DESC1* pDesc, IDXGIOutput* pRestrictToOutput, IDXGISwapChain1** ppSwapChain) = 0;

			virtual HRESULT GetSharedResourceAdapterLuid(HANDLE hResource, LUID * pLuid) = 0;

			virtual HRESULT RegisterStereoStatusWindow(HWND WindowHandle, UINT wMsg, DWORD * pdwCookie) = 0;

			virtual HRESULT RegisterStereoStatusEvent(HANDLE hEvent, DWORD * pdwCookie) = 0;

			virtual void UnregisterStereoStatus(DWORD dwCookie) = 0;

			virtual HRESULT RegisterOcclusionStatusWindow(HWND WindowHandle, UINT wMsg, DWORD * pdwCookie) = 0;

			virtual HRESULT RegisterOcclusionStatusEvent(HANDLE hEvent, DWORD * pdwCookie) = 0;

			virtual void UnregisterOcclusionStatus(DWORD dwCookie) = 0;

			virtual HRESULT CreateSwapChainForComposition(IUnknown * pDevice, const DXGI_SWAP_CHAIN_DESC1* pDesc, IDXGIOutput* pRestrictToOutput, IDXGISwapChain1** ppSwapChain) = 0;
		};

		enum DXGI_GRAPHICS_PREEMPTION_GRANULARITY
		{
			DXGI_GRAPHICS_PREEMPTION_DMA_BUFFER_BOUNDARY  = 0,
			DXGI_GRAPHICS_PREEMPTION_PRIMITIVE_BOUNDARY   = 1,
			DXGI_GRAPHICS_PREEMPTION_TRIANGLE_BOUNDARY    = 2,
			DXGI_GRAPHICS_PREEMPTION_PIXEL_BOUNDARY       = 3,
			DXGI_GRAPHICS_PREEMPTION_INSTRUCTION_BOUNDARY = 4
		};

		enum DXGI_COMPUTE_PREEMPTION_GRANULARITY
		{
			DXGI_COMPUTE_PREEMPTION_DMA_BUFFER_BOUNDARY   = 0,
			DXGI_COMPUTE_PREEMPTION_DISPATCH_BOUNDARY     = 1,
			DXGI_COMPUTE_PREEMPTION_THREAD_GROUP_BOUNDARY = 2,
			DXGI_COMPUTE_PREEMPTION_THREAD_BOUNDARY       = 3,
			DXGI_COMPUTE_PREEMPTION_INSTRUCTION_BOUNDARY  = 4
		};

		struct DXGI_ADAPTER_DESC2
		{
			wchar_t                              Description[128];
			UINT                                 VendorId;
			UINT                                 DeviceId;
			UINT                                 SubSysId;
			UINT                                 Revision;
			SIZE_T                               DedicatedVideoMemory;
			SIZE_T                               DedicatedSystemMemory;
			SIZE_T                               SharedSystemMemory;
			LUID                                 AdapterLuid;
			UINT                                 Flags;
			DXGI_GRAPHICS_PREEMPTION_GRANULARITY GraphicsPreemptionGranularity;
			DXGI_COMPUTE_PREEMPTION_GRANULARITY  ComputePreemptionGranularity;
		};

		MIDL_INTERFACE("0AA1AE0A-FA0E-4B84-8644-E05FF8E5ACB5")
		IDXGIAdapter2 : public IDXGIAdapter1
		{
		public:
			virtual HRESULT GetDesc2(DXGI_ADAPTER_DESC2 * pDesc) = 0;
		};

		MIDL_INTERFACE("00cddea8-939b-4b83-a340-a685226666cc")
		IDXGIOutput1 : public IDXGIOutput
		{
		public:
			virtual HRESULT GetDisplayModeList1(DXGI_FORMAT EnumFormat, UINT Flags, UINT * pNumModes, DXGI_MODE_DESC1 * pDesc) = 0;

			virtual HRESULT FindClosestMatchingMode1(const DXGI_MODE_DESC1* pModeToMatch, DXGI_MODE_DESC1* pClosestMatch, IUnknown* pConcernedDevice) = 0;

			virtual HRESULT GetDisplaySurfaceData1(IDXGIResource * pDestination) = 0;

			virtual HRESULT DuplicateOutput(IUnknown * pDevice, IDXGIOutputDuplication * *ppOutputDuplication) = 0;
		};
	}
} // namespace Brainfryer::DX12
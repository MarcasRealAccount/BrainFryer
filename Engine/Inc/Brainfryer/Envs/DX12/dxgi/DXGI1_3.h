#pragma once

#include "DXGI1_2.h"

namespace Brainfryer::DX12
{
	extern "C"
	{
		static constexpr UINT DXGI_CREATE_FACTORY_DEBUG = 0x1;

		HRESULT CreateDXGIFactory2(UINT Flags, const GUID& riid, void** ppFactory);
		HRESULT DXGIGetDebugInterface1(UINT Flags, const GUID& riid, void** pDebug);

		MIDL_INTERFACE("6007896c-3244-4afd-bf18-a6d3beda5023")
		IDXGIDevice3 : public IDXGIDevice2
		{
		public:
			virtual void Trim() = 0;
		};

		struct DXGI_MATRIX_3X2_F
		{
			FLOAT _11;
			FLOAT _12;
			FLOAT _21;
			FLOAT _22;
			FLOAT _31;
			FLOAT _32;
		};

		MIDL_INTERFACE("a8be2ac4-199f-4946-b331-79599fb98de7")
		IDXGISwapChain2 : public IDXGISwapChain1
		{
		public:
			virtual HRESULT SetSourceSize(UINT Width, UINT Height)       = 0;
			virtual HRESULT GetSourceSize(UINT * pWidth, UINT * pHeight) = 0;

			virtual HRESULT SetMaximumFrameLatency(UINT MaxLatency)    = 0;
			virtual HRESULT GetMaximumFrameLatency(UINT * pMaxLatency) = 0;

			virtual HANDLE GetFrameLatencyWaitableObject() = 0;

			virtual HRESULT SetMatrixTransform(const DXGI_MATRIX_3X2_F* pMatrix) = 0;
			virtual HRESULT GetMatrixTransform(DXGI_MATRIX_3X2_F * pMatrix)      = 0;
		};

		MIDL_INTERFACE("595e39d1-2724-4663-99b1-da969de28364")
		IDXGIOutput2 : public IDXGIOutput1
		{
		public:
			virtual bool SupportsOverlays() = 0;
		};

		MIDL_INTERFACE("25483823-cd46-4c7d-86ca-47aa95b837bd")
		IDXGIFactory3 : public IDXGIFactory2
		{
		public:
			virtual UINT GetCreationFlags() = 0;
		};

		struct DXGI_DECODE_SWAP_CHAIN_DESC
		{
			UINT Flags;
		};

		enum DXGI_MULTIPLANE_OVERLAY_YCbCr_FLAGS
		{
			DXGI_MULTIPLANE_OVERLAY_YCbCr_FLAG_NOMINAL_RANGE = 0x1,
			DXGI_MULTIPLANE_OVERLAY_YCbCr_FLAG_BT709         = 0x2,
			DXGI_MULTIPLANE_OVERLAY_YCbCr_FLAG_xvYCC         = 0x4
		};

		MIDL_INTERFACE("2633066b-4514-4c7a-8fd8-12ea98059d18")
		IDXGIDecodeSwapChain : public IUnknown
		{
		public:
			virtual HRESULT PresentBuffer(UINT BufferToPresent, UINT SyncInterval, UINT Flags) = 0;

			virtual HRESULT SetSourceRect(const RECT* pRect) = 0;
			virtual HRESULT SetTargetRect(const RECT* pRect) = 0;

			virtual HRESULT SetDestSize(UINT Width, UINT Height) = 0;

			virtual HRESULT GetSourceRect(RECT * pRect) = 0;
			virtual HRESULT GetTargetRect(RECT * pRect) = 0;

			virtual HRESULT GetDestSize(UINT * pWidth, UINT * pHeight) = 0;

			virtual HRESULT SetColorSpace(DXGI_MULTIPLANE_OVERLAY_YCbCr_FLAGS ColorSpace) = 0;

			virtual DXGI_MULTIPLANE_OVERLAY_YCbCr_FLAGS GetColorSpace(void) = 0;
		};

		MIDL_INTERFACE("41e7d1f2-a591-4f7b-a2e5-fa9c843e1c12")
		IDXGIFactoryMedia : public IUnknown
		{
		public:
			virtual HRESULT CreateSwapChainForCompositionSurfaceHandle(IUnknown * pDevice, HANDLE hSurface, const DXGI_SWAP_CHAIN_DESC1* pDesc, IDXGIOutput* pRestrictToOutput, IDXGISwapChain1** ppSwapChain) = 0;

			virtual HRESULT CreateDecodeSwapChainForCompositionSurfaceHandle(IUnknown * pDevice, HANDLE hSurface, DXGI_DECODE_SWAP_CHAIN_DESC * pDesc, IDXGIResource * pYuvDecodeBuffers, IDXGIOutput * pRestrictToOutput, IDXGIDecodeSwapChain * *ppSwapChain) = 0;
		};

		enum DXGI_FRAME_PRESENTATION_MODE
		{
			DXGI_FRAME_PRESENTATION_MODE_COMPOSED            = 0,
			DXGI_FRAME_PRESENTATION_MODE_OVERLAY             = 1,
			DXGI_FRAME_PRESENTATION_MODE_NONE                = 2,
			DXGI_FRAME_PRESENTATION_MODE_COMPOSITION_FAILURE = 3
		};

		struct DXGI_FRAME_STATISTICS_MEDIA
		{
			UINT                         PresentCount;
			UINT                         PresentRefreshCount;
			UINT                         SyncRefreshCount;
			LARGE_INTEGER                SyncQPCTime;
			LARGE_INTEGER                SyncGPUTime;
			DXGI_FRAME_PRESENTATION_MODE CompositionMode;
			UINT                         ApprovedPresentDuration;
		};

		MIDL_INTERFACE("dd95b90b-f05f-4f6a-bd65-25bfb264bd84")
		IDXGISwapChainMedia : public IUnknown
		{
		public:
			virtual HRESULT GetFrameStatisticsMedia(DXGI_FRAME_STATISTICS_MEDIA * pStats) = 0;

			virtual HRESULT SetPresentDuration(UINT Duration) = 0;

			virtual HRESULT CheckPresentDurationSupport(UINT DesiredPresentDuration, UINT * pClosestSmallerPresentDuration, UINT * pClosestLargerPresentDuration) = 0;
		};

		enum DXGI_OVERLAY_SUPPORT_FLAG
		{
			DXGI_OVERLAY_SUPPORT_FLAG_DIRECT  = 0x1,
			DXGI_OVERLAY_SUPPORT_FLAG_SCALING = 0x2
		};

		MIDL_INTERFACE("8a6bb301-7e7e-41F4-a8e0-5b32f7f99b18")
		IDXGIOutput3 : public IDXGIOutput2
		{
		public:
			virtual HRESULT CheckOverlaySupport(DXGI_FORMAT EnumFormat, IUnknown * pConcernedDevice, UINT * pFlags) = 0;
		};
	}
} // namespace Brainfryer::DX12
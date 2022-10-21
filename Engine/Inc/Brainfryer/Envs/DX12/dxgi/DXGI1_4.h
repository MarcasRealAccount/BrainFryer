#pragma once

#include "DXGI1_3.h"

namespace Brainfryer::DX12
{
	extern "C"
	{
		enum DXGI_SWAP_CHAIN_COLOR_SPACE_SUPPORT_FLAG
		{
			DXGI_SWAP_CHAIN_COLOR_SPACE_SUPPORT_FLAG_PRESENT         = 0x1,
			DXGI_SWAP_CHAIN_COLOR_SPACE_SUPPORT_FLAG_OVERLAY_PRESENT = 0x2
		};

		MIDL_INTERFACE("94d99bdb-f1f8-4ab0-b236-7da0170edab1")
		IDXGISwapChain3 : public IDXGISwapChain2
		{
		public:
			virtual UINT GetCurrentBackBufferIndex() = 0;

			virtual HRESULT CheckColorSpaceSupport(DXGI_COLOR_SPACE_TYPE ColorSpace, UINT * pColorSpaceSupport) = 0;

			virtual HRESULT SetColorSpace1(DXGI_COLOR_SPACE_TYPE ColorSpace) = 0;

			virtual HRESULT ResizeBuffers1(UINT BufferCount, UINT Width, UINT Height, DXGI_FORMAT Format, UINT SwapChainFlags, const UINT* pCreationNodeMask, IUnknown* const* ppPresentQueue) = 0;
		};

		enum DXGI_OVERLAY_COLOR_SPACE_SUPPORT_FLAG
		{
			DXGI_OVERLAY_COLOR_SPACE_SUPPORT_FLAG_PRESENT = 0x1
		};

		MIDL_INTERFACE("dc7dca35-2196-414d-9F53-617884032a60")
		IDXGIOutput4 : public IDXGIOutput3
		{
		public:
			virtual HRESULT CheckOverlayColorSpaceSupport(DXGI_FORMAT Format, DXGI_COLOR_SPACE_TYPE ColorSpace, IUnknown * pConcernedDevice, UINT * pFlags) = 0;
		};

		MIDL_INTERFACE("1bc6ea02-ef36-464f-bf0c-21ca39e5168a")
		IDXGIFactory4 : public IDXGIFactory3
		{
		public:
			virtual HRESULT EnumAdapterByLuid(LUID AdapterLuid, const GUID& riid, void** ppvAdapter) = 0;

			virtual HRESULT EnumWarpAdapter(const GUID& riid, void** ppvAdapter) = 0;
		};

		enum DXGI_MEMORY_SEGMENT_GROUP
		{
			DXGI_MEMORY_SEGMENT_GROUP_LOCAL     = 0,
			DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL = 1
		};

		struct DXGI_QUERY_VIDEO_MEMORY_INFO
		{
			std::uint64_t Budget;
			std::uint64_t CurrentUsage;
			std::uint64_t AvailableForReservation;
			std::uint64_t CurrentReservation;
		};

		MIDL_INTERFACE("645967A4-1392-4310-A798-8053CE3E93FD")
		IDXGIAdapter3 : public IDXGIAdapter2
		{
		public:
			virtual HRESULT RegisterHardwareContentProtectionTeardownStatusEvent(HANDLE hEvent, DWORD * pdwCookie) = 0;
			virtual void    UnregisterHardwareContentProtectionTeardownStatus(DWORD dwCookie)                      = 0;

			virtual HRESULT QueryVideoMemoryInfo(UINT NodeIndex, DXGI_MEMORY_SEGMENT_GROUP MemorySegmentGroup, DXGI_QUERY_VIDEO_MEMORY_INFO * pVideoMemoryInfo) = 0;

			virtual HRESULT SetVideoMemoryReservation(UINT NodeIndex, DXGI_MEMORY_SEGMENT_GROUP MemorySegmentGroup, std::uint64_t Reservation) = 0;

			virtual HRESULT RegisterVideoMemoryBudgetChangeNotificationEvent(HANDLE hEvent, DWORD * pdwCookie) = 0;
			virtual void    UnregisterVideoMemoryBudgetChangeNotification(DWORD dwCookie)                      = 0;
		};
	}
} // namespace Brainfryer::DX12
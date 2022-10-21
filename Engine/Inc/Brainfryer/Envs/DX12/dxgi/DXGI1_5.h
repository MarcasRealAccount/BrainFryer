#pragma once

#include "DXGI1_4.h"

namespace Brainfryer::DX12
{
	extern "C"
	{
		enum DXGI_OUTDUPL_FLAG
		{
			DXGI_OUTDUPL_COMPOSITED_UI_CAPTURE_ONLY = 1
		};

		MIDL_INTERFACE("80A07424-AB52-42EB-833C-0C42FD282D98")
		IDXGIOutput5 : public IDXGIOutput4
		{
		public:
			virtual HRESULT DuplicateOutput1(IUnknown * pDevice, UINT Flags, UINT SupportedFormatsCount, const DXGI_FORMAT* pSupportedFormats, IDXGIOutputDuplication** ppOutputDuplication) = 0;
		};

		enum DXGI_HDR_METADATA_TYPE
		{
			DXGI_HDR_METADATA_TYPE_NONE      = 0,
			DXGI_HDR_METADATA_TYPE_HDR10     = 1,
			DXGI_HDR_METADATA_TYPE_HDR10PLUS = 2
		};

		struct DXGI_HDR_METADATA_HDR10
		{
			std::uint16_t RedPrimary[2];
			std::uint16_t GreenPrimary[2];
			std::uint16_t BluePrimary[2];
			std::uint16_t WhitePoint[2];
			UINT          MaxMasteringLuminance;
			UINT          MinMasteringLuminance;
			std::uint16_t MaxContentLightLevel;
			std::uint16_t MaxFrameAverageLightLevel;
		};

		struct DXGI_HDR_METADATA_HDR10PLUS
		{
			BYTE Data[72];
		};

		MIDL_INTERFACE("3D585D5A-BD4A-489E-B1F4-3DBCB6452FFB")
		IDXGISwapChain4 : public IDXGISwapChain3
		{
		public:
			virtual HRESULT SetHDRMetaData(DXGI_HDR_METADATA_TYPE Type, UINT Size, void* pMetaData) = 0;
		};

		using DXGI_OFFER_RESOURCE_FLAGS = enum _DXGI_OFFER_RESOURCE_FLAGS {
			DXGI_OFFER_RESOURCE_FLAG_ALLOW_DECOMMIT = 0x1
		};

		using DXGI_RECLAIM_RESOURCE_RESULTS = enum _DXGI_RECLAIM_RESOURCE_RESULTS {
			DXGI_RECLAIM_RESOURCE_RESULT_OK            = 0,
			DXGI_RECLAIM_RESOURCE_RESULT_DISCARDED     = 1,
			DXGI_RECLAIM_RESOURCE_RESULT_NOT_COMMITTED = 2
		};

		MIDL_INTERFACE("95B4F95F-D8DA-4CA4-9EE6-3B76D5968A10")
		IDXGIDevice4 : public IDXGIDevice3
		{
		public:
			virtual HRESULT OfferResources1(UINT NumResources, IDXGIResource* const* ppResources, DXGI_OFFER_RESOURCE_PRIORITY Priority, UINT Flags) = 0;

			virtual HRESULT ReclaimResources1(UINT NumResources, IDXGIResource* const* ppResources, DXGI_RECLAIM_RESOURCE_RESULTS* pResults) = 0;
		};

		enum DXGI_FEATURE
		{
			DXGI_FEATURE_PRESENT_ALLOW_TEARING = 0
		};

		MIDL_INTERFACE("7632e1f5-ee65-4dca-87fd-84cd75f8838d")
		IDXGIFactory5 : public IDXGIFactory4
		{
		public:
			virtual HRESULT CheckFeatureSupport(DXGI_FEATURE Feature, void* pFeatureSupportData, UINT FeatureSupportDataSize) = 0;
		};
	}
} // namespace Brainfryer::DX12
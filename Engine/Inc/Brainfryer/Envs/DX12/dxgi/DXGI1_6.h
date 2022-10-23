#pragma once

#include "DXGI1_5.h"

namespace Brainfryer::DX12
{
	extern "C"
	{
		HRESULT DXGIDeclareAdapterRemovalSupport();
		enum DXGI_ADAPTER_FLAG3
		{
			DXGI_ADAPTER_FLAG3_NONE                         = 0,
			DXGI_ADAPTER_FLAG3_REMOTE                       = 1,
			DXGI_ADAPTER_FLAG3_SOFTWARE                     = 2,
			DXGI_ADAPTER_FLAG3_ACG_COMPATIBLE               = 4,
			DXGI_ADAPTER_FLAG3_SUPPORT_MONITORED_FENCES     = 8,
			DXGI_ADAPTER_FLAG3_SUPPORT_NON_MONITORED_FENCES = 0x10,
			DXGI_ADAPTER_FLAG3_KEYED_MUTEX_CONFORMANCE      = 0x20,
			DXGI_ADAPTER_FLAG3_FORCE_DWORD                  = 0xffffffff
		};

		struct DXGI_ADAPTER_DESC3
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
			DXGI_ADAPTER_FLAG3                   Flags;
			DXGI_GRAPHICS_PREEMPTION_GRANULARITY GraphicsPreemptionGranularity;
			DXGI_COMPUTE_PREEMPTION_GRANULARITY  ComputePreemptionGranularity;
		};

		MIDL_INTERFACE("3c8d99d1-4fbf-4181-a82c-af66bf7bd24e")
		IDXGIAdapter4 : public IDXGIAdapter3
		{
		public:
			virtual HRESULT GetDesc3(DXGI_ADAPTER_DESC3 * pDesc) = 0;
		};

		struct DXGI_OUTPUT_DESC1
		{
			wchar_t               DeviceName[32];
			RECT                  DesktopCoordinates;
			BOOL                  AttachedToDesktop;
			DXGI_MODE_ROTATION    Rotation;
			HMONITOR              Monitor;
			UINT                  BitsPerColor;
			DXGI_COLOR_SPACE_TYPE ColorSpace;
			FLOAT                 RedPrimary[2];
			FLOAT                 GreenPrimary[2];
			FLOAT                 BluePrimary[2];
			FLOAT                 WhitePoint[2];
			FLOAT                 MinLuminance;
			FLOAT                 MaxLuminance;
			FLOAT                 MaxFullFrameLuminance;
		};

		enum DXGI_HARDWARE_COMPOSITION_SUPPORT_FLAGS
		{
			DXGI_HARDWARE_COMPOSITION_SUPPORT_FLAG_FULLSCREEN       = 1,
			DXGI_HARDWARE_COMPOSITION_SUPPORT_FLAG_WINDOWED         = 2,
			DXGI_HARDWARE_COMPOSITION_SUPPORT_FLAG_CURSOR_STRETCHED = 4
		};

		MIDL_INTERFACE("068346e8-aaec-4b84-add7-137f513f77a1")
		IDXGIOutput6 : public IDXGIOutput5
		{
		public:
			virtual HRESULT GetDesc1(DXGI_OUTPUT_DESC1 * pDesc) = 0;

			virtual HRESULT CheckHardwareCompositionSupport(UINT * pFlags) = 0;
		};

		enum DXGI_GPU_PREFERENCE
		{
			DXGI_GPU_PREFERENCE_UNSPECIFIED      = 0,
			DXGI_GPU_PREFERENCE_MINIMUM_POWER    = (DXGI_GPU_PREFERENCE_UNSPECIFIED + 1),
			DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE = (DXGI_GPU_PREFERENCE_MINIMUM_POWER + 1)
		};

		MIDL_INTERFACE("c1b6694f-ff09-44a9-b03c-77900a0a1d17")
		IDXGIFactory6 : public IDXGIFactory5
		{
		public:
			virtual HRESULT EnumAdapterByGpuPreference(UINT Adapter, DXGI_GPU_PREFERENCE GpuPreference, const GUID& riid, void** ppvAdapter) = 0;
		};

		MIDL_INTERFACE("a4966eed-76db-44da-84c1-ee9a7afb20a8")
		IDXGIFactory7 : public IDXGIFactory6
		{
		public:
			virtual HRESULT RegisterAdaptersChangedEvent(HANDLE hEvent, DWORD * pdwCookie) = 0;

			virtual HRESULT UnregisterAdaptersChangedEvent(DWORD dwCookie) = 0;
		};

		HRESULT DXGIDisableVBlankVirtualization();
	}
} // namespace Brainfryer::DX12
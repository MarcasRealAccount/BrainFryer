#pragma once

#include "Brainfryer/Renderer/Filter.h"
#include "DX12.h"

namespace Brainfryer::DX12
{
	constexpr D3D12_FILTER DX12Filter(EFilter minFilter, EFilter magFilter)
	{
		switch (minFilter)
		{
		case EFilter::Nearest:
			switch (magFilter)
			{
			case EFilter::Nearest: return D3D12_FILTER_MIN_MAG_MIP_POINT;
			case EFilter::Linear: return D3D12_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
			}
		case EFilter::Linear:
			switch (magFilter)
			{
			case EFilter::Nearest: return D3D12_FILTER_MIN_LINEAR_MAG_MIP_POINT;
			case EFilter::Linear: return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
			}
		}
		return D3D12_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	}
} // namespace Brainfryer::DX12
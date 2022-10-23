#pragma once

#include "Brainfryer/Renderer/ComparisonFunc.h"
#include "DX12.h"

namespace Brainfryer::DX12
{
	constexpr D3D12_COMPARISON_FUNC DX12ComparisonFunc(EComparisonFunc func)
	{
		switch (func)
		{
		case EComparisonFunc::Never: return D3D12_COMPARISON_FUNC_NEVER;
		case EComparisonFunc::LT: return D3D12_COMPARISON_FUNC_LESS;
		case EComparisonFunc::EQ: return D3D12_COMPARISON_FUNC_EQUAL;
		case EComparisonFunc::LE: return D3D12_COMPARISON_FUNC_LESS_EQUAL;
		case EComparisonFunc::GT: return D3D12_COMPARISON_FUNC_GREATER;
		case EComparisonFunc::NE: return D3D12_COMPARISON_FUNC_NOT_EQUAL;
		case EComparisonFunc::GE: return D3D12_COMPARISON_FUNC_GREATER_EQUAL;
		case EComparisonFunc::Always: return D3D12_COMPARISON_FUNC_ALWAYS;
		}
		return D3D12_COMPARISON_FUNC_NEVER;
	}
} // namespace Brainfryer::DX12
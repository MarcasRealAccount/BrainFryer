#pragma once

#include "Brainfryer/Renderer/PrimitiveTopology.h"
#include "DX12.h"

namespace Brainfryer::DX12
{
	constexpr D3D12_PRIMITIVE_TOPOLOGY DX12PrimitiveTopology(EPrimitiveTopology topology)
	{
		switch (topology)
		{
		case EPrimitiveTopology::Triangles: return D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		}
		return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
	}

	constexpr D3D12_PRIMITIVE_TOPOLOGY_TYPE DX12PrimitiveTopologyType(EPrimitiveTopology topology)
	{
		switch (topology)
		{
		case EPrimitiveTopology::Triangles: return D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		}
		return D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
	}
} // namespace Brainfryer::DX12
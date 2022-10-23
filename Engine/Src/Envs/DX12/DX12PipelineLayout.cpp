#include "Brainfryer/Envs/DX12/DX12PipelineLayout.h"
#include "Brainfryer/Envs/DX12/DX12Context.h"

namespace Brainfryer::DX12
{
	DX12PipelineLayout::DX12PipelineLayout(PipelineLayoutInfo info)
	    : m_Info(std::move(info))
	{
		auto context = Context::Get<DX12Context>();

		std::size_t totalRangeCount = 0;
		for (std::size_t i = 0; i < m_Info.parameters.size(); ++i)
		{
			auto& param = m_Info.parameters[i];
			if (param.type == EPipelineLayoutParameterType::DescriptorTable)
				totalRangeCount += std::get<PipelineLayoutDescriptorTable>(param.value).ranges.size();
		}
		std::vector<D3D12_DESCRIPTOR_RANGE> allRanges(totalRangeCount);
		std::size_t                         rangeOffset = 0;
		std::vector<D3D12_ROOT_PARAMETER>   parameters(m_Info.parameters.size());
		for (std::size_t i = 0; i < parameters.size(); ++i)
		{
			auto& param                = m_Info.parameters[i];
			auto& parameter            = parameters[i];
			parameter.ParameterType    = DX12PipelineLayoutParameterType(param.type);
			parameter.ShaderVisibility = DX12ShaderVisibility(param.visiblity);
			switch (param.type)
			{
			case EPipelineLayoutParameterType::DescriptorTable:
			{
				auto&                   descriptorTable = std::get<PipelineLayoutDescriptorTable>(param.value);
				D3D12_DESCRIPTOR_RANGE* ranges          = allRanges.data() + rangeOffset;
				rangeOffset += descriptorTable.ranges.size();

				parameter.DescriptorTable.NumDescriptorRanges = static_cast<UINT>(descriptorTable.ranges.size());
				parameter.DescriptorTable.pDescriptorRanges   = ranges;
				for (std::size_t j = 0; j < descriptorTable.ranges.size(); ++j)
				{
					auto& rng                               = descriptorTable.ranges[j];
					auto& range                             = ranges[j];
					range.RangeType                         = DX12PipelineLayoutDescriptorRangeType(rng.type);
					range.NumDescriptors                    = rng.numDescriptors;
					range.BaseShaderRegister                = rng.binding;
					range.RegisterSpace                     = rng.space;
					range.OffsetInDescriptorsFromTableStart = rng.offset;
				}
				break;
			}
			case EPipelineLayoutParameterType::Constants:
			{
				auto& constants = std::get<PipelineLayoutConstants>(param.value);

				parameter.Constants.ShaderRegister = constants.binding;
				parameter.Constants.RegisterSpace  = constants.space;
				parameter.Constants.Num32BitValues = constants.num32BitValues;
				break;
			}
			case EPipelineLayoutParameterType::ConstantBuffer:
			case EPipelineLayoutParameterType::ShaderResourceView:
			case EPipelineLayoutParameterType::UniformView:
			{
				auto& descriptor = std::get<PipelineLayoutDescriptor>(param.value);

				parameter.Descriptor.ShaderRegister = descriptor.binding;
				parameter.Descriptor.RegisterSpace  = descriptor.space;
				break;
			}
			}
		}

		D3D12_ROOT_SIGNATURE_DESC desc {};
		desc.NumParameters = static_cast<UINT>(parameters.size());
		desc.pParameters   = parameters.data();
		desc.Flags         = DX12PipelineLayoutFlags(info.flags);

		Com<ID3D10Blob> blob;
		Com<ID3D10Blob> error;
		if (!HRVLog(D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1_0, blob, error)))
			return;

		HRVLog(context->device()->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(), m_RootSignature, m_RootSignature));
	}

	DX12PipelineLayout::~DX12PipelineLayout()
	{
	}
} // namespace Brainfryer::DX12
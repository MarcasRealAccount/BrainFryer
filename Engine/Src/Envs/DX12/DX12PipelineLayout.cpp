#include "Brainfryer/Envs/DX12/DX12PipelineLayout.h"
#include "Brainfryer/Envs/DX12/DX12ComparisonFunc.h"
#include "Brainfryer/Envs/DX12/DX12Context.h"
#include "Brainfryer/Envs/DX12/DX12Filter.h"
#include "Brainfryer/Envs/DX12/DX12ImageAddressMode.h"

namespace Brainfryer::DX12
{
	DX12PipelineLayout::DX12PipelineLayout(PipelineLayoutInfo info)
	    : m_Info(std::move(info))
	{
		auto  context = Context::Get<DX12Context>();
		auto& device  = context->device();

		D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData {};
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
		if (!HRValidate(device->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
			return;

		std::size_t totalRangeCount = 0;
		for (std::size_t i = 0; i < m_Info.parameters.size(); ++i)
		{
			auto& param = m_Info.parameters[i];
			if (param.type == EPipelineLayoutParameterType::DescriptorTable)
				totalRangeCount += std::get<PipelineLayoutDescriptorTable>(param.value).ranges.size();
		}
		std::vector<D3D12_DESCRIPTOR_RANGE1> allRanges(totalRangeCount);
		std::size_t                          rangeOffset = 0;
		std::vector<D3D12_ROOT_PARAMETER1>   parameters(m_Info.parameters.size());
		for (std::size_t i = 0; i < parameters.size(); ++i)
		{
			auto& param                = m_Info.parameters[i];
			auto& parameter            = parameters[i];
			parameter.ParameterType    = DX12PipelineLayoutParameterType(param.type);
			parameter.ShaderVisibility = DX12ShaderVisibility(param.visibility);
			switch (param.type)
			{
			case EPipelineLayoutParameterType::DescriptorTable:
			{
				auto&                    descriptorTable = std::get<PipelineLayoutDescriptorTable>(param.value);
				D3D12_DESCRIPTOR_RANGE1* ranges          = allRanges.data() + rangeOffset;
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
					range.Flags                             = DX12PipelineLayoutDescriptorRangeFlags(rng.flags);
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
			case EPipelineLayoutParameterType::UnorderedAccess:
			{
				auto& descriptor = std::get<PipelineLayoutDescriptor>(param.value);

				parameter.Descriptor.ShaderRegister = descriptor.binding;
				parameter.Descriptor.RegisterSpace  = descriptor.space;
				break;
			}
			}
		}

		std::vector<D3D12_STATIC_SAMPLER_DESC> samplers(m_Info.staticSamplers.size());
		for (std::size_t i = 0; i < samplers.size(); ++i)
		{
			auto& staticSampler      = m_Info.staticSamplers[i];
			auto& sampler            = samplers[i];
			sampler.Filter           = DX12Filter(staticSampler.minFilter, staticSampler.magFilter);
			sampler.AddressU         = DX12ImageAddressMode(staticSampler.addressU);
			sampler.AddressV         = DX12ImageAddressMode(staticSampler.addressV);
			sampler.AddressW         = DX12ImageAddressMode(staticSampler.addressW);
			sampler.MipLODBias       = staticSampler.mipLodBias;
			sampler.MaxAnisotropy    = staticSampler.maxAnisotropy;
			sampler.ComparisonFunc   = DX12ComparisonFunc(staticSampler.comparisonFunc);
			sampler.BorderColor      = DX12BorderColor(staticSampler.borderColor);
			sampler.MinLOD           = staticSampler.minLod;
			sampler.MaxLOD           = staticSampler.maxLod;
			sampler.ShaderRegister   = staticSampler.binding;
			sampler.RegisterSpace    = staticSampler.space;
			sampler.ShaderVisibility = DX12ShaderVisibility(staticSampler.visibility);
		}

		D3D12_VERSIONED_ROOT_SIGNATURE_DESC desc {};
		desc.Version                    = D3D_ROOT_SIGNATURE_VERSION_1_1;
		desc.Desc_1_1.NumParameters     = static_cast<UINT>(parameters.size());
		desc.Desc_1_1.pParameters       = parameters.data();
		desc.Desc_1_1.NumStaticSamplers = static_cast<UINT>(samplers.size());
		desc.Desc_1_1.pStaticSamplers   = samplers.data();
		desc.Desc_1_1.Flags             = DX12PipelineLayoutFlags(m_Info.flags);

		Com<ID3D10Blob> blob;
		Com<ID3D10Blob> error;
		if (!HRVLog(D3D12SerializeVersionedRootSignature(&desc, blob, error)))
			return;

		HRVLog(device->CreateRootSignature(0, blob->GetBufferPointer(), blob->GetBufferSize(), m_RootSignature, m_RootSignature));
	}

	DX12PipelineLayout::~DX12PipelineLayout()
	{
	}
} // namespace Brainfryer::DX12
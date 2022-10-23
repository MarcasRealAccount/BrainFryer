#pragma once

#include "Brainfryer/Renderer/PipelineLayout.h"
#include "DX12.h"

namespace Brainfryer::DX12
{
	constexpr D3D12_ROOT_SIGNATURE_FLAGS DX12PipelineLayoutFlags(EPipelineLayoutFlags flags)
	{
		std::uint32_t flg = static_cast<std::uint32_t>(flags);
		std::uint32_t out = 0;
		if (flg & static_cast<std::uint32_t>(EPipelineLayoutFlags::AllowInputAssemblerInputLayout)) out |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		return static_cast<D3D12_ROOT_SIGNATURE_FLAGS>(out);
	}

	constexpr D3D12_ROOT_PARAMETER_TYPE DX12PipelineLayoutParameterType(EPipelineLayoutParameterType type)
	{
		switch (type)
		{
		case EPipelineLayoutParameterType::DescriptorTable: return D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		case EPipelineLayoutParameterType::Constants: return D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		case EPipelineLayoutParameterType::ConstantBuffer: return D3D12_ROOT_PARAMETER_TYPE_CBV;
		case EPipelineLayoutParameterType::ShaderResourceView: return D3D12_ROOT_PARAMETER_TYPE_SRV;
		case EPipelineLayoutParameterType::UniformView: return D3D12_ROOT_PARAMETER_TYPE_UAV;
		}
		return D3D12_ROOT_PARAMETER_TYPE_SRV;
	}

	constexpr D3D12_DESCRIPTOR_RANGE_TYPE DX12PipelineLayoutDescriptorRangeType(EPipelineLayoutDescriptorRangeType type)
	{
		switch (type)
		{
		case EPipelineLayoutDescriptorRangeType::ShaderResourceView: return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		case EPipelineLayoutDescriptorRangeType::UniformView: return D3D12_DESCRIPTOR_RANGE_TYPE_UAV;
		case EPipelineLayoutDescriptorRangeType::ConstantBuffer: return D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
		case EPipelineLayoutDescriptorRangeType::Sampler: return D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER;
		}
		return D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	}

	constexpr D3D12_SHADER_VISIBILITY DX12ShaderVisibility(EShaderVisibility visibility)
	{
		switch (visibility)
		{
		case EShaderVisibility::All: return D3D12_SHADER_VISIBILITY_ALL;
		case EShaderVisibility::Vertex: return D3D12_SHADER_VISIBILITY_VERTEX;
		case EShaderVisibility::Hull: return D3D12_SHADER_VISIBILITY_HULL;
		case EShaderVisibility::Domain: return D3D12_SHADER_VISIBILITY_DOMAIN;
		case EShaderVisibility::Geometry: return D3D12_SHADER_VISIBILITY_GEOMETRY;
		case EShaderVisibility::Pixel: return D3D12_SHADER_VISIBILITY_PIXEL;
		case EShaderVisibility::Amplification: return D3D12_SHADER_VISIBILITY_AMPLIFICATION;
		case EShaderVisibility::Mesh: return D3D12_SHADER_VISIBILITY_MESH;
		}
		return D3D12_SHADER_VISIBILITY_ALL;
	}

	class DX12PipelineLayout : public PipelineLayout
	{
	public:
		DX12PipelineLayout(PipelineLayoutInfo info);
		~DX12PipelineLayout();

		virtual const PipelineLayoutInfo& info() const override { return m_Info; }

		virtual bool initialized() const { return m_RootSignature.valid(); }

		auto& handle() { return m_RootSignature; }
		auto& handle() const { return m_RootSignature; }

	private:
		PipelineLayoutInfo       m_Info;
		Com<ID3D12RootSignature> m_RootSignature;
	};
} // namespace Brainfryer::DX12
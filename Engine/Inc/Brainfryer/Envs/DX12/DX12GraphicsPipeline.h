#pragma once

#include "Brainfryer/Renderer/GraphicsPipeline.h"
#include "DX12.h"

namespace Brainfryer::DX12
{
	constexpr D3D12_BLEND DX12Blend(EBlend blend)
	{
		switch (blend)
		{
		case EBlend::Zero: return D3D12_BLEND_ZERO;
		case EBlend::One: return D3D12_BLEND_ONE;
		case EBlend::SrcColor: return D3D12_BLEND_SRC_COLOR;
		case EBlend::InvSrcColor: return D3D12_BLEND_INV_SRC_COLOR;
		case EBlend::SrcAlpha: return D3D12_BLEND_SRC_ALPHA;
		case EBlend::InvSrcAlpha: return D3D12_BLEND_INV_SRC_ALPHA;
		case EBlend::DstAlpha: return D3D12_BLEND_DEST_ALPHA;
		case EBlend::InvDstAlpha: return D3D12_BLEND_INV_DEST_ALPHA;
		case EBlend::SrcAlphaSat: return D3D12_BLEND_SRC_ALPHA_SAT;
		case EBlend::BlendFactor: return D3D12_BLEND_BLEND_FACTOR;
		case EBlend::InvBlendFactor: return D3D12_BLEND_INV_BLEND_FACTOR;
		case EBlend::Src1Color: return D3D12_BLEND_SRC1_COLOR;
		case EBlend::InvSrc1Color: return D3D12_BLEND_INV_SRC1_COLOR;
		case EBlend::Src1Alpha: return D3D12_BLEND_SRC1_ALPHA;
		case EBlend::InvSrc1Alpha: return D3D12_BLEND_INV_SRC1_ALPHA;
		case EBlend::AlphaFactor: return D3D12_BLEND_ALPHA_FACTOR;
		case EBlend::InvAlphaFactor: return D3D12_BLEND_INV_ALPHA_FACTOR;
		}
		return D3D12_BLEND_ONE;
	}

	constexpr D3D12_BLEND_OP DX12BlendOp(EBlendOp op)
	{
		switch (op)
		{
		case EBlendOp::Add: return D3D12_BLEND_OP_ADD;
		case EBlendOp::Subtract: return D3D12_BLEND_OP_SUBTRACT;
		case EBlendOp::RevSubtract: return D3D12_BLEND_OP_REV_SUBTRACT;
		case EBlendOp::Min: return D3D12_BLEND_OP_MIN;
		case EBlendOp::Max: return D3D12_BLEND_OP_MAX;
		}
		return D3D12_BLEND_OP_ADD;
	}

	constexpr D3D12_LOGIC_OP DX12LogicOp(ELogicOp op)
	{
		switch (op)
		{
		case ELogicOp::Clear: return D3D12_LOGIC_OP_CLEAR;
		case ELogicOp::Set: return D3D12_LOGIC_OP_SET;
		case ELogicOp::Copy: return D3D12_LOGIC_OP_COPY;
		case ELogicOp::CopyInverted: return D3D12_LOGIC_OP_COPY_INVERTED;
		case ELogicOp::Noop: return D3D12_LOGIC_OP_NOOP;
		case ELogicOp::Invert: return D3D12_LOGIC_OP_INVERT;
		case ELogicOp::And: return D3D12_LOGIC_OP_AND;
		case ELogicOp::Nand: return D3D12_LOGIC_OP_NAND;
		case ELogicOp::Or: return D3D12_LOGIC_OP_OR;
		case ELogicOp::Nor: return D3D12_LOGIC_OP_NOR;
		case ELogicOp::Xor: return D3D12_LOGIC_OP_XOR;
		case ELogicOp::Equiv: return D3D12_LOGIC_OP_EQUIV;
		case ELogicOp::AndReverse: return D3D12_LOGIC_OP_AND_REVERSE;
		case ELogicOp::AndInverted: return D3D12_LOGIC_OP_AND_INVERTED;
		case ELogicOp::OrReverse: return D3D12_LOGIC_OP_OR_REVERSE;
		case ELogicOp::OrInverted: return D3D12_LOGIC_OP_OR_INVERTED;
		}
		return D3D12_LOGIC_OP_CLEAR;
	}

	class DX12GraphicsPipeline : public GraphicsPipeline
	{
	public:
		DX12GraphicsPipeline(const GraphicsPipelineInfo& info);
		~DX12GraphicsPipeline();

		virtual void bind(CommandList* commandList) override;

		virtual PipelineLayout* pipelineLayout() const { return m_PipelineLayout; }

		virtual bool initialized() const override { return m_Pipeline.valid(); }

		auto& handle() { return m_Pipeline; }
		auto& handle() const { return m_Pipeline; }

		Com<ID3D12RootSignature>& rootSignature();
		Com<ID3D12RootSignature>& rootSignature() const;

	private:
		PipelineLayout* m_PipelineLayout;

		Com<ID3D12PipelineState> m_Pipeline;
	};
} // namespace Brainfryer::DX12
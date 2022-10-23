#include "Brainfryer/Envs/DX12/DX12GraphicsPipeline.h"
#include "Brainfryer/Envs/DX12/DX12CommandList.h"
#include "Brainfryer/Envs/DX12/DX12Context.h"
#include "Brainfryer/Envs/DX12/DX12Format.h"
#include "Brainfryer/Envs/DX12/DX12PipelineLayout.h"
#include "Brainfryer/Envs/DX12/DX12PrimitiveTopology.h"

namespace Brainfryer::DX12
{
	DX12GraphicsPipeline::DX12GraphicsPipeline(const GraphicsPipelineInfo& info)
	    : m_PipelineLayout(info.pipelineLayout)
	{
		auto context = Context::Get<DX12Context>();

		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc {};
		desc.pRootSignature = static_cast<DX12PipelineLayout*>(m_PipelineLayout)->handle().get();

		desc.VS.pShaderBytecode = info.vertexShader.data.data();
		desc.VS.BytecodeLength  = info.vertexShader.data.size();
		desc.PS.pShaderBytecode = info.pixelShader.data.data();
		desc.PS.BytecodeLength  = info.pixelShader.data.size();
		desc.DS.pShaderBytecode = info.domainShader.data.data();
		desc.DS.BytecodeLength  = info.domainShader.data.size();
		desc.HS.pShaderBytecode = info.hullShader.data.data();
		desc.HS.BytecodeLength  = info.hullShader.data.size();
		desc.GS.pShaderBytecode = info.geometryShader.data.data();
		desc.GS.BytecodeLength  = info.geometryShader.data.size();

		desc.StreamOutput.pSODeclaration   = nullptr;
		desc.StreamOutput.NumEntries       = 0;
		desc.StreamOutput.pBufferStrides   = nullptr;
		desc.StreamOutput.NumStrides       = 0;
		desc.StreamOutput.RasterizedStream = 0;

		desc.BlendState.AlphaToCoverageEnable  = false;
		desc.BlendState.IndependentBlendEnable = false;
		for (std::uint8_t i = 0; i < 8; ++i)
		{
			auto& renderTarget                 = desc.BlendState.RenderTarget[i];
			renderTarget.BlendEnable           = false;
			renderTarget.LogicOpEnable         = false;
			renderTarget.SrcBlend              = D3D12_BLEND_ZERO;
			renderTarget.DestBlend             = D3D12_BLEND_ONE;
			renderTarget.BlendOp               = D3D12_BLEND_OP_ADD;
			renderTarget.SrcBlendAlpha         = D3D12_BLEND_ZERO;
			renderTarget.DestBlendAlpha        = D3D12_BLEND_ONE;
			renderTarget.BlendOpAlpha          = D3D12_BLEND_OP_ADD;
			renderTarget.LogicOp               = D3D12_LOGIC_OP_CLEAR;
			renderTarget.RenderTargetWriteMask = 0xF;
		}

		desc.SampleMask = 0xFFFF'FFFF;

		desc.RasterizerState.FillMode              = D3D12_FILL_MODE_SOLID;
		desc.RasterizerState.CullMode              = D3D12_CULL_MODE_BACK;
		desc.RasterizerState.FrontCounterClockwise = false;
		desc.RasterizerState.DepthBias             = 0;
		desc.RasterizerState.DepthBiasClamp        = 1.0f;
		desc.RasterizerState.SlopeScaledDepthBias  = 1.0f;
		desc.RasterizerState.DepthClipEnable       = false;
		desc.RasterizerState.MultisampleEnable     = false;
		desc.RasterizerState.AntialiasedLineEnable = false;
		desc.RasterizerState.ForcedSampleCount     = 1;
		desc.RasterizerState.ConservativeRaster    = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

		desc.DepthStencilState.DepthEnable                  = false;
		desc.DepthStencilState.DepthWriteMask               = D3D12_DEPTH_WRITE_MASK_ALL;
		desc.DepthStencilState.DepthFunc                    = D3D12_COMPARISON_FUNC_LESS;
		desc.DepthStencilState.StencilEnable                = false;
		desc.DepthStencilState.StencilReadMask              = 0xFF;
		desc.DepthStencilState.StencilWriteMask             = 0xFF;
		desc.DepthStencilState.FrontFace.StencilFailOp      = D3D12_STENCIL_OP_KEEP;
		desc.DepthStencilState.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		desc.DepthStencilState.FrontFace.StencilPassOp      = D3D12_STENCIL_OP_KEEP;
		desc.DepthStencilState.FrontFace.StencilFunc        = D3D12_COMPARISON_FUNC_LESS;
		desc.DepthStencilState.BackFace.StencilFailOp       = D3D12_STENCIL_OP_KEEP;
		desc.DepthStencilState.BackFace.StencilDepthFailOp  = D3D12_STENCIL_OP_KEEP;
		desc.DepthStencilState.BackFace.StencilPassOp       = D3D12_STENCIL_OP_KEEP;
		desc.DepthStencilState.BackFace.StencilFunc         = D3D12_COMPARISON_FUNC_LESS;

		std::vector<D3D12_INPUT_ELEMENT_DESC> inputs(info.inputs.size());
		for (std::size_t i = 0; i < inputs.size(); ++i)
		{
			auto& in                   = info.inputs[i];
			auto& input                = inputs[i];
			input.SemanticName         = in.name.c_str();
			input.SemanticIndex        = in.index;
			input.Format               = DX12Format(in.format);
			input.InputSlot            = in.binding;
			input.AlignedByteOffset    = in.offset;
			input.InstanceDataStepRate = in.instanceStep;
			if (in.instanceStep == 0)
				input.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			else
				input.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA;
		}

		desc.InputLayout.pInputElementDescs = inputs.data();
		desc.InputLayout.NumElements        = static_cast<UINT>(inputs.size());

		desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

		desc.PrimitiveTopologyType = DX12PrimitiveTopologyType(info.primitiveTopology);

		desc.NumRenderTargets = info.renderTargetCount;
		for (std::uint8_t i = 0; i < info.renderTargetCount; ++i)
			desc.RTVFormats[i] = DX12Format(info.rtvFormats[i]);
		desc.DSVFormat = DX12Format(info.dsvFormat);

		desc.SampleDesc.Count   = 1;
		desc.SampleDesc.Quality = 0;

		desc.NodeMask = 0;

		desc.CachedPSO.pCachedBlob           = nullptr;
		desc.CachedPSO.CachedBlobSizeInBytes = 0;

		desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		HRVLog(context->device()->CreateGraphicsPipelineState(&desc, m_Pipeline, m_Pipeline));
	}

	DX12GraphicsPipeline::~DX12GraphicsPipeline()
	{
	}

	void DX12GraphicsPipeline::bind(CommandList* commandList)
	{
		auto& handle = static_cast<DX12CommandList*>(commandList)->handle();
		handle->SetPipelineState(m_Pipeline.get());
		handle->SetGraphicsRootSignature(rootSignature().get());
	}

	Com<ID3D12RootSignature>& DX12GraphicsPipeline::rootSignature()
	{
		return static_cast<DX12PipelineLayout*>(m_PipelineLayout)->handle();
	}

	Com<ID3D12RootSignature>& DX12GraphicsPipeline::rootSignature() const
	{
		return static_cast<DX12PipelineLayout*>(m_PipelineLayout)->handle();
	}
} // namespace Brainfryer::DX12
#include "ImGuiRenderer.h"
#include "Envs/DX12/DXCompiler.h"

#include <Brainfryer/Renderer/Buffer.h>
#include <Brainfryer/Renderer/CommandAllocator.h>
#include <Brainfryer/Renderer/CommandList.h>
#include <Brainfryer/Renderer/Context.h>
#include <Brainfryer/Renderer/DescriptorHeap.h>
#include <Brainfryer/Renderer/GraphicsPipeline.h>
#include <Brainfryer/Renderer/Image.h>
#include <Brainfryer/Renderer/PipelineLayout.h>
#include <Brainfryer/Utils/Exception.h>
#include <Brainfryer/Utils/UID.h>

namespace Brainfryer::Editor
{
	struct RendererBackendData
	{
	public:
		Swapchain*      swapchain      = nullptr;
		DescriptorHeap* descriptorHeap = nullptr;

		std::unique_ptr<GraphicsPipeline> pipeline;
		std::unique_ptr<PipelineLayout>   pipelineLayout;

		std::unique_ptr<Image> fontImage;
		DescriptorHeapRef      fontImageDescriptor;
	};

	struct RendererViewportData
	{
	public:
		Swapchain* swapchain = nullptr;
		bool       owns      = false;
		UID        cmdListID;

		std::vector<std::unique_ptr<Buffer>> renderVertexBuffers;
		std::vector<std::unique_ptr<Buffer>> renderIndexBuffers;
	};

	static void PlatformInit();
	static void PlatformShutdown();
	static void CreateDeviceObjects();
	static void CreateFontsTexture();
	static void SetupRenderState(ImDrawData* drawData, CommandList* commandList, Buffer* rvb, Buffer* rib);

	static RendererBackendData* GetRendererBackendData()
	{
		return ImGui::GetCurrentContext() ? reinterpret_cast<RendererBackendData*>(ImGui::GetIO().BackendRendererUserData) : nullptr;
	}

	static RendererViewportData* GetRendererViewportData(ImGuiViewport* vp)
	{
		return reinterpret_cast<RendererViewportData*>(vp->RendererUserData);
	}

	void ImGuiRendererInit(Swapchain* swapchain, DescriptorHeap* descriptorHeap)
	{
		ImGuiIO& io = ImGui::GetIO();
		Assert(!io.BackendRendererUserData, "Already initialized a renderer backend!");

		RendererBackendData* bd = new RendererBackendData();
		bd->swapchain           = swapchain;
		bd->descriptorHeap      = descriptorHeap;

		io.BackendRendererName     = "BrainFryer";
		io.BackendRendererUserData = bd;
		io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
		io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			PlatformInit();

		ImGuiViewport*        vp = ImGui::GetMainViewport();
		RendererViewportData* vd = new RendererViewportData();
		vd->swapchain            = bd->swapchain;
		vd->owns                 = false;
		vp->RendererUserData     = vd;
		vd->renderVertexBuffers.resize(Context::FrameCount());
		vd->renderIndexBuffers.resize(Context::FrameCount());
	}

	void ImGuiRendererShutdown()
	{
		RendererBackendData* bd = GetRendererBackendData();
		Assert(bd, "No renderer backend to shutdown, or already shutdown?");
		ImGuiIO& io = ImGui::GetIO();

		ImGuiViewport* vp = ImGui::GetMainViewport();
		if (RendererViewportData* vd = GetRendererViewportData(vp))
		{
			// TODO(MarcasRealAccount): Free viewport data
			delete vd;
			vp->RendererUserData = nullptr;
		}

		PlatformShutdown();

		io.BackendRendererName     = nullptr;
		io.BackendRendererUserData = nullptr;
		delete bd;
	}

	void ImGuiRendererNewFrame()
	{
		RendererBackendData* bd = GetRendererBackendData();
		Assert(bd, "Did you call ImGuiRendererInit()?");

		if (!bd->pipeline)
			CreateDeviceObjects();
	}

	void ImGuiRendererDrawData(ImDrawData* drawData, CommandList* commandList)
	{
		if (drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f)
			return;

		RendererBackendData*  bd  = GetRendererBackendData();
		RendererViewportData* vd  = GetRendererViewportData(drawData->OwnerViewport);
		auto&                 rvb = vd->renderVertexBuffers[Context::FrameIndex()];
		auto&                 rib = vd->renderIndexBuffers[Context::FrameIndex()];

		if (!rvb || rvb->size() < drawData->TotalVtxCount * sizeof(ImDrawVert))
		{
			BufferInfo bufferInfo {};
			bufferInfo.heapType  = EHeapType::Upload;
			bufferInfo.alignment = 0;
			bufferInfo.size      = (drawData->TotalVtxCount + 5000) * sizeof(ImDrawVert);

			rvb = Buffer::Create(bufferInfo);
			Assert(rvb.get(), "Failed to initialize ImGui render vertex buffer!");
		}
		if (!rib || rib->size() < drawData->TotalIdxCount * sizeof(ImDrawIdx))
		{
			BufferInfo bufferInfo {};
			bufferInfo.heapType  = EHeapType::Upload;
			bufferInfo.alignment = 0;
			bufferInfo.size      = (drawData->TotalIdxCount + 10000) * sizeof(ImDrawIdx);

			rib = Buffer::Create(bufferInfo);
			Assert(rib.get(), "Failed to initialize ImGui render index buffer!");
		}

		auto        vtxResource = rvb->map();
		auto        idxResource = rib->map();
		ImDrawVert* vtxDst      = reinterpret_cast<ImDrawVert*>(vtxResource);
		ImDrawIdx*  idxDst      = reinterpret_cast<ImDrawIdx*>(idxResource);
		for (int i = 0; i < drawData->CmdListsCount; ++i)
		{
			const ImDrawList* cmdList = drawData->CmdLists[i];
			std::memcpy(vtxDst, cmdList->VtxBuffer.Data, cmdList->VtxBuffer.Size * sizeof(ImDrawVert));
			std::memcpy(idxDst, cmdList->IdxBuffer.Data, cmdList->IdxBuffer.Size * sizeof(ImDrawIdx));
			vtxDst += cmdList->VtxBuffer.Size;
			idxDst += cmdList->IdxBuffer.Size;
		}
		rvb->unmap();
		rib->unmap();

		SetupRenderState(drawData, commandList, rvb.get(), rib.get());

		std::uint32_t globalVtxOffset = 0;
		std::uint32_t globalIdxOffset = 0;
		ImVec2        clipOff         = drawData->DisplayPos;
		for (int i = 0; i < drawData->CmdListsCount; ++i)
		{
			const ImDrawList* cmdList = drawData->CmdLists[i];
			for (int cmdI = 0; cmdI < cmdList->CmdBuffer.Size; ++cmdI)
			{
				const ImDrawCmd* pcmd = &cmdList->CmdBuffer[cmdI];
				if (pcmd->UserCallback)
				{
					if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
						SetupRenderState(drawData, commandList, rvb.get(), rib.get());
					else
						pcmd->UserCallback(cmdList, pcmd);
				}
				else
				{
					ImVec2 clipMin(pcmd->ClipRect.x - clipOff.x, pcmd->ClipRect.y - clipOff.y);
					ImVec2 clipMax(pcmd->ClipRect.z - clipOff.x, pcmd->ClipRect.w - clipOff.y);
					if (clipMax.x <= clipMin.x || clipMax.y <= clipMin.y)
						continue;

					commandList->bindDescriptorTable(1, { bd->descriptorHeap, static_cast<std::uint32_t>(reinterpret_cast<std::uint64_t>(pcmd->GetTexID())) });
					commandList->setScissors({ Rect { static_cast<std::int32_t>(clipMin.x), static_cast<std::int32_t>(clipMin.y), static_cast<std::uint32_t>(clipMax.x), static_cast<std::uint32_t>(clipMax.y) } });
					commandList->drawIndexedInstanced(pcmd->ElemCount, 1, pcmd->IdxOffset + globalIdxOffset, pcmd->VtxOffset + globalVtxOffset, 0);
				}
			}
			globalIdxOffset += cmdList->IdxBuffer.Size;
			globalVtxOffset += cmdList->VtxBuffer.Size;
		}
	}

	static void CreateWindow(ImGuiViewport* vp);
	static void DestroyWindow(ImGuiViewport* vp);
	static void SetWindowSize(ImGuiViewport* vp, ImVec2 size);
	static void RenderWindow(ImGuiViewport* vp, void* renderArg);
	static void SwapBuffers(ImGuiViewport* vp, void* renderArg);

	void PlatformInit()
	{
		ImGuiPlatformIO& platformIO       = ImGui::GetPlatformIO();
		platformIO.Renderer_CreateWindow  = &CreateWindow;
		platformIO.Renderer_DestroyWindow = &DestroyWindow;
		platformIO.Renderer_SetWindowSize = &SetWindowSize;
		platformIO.Renderer_RenderWindow  = &RenderWindow;
		platformIO.Renderer_SwapBuffers   = &SwapBuffers;
	}

	void PlatformShutdown()
	{
		ImGui::DestroyPlatformWindows();
	}

	void CreateWindow(ImGuiViewport* vp)
	{
		RendererViewportData* vd = new RendererViewportData();
		vp->RendererUserData     = vd;

		SwapchainInfo swapchainInfo {};
		swapchainInfo.window      = reinterpret_cast<Window*>(vp->PlatformHandle);
		swapchainInfo.bufferCount = 3;

		vd->swapchain = Swapchain::Create(swapchainInfo).release();
		vd->owns      = true;
		vd->cmdListID = Context::NewCMDList();
		vd->renderVertexBuffers.resize(Context::FrameCount());
		vd->renderIndexBuffers.resize(Context::FrameCount());
	}

	void DestroyWindow(ImGuiViewport* vp)
	{
		RendererViewportData* vd = GetRendererViewportData(vp);
		if (!vd)
			return;
		if (vd->owns)
		{
			Context::DestroyCMDList(vd->cmdListID);
			Context::WaitForGPU();
			delete vd->swapchain;
		}
		delete vd;
		vp->RendererUserData = nullptr;
	}

	void SetWindowSize([[maybe_unused]] ImGuiViewport* vp, [[maybe_unused]] ImVec2 size) {}

	void RenderWindow(ImGuiViewport* vp, [[maybe_unused]] void* renderArg)
	{
		RendererBackendData*  bd = GetRendererBackendData();
		RendererViewportData* vd = GetRendererViewportData(vp);

		auto commandList = Context::CurrentCMDList(vd->cmdListID);

		commandList->begin();

		commandList->setDescriptorHeaps({ bd->descriptorHeap });

		vd->swapchain->bind(commandList);
		vd->swapchain->clear(commandList, 0.0f, 0.0f, 0.0f, 1.0f);

		ImGuiRendererDrawData(vp->DrawData, commandList);

		vd->swapchain->unbind(commandList);

		commandList->end();

		Context::ExecuteCommandLists({ commandList });
	}

	void SwapBuffers(ImGuiViewport* vp, [[maybe_unused]] void* renderArg)
	{
		RendererViewportData* vd = GetRendererViewportData(vp);
		vd->swapchain->present();
	}

	void CreateDeviceObjects()
	{
		RendererBackendData* bd = GetRendererBackendData();
		Assert(bd, "ImGui renderer backend missing!");

		if (!bd->pipelineLayout || !bd->pipelineLayout->initialized())
		{
			PipelineLayoutInfo pipelineLayoutInfo {};
			pipelineLayoutInfo.parameters.emplace_back(PipelineLayoutConstants { 0, 0, 16 }, EShaderVisibility::Vertex);
			pipelineLayoutInfo.parameters.emplace_back(PipelineLayoutDescriptorTable { { { EPipelineLayoutDescriptorRangeType::ShaderResourceView, 1, 0, 0, 0 } } }, EShaderVisibility::Pixel);
			pipelineLayoutInfo.staticSamplers.emplace_back(PipelineLayoutStaticSampler {
			    EFilter::Linear,
			    EFilter::Linear,
			    EImageAddressMode::Wrap,
			    EImageAddressMode::Wrap,
			    EImageAddressMode::Wrap,
			    0.0f,
			    0,
			    EComparisonFunc::Always,
			    EBorderColor::TransparentBlack,
			    0.0f,
			    0.0f,
			    0,
			    0,
			    EShaderVisibility::Pixel });
			pipelineLayoutInfo.flags |= PipelineLayoutFlags::AllowInputAssemblerInputLayout;
			bd->pipelineLayout = PipelineLayout::Create(pipelineLayoutInfo);
			if (!bd->pipelineLayout->initialized())
				return;
		}

		std::uint32_t compileFlags = 0;
		if constexpr (Core::s_IsConfigDebug)
			compileFlags |= DX12::D3DCOMPILE_DEBUG | DX12::D3DCOMPILE_SKIP_OPTIMIZATION;

		GraphicsPipelineInfo pipelineInfo {};
		pipelineInfo.pipelineLayout    = bd->pipelineLayout.get();
		pipelineInfo.vertexShader.data = DX12::CompileShader(L"Shaders/ImGui/ImGui.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0);
		pipelineInfo.pixelShader.data  = DX12::CompileShader(L"Shaders/ImGui/ImGui.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0);
		pipelineInfo.inputs.emplace_back(GraphicsPipelineInput { "POSITION", 0, EFormat::R32G32_FLOAT, 0, static_cast<std::uint32_t>(IM_OFFSETOF(ImDrawVert, pos)), 0 });
		pipelineInfo.inputs.emplace_back(GraphicsPipelineInput { "TEXCOORD", 0, EFormat::R32G32_FLOAT, 0, static_cast<std::uint32_t>(IM_OFFSETOF(ImDrawVert, uv)), 0 });
		pipelineInfo.inputs.emplace_back(GraphicsPipelineInput { "COLOR", 0, EFormat::R8G8B8A8_UNORM, 0, static_cast<std::uint32_t>(IM_OFFSETOF(ImDrawVert, col)), 0 });
		pipelineInfo.primitiveTopology = EPrimitiveTopology::Triangles;
		pipelineInfo.windingOrder      = EWindingOrder::CW;
		pipelineInfo.renderTargetCount = 1;
		pipelineInfo.rtvFormats[0]     = EFormat::R8G8B8A8_UNORM;

		pipelineInfo.blend.alphaCoverageEnable    = false;
		pipelineInfo.blend.independentBlendEnable = false;

		auto& renderTarget         = pipelineInfo.blend.renderTarget[0];
		renderTarget.blendEnable   = true;
		renderTarget.logicOpEnable = false;
		renderTarget.srcBlend      = EBlend::SrcAlpha;
		renderTarget.dstBlend      = EBlend::InvSrcAlpha;
		renderTarget.blendOp       = EBlendOp::Add;
		renderTarget.srcBlendAlpha = EBlend::One;
		renderTarget.dstBlendAlpha = EBlend::InvSrcAlpha;
		renderTarget.blendOpAlpha  = EBlendOp::Add;
		renderTarget.logicOp       = ELogicOp::Clear;
		renderTarget.writeMask     = 0xFF;

		bd->pipeline = GraphicsPipeline::Create(pipelineInfo);
		if (!bd->pipeline->initialized())
			return;

		CreateFontsTexture();
	}

	void CreateFontsTexture()
	{
		ImGuiIO&             io = ImGui::GetIO();
		RendererBackendData* bd = GetRendererBackendData();
		unsigned char*       pixels;
		int                  width, height;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

		std::unique_ptr<CommandAllocator> commandAllocator;
		{
			CommandAllocatorInfo info {};
			info.type        = ECommandListType::Direct;
			commandAllocator = CommandAllocator::Create(info);
		}
		if (!commandAllocator->initialized())
			return;

		auto loadCommandLists = commandAllocator->allocate(1);
		if (loadCommandLists.empty())
			return;
		auto& loadCommandList = loadCommandLists[0];
		loadCommandList->begin(nullptr);

		std::unique_ptr<Buffer> fontImageStagingBuffer;
		{
			ImageInfo imageInfo {};
			imageInfo.type         = EImageType::_2D;
			imageInfo.format       = EFormat::R8G8B8A8_UNORM;
			imageInfo.initialState = ImageState::CopyDst;
			imageInfo.alignment    = 0;
			imageInfo.width        = static_cast<std::uint16_t>(width);
			imageInfo.height       = static_cast<std::uint16_t>(height);
			imageInfo.depth        = 1;
			imageInfo.flags        = ImageFlags::None;
			bd->fontImage          = Image::Create(imageInfo);
			Assert(bd->fontImage->initialized(), "Failed to initialize imgui font image!");

			std::uint32_t uploadPitch = static_cast<std::uint32_t>((width * 4 + 255) & ~(255));
			std::uint64_t stagingSize = bd->fontImage->requiredBufferSize();

			BufferInfo imageStagingInfo {};
			imageStagingInfo.heapType  = EHeapType::Upload;
			imageStagingInfo.alignment = 0;
			imageStagingInfo.size      = stagingSize;
			fontImageStagingBuffer     = Buffer::Create(imageStagingInfo);
			Assert(fontImageStagingBuffer->initialized(), "Failed to initialize imgui font image staging buffer!");

			void* mapped = fontImageStagingBuffer->map();
			for (int y = 0; y < height; y++)
				memcpy((void*) ((uintptr_t) mapped + y * uploadPitch), pixels + y * width * 4, width * 4);
			fontImageStagingBuffer->unmap();

			// TODO(MarcasRealAccount): Do something about Row Pitch!
			bd->fontImage->copyFrom(loadCommandList.get(), { fontImageStagingBuffer.get(), 0, EFormat::R8G8B8A8_UNORM, static_cast<std::uint16_t>(width), static_cast<std::uint16_t>(height), 1, uploadPitch });
			bd->fontImage->transition(loadCommandList.get(), ImageState::PixelShaderResource);
		}

		ImageView view {};
		view.image     = bd->fontImage.get();
		view.type      = EImageType::_2D;
		view.mipLevels = 1;

		bd->fontImageDescriptor = bd->descriptorHeap->createImageView(view);
		io.Fonts->SetTexID(reinterpret_cast<ImTextureID>(static_cast<std::uint64_t>(bd->fontImageDescriptor.index())));
		loadCommandList->end();

		Context::ExecuteCommandLists({ loadCommandList.get() });

		Context::WaitForGPU();
	}

	void SetupRenderState(ImDrawData* drawData, CommandList* commandList, Buffer* rvb, Buffer* rib)
	{
		RendererBackendData* bd = GetRendererBackendData();

		struct VertexConstantBuffer
		{
			float mvp[4][4];
		} vertexConstantBuffer;
		{
			float L = drawData->DisplayPos.x;
			float R = drawData->DisplayPos.x + drawData->DisplaySize.x;
			float T = drawData->DisplayPos.y;
			float B = drawData->DisplayPos.y + drawData->DisplaySize.y;
			float mvp[4][4] {
				{ 2.0f / (R - L), 0.0f, 0.0f, 0.0f },
				{ 0.0f, 2.0f / (T - B), 0.0f, 0.0f },
				{ 0.0f, 0.0f, 0.5f, 0.0f },
				{ (R + L) / (L - R), (T + B) / (B - T), 0.5f, 1.0f }
			};
			std::memcpy(vertexConstantBuffer.mvp, mvp, sizeof(mvp));
		}

		commandList->setVertexBuffers(0, { { rvb, 0, static_cast<std::uint32_t>(rvb->size()), sizeof(ImDrawVert) } });
		commandList->setIndexBuffer({ rib, 0, static_cast<std::uint32_t>(rib->size()), sizeof(ImDrawIdx) == 2 ? EFormat::R16_UINT : EFormat::R32_UINT });
		commandList->setPrimitiveTopology(EPrimitiveTopology::Triangles);
		bd->pipeline->bind(commandList);
		commandList->bind32BitConstants(0, 16, &vertexConstantBuffer, 0);

		const float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
		commandList->setBlendFactor(blendFactor);
	}
} // namespace Brainfryer::Editor
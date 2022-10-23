#pragma once

#include "Brainfryer/Utils/Flags.h"
#include "Brainfryer/Utils/Point.h"
#include "Brainfryer/Utils/Rect.h"
#include "BufferView.h"
#include "Format.h"

#include <memory>

namespace Brainfryer
{
	class CommandList;

	using EImageFlags = Utils::Flags<>;
	using EImageState = Utils::Flags<>;

	namespace ImageFlags
	{
		static constexpr EImageFlags None              = 0x0;
		static constexpr EImageFlags AllowRenderTarget = 0x1;
		static constexpr EImageFlags AllowDepthStencil = 0x2;
	}; // namespace ImageFlags

	namespace ImageState
	{
		static constexpr EImageState Common                 = 0x0000000;
		static constexpr EImageState RenderTarget           = 0x0000004;
		static constexpr EImageState UnorderedAccess        = 0x0000008;
		static constexpr EImageState DepthWrite             = 0x0000010;
		static constexpr EImageState DepthRead              = 0x0000020;
		static constexpr EImageState NonPixelShaderResource = 0x0000040;
		static constexpr EImageState PixelShaderResource    = 0x0000080;
		static constexpr EImageState IndirectArgument       = 0x0000200;
		static constexpr EImageState CopyDst                = 0x0000400;
		static constexpr EImageState CopySrc                = 0x0000800;
		static constexpr EImageState ResolveDst             = 0x0001000;
		static constexpr EImageState ResolveSrc             = 0x0002000;
		static constexpr EImageState ShadingRateSrc         = 0x1000000;
	}; // namespace ImageState

	enum class EImageType
	{
		_1D,
		_1DArray,
		_2D,
		_2DArray,
		_3D
	};

	struct ImageInfo
	{
	public:
		EImageType    type;
		EFormat       format;
		EImageState   initialState;
		std::uint32_t alignment;
		std::uint16_t width;
		std::uint16_t height;
		std::uint16_t depth;
		EImageFlags   flags;
	};

	class Image
	{
	public:
		static std::unique_ptr<Image> Create(const ImageInfo& info);

	public:
		virtual ~Image() = default;

		virtual void copyFrom(CommandList* commandList, BufferImageView bufferView, Point3D destOffset = { 0, 0, 0 }, Rect3D bufferRect = { -1, -1, -1, 0, 0, 0 }) = 0;

		virtual void transition(CommandList* commandList, EImageState state) = 0;

		virtual std::uint64_t requiredBufferSize() = 0;

		virtual EImageType    type() const   = 0;
		virtual EFormat       format() const = 0;
		virtual EImageState   state() const  = 0;
		virtual std::uint16_t width() const  = 0;
		virtual std::uint16_t height() const = 0;
		virtual std::uint16_t depth() const  = 0;

		virtual bool isRenderTarget() const = 0;
		virtual bool isDepthStencil() const = 0;

		virtual bool initialized() const = 0;
	};
} // namespace Brainfryer
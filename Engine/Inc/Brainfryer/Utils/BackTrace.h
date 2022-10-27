#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include <spdlog/fmt/fmt.h>

namespace Brainfryer::Utils
{
	struct SourceLocation
	{
	public:
		SourceLocation()
		    : m_Line(0),
		      m_Column(0) {}

		void setFile(std::filesystem::path file, std::size_t line, std::size_t column)
		{
			m_File   = std::move(file);
			m_Line   = line;
			m_Column = column;
		}
		void setFunction(std::string function) { m_Function = std::move(function); }

		auto& file() const { return m_File; }
		auto& function() const { return m_Function; }
		auto  line() const { return m_Line; }
		auto  column() const { return m_Column; }

	private:
		std::filesystem::path m_File;
		std::string           m_Function;
		std::size_t           m_Line;
		std::size_t           m_Column;
	};

	struct StackFrame
	{
	public:
		StackFrame() : m_Address(nullptr), m_Offset(0) {}
		StackFrame(void* address, std::size_t offset, SourceLocation source) : m_Address(address), m_Offset(offset), m_Source(std::move(source)) {}

		void setAddress(void* address, std::size_t offset)
		{
			m_Address = address;
			m_Offset  = offset;
		}
		void setSource(SourceLocation source) { m_Source = std::move(source); }

		auto  address() const { return m_Address; }
		auto  offset() const { return m_Offset; }
		auto& source() const { return m_Source; }

		bool hasSource() const { return !m_Source.file().empty(); }

	private:
		void*          m_Address;
		std::size_t    m_Offset;
		SourceLocation m_Source;
	};

	struct BackTrace
	{
	public:
		auto& frames() { return m_Frames; }
		auto& frames() const { return m_Frames; }

	private:
		std::vector<StackFrame> m_Frames;
	};

	BackTrace CaptureBackTrace(std::uint32_t skip, std::uint32_t maxFrames);
} // namespace Brainfryer::Utils

template <>
struct fmt::formatter<Brainfryer::Utils::SourceLocation>
{
public:
	constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
	{
		return ctx.begin();
	}

	template <class FormatContext>
	auto format(const Brainfryer::Utils::SourceLocation& location, FormatContext& ctx) -> decltype(ctx.out())
	{
		return fmt::format_to(ctx.out(), "{}({}:{}): {}()", location.file().string(), location.line(), location.column(), location.function());
	}
};

template <>
struct fmt::formatter<Brainfryer::Utils::BackTrace>
{
public:
	constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
	{
		return ctx.begin();
	}

	template <class FormatContext>
	auto format(const Brainfryer::Utils::BackTrace& backTrace, FormatContext& ctx) -> decltype(ctx.out())
	{
		if (backTrace.frames().empty())
			return ctx.out();

		auto out     = ctx.out();
		bool newline = false;
		for (auto& frame : backTrace.frames())
		{
			if (newline)
				out = fmt::format_to(out, "\n");
			newline = true;
			if (frame.hasSource())
				out = fmt::format_to(out, "At {}", frame.source());
			else
				out = fmt::format_to(out, "At {:p} + {:X}", frame.address(), frame.offset());
		}

		return out;
	}
};
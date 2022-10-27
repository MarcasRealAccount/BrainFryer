#pragma once

#include "BackTrace.h"

#include <spdlog/fmt/fmt.h>

#include <utility>

namespace Brainfryer
{
	namespace Utils
	{
		struct Exception
		{
		public:
			Exception(std::string title, std::string message, BackTrace backTrace = CaptureBackTrace(0, 10))
			    : m_Title(std::move(title)), m_Message(std::move(message)), m_BackTrace(std::move(backTrace)) {}
			template <class... Args>
			Exception(std::string title, fmt::format_string<Args...> format, Args&&... args, BackTrace backTrace = CaptureBackTrace(0, 10))
			    : m_Title(std::move(title)), m_Message(fmt::format(format, std::forward<Args>(args)...)), m_BackTrace(std::move(backTrace))
			{
			}

			auto& title() const { return m_Title; }
			auto& what() const { return m_Message; }
			auto& backTrace() const { return m_BackTrace; }

		private:
			std::string m_Title;
			std::string m_Message;
			BackTrace   m_BackTrace;
		};

		BackTrace& LastBackTrace();
		void       HookThrow();
	} // namespace Utils

	inline void Assert(bool condition, std::string message)
	{
		if (!condition)
			throw Utils::Exception("Assertion", std::move(message), Utils::CaptureBackTrace(1, 10));
	}

	template <class... Args>
	void Assert(bool condition, fmt::format_string<Args...> format, Args&&... args)
	{
		if (!condition)
			throw Utils::Exception("Assertion", std::move(format), std::forward<Args>(args)..., Utils::CaptureBackTrace(1, 10));
	}
} // namespace Brainfryer

template <>
struct fmt::formatter<Brainfryer::Utils::Exception>
{
public:
	constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin())
	{
		return ctx.begin();
	}

	template <class FormatContext>
	auto format(const Brainfryer::Utils::Exception& exception, FormatContext& ctx) -> decltype(ctx.out())
	{
		if (exception.backTrace().frames().empty())
			return fmt::format_to(ctx.out(), "{}: \"{}\"", exception.title(), exception.what());
		else
			return fmt::format_to(ctx.out(), "{}: \"{}\"\n{}", exception.title(), exception.what(), exception.backTrace());
	}
};
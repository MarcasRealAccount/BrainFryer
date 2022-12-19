#pragma once

#include "BackTrace.h"
#include "Brainfryer/Window/Window.h"
#include "Log.h"

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

	template <class F, class... Args>
	std::uint32_t SafeExecute(F&& f, Args&&... args)
	{
		try
		{
			f(std::forward<Args>(args)...);
			return 0;
		}
		catch (const Utils::Exception& exception)
		{
			Log::GetOrCreateLogger(exception.title())->critical("{}", exception);
			Window::FatalErrorBox(exception.what(), exception.title(), exception.backTrace());
			return 0x7FFF'FFFF;
		}
		catch (const std::exception& exception)
		{
			auto& backtrace = Utils::LastBackTrace();
			if (backtrace.frames().empty())
				Log::Critical("{}", exception.what());
			else
				Log::Critical("{}\n{}", exception.what(), backtrace);
			Window::FatalErrorBox(exception.what(), "", backtrace);
			return 0x0000'7FFF;
		}
		catch (...)
		{
			auto& backtrace = Utils::LastBackTrace();
			if (backtrace.frames().empty())
				Log::Critical("Uncaught exception occurred");
			else
				Log::Critical("Uncaught exception occurred\n{}", backtrace);
			Window::FatalErrorBox("Uncaught exception occurred", "", backtrace);
			return 0x007F'FFFF;
		}
	}

	inline void Assert(bool condition, std::string message)
	{
		if constexpr (Core::s_IsConfigDebug)
		{
			if (!condition)
			{
				if constexpr (!Core::s_IsConfigDist)
					__debugbreak();

				throw Utils::Exception("Assertion", std::move(message), Utils::CaptureBackTrace(1, 10));
			}
		}
	}

	template <class... Args>
	void Assert(bool condition, fmt::format_string<Args...> format, Args&&... args)
	{
		if constexpr (Core::s_IsConfigDebug)
		{
			if (!condition)
			{
				if constexpr (!Core::s_IsConfigDist)
					__debugbreak();

				throw Utils::Exception("Assertion", std::move(format), std::forward<Args>(args)..., Utils::CaptureBackTrace(1, 10));
			}
		}
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
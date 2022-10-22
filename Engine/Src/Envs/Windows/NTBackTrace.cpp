#include "Brainfryer/Envs/Windows/NTBackTrace.h"
#include "Brainfryer/Envs/Windows/NT.h"
#include "Brainfryer/Utils/UTFConv.h"

namespace Brainfryer::Windows
{
	static struct NTSymInitializer
	{
	public:
		NTSymInitializer()
		    : m_Process(GetCurrentProcess())
		{
			if constexpr (Core::s_IsSystemWindows)
			{
				SymSetOptions(SYMOPT_UNDNAME | SYMOPT_LOAD_LINES);
				SymInitializeW(m_Process, L"", true);
			}
		}

		~NTSymInitializer()
		{
			SymCleanup(m_Process);
		}

		HANDLE m_Process;
	} s_SymInitializer;

	Utils::BackTrace CaptureBackTrace(std::uint32_t skip, std::uint32_t maxFrames)
	{
		Utils::BackTrace backtrace;

		void** framePtrs  = new void*[maxFrames];
		WORD   frameCount = RtlCaptureStackBackTrace(2 + skip, maxFrames, framePtrs, nullptr);

		alignas(SYMBOL_INFOW) std::uint8_t symbolBuffer[sizeof(SYMBOL_INFOW) + MAX_SYM_NAME * sizeof(wchar_t)];

		auto& frames = backtrace.frames();
		frames.resize(frameCount);
		for (std::size_t i = 0; i < frameCount; ++i)
		{
			auto&         frame   = frames[i];
			void*         address = framePtrs[i];
			std::uint64_t offset  = 0;

			SYMBOL_INFOW* symbol = reinterpret_cast<SYMBOL_INFOW*>(&symbolBuffer);
			symbol->SizeOfStruct = sizeof(SYMBOL_INFOW);
			symbol->MaxNameLen   = MAX_SYM_NAME;

			if (SymFromAddrW(s_SymInitializer.m_Process, reinterpret_cast<std::uint64_t>(address), &offset, symbol))
			{
				frame.setAddress(reinterpret_cast<void*>(symbol->Address), offset);

				Utils::SourceLocation source;
				source.setFunction(Utils::UTF::ConvertWideToUTF8(symbol->Name));

				std::uint32_t    column = 0;
				IMAGEHLP_LINEW64 line {};
				line.SizeOfStruct = sizeof(IMAGEHLP_LINEW64);
				if (SymGetLineFromAddrW64(s_SymInitializer.m_Process, reinterpret_cast<std::uint64_t>(address), &column, &line))
					source.setFile(Utils::UTF::ConvertWideToUTF8(line.FileName), line.LineNumber, column);

				frame.setSource(std::move(source));
			}
			else
			{
				frame.setAddress(address, 0);
			}
		}

		delete[] framePtrs;

		return backtrace;
	}
} // namespace Brainfryer::Windows
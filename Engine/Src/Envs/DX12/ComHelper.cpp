#include "Brainfryer/Envs/DX12/ComHelper.h"
#include "Brainfryer/Envs/Windows/Win32.h"
#include "Brainfryer/Utils/Exception.h"
#include "Brainfryer/Utils/Log.h"
#include "Brainfryer/Utils/UTFConv.h"

namespace Brainfryer::DX12
{
	class ComError
	{
	public:
		ComError(HRESULT hr, IErrorInfo* perrinfo = nullptr, bool fAddRef = false) noexcept
		    : m_HResult(hr), m_ErrorInfo(perrinfo), m_Message(nullptr)
		{
			if (m_ErrorInfo && fAddRef)
				m_ErrorInfo->AddRef();
		}
		ComError(const ComError& copy) noexcept
		    : m_HResult(copy.m_HResult), m_ErrorInfo(copy.m_ErrorInfo), m_Message(nullptr)
		{
			if (m_ErrorInfo)
				m_ErrorInfo->AddRef();
		}
		virtual ~ComError() noexcept
		{
			if (m_ErrorInfo)
				m_ErrorInfo->Release();
			if (m_Message)
				Windows::LocalFree(m_Message);
		}

		ComError& operator=(const ComError& copy) noexcept
		{
			if (m_ErrorInfo)
				m_ErrorInfo->Release();
			if (m_Message)
				Windows::LocalFree(m_Message);

			m_HResult   = copy.m_HResult;
			m_ErrorInfo = copy.m_ErrorInfo;
			m_Message   = nullptr;
			if (m_ErrorInfo)
				m_ErrorInfo->AddRef();
			return *this;
		}

		HRESULT Error() const noexcept
		{
			return m_HResult;
		}

		WORD WCode() const noexcept
		{
			return HRESULTToWCode(m_HResult);
		}

		IErrorInfo* ErrorInfo() const noexcept
		{
			if (m_ErrorInfo)
				m_ErrorInfo->AddRef();
			return m_ErrorInfo;
		}

		wchar_t* Description() const
		{
			wchar_t* bstr = nullptr;
			if (m_ErrorInfo)
				m_ErrorInfo->GetDescription(&bstr);
			return bstr;
		}

		DWORD HelpContext() const noexcept
		{
			DWORD helpContext = 0;
			if (m_ErrorInfo)
				m_ErrorInfo->GetHelpContext(&helpContext);
			return helpContext;
		}

		wchar_t* HelpFile() const
		{
			wchar_t* bstr = nullptr;
			if (m_ErrorInfo)
				m_ErrorInfo->GetHelpFile(&bstr);
			return bstr;
		}

		wchar_t* Source() const
		{
			wchar_t* bstr = nullptr;
			if (m_ErrorInfo)
				m_ErrorInfo->GetSource(&bstr);
			return bstr;
		}

		GUID GUID() const noexcept
		{
			::GUID guid { 0, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0 } };
			if (m_ErrorInfo)
				m_ErrorInfo->GetGUID(&guid);
			return guid;
		}

		const wchar_t* ErrorMessage() const noexcept
		{
			if (m_Message)
				return m_Message;

			Windows::FormatMessageW(Windows::FORMAT_MESSAGE_ALLOCATE_BUFFER |
			                            Windows::FORMAT_MESSAGE_FROM_SYSTEM |
			                            Windows::FORMAT_MESSAGE_IGNORE_INSERTS,
			                        nullptr,
			                        m_HResult,
			                        Windows::MAKELANGID(Windows::LANG_NEUTRAL, Windows::SUBLANG_DEFAULT),
			                        reinterpret_cast<LPWSTR>(&m_Message),
			                        0,
			                        nullptr);
			if (m_Message)
			{
				std::size_t nLen = Windows::lstrlenW(m_Message);
				if (nLen > 1 && m_Message[nLen - 1] == '\n')
				{
					m_Message[nLen - 1] = 0;
					if (nLen > 2 && m_Message[nLen - 2] == '\r')
						m_Message[nLen - 2] = 0;
				}
			}
			else
			{
				m_Message = reinterpret_cast<wchar_t*>(Windows::LocalAlloc(0, 32 * sizeof(wchar_t)));
				if (m_Message)
				{
					WORD wCode = WCode();
					if (wCode)
						swprintf(m_Message, 32, L"IDispatch error #%d", wCode);
					else
						swprintf(m_Message, 32, L"Unknown error 0x%0lX", m_HResult);
				}
			}
			return m_Message;
		}

		static HRESULT WCodeToHRESULT(WORD wCode) noexcept
		{
			return wCode >= 0xFE00 ? WCODE_HRESULT_LAST : (WCODE_HRESULT_FIRST + wCode);
		}

		static WORD HRESULTToWCode(HRESULT hr) noexcept
		{
			return (hr >= WCODE_HRESULT_FIRST && hr <= WCODE_HRESULT_LAST) ? WORD(hr - WCODE_HRESULT_FIRST) : 0;
		}

	private:
		enum
		{
			WCODE_HRESULT_FIRST = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x200),
			WCODE_HRESULT_LAST  = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF + 1, 0) - 1
		};
		HRESULT          m_HResult;
		IErrorInfo*      m_ErrorInfo;
		mutable wchar_t* m_Message;
	};

	std::string HRMessage(HRESULT hr)
	{
		return Utils::UTF::ConvertWideToUTF8(ComError { hr }.ErrorMessage());
	}

	void HRVLT(HRESULT hr)
	{
		if (hr == 0)
			return;

		std::string msg = HRMessage(hr);
		if (hr < 0)
			throw Utils::Exception("DX12", std::move(msg), Utils::CaptureBackTrace(1, 10));

		Log::GetOrCreateLogger("DX12")->info("{}", msg);
	}
} // namespace Brainfryer::DX12
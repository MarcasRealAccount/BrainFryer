#pragma once

#include "Brainfryer/Envs/Windows/Types.h"
#include "Brainfryer/Utils/Core.h"

#include <concepts>

#if BUILD_IS_SYSTEM_WINDOWS
#define DX12_UUIDOF(x) __uuidof(x)
#else
#define DX12_UUIDOF(x)             \
	{                              \
		0, 0, 0,                   \
		{                          \
			0, 0, 0, 0, 0, 0, 0, 0 \
		}                          \
	}
#endif

using GUID = struct _GUID
{
	std::uint32_t Data1;
	std::uint16_t Data2;
	std::uint16_t Data3;
	std::uint8_t  Data4[8];
};
#define GUID_DEFINED

namespace Brainfryer::DX12
{
	using namespace Brainfryer::Windows::Types;

	using HRESULT = std::int32_t;

	constexpr bool HRValidate(HRESULT hr) { return hr >= 0; }
	std::string    HRMessage(HRESULT hr);

	template <class T>
	concept ComInterface = requires(T* t) {
		                       {
			                       t->AddRef()
			                       } -> std::same_as<DWORD>;
		                       {
			                       t->Release()
			                       } -> std::same_as<DWORD>;
	                       };

	template <class U, class T>
	concept ComCastableTo = ComInterface<U> &&
	                        ComInterface<T> &&
	                        (std::derived_from<U, T> ||
	                         std::derived_from<T, U>) &&
	                        requires(U* u, T** t) {
		                        {
			                        u->QueryInterface(t)
			                        } -> std::same_as<HRESULT>;
	                        };

	template <ComInterface T>
	struct Com
	{
	public:
		static constexpr GUID UUID = DX12_UUIDOF(T);

	public:
		Com() : m_Ptr(nullptr) {}
		Com(const Com& copy) : m_Ptr(copy.m_Ptr) { addRef(); }
		Com(Com&& move) noexcept : m_Ptr(move.m_Ptr) { move.m_Ptr = nullptr; }
		template <ComCastableTo<T> U>
		Com(Com<U>& copy)
		    : m_Ptr(nullptr)
		{
			if (copy.valid())
				copy->QueryInterface(&m_Ptr);
		}
		~Com() { release(); }

		Com& operator=(const Com& copy)
		{
			release();
			m_Ptr = copy.m_Ptr;
			addRef();
			return *this;
		}
		Com& operator=(Com&& move) noexcept
		{
			release();
			m_Ptr      = move.m_Ptr;
			move.m_Ptr = nullptr;
			return *this;
		}
		template <ComCastableTo<T> U>
		Com& operator=(Com<U>& copy)
		{
			release();
			if (copy.valid())
				copy->QueryInterface(&m_Ptr);
			return *this;
		}

		void addRef()
		{
			if (m_Ptr)
				m_Ptr->AddRef();
		}

		void release()
		{
			if (m_Ptr)
				m_Ptr->Release();
			m_Ptr = nullptr;
		}

		bool valid() const { return !!m_Ptr; }

		T*       get() { return m_Ptr; }
		const T* get() const { return m_Ptr; }

		T*       operator->() { return m_Ptr; }
		const T* operator->() const { return m_Ptr; }
		T&       operator*() { return *m_Ptr; }
		const T& operator*() const { return *m_Ptr; }
		         operator T&() { return *m_Ptr; }
		         operator const T&() const { return *m_Ptr; }

		operator T**() { return &m_Ptr; }
		operator const T* const*() const { return &m_Ptr; }
		operator void**() { return reinterpret_cast<void**>(&m_Ptr); }
		operator const void* const*() const { return reinterpret_cast<const void* const*>(&m_Ptr); }

		operator const GUID&() const { return UUID; }

	private:
		T* m_Ptr;
	};
} // namespace Brainfryer::DX12
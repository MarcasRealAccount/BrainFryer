#pragma once

#include "UID.h"

#include <functional>
#include <type_traits>
#include <unordered_map>

namespace Brainfryer::Utils
{
	template <bool B, class T1, class T2>
	struct Select
	{
		using Type = T1;
	};

	template <class T1, class T2>
	struct Select<true, T1, T2>
	{
		using Type = T2;
	};

	template <class T>
	concept NonRValue = !
	std::is_rvalue_reference_v<T>;

	template <class T>
	using LargeByRef = typename Select<sizeof(T) <= 16, T&, T>::Type;

	template <NonRValue... Args>
	class Event
	{
	public:
		using CallbackType = std::function<void(LargeByRef<Args>...)>;

	public:
		void invoke(LargeByRef<Args>... args)
		{
			for (auto& callback : m_Callbacks)
				callback.second(args...);
		}

		void operator()(LargeByRef<Args>... args) { invoke(args...); }

		UID attachCallback(CallbackType callback)
		{
			UID id = UID::Random(0);
			while (m_Callbacks.find(id) != m_Callbacks.end())
				id = UID::Random(0);
			m_Callbacks.insert_or_assign(id, std::move(callback));
			return id;
		}

		void detachCallback(UID id)
		{
			auto itr = m_Callbacks.find(id);
			if (itr != m_Callbacks.end())
				m_Callbacks.erase(itr);
		}

		UID operator+=(CallbackType callback)
		{
			return attachCallback(std::move(callback));
		}

		Event& operator-=(UID id)
		{
			detachCallback(id);
			return *this;
		}

	private:
		std::unordered_map<UID, CallbackType> m_Callbacks;
	};
} // namespace Brainfryer::Utils
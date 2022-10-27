#pragma once

#include <cstdint>

#include <functional>

namespace Brainfryer
{
	struct UID
	{
	public:
		static constexpr std::uint64_t c_IDBits = 48;
		static constexpr std::uint64_t c_IDMask = (1ULL << c_IDBits) - 1ULL;

	public:
		static UID Random(std::uint16_t type);

	public:
		constexpr UID() : m_Value(0) {}
		constexpr UID(std::uint16_t type, std::uint64_t id)
		    : m_Value(static_cast<std::uint64_t>(type) << c_IDBits | (id & c_IDMask)) {}
		constexpr UID(const UID& copy)
		    : m_Value(copy.m_Value) {}

		operator std::uint64_t() const { return m_Value; }

		std::uint16_t type() const { return static_cast<std::uint16_t>(m_Value >> c_IDBits); }
		std::uint64_t id() const { return m_Value & c_IDMask; }

	private:
		std::uint64_t m_Value;
	};
} // namespace Brainfryer

template <>
struct std::hash<Brainfryer::UID>
{
public:
	std::size_t operator()(Brainfryer::UID uid) const
	{
		return uid;
	}
};
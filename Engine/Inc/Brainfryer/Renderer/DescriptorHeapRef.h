#pragma once

#include <cstdint>

namespace Brainfryer
{
	class DescriptorHeap;

	struct DescriptorHeapRef
	{
	public:
		DescriptorHeapRef();
		DescriptorHeapRef(DescriptorHeap* heap, std::uint32_t index);
		DescriptorHeapRef(const DescriptorHeapRef& copy);
		DescriptorHeapRef(DescriptorHeapRef&& move) noexcept;

		DescriptorHeapRef& operator=(const DescriptorHeapRef& copy);
		DescriptorHeapRef& operator=(DescriptorHeapRef&& move) noexcept;
		~DescriptorHeapRef();

		auto heap() const { return m_Heap; }
		auto index() const { return m_Index; }

	private:
		DescriptorHeap* m_Heap;
		std::uint32_t   m_Index;
	};
} // namespace Brainfryer
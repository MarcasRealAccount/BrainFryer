#include "Brainfryer/Renderer/DescriptorHeapRef.h"
#include "Brainfryer/Renderer/DescriptorHeap.h"

namespace Brainfryer
{
	DescriptorHeapRef::DescriptorHeapRef()
	    : m_Heap(nullptr),
	      m_Index(0) {}

	DescriptorHeapRef::DescriptorHeapRef(DescriptorHeap* heap, std::uint32_t index)
	    : m_Heap(heap),
	      m_Index(index)
	{
		if (m_Heap)
			m_Heap->incRef(m_Index);
	}

	DescriptorHeapRef::DescriptorHeapRef(const DescriptorHeapRef& copy)
	    : m_Heap(copy.m_Heap),
	      m_Index(copy.m_Index)
	{
		if (m_Heap)
			m_Heap->incRef(m_Index);
	}

	DescriptorHeapRef::DescriptorHeapRef(DescriptorHeapRef&& move) noexcept
	    : m_Heap(move.m_Heap),
	      m_Index(move.m_Index)
	{
		move.m_Heap  = nullptr;
		move.m_Index = 0;
	}

	DescriptorHeapRef& DescriptorHeapRef::operator=(const DescriptorHeapRef& copy)
	{
		if (m_Heap)
			m_Heap->decRef(m_Index);
		m_Heap  = copy.m_Heap;
		m_Index = copy.m_Index;
		if (m_Heap)
			m_Heap->incRef(m_Index);
		return *this;
	}

	DescriptorHeapRef& DescriptorHeapRef::operator=(DescriptorHeapRef&& move) noexcept
	{
		if (m_Heap)
			m_Heap->decRef(m_Index);
		m_Heap       = move.m_Heap;
		m_Index      = move.m_Index;
		move.m_Heap  = nullptr;
		move.m_Index = 0;
		return *this;
	}

	DescriptorHeapRef::~DescriptorHeapRef()
	{
		if (m_Heap)
			m_Heap->decRef(m_Index);
	}
} // namespace Brainfryer
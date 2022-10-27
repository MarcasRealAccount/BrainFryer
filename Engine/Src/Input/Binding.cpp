#include "Brainfryer/Input/Binding.h"

namespace Brainfryer::Input
{
	bool operator==(Binding lhs, Binding rhs)
	{
		return lhs.m_Location == rhs.m_Location && lhs.m_DeviceID == rhs.m_DeviceID && lhs.m_Index == rhs.m_Index;
	}
} // namespace Brainfryer::Input
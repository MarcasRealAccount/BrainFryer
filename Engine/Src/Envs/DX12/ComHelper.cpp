#include "Brainfryer/Envs/DX12/ComHelper.h"
#include "Brainfryer/Utils/UTFConv.h"

#include <comdef.h>

namespace Brainfryer::DX12
{
	std::string HRMessage(HRESULT hr)
	{
		return Utils::UTF::ConvertWideToUTF8(_com_error { hr }.ErrorMessage());
	}
} // namespace Brainfryer::DX12
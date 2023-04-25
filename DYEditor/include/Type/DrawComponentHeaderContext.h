#pragma once

namespace DYE::DYEditor
{
	struct DrawComponentHeaderContext
	{
		bool ComponentChanged = false;
		bool IsModificationActivated = false;
		bool IsModificationDeactivated = false;
		bool IsModificationDeactivatedAfterEdit = false;
	};
}
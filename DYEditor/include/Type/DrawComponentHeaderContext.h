#pragma once

namespace DYE::DYEditor
{
	struct DrawComponentHeaderContext
	{
		char const *DrawnComponentTypeName = nullptr;
		bool IsInDebugMode = false;

		bool ComponentChanged = false;
		bool IsModificationActivated = false;
		bool IsModificationDeactivated = false;
		bool IsModificationDeactivatedAfterEdit = false;
	};
}
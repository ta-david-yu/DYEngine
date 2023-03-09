#include "UserTypeRegister.h"

#include "Util/Logger.h"

namespace DYE::DYEditor
{
	RegisterUserTypeFunction* RegisterUserTypeFunctionPointer = nullptr;

	bool AssignRegisterUserTypeFunctionPointer(RegisterUserTypeFunction* functionPointer)
	{
		RegisterUserTypeFunctionPointer = functionPointer;
		return true;
	}

	void RegisterUserTypes()
	{
		if (RegisterUserTypeFunctionPointer == nullptr)
		{
			return;
		}

		DYE_LOG("<< Register User Types to DYEditor::TypeRegistry >>");
		RegisterUserTypeFunctionPointer();
	}
}
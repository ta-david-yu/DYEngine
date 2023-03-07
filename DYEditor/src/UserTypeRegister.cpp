#include "UserTypeRegister.h"

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

		RegisterUserTypeFunctionPointer();
	}
}
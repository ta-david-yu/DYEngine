#pragma once

namespace DYE::DYEditor
{
    using RegisterUserTypeFunction = void();

    bool AssignRegisterUserTypeFunctionPointer(RegisterUserTypeFunction *functionPointer);

    void RegisterUserTypes();
}

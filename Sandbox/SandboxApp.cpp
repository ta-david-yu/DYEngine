#include "AppEntryPoint.h"
#include "Application.h"

///
/// \return a new allocated Application object
DYE::Application * DYE::CreateApplication()
{
    return new Application { "Main App", 60 };
}

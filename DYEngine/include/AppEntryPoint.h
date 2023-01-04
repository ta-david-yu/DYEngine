#pragma once

#include "Core/Application.h"

// when using SDL, the main function should be like `int main(int argc, char **argv)`
int main(int argc, char *argv[])
{
    auto pApp = DYE::CreateApplication();

	DYE::Application::RegisterApplication(pApp);

	pApp->Run();

    delete pApp;

    return 0;
}
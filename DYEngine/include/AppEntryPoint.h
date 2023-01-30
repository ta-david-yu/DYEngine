#pragma once

#include "Core/Application.h"

// when using SDL, the main function should be like `int main(int argc, char **argv)`
int main(int argc, char *argv[])
{
    auto pApp = DYE::CreateApplication();

	pApp->Run();

    delete pApp;

    return 0;
}
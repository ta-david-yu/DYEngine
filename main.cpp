#include <iostream>
#include "Application.h"

// when using SDL, the main function should be like `int main(int argc, char **argv)`
int main(int argc, char *argv[])
{
    auto app = DYE::Application();
    app.Run();

    return 0;
}

#include <iostream>
#include "Application.h"
#include "ApplicationEvent.h"

// when using SDL, the main function should be like `int main(int argc, char **argv)`
int main(int argc, char *argv[])
{
    DYE::Application app { "Main App", 60 };
    app.Run();

    return 0;
}

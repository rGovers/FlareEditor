#include <stdio.h>

#include "Application.h"

int main(int a_argc, char* a_argv[])
{
    Application app = Application(1280, 720, "FlareEditor");
    app.Run();

    return 0;
}
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <string>

#include "AppMain.h"
#include "FlareEditorConfig.h"
#include "Logger.h"

int main(int a_argc, char* a_argv[])
{
    Logger::Message("FlareEditor " + std::to_string(FLAREEDITOR_VERSION_MAJOR) + "." + std::to_string(FLAREEDITOR_VERSION_MINOR));

    Application* app = new AppMain();
    app->Run();

    delete app;

    return 0;
}
#include "Windows/PropertiesWindow.h"

#include "Runtime/RuntimeManager.h"

PropertiesWindow::PropertiesWindow(RuntimeManager* a_runtime) : Window("Properties")
{
    m_runtime = a_runtime;
}
PropertiesWindow::~PropertiesWindow()
{

}

void PropertiesWindow::Update(double a_delta)
{
    m_runtime->ExecFunction("FlareEditor", "AssetProperties", ":OnGUI", NULL);
}
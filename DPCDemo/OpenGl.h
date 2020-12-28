#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wignored-attributes"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#pragma clang diagnostic pop
#include <string>
#include "OpenGlCapabilities.h"
#include "Viewer.h"

class OpenGl
{
    std::string windowTitle;
    GLFWwindow* window;

    OpenGlCapabilities capabilities;
public:
    OpenGl(std::string windowTitle);
    bool Load(Viewer* viewer);
    void Unload();

    GLFWwindow* GetWindow();
    const OpenGlCapabilities& GetCapabilities() const;
    void DisplayFrame();
};


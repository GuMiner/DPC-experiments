#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wignored-attributes"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#pragma clang diagnostic pop

// Defines methods deriving classes must implement to handle GLFW key callbacks.
class IGlfwKeyHandler {
public:
    virtual void GlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) = 0;
    virtual void GlfwCharCallback(GLFWwindow* window, unsigned int character) = 0;
};
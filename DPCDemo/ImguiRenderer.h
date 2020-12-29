#pragma once
#include <GLFW\glfw3.h>
#include "Camera.h"
#include "ShaderFactory.h"
#include "IGlfwKeyHandler.h"
#include "IGlfwMouseHandler.h"
#include "IRenderable.h"

struct ImguiRendererProgram
{
    GLuint programId;
    GLuint vao;
    GLuint vbo;
    GLuint elements;
    GLuint fontTexture;

    GLuint textureAttributeLocation;
    GLuint projMtxAttributeLocation;
};

// Renders IMGUI-type GUI. Modified from imgui_impl_glfw_gl3.h/cpp
class ImguiRenderer : public IGlfwKeyHandler, public IGlfwMouseHandler, public IRenderable
{
    static GLFWwindow* window;

    static const char* GetClipboardText(void* userData)
    {
        return glfwGetClipboardString(window);
    }

    static void SetClipboardText(void* userData, const char* text)
    {
        glfwSetClipboardString(window, text);
    }

    ImguiRendererProgram program;
    float mouseWheelPos;
    bool isMousePress[3];


    void SetStyle();

public:
    ImguiRenderer();

    bool LoadImGui(GLFWwindow* window, ShaderFactory* shaderFactory);
    void UnloadImGui();


    void Update(float currentTime, float lastFrameTime, const Camera& camera) override;
    void Render(float currentTime, const glm::mat4& projectionMatrix) override;

    // Inherited via IGlfwKeyHandler
    virtual void GlfwKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) override;
    virtual void GlfwCharCallback(GLFWwindow* window, unsigned int character) override;

    // Inherited via IGlfwMouseHandler
    virtual void GlfwMouseButtonCallbacks(GLFWwindow* window, int button, int action, int mods) override;
    virtual void GlfwScrollCallback(GLFWwindow* window, double xoffset, double yoffset) override;

    ~ImguiRenderer();
};

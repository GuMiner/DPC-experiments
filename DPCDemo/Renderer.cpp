#include <iostream>
#include <thread>
#include <imgui.h>
#include "Input.h"
#include "Renderer.h"

Renderer::Renderer() : 
    fpsCounter(), guiRenderer(), opengl("DPC++ Demo"), shaderFactory() {

}

bool Renderer::Init() {
    // Setup GLFW
    if (!glfwInit())
    {
        std::cout << "GLFW startup failure" << std::endl;
        return false;
    }

    ImGui::CreateContext();

    if (!opengl.Load(&viewer))
    {
        std::cout << "OpenGL startup failure" << std::endl;
        return false;
    }

    if (ImGui::GetIO().Fonts->AddFontFromFileTTF("fonts/DejaVuSans.ttf", 15.f) == nullptr)
    {
        std::cout << "Unable to load the custom font for IMGUI to use." << std::endl;
        return false;
    }

    if (!guiRenderer.LoadImGui(opengl.GetWindow(), &shaderFactory))
    {
        std::cout << "Unable to load IM GUI!" << std::endl;
        return false;
    }

    updateOrder = { &guiRenderer, &viewer, &fpsCounter };
    renderOrder = { &viewer, &fpsCounter, &guiRenderer };
    return true;
}

void Renderer::Teardown() {
    guiRenderer.UnloadImGui();
    ImGui::DestroyContext();

    opengl.Unload();
    glfwTerminate();
}

void Renderer::update(float currentTime, float frameTime)
{
    const Camera& camera = viewer.GetCamera();

    for (const auto& item : updateOrder)
    {
        item->Update(currentTime, frameTime, camera);
    }

    // world.Update(viewer.GetCamera().position, currentTime, frameTime);
}

void Renderer::render(float currentTime, glm::mat4& viewMatrix)
{
    glm::mat4 projectionMatrix = viewer.perspectiveMatrix * viewMatrix;

    // Clear the screen (and depth buffer) before any rendering begins.
    const GLfloat color[] = { 0, 0.2f, 0, 1 };
    const GLfloat depth = 1.0f;
    glClearBufferfv(GL_COLOR, 0, color);
    glClearBufferfv(GL_DEPTH, 0, &depth);

    // world.Render(projectionMatrix);

    for (const auto& item : renderOrder)
    {
        item->Render(currentTime, projectionMatrix);
    }
}

void Renderer::Run()
{
    float gameTime = 0;
    float lastFrameTime = 0.06f;

    while (!glfwWindowShouldClose(opengl.GetWindow()))
    {
        glfwPollEvents();

        // Run the game and render if not paused.
        if (Input::hasFocus)
        {
            update(gameTime, lastFrameTime);

            render(gameTime, viewer.viewMatrix);
            opengl.DisplayFrame();
        }

        float now = (float)glfwGetTime();
        lastFrameTime = (now - gameTime);
        gameTime += lastFrameTime;

        // Delay to run approximately at our maximum framerate.
        long sleepDelay = ((long)1e6 / viewer.MaxFramerate) - (long)(lastFrameTime * 1e6);
        if (sleepDelay > 0)
        {
            std::this_thread::sleep_for(std::chrono::microseconds(sleepDelay));
        }
    }
}
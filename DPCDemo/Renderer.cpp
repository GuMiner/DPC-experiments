#include <iostream>
#include <thread>
#include <imgui.h>

#include "Input.h"
#include "SimConstants.h"
#include "Renderer.h"

Renderer::Renderer(Synchronizer* const sync) :
    sync(sync),
    guiRenderer(), opengl("DPC++ Demo"), shaderFactory(), axis(),
    fanRenderer(), particleRenderer() {
}

bool Renderer::Init(FanMesh* const fanMesh) {
    // Infrastructure
    if (!glfwInit()) {
        std::cout << "GLFW startup failure" << std::endl;
        return false;
    }

    ImGui::CreateContext();

    if (!opengl.Load(&viewer)) {
        std::cout << "OpenGL startup failure" << std::endl;
        return false;
    }

    if (ImGui::GetIO().Fonts->AddFontFromFileTTF("fonts/DejaVuSans.ttf", 15.f) == nullptr) {
        std::cout << "Unable to load the custom font for IMGUI to use." << std::endl;
        return false;
    }

    if (!guiRenderer.LoadImGui(opengl.GetWindow(), &shaderFactory)) {
        std::cout << "Unable to load IM GUI!" << std::endl;
        return false;
    }

    // Axis / bounding box
    if (!axis.Init(shaderFactory)) {
        std::cout << "Unable to load Axis!" << std::endl;
        return false;
    }

    // Particles 
    if (!particleRenderer.Init(shaderFactory)) {
        std::cout << "Unable to load ParticleRenderer!" << std::endl;
        return false;
    }

    // Fan
    if (!fanRenderer.Init(shaderFactory, fanMesh->Vertices, fanMesh->Normals)) {
        std::cout << "Unable to load MeshRenderer for the fan!" << std::endl;
        return false;
    }

    updateOrder = { &guiRenderer, &viewer, &axis, &fanRenderer, &particleRenderer, &fpsCounter };
    renderOrder = { &viewer, &axis, &fanRenderer, &particleRenderer, &fpsCounter, &guiRenderer };
    return true;
}

void Renderer::Teardown() {
    guiRenderer.UnloadImGui();
    ImGui::DestroyContext();

    opengl.Unload();
    glfwTerminate();
}

void Renderer::update(float currentTime, float frameTime) {
    const Camera& camera = viewer.GetCamera();

    for (const auto& item : updateOrder) {
        item->Update(currentTime, frameTime, camera);
    }

    sync->ReadUpdatedParticlePositions([this](std::vector<glm::vec3>& particlePositions, std::vector<float>& particleSpeeds) {
        particleRenderer.Transfer(particlePositions, particleSpeeds);
    });

    sync->ReadUpdatedFanPosition([this](glm::mat4& fanMeshMatrix) {
        fanRenderer.UpdateMeshMatrix(fanMeshMatrix);
    });

    if (Input::IsKeyPressed(GLFW_KEY_R)) {
        sync->reset = true; // Results in multiple resets, but that isn't a problem.
    }
}

void Renderer::render(float currentTime, glm::mat4& viewMatrix) {
    glm::mat4 projectionMatrix = viewer.perspectiveMatrix * viewMatrix;
    
    // Compute particle sizes so they appear 3D, accounting for the current particle radius. 
    float pointScale = PARTICLE_RADIUS * 2.0f * viewer.ScreenHeight /
        tanf(viewer.GetFovY() * (float)3.14159f / 360.0f);
    particleRenderer.SetViewMatrix(viewMatrix, pointScale);

    // Clear the screen (and depth buffer) before any rendering begins.
    const GLfloat color[] = { 0, 0.2f, 0, 1 };
    const GLfloat depth = 1.0f;
    glClearBufferfv(GL_COLOR, 0, color);
    glClearBufferfv(GL_DEPTH, 0, &depth);

    for (const auto& item : renderOrder) {
        item->Render(currentTime, projectionMatrix);
    }
}

void Renderer::Run() {
    float simViewTime = 0;
    float lastFrameTime = 0.06f;

    while (!glfwWindowShouldClose(opengl.GetWindow())) {
        glfwPollEvents();

        // Run the simulation viewer and render if not paused.
        if (Input::hasFocus) {
            update(simViewTime, lastFrameTime);

            render(simViewTime, viewer.viewMatrix);
            opengl.DisplayFrame();
        }

        float now = (float)glfwGetTime();
        lastFrameTime = (now - simViewTime);
        simViewTime += lastFrameTime;

        // Delay to run approximately at our maximum framerate.
        long sleepDelay = ((long)1e6 / viewer.MaxFramerate) - (long)(lastFrameTime * 1e6);
        if (sleepDelay > 0) {
            std::this_thread::sleep_for(std::chrono::microseconds(sleepDelay));
        }
    }
}
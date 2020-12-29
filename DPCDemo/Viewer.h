#pragma once
#include <glm\vec2.hpp>
#include <glm\vec3.hpp>
#include <glm\mat4x4.hpp>
#include "Camera.h"
#include "IMotionTranslator.h"
#include "IRenderable.h"

// Takes in user input and a camera to output a perspective and view matrix.
class Viewer : public IRenderable
{
    float fovY;

    float aspectRatio;
    float nearPlane;
    float farPlane;

    Camera camera;
    IMotionTranslator* motionTranslator;

    void UpdateMatrices();

public:
    glm::mat4 perspectiveMatrix;
    glm::mat4 viewMatrix;

    int ScreenWidth;
    int ScreenHeight;
    int MaxFramerate;

    Viewer();
    ~Viewer();

    // Updates the view position from user input.
    void Update(float currentTime, float lastFrameTime, const Camera& camera) override;
    void Render(float currentTime, const glm::mat4& projectionMatrix) override;
    void SetScreenSize(int width, int height);

    Camera GetCamera() const;
    float GetAspectRatio() const;
    float GetFovY() const;
};


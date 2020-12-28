#pragma once
#include <glm\vec2.hpp>
#include <glm\vec3.hpp>
#include <glm\mat4x4.hpp>
#include "Camera.h"
#include "IMotionTranslator.h"

// Takes in user input and a camera to output a perspective and view matrix.
class Viewer
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
    void Update(float frameTime);
    void Render();
    void SetScreenSize(int width, int height);

    Camera GetCamera() const;
    float GetAspectRatio() const;
    float GetFovY() const;
};


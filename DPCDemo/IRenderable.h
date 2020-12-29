#pragma once
#include <glm\mat4x4.hpp>
#include "Camera.h"

// Takes in user input and translate it to camera motions.
class IRenderable
{
public:
    virtual void Update(float currentTime, float lastFrameTime, const Camera& camera) = 0;
    virtual void Render(float currentTime, const glm::mat4& projectionMatrix) = 0;
};

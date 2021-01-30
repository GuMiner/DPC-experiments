#pragma once
#include "IRenderable.h"

class FpsCounter : public IRenderable {
    float fpsTimeAggregated;
    int fpsFramesCounted;
    float lastFrameRate;

public:
    FpsCounter();
    void Update(float currentTime, float lastFrameTime, const Camera& camera) override;
    void Render(float currentTime, const glm::mat4& projectionMatrix) override;
};


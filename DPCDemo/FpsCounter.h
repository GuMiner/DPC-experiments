#pragma once
class FpsCounter
{
    float fpsTimeAggregated;
    int fpsFramesCounted;
    float lastFrameRate;

public:
    FpsCounter();
    void UpdateFps(float frameTime);
    void Render();
};


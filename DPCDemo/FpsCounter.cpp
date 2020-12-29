#include <imgui.h>
#include "FpsCounter.h"

FpsCounter::FpsCounter() :
    fpsTimeAggregated(0.0f), fpsFramesCounted(0), lastFrameRate(60.0f)
{
}

void FpsCounter::Update(float currentTime, float lastFrameTime, const Camera& camera)
{
    ++fpsFramesCounted;
    fpsTimeAggregated += lastFrameTime;
    if (fpsTimeAggregated > 0.50f)
    {
        lastFrameRate = (float)fpsFramesCounted / (float)fpsTimeAggregated;
        fpsTimeAggregated = 0;
        fpsFramesCounted = 0;
    }
}

void FpsCounter::Render(float currentTime, const glm::mat4& projectionMatrix)
{
    ImGui::Begin("FPS", nullptr);
    ImGui::SetWindowSize(ImVec2(100, 100));
    ImGui::SetCursorPos(ImVec2(5, 20));
    ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), "%.0f", lastFrameRate);
    ImGui::End();
}

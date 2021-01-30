#pragma once
#include "ColorVbo.hpp"
#include "IRenderable.h"
#include "PositionVbo.hpp"
#include "ShaderFactory.h"

class Axis : public IRenderable {
    GLuint programId;
    GLuint projectionMatrixLocation;

    GLuint vao;
    PositionVbo positionVbo;
    ColorVbo colorVbo;

public:
    Axis();
    bool Init(ShaderFactory& shaderFactory);
    virtual void Update(float currentTime, float lastFrameTime, const Camera& camera) override;
    virtual void Render(float currentTime, const glm::mat4& projectionMatrix) override;
};


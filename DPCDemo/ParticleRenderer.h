#pragma once
#include <vector>
#include <glm\vec3.hpp>

#include "ColorVbo.hpp"
#include "IRenderable.h"
#include "PositionVbo.hpp"
#include "ShaderFactory.h"

class ParticleRenderer : public IRenderable {
    GLuint programId;
    GLuint projectionMatrixLocation;

    GLuint vao;
    PositionVbo positionVbo;
    ColorVbo colorVbo;

public:
    ParticleRenderer();
    bool Init(ShaderFactory& shaderFactory);
    void Transfer(std::vector<glm::vec3>& particlePositions);
    virtual void Update(float currentTime, float lastFrameTime, const Camera& camera) override;
    virtual void Render(float currentTime, const glm::mat4& projectionMatrix) override;
};

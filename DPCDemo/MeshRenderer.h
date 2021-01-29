#pragma once
#include <vector>
#include <glm\vec3.hpp>
#include <glm\mat4x4.hpp>

#include "IRenderable.h"
#include "NormalVbo.hpp"
#include "PositionVbo.hpp"
#include "ShaderFactory.h"

class MeshRenderer : public IRenderable
{
    GLuint programId;
    GLuint projectionMatrixLocation;

    GLuint vao;
    PositionVbo positionVbo;
    NormalVbo normalVbo;

    GLuint meshMatrixLocation;
    glm::mat4 meshMatrix;
public:
    MeshRenderer();
    bool Init(ShaderFactory& shaderFactory, std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& normals);
    
    void UpdateMeshMatrix(glm::mat4& updatedMeshMatrix);
    virtual void Update(float currentTime, float lastFrameTime, const Camera& camera) override;
    virtual void Render(float currentTime, const glm::mat4& projectionMatrix) override;
};

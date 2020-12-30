#pragma once
#include <vector>
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wignored-attributes"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#pragma clang diagnostic pop
#include <glm\vec3.hpp>
#include "VboBase.hpp"

class PositionVbo : public VboBase<glm::vec3>
{
    GLenum usageType;

public:
    PositionVbo(GLenum usageType = GL_DYNAMIC_DRAW)
        : usageType(usageType)
    {

    }

    virtual void SetupOpenGlBuffers() override
    {
        InitializeToLocation(0);
    }

    virtual void TransferToOpenGl() override
    {
        SendToOpenGl(3, usageType);
    }
};
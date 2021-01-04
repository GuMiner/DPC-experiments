#pragma once
#include <glm\vec3.hpp>
#include <glm\geometric.hpp>

// Defines a 3D LH camera
struct Camera
{
    glm::vec3 position;
    glm::vec3 forwards; // Starts at Z+, normalized
    glm::vec3 up; // Y+, normalized

    glm::vec3 right; // X+, computed
    glm::vec3 target; // Where the camera points to, normalized.

    // LH coordinate system, Z+ is into the screen with X+ == left, Y+ == up
    Camera() :
        position(glm::vec3(3.18f, 2.31f, 1.85f)),
        forwards(glm::vec3(-0.75f, -0.53f, -0.40f)), up(glm::vec3(-0.30f, -0.26f, 0.92f))
    {
        ComputeNormals();
    }

    void ComputeNormals()
    {
        up = glm::normalize(up);
        forwards = glm::normalize(forwards);

        right = glm::cross(up, forwards); // In LH geometry this gives us the X+ normalized vector
        target = position + forwards;
    }
};
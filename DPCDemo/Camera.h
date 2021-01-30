#pragma once
#include <glm\vec3.hpp>
#include <glm\geometric.hpp>

// Defines a 3D LH camera
struct Camera {
    glm::vec3 position;
    glm::vec3 forwards; // Starts at Z+, normalized
    glm::vec3 up; // Y+, normalized

    glm::vec3 right; // X+, computed
    glm::vec3 target; // Where the camera points to, normalized.

    // LH coordinate system, Z+ is into the screen with X+ == left, Y+ == up
    Camera() :
        // Moved around a bit to see the complete simulation cube upon startup.
        position(glm::vec3(25.67f, 24.00f, 15.55f)),
        forwards(glm::vec3(-0.76f, -0.51f, -0.39f)),
        up(glm::vec3(-0.29f, -0.26f, 0.92f)) {
        ComputeNormals();
    }

    void ComputeNormals() {
        up = glm::normalize(up);
        forwards = glm::normalize(forwards);

        right = glm::cross(up, forwards); // In LH geometry this gives us the X+ normalized vector
        target = position + forwards;
    }
};
#include <glm/gtx/rotate_vector.hpp>

#include "SimConstants.h"
#include "DefaultMotionTranslator.h"

IMotionTranslator::~IMotionTranslator() {
}

DefaultMotionTranslator::DefaultMotionTranslator()
    : wasMouseDown(false) {
}

bool DefaultMotionTranslator::CheckMouseRotation(Camera* camera) {
    bool rotatedAroundUp = false;
    bool rotatedAroundRight = false;
    if (Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_2)) {
        if (!wasMouseDown) {
            wasMouseDown = true;
            lastMousePos = Input::GetMousePos();
        }

        glm::ivec2 currentMousePos = Input::GetMousePos();
        glm::ivec2 difference = currentMousePos - lastMousePos;
        lastMousePos = currentMousePos;

        glm::vec2 rotation = MOUSE_ROTATION_SPEED * glm::vec2(-(float)difference.x, -(float)difference.y);
        if (difference.x != 0) {
            camera->forwards = glm::rotate(camera->forwards, rotation.x, camera->up);
            rotatedAroundUp = true;
        }

        if (difference.y != 0) {
            camera->up = glm::rotate(camera->up, rotation.y, camera->right);
            camera->forwards = glm::rotate(camera->forwards, rotation.y, camera->right);
            rotatedAroundRight = true;
        }
    } else {
        wasMouseDown = false;
    }

    return rotatedAroundUp || rotatedAroundRight;
}

bool DefaultMotionTranslator::Update(Camera* camera, float frameTime) {
    float speed = frameTime * KEYBOARD_MOVEMENT_SPEED;

    bool movedForwards = DialVariable(GLFW_KEY_A, GLFW_KEY_Z, speed * camera->forwards, &camera->position);
    bool movedLeftRight = DialVariable(GLFW_KEY_W, GLFW_KEY_Q, speed * camera->right, &camera->position);
    bool movedUpDown = DialVariable(GLFW_KEY_S, GLFW_KEY_X, speed * camera->up, &camera->position);

    bool mouseRotated = CheckMouseRotation(camera);

    bool keyRotated = false;
    if (Input::IsKeyPressed(GLFW_KEY_E)) {
        camera->up = glm::rotate(camera->up, KEYBOARD_ROLL_SPEED * frameTime, camera->forwards);
        keyRotated = true;
    }

    if (Input::IsKeyPressed(GLFW_KEY_D)) {
        camera->up = glm::rotate(camera->up, -KEYBOARD_ROLL_SPEED * frameTime, camera->forwards);
        keyRotated = true;
    }

    camera->ComputeNormals();
    return movedForwards || movedLeftRight || movedUpDown || mouseRotated || keyRotated;
}
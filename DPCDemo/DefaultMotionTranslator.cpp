#include <glm/gtx/rotate_vector.hpp>
#include "DefaultMotionTranslator.h"

DefaultMotionTranslator::DefaultMotionTranslator()
    : wasMouseDown(false)
{
}

bool DefaultMotionTranslator::CheckMouseRotation(Camera* camera)
{
    const float mouseRotationSpeed = 0.001f;
    bool rotatedAroundUp = false;
    bool rotatedAroundRight = false;
    if (Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_2))
    {
        if (!wasMouseDown)
        {
            wasMouseDown = true;
            lastMousePos = Input::GetMousePos();
        }

        glm::ivec2 currentMousePos = Input::GetMousePos();
        glm::ivec2 difference = currentMousePos - lastMousePos;
        lastMousePos = currentMousePos;

        glm::vec2 rotation = mouseRotationSpeed * glm::vec2(-(float)difference.x, -(float)difference.y);
        if (difference.x != 0)
        {
            camera->forwards = glm::rotate(camera->forwards, rotation.x, camera->up);
            rotatedAroundUp = true;
        }

        if (difference.y != 0)
        {
            camera->up = glm::rotate(camera->up, rotation.y, camera->right);
            camera->forwards = glm::rotate(camera->forwards, rotation.y, camera->right);
            rotatedAroundRight = true;
        }
    }
    else
    {
        wasMouseDown = false;
    }

    return rotatedAroundUp || rotatedAroundRight;
}

bool DefaultMotionTranslator::Update(Camera* camera, float frameTime)
{
    const float motionSpeed = 2.0f;
    float speed = frameTime * motionSpeed;

    bool movedForwards = DialVariable(GLFW_KEY_A, GLFW_KEY_Z, speed * camera->forwards, &camera->position);
    bool movedLeftRight = DialVariable(GLFW_KEY_W, GLFW_KEY_Q, speed * camera->right, &camera->position);
    bool movedUpDown = DialVariable(GLFW_KEY_S, GLFW_KEY_X, speed * camera->up, &camera->position);

    bool mouseRotated = CheckMouseRotation(camera);

    const float keyRotationSpeed = 1.0f;
    bool keyRotated = false;
    if (Input::IsKeyPressed(GLFW_KEY_E))
    {
        camera->up = glm::rotate(camera->up, keyRotationSpeed * frameTime, camera->forwards);
        keyRotated = true;
    }

    if (Input::IsKeyPressed(GLFW_KEY_D))
    {
        camera->up = glm::rotate(camera->up, -keyRotationSpeed * frameTime, camera->forwards);
        keyRotated = true;
    }

    camera->ComputeNormals();
    return movedForwards || movedLeftRight || movedUpDown || mouseRotated || keyRotated;
}
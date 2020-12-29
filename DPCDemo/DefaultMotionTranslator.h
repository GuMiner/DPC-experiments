#pragma once
#include "Input.h"
#include "IMotionTranslator.h"

// Converts a descent-style IO input into camera motion.
class DefaultMotionTranslator : public IMotionTranslator
{
    template <typename T>
    bool DialVariable(int posKeyId, int negKeyId, T amount, T* variable) const
    {
        bool variableDialed = false;
        if (Input::IsKeyPressed(posKeyId))
        {
            (*variable) += amount;
            variableDialed = true;
        }

        if (Input::IsKeyPressed(negKeyId))
        {
            (*variable) -= amount;
            variableDialed = true;
        }

        return variableDialed;
    }

    template <typename T>
    bool DialVariables(int posKeyId, int negKeyId, T amount, T* variable, T secondAmount, T* secondVariable) const
    {
        bool variableDialed = DialVariable(posKeyId, negKeyId, amount, variable);
        variableDialed = DialVariable(posKeyId, negKeyId, secondAmount, secondVariable) || variableDialed;

        return variableDialed;
    }

    bool wasMouseDown;
    glm::ivec2 lastMousePos;
    bool CheckMouseRotation(Camera* camera);

public:
    DefaultMotionTranslator();
    bool Update(Camera* camera, float frameTime);
        
};


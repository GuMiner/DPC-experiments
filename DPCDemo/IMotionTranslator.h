#pragma once
#include "Camera.h"

// Takes in user input and translate it to camera motions.
class IMotionTranslator
{
public:
    virtual bool Update(Camera* camera, float frameTime) = 0;
};

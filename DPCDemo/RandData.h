#pragma once
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "Random.h"

struct RandData {
public:
	RandData(Random& random);

	glm::vec2 position;
	glm::vec3 velocity;

	int plane;
};
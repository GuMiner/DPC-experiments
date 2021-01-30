#pragma once
#include "glm/vec3.hpp"
#include "Random.h"

struct RandData {
public:
	RandData(Random& random);

	glm::vec3 position;
	glm::vec3 velocity;

	int plane;
};
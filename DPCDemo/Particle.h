#pragma once
#include "glm/vec3.hpp"
#include "Random.h"

struct Particle {
public:
	Particle(Random& random, float zMin, float zMax);

	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec3 acceleration;

	float mass;
};
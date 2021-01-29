#pragma once
#include "glm/vec3.hpp"

// A particle exiting the simulation grid, ready for analysis.
struct ExitingParticle {
public:
	bool valid;

	glm::vec3 position;
	glm::vec3 velocity;

	float mass;
};
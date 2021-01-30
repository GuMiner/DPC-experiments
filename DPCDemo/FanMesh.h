#pragma once
#include <vector>

#include "glm/vec3.hpp"

struct FanMesh {
public:
	// Scaled and moved to be centered at 5, 5, 5, with a size of 3, 3, ?
	std::vector<glm::vec3> Vertices; // Stored in triplets
	std::vector<glm::vec3> Normals;

	// Bounds
	glm::vec3 min;
	glm::vec3 max;

	// Computed directly from volume
	glm::vec3 centroid;
	float mass; // TODO units
};
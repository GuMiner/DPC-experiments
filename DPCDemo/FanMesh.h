#pragma once
#include <vector>
#include "glm/vec3.hpp"

struct FanMesh {
public:
	// Stored in triplets
	// Scaled and moved to be centered at 5, 5, 5, with a size of 3, 3, ?
	std::vector<glm::vec3> Vertices;
	std::vector<glm::vec3> Normals;

	// Computed directly from volume
	glm::vec3 centroid;
	float mass; // TODO units
};
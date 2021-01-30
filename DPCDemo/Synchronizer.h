#include <atomic>
#include <functional>
#include <vector>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include "Particle.h"

#pragma once
class Synchronizer {
	// Particles
	// To GUI
	std::vector<glm::vec3> particlePositions;
	std::atomic<bool> shouldReadUpdate;

	// From GUI
	std::atomic<bool> hasReadUpdate;

	// Fan
	// To GUI
	glm::mat4 fanMeshPosition;
	std::atomic<bool> shouldReadMeshUpdate;

	// From GUI
	std::atomic<bool> hasReadMeshUpdate;

public:
	Synchronizer(std::atomic<bool>* shouldStopSimulating);

	void UpdateParticlePositions(std::vector<Particle>& particles);
	void ReadUpdatedParticlePositions(std::function<void(std::vector<glm::vec3>&)> updater);
	void UpdateFanPosition(glm::mat4& meshPosition);
	void ReadUpdatedFanPosition(std::function<void(glm::mat4&)> updater);

	// FROM GUI -- IO
	std::atomic<bool> reset;
	std::atomic<bool>* shouldStopSimulating;
};


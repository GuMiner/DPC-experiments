#pragma once
#include <atomic>
#include <chrono>
#include <vector>
#include "glm/mat4x4.hpp"

#include "FanMesh.h"
#include "ExitingParticle.h"
#include "Particle.h"
#include "RandData.h"

#include "SimConstants.h"
#include "Random.h"

#if ENABLE_GUI
#include "Synchronizer.h"
#endif

class Simulation {
	std::atomic<bool>* shouldStopSimulating;
	Random randomGenerator;

#if ENABLE_GUI
	Synchronizer* sync;
#endif

	// Fan data
	FanMesh* const fanMesh;
	float fanZAngle;
	glm::mat4 fanRotationMatrix;
	std::vector<glm::vec3> fanVertices;
	void UpdateFanRotation();
	void UpdateMeshVertices();

	// Particle data
	std::vector<Particle> particles;
	std::vector<ExitingParticle> exitingParticles;
	std::vector<RandData> randomData;
	void InitParticleData();
	void ResetRandomBuffer();

	// Aggregate results data
	glm::vec3 exitingEnergyFlow = glm::vec3(0.0f);
	glm::vec3 avgExitPosition = glm::vec3(0.0f);
	long exitedParticles = 0;
	float exitingTotalEnergy = 0.0f;
	void UpdateAggregateResults();
	void LogSimulationStatus(long count, std::chrono::steady_clock::time_point& simStart);
	void LogFinalResults(long count);
	
public:
	Simulation(std::atomic<bool>* shouldStopSimulating, FanMesh* const fanMesh);
	void Simulate(std::string deviceToSelect);

#if ENABLE_GUI
	void EnableSimGuiSyncing(Synchronizer* const sync);
#endif

};


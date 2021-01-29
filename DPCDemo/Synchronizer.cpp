#include "Synchronizer.h"

Synchronizer::Synchronizer(std::atomic<bool>* shouldStopSimulating)
	: particlePositions(),
	  shouldReadUpdate(false), hasReadUpdate(true),
      shouldReadMeshUpdate(false), hasReadMeshUpdate(true),
	  reset(false), shouldStopSimulating(shouldStopSimulating)
{
}

void Synchronizer::UpdateParticlePositions(std::vector<Particle>& particles)
{
    if (hasReadUpdate.load())
    {
        hasReadUpdate = false;

        particlePositions.clear();
        for (const Particle& particle : particles)
        {
            particlePositions.push_back(particle.position);
        }

        shouldReadUpdate = true;
    }
}

void Synchronizer::ReadUpdatedParticlePositions(std::function<void(std::vector<glm::vec3>&)> updater)
{
    if (shouldReadUpdate.load())
    {
        shouldReadUpdate = false;
        updater(particlePositions);
        hasReadUpdate = true;
    }
}

void Synchronizer::UpdateFanPosition(glm::mat4& meshPosition)
{
    if (hasReadMeshUpdate.load())
    {
        hasReadMeshUpdate = false;
        fanMeshPosition = meshPosition;
        shouldReadMeshUpdate = true;
    }
}

void Synchronizer::ReadUpdatedFanPosition(std::function<void(glm::mat4&)> updater)
{
    if (shouldReadMeshUpdate.load())
    {
        shouldReadMeshUpdate = false;
        updater(fanMeshPosition);
        hasReadMeshUpdate = true;
    }
}
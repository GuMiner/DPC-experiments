#include <glm\geometric.hpp>

#include "SimConstants.h"
#include "RandData.h"

RandData::RandData(Random& random) :
	// Use slightly more storage for computational simplicity later on.
	position(random.ZeroToOne(), random.ZeroToOne(), random.ZeroToOne()),
	velocity(random.NegOneToOne(), random.NegOneToOne(), random.NegOneToOne()),
	plane(0)
{
	position = position * SIM_MAX;
	velocity = PARTICLE_INIT_VELOCITY * glm::normalize(velocity);
	plane = (int)(random.ZeroToOne() * 6); // Will never be 6 as this rounds down.
}
#include "SimConstants.h"
#include "Particle.h"

Particle::Particle(Random& random, float zMin, float zMax) :
	position(random.ZeroToOne() * SIM_MAX, random.ZeroToOne() * SIM_MAX, 0),
	velocity(
		random.NegOneToOne() * INIT_RAND_VELOCITY,
		random.NegOneToOne() * INIT_RAND_VELOCITY,
		random.NegOneToOne() * INIT_RAND_VELOCITY),
	acceleration(0, 0, 0),
	mass(PARTICLE_MASS)
{
	// Don't randomly place particles from zMin to zMax (the fan)
	float zRange = (SIM_MAX - (zMax - zMin)) * random.ZeroToOne();
	if (zRange < zMin)
	{
		position.z = zRange;
	}
	else
	{
		position.z = zMax + (zRange - zMin);
	}
}
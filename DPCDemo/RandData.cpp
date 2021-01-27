#include "SimConstants.h"
#include "RandData.h"

RandData::RandData(Random& random) :
	position(random.ZeroToOne() * SIM_MAX, random.ZeroToOne()* SIM_MAX),
	velocity(
		random.NegOneToOne() * INIT_RAND_VELOCITY,
		random.NegOneToOne() * INIT_RAND_VELOCITY,
		random.NegOneToOne() * INIT_RAND_VELOCITY),
	plane(0)
{
	// TODO compute plane.
}
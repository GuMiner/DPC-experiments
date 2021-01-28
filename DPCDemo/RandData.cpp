#include "SimConstants.h"
#include "RandData.h"

RandData::RandData(Random& random) :
	position( // Use slightly more storage for computational simplicity later on.
		random.ZeroToOne() * SIM_MAX,
		random.ZeroToOne() * SIM_MAX,
		random.ZeroToOne() * SIM_MAX),
	velocity(
		random.NegOneToOne() * INIT_RAND_VELOCITY,
		random.NegOneToOne() * INIT_RAND_VELOCITY,
		random.NegOneToOne() * INIT_RAND_VELOCITY),
	plane(0)
{
	plane = (int)(random.ZeroToOne() * 6); // Will never be 6 as this rounds down.
}
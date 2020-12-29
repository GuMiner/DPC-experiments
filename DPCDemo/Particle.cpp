#include "Particle.h"

Particle::Particle(Random& random) :
	position(random.ZeroToOne(), random.ZeroToOne(), random.ZeroToOne()),
	velocity(random.NegOneToOne(), random.NegOneToOne(), random.NegOneToOne()),
	acceleration(0, 0, 0),
	mass(random.ZeroToOne() + 1.0f) {

}
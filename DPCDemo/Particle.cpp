#include "Particle.h"

Particle::Particle(Random& random) :
	position(random.ZeroToOne(), random.ZeroToOne(), random.ZeroToOne()),
	velocity(random.NegOneToOne() * 10.0f, random.NegOneToOne() * 10.0f, random.NegOneToOne() * 10.0f),
	acceleration(0, 0, 0),
	mass(random.ZeroToOne() * 10.0f + 1.0f) {

}
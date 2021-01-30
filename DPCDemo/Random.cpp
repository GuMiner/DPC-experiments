#include <random>
#include <functional>

#include "SimConstants.h"
#include "Random.h"

Random::Random() : 
	randomDevice(), generator(RAND_GENERATOR_SEED), negOneToOne(-1.0, 1.0), zeroToOne(0.0, 1.0) {
}

float Random::NegOneToOne() {
	return negOneToOne(generator);
}

float Random::ZeroToOne() {
	return zeroToOne(generator);
}
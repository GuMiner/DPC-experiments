#include <random>
#include <functional>
#include "Random.h"

Random::Random() : 
	randomDevice(), generator(4321), negOneToOne(-1.0, 1.0), zeroToOne(0.0, 1.0)
{

	
}

float Random::NegOneToOne()
{
	return negOneToOne(generator);
}

float Random::ZeroToOne()
{
	return zeroToOne(generator);
}
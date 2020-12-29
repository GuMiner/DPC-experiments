#pragma once
#include <random>

class Random
{
	std::random_device randomDevice;
	std::mt19937 generator;

	std::uniform_real_distribution<float> negOneToOne;
	std::uniform_real_distribution<float> zeroToOne;
public:
	Random();
	float NegOneToOne();
	float ZeroToOne();
};


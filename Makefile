all: dpcpp -O3 -g -std=c++17 -o DPCDemo SimConstants.h Simulation.h Simulation.cpp Random.h Random.cpp RandData.h RandData.cpp Particle.h Particle.cpp ModelLoader.h ModelLoader.cpp FanMesh.h ExitingParticle.h DPCDemo.cpp DeviceQuerier.h DeviceQuerier.cpp

run: 
	./DPCDemo

clean: 
	rm -f DPCDemo
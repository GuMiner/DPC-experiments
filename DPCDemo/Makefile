all: 
	dpcpp SimConstants.h Simulation.h Simulation.cpp Random.h Random.cpp RandData.h RandData.cpp Particle.h Particle.cpp ModelLoader.h ModelLoader.cpp FanMesh.h ExitingParticle.h DPCDemo.cpp DeviceQuerier.h DeviceQuerier.cpp -Idevcloud-dep/include -std=c++17

run: run-cpu

run-cpu:
	./a.out

run-gpu:
	./a.out --device gpu

run-fpga:
	./a.out --device fpga

clean: 
	rm -f a.out
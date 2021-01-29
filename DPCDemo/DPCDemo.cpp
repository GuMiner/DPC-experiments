#include <array>
#include <iomanip>
#include <iostream>
#include <map>
#include <thread>
#include <vector>
#include <CL/sycl.hpp>
#include <CL/sycl/INTEL/fpga_extensions.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/transform.hpp>

#include "SimConstants.h"

#if ENABLE_GUI
#include "Synchronizer.h"
#include "Renderer.h"
Renderer* renderer;
#endif

#include "DeviceQuerier.h"
#include "ModelLoader.h"
#include "ExitingParticle.h"
#include "Particle.h"
#include "RandData.h"
#include "Random.h"

using namespace cl::sycl;

#if ENABLE_GUI
Synchronizer* sync;
#endif

FanMesh* fanMesh;
std::atomic<bool> shouldStopSimulating(false);

//************************************
// Function description: create a device queue with the default selector or
// explicit FPGA selector when FPGA macro is defined
//    return: DPC++ queue object
//************************************
queue create_device_queue() {
  // create device selector for the device of your interest
#if FPGA_EMULATOR
  // DPC++ extension: FPGA emulator selector on systems without FPGA card
  INTEL::fpga_emulator_selector dselector;
#elif FPGA
  // DPC++ extension: FPGA selector on systems with FPGA card
  INTEL::fpga_selector dselector;
#elif GPU
    gpu_selector dselector;
#else
  // the default device selector: it will select the most performant device
  // available at runtime.
  cpu_selector dselector;
  // default_selector dselector;
#endif

  // create an async exception handler so the program fails more gracefully.
  auto ehandler = [](cl::sycl::exception_list exceptionList) {
    for (std::exception_ptr const &e : exceptionList) {
      try {
        std::rethrow_exception(e);
      }
      catch (cl::sycl::exception const &e) {
        std::cout << "Caught an asynchronous DPC++ exception, terminating the program: " << e.what() << std::endl;
        std::terminate();
      }
    }
  };

  try {
    // create the devices queue with the selector above and the exception
    // handler to catch async runtime errors the device queue is used to enqueue
    // the kernels and encapsulates all the states needed for execution
    queue q(dselector, ehandler);
    DeviceQuerier::OutputDeviceInfo(q.get_device());

    return q;
  } catch (cl::sycl::exception const &e) {
    // catch the exception from devices that are not supported.
    std::cout << "An exception was caught when creating a device queue: " << e.what() << std::endl;
    std::terminate();
  }
}

void ResetRandomBuffer(Random& randomGenerator, std::vector<RandData>& randomBuffer)
{
    // It's very painful to generate random number generators (no API support)
    // So, do it on the host and pass that back to the device https://www.khronos.org/files/sycl/sycl-12-reference-card.pdf
    randomBuffer.clear();
    for (long i = 0; i < PARTICLE_COUNT; i++)
    {
        randomBuffer.push_back(RandData(randomGenerator));
    }
}

glm::mat4 UpdateFanRotation(float angle)
{
    return glm::translate(glm::vec3(SIM_MAX / 2.0f, SIM_MAX / 2.0f, 0.0f)) *
        glm::rotate(angle, glm::vec3(0.0f, 0.0f, 1.0f)) *
        glm::translate(glm::vec3(-SIM_MAX / 2.0f, -SIM_MAX / 2.0f, 0.0f));
}


void SimulateParticles(FanMesh* fanMesh) {
    queue q = create_device_queue();

    std::vector<Particle> particles;
    std::vector<ExitingParticle> exitingParticles;
    std::vector<RandData> randomData;

    const float PARTICLE_DEFAULT_ENERGY = PARTICLE_MASS * PARTICLE_INIT_VELOCITY * PARTICLE_INIT_VELOCITY;
    glm::vec3 exitingEnergyFlow = glm::vec3(0.0f);
    glm::vec3 avgExitPosition = glm::vec3(0.0f);
    long exitedParticles = 0;
    float exitingTotalEnergy = 0.0f;

    float fanZAngle = 0;
    glm::mat4 fanRotationMatrix = UpdateFanRotation(fanZAngle);

    Random randomGenerator = Random();
    for (long i = 0; i < PARTICLE_COUNT; i++)
    {
        particles.push_back(Particle(randomGenerator, fanMesh->min[2], fanMesh->max[2]));
        
        ExitingParticle exitingParticle;
        exitingParticle.valid = false;
        exitingParticles.push_back(exitingParticle);
    }

    ResetRandomBuffer(randomGenerator, randomData);
    range<1> particleRange(PARTICLE_COUNT);

    long count = 0;
    while(!shouldStopSimulating.load()) {
#if ENABLE_GUI
        if (sync->reset.load()) {
            particles.clear();
            for (long i = 0; i < PARTICLE_COUNT; i++)
            {
                particles.push_back(Particle(randomGenerator, fanMesh->min[2], fanMesh->max[2]));
            }

            sync->reset = false;
        }
#endif

        auto simStart = std::chrono::steady_clock::now();

        // Enforce a scope so buffers close / return data to the host after each step.
        {
            buffer<Particle, 1> particleBuffer(particles.data(), particles.size(),
                { cl::sycl::property::buffer::use_host_ptr() });

            buffer<RandData, 1> randomBuffer(randomData.data(), randomData.size(),
                { cl::sycl::property::buffer::use_host_ptr() });

            buffer<ExitingParticle, 1> exitingBuffer(exitingParticles.data(), exitingParticles.size(),
                { cl::sycl::property::buffer::use_host_ptr() });

            try {
                // Simulate particle-vs-particle collisions.
                q.submit([&](handler& handler) {
                    auto p = particleBuffer.get_access<cl::sycl::access::mode::read_write>(handler);
                    handler.parallel_for(particleRange, [=](nd_item<1> it) {
                        auto i = it.get_global_id();

                        glm::vec3 acc = glm::vec3(0.0f);
                        for (int j = 0; j < PARTICLE_COUNT; j++) {
                            glm::vec3 distance = p[j].position - p[i].position;

                            // Only deal with repulsion if the particles are close together.
                            float distanceSqd = glm::length2(distance);
                            if (distanceSqd < SQUARED_REPULSION_DISTANCE)
                            {
                                float distance_inv = 1.0f / sycl::sqrt(distanceSqd + TOO_CLOSE_SOFTENING_CONST);
                                
                                // F = ma and Coulomb's law, approximately.
                                acc += REPULSION_CONST * distance * distance_inv / p[i].mass;
                            }
                        }

                        p[i].acceleration = acc;
                        });
                    }).wait_and_throw();

                // Kernel 2
                q.submit([&](handler& handler) {
                    auto p = particleBuffer.get_access<cl::sycl::access::mode::read_write>(handler);
                    auto r = randomBuffer.get_access<cl::sycl::access::mode::read>(handler);
                    auto e = exitingBuffer.get_access<cl::sycl::access::mode::write>(handler);
                    handler.parallel_for(particleRange, [=](nd_item<1> it) {
                        auto i = it.get_global_id();

                        // Move particles
                        p[i].velocity += p[i].acceleration * TIME_STEP;
                        p[i].position += p[i].velocity * TIME_STEP;
                        p[i].acceleration = glm::vec3(0.0f, 0.0f, 0.0f);

                        // Simulate infinite particles by randomly adding in a particle at the edge
                        // (only if this particle has gone past the edge)
                        if (p[i].position.x < 0 || p[i].position.y < 0 || p[i].position.z < 0 ||
                            p[i].position.x > SIM_MAX || p[i].position.y > SIM_MAX || p[i].position.z > SIM_MAX)
                        {
                            // Add this particle as an exiting particle for simulation results.
                            e[i].valid = true;
                            e[i].mass = p[i].mass;
                            e[i].position = p[i].position;
                            e[i].velocity = p[i].velocity;

                            // Make a new particle somewhere random
                            p[i].position = r[i].position;

                            // Flatten to one plane based on the randomly selected one.
                            switch (r[i].plane)
                            {
                            case 0:
                                p[i].position.x = 0;
                                break;
                            case 1:
                                p[i].position.x = SIM_MAX;
                                break;
                            case 2:
                                p[i].position.y = 0;
                                break;
                            case 3:
                                p[i].position.y = SIM_MAX;
                                break;
                            case 4:
                                p[i].position.z = 0;
                                break;
                            case 5:
                            default:
                                p[i].position.z = SIM_MAX;
                                break;
                            }

                            // Give it a new random velocity
                            p[i].velocity = r[i].velocity;
                        }
                    });
                }).wait_and_throw();
            }
            catch (sycl::exception const& e) {
                std::cout << "SYCL DPC++ Exception:" << std::endl
                    << e.what() << std::endl;
            }
        }

#if ENABLE_GUI
        sync->UpdateParticlePositions(particles);
        sync->UpdateFanPosition(fanRotationMatrix);
#endif

        // Prepare for the next iteration.
        ResetRandomBuffer(randomGenerator, randomData);

        fanZAngle += FAN_ROTATION_SPEED * TIME_STEP;
        fanRotationMatrix = UpdateFanRotation(fanZAngle);

        // Extract out particles that exited and updated the average energy flow out.
        for (long i = 0; i < PARTICLE_COUNT; i++)
        {
            if (exitingParticles[i].valid)
            {
                // The particle's default energy is subtracted to avoid having to do that later.
                // KE (vector) = m * v^2 (v vector normalized)
                float energyFlowOut = exitingParticles[i].mass * glm::length2(exitingParticles[i].velocity) - PARTICLE_DEFAULT_ENERGY;
                exitingEnergyFlow += (energyFlowOut * glm::normalize(exitingParticles[i].velocity));
                exitingTotalEnergy += energyFlowOut;

                ++exitedParticles;
                avgExitPosition += exitingParticles[i].position;
                exitingParticles[i].valid = false;
            }
        }

        // Log simulation status
        float elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(
            std::chrono::steady_clock::now() - simStart).count();
        if ((count % SIM_STATS_REPORT_INTERVAL) == 0)
        {
            std::cout << "  " << 
                count << ": " << 
                count * TIME_STEP << " sec (in " <<
                elapsed_seconds << " sec)." << std::endl;
        } 

        count++;

// GUI runs forever for diagnostics.
#if !ENABLE_GUI || AUTO_EXIT
        if (count == MAX_SIMULATION_STEPS) {
            shouldStopSimulating = true;
        }
#endif
    }

    // These results are going to have the default random flow of generated particles, in addition to any fan contributions.
    // That's fine for me, because I am interested in the comparison of these values with other fans, not the absolute values.
    avgExitPosition = avgExitPosition / (float)(exitedParticles + 1);
    
    // Validation sanity checks:
    // - The exit position should be 5, 5, 5 on average (without a fan).
    // - The directional energy flow should be very low (again, without a fan)
    // - The energy transfer score should be > directionality performance score (always)
    // - Both scores should be near zero (without a fan).
    std::cout << "Average exit position: ("
        << avgExitPosition[0] << ", "
        << avgExitPosition[1] << ", "
        << avgExitPosition[2] << "), with "
        << exitedParticles << " particles." << std::endl;
    std::cout << "Directional Energy Flow: ("
        << exitingEnergyFlow[0] << ", "
        << exitingEnergyFlow[1] << ", "
        << exitingEnergyFlow[2] << ")." << std::endl;
    std::cout << std::endl;
    std::cout << "Energy Transfer Performance Score: " << exitingTotalEnergy << std::endl;
    std::cout << "Directionality Performance Score: " << glm::length(exitingEnergyFlow) << std::endl;
    std::cout << "Done." << std::endl;
}

void RenderThread() {
    sync = new Synchronizer(&shouldStopSimulating);
    renderer = new Renderer(sync);
    if (!renderer->Init(fanMesh))
    {
        std::cout << "Failed to initialize the GUI" << std::endl;
        delete renderer;

        return;
    }

    renderer->Run();

    renderer->Teardown();
    delete renderer;

    shouldStopSimulating = true;
}

int main() {
    // TODO -- make an argument.
    std::string path = std::string("test-fans/plane.stl");

    ModelLoader* modelLoader = new ModelLoader();
    fanMesh = new FanMesh();
    modelLoader->Load(path, fanMesh);

#if ENABLE_GUI
    std::thread rendererThread (&RenderThread);
#endif

    SimulateParticles(fanMesh);

#if ENABLE_GUI
    rendererThread.join();
#endif

    delete modelLoader;
    delete fanMesh;
    return 0;
}
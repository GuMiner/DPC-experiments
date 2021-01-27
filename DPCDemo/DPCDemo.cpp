#include <array>
#include <iomanip>
#include <iostream>
#include <map>
#include <thread>
#include <vector>
#include <CL/sycl.hpp>
#include <CL/sycl/INTEL/fpga_extensions.hpp>

#include "SimConstants.h"

#if ENABLE_GUI
#include "Renderer.h"
Renderer* renderer;
#endif

#include "DeviceQuerier.h"
#include "ModelLoader.h"
#include "Particle.h"
#include "Random.h"

using namespace cl::sycl;

#if ENABLE_GUI
// TO GUI
std::vector<glm::vec3> particlePositions;
std::atomic<bool> shouldReadUpdate(false);

// FROM GUI
std::atomic<bool> hasReadUpdate(true);
std::atomic<bool> reset(false);
#endif

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

void SimulateParticles(FanMesh* fanMesh) {
    queue q = create_device_queue();

    std::vector<Particle> particles;

    Random randomGenerator = Random();
    for (long i = 0; i < PARTICLE_COUNT; i++)
    {
        particles.push_back(Particle(randomGenerator, fanMesh->min[2], fanMesh->max[2]));
    }

    range<1> particleRange(PARTICLE_COUNT);

    long count = 0;
    while(!shouldStopSimulating.load()) {
#if ENABLE_GUI
        if (reset.load()) {
            particles.clear();
            for (long i = 0; i < PARTICLE_COUNT; i++)
            {
                particles.push_back(Particle(randomGenerator, fanMesh->min[2], fanMesh->max[2]));
            }

            reset = false;
        }
#endif

        auto simStart = std::chrono::steady_clock::now();

        {
            buffer<Particle, 1> particleBuffer(particles.data(), PARTICLE_COUNT,
                { cl::sycl::property::buffer::use_host_ptr() });

            try {
                // Simulate
                q.submit([&](handler& handler) {
                    auto p = particleBuffer.get_access<cl::sycl::access::mode::read_write>(handler);
                    handler.parallel_for(particleRange, [=](nd_item<1> it) {
                        auto i = it.get_global_id();
                        float acc0 = p[i].acceleration[0];
                        float acc1 = p[i].acceleration[1];
                        float acc2 = p[i].acceleration[2];
                        for (int j = 0; j < 1; j++) { // particleCount >>>>>> Should drastically speed up performance, at the cost of not doing the right thing.
                            float dx, dy, dz;
                            float distance_sqr = 0.0;
                            float distance_inv = 0.0;

                            dx = p[j].position[0] - p[i].position[0];  // 1flop
                            dy = p[j].position[1] - p[i].position[1];  // 1flop
                            dz = p[j].position[2] - p[i].position[2];  // 1flop

                            distance_sqr =
                                dx * dx + dy * dy + dz * dz + NEARBY_PARTICLE_SOFTENING_CONST;  // 6flops
                            distance_inv = 100.0f / sycl::sqrt(distance_sqr);       // 1div+1sqrt

                            acc0 += dx * GRAVITY_CONST * p[j].mass * distance_inv * distance_inv *
                                distance_inv;  // 6flops
                            acc1 += dy * GRAVITY_CONST * p[j].mass * distance_inv * distance_inv *
                                distance_inv;  // 6flops
                            acc2 += dz * GRAVITY_CONST * p[j].mass * distance_inv * distance_inv *
                                distance_inv;  // 6flops
                        }
                        p[i].acceleration[0] = acc0;
                        p[i].acceleration[1] = acc1;
                        p[i].acceleration[2] = acc2;
                        });
                    }).wait_and_throw();

                // Kernel 2
                q.submit([&](handler& handler) {
                    auto p = particleBuffer.get_access<cl::sycl::access::mode::read_write>(handler);
                    handler.parallel_for(particleRange, [=](nd_item<1> it) {
                            auto i = it.get_global_id();

                            // Move particles
                            p[i].velocity[0] += p[i].acceleration[0] * TIME_STEP;
                            p[i].velocity[1] += p[i].acceleration[1] * TIME_STEP;
                            p[i].velocity[2] += p[i].acceleration[2] * TIME_STEP;

                            p[i].position[0] += p[i].velocity[0] * TIME_STEP;
                            p[i].position[1] += p[i].velocity[1] * TIME_STEP;
                            p[i].position[2] += p[i].velocity[2] * TIME_STEP;
                          
                            p[i].acceleration[0] = 0.0f;
                            p[i].acceleration[1] = 0.0f;
                            p[i].acceleration[2] = 0.0f;

                            // Simulate infinite particles by randomly adding in a particle at the edge,
                            //  if the particle has gone outside of the edge.

                            // Handle bounding box boundary conditions
                            //for (int j = 0; j < 3; j++) {
                            //    if (p[i].position[j] < 0) {
                            //        p[i].velocity[j] = -p[i].velocity[j];
                            //        p[i].position[j] = -p[i].position[j];
                            //    }
                            //
                            //    if (p[i].position[j] > 1) {
                            //        p[i].velocity[j] = -p[i].velocity[j];
                            //        p[i].position[j] = 2 - p[i].position[j];
                            //    }
                            //}
                        });
                    }).wait_and_throw();
            }
            catch (sycl::exception const& e) {
                std::cout << "SYCL DPC++ Exception:" << std::endl
                    << e.what() << std::endl;
            }
        }

#if ENABLE_GUI
        if (hasReadUpdate.load())
        {
            hasReadUpdate = false;

            particlePositions.clear();
            for (const Particle& particle: particles)
            {
                particlePositions.push_back(particle.position);
            }

            shouldReadUpdate = true;
        }
#endif

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
#if !ENABLE_GUI
        if (count == MAX_SIMULATION_STEPS) {
            shouldStopSimulating = true;
        }
#endif
    }

    std::cout << "Done." << std::endl;
}

void RenderThread() {
    renderer = new Renderer(
        &shouldReadUpdate, &hasReadUpdate, &particlePositions, &reset);
    if (!renderer->Init())
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

    particlePositions = std::vector<glm::vec3>();

    ModelLoader* modelLoader = new ModelLoader();
    FanMesh* fanMesh = new FanMesh();
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
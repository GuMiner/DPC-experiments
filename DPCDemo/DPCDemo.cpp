#define ENABLE_GUI 1

#define FPGA 0
#define FPGA_EMULATOR 0

#include <array>
#include <iomanip>
#include <iostream>
#include <map>
#include <thread>
#include <vector>
#include <CL/sycl.hpp>
#if FPGA || FPGA_EMULATOR
#include <CL/sycl/INTEL/fpga_extensions.hpp>
#endif

#if ENABLE_GUI
#include "Renderer.h"
Renderer* renderer;
#endif

#include "DeviceQuerier.h"
#include "Particle.h"
#include "Random.h"

using namespace cl::sycl;

// output message for runtime exceptions
#define EXCEPTION_MSG \
  "    If you are targeting an FPGA hardware, please ensure that an FPGA board is plugged to the system, \n\
        set up correctly and compile with -DFPGA  \n\
    If you are targeting the FPGA emulator, compile with -DFPGA_EMULATOR.\n"

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
#else
  // the default device selector: it will select the most performant device
  // available at runtime.
  default_selector dselector;
#endif

  // create an async exception handler so the program fails more gracefully.
  auto ehandler = [](cl::sycl::exception_list exceptionList) {
    for (std::exception_ptr const &e : exceptionList) {
      try {
        std::rethrow_exception(e);
      } catch (cl::sycl::exception const &e) {
        std::cout << "Caught an asynchronous DPC++ exception, terminating the "
                     "program."
                  << std::endl;
        std::cout << EXCEPTION_MSG;
        std::terminate();
      }
    }
  };

  try {
    // create the devices queue with the selector above and the exception
    // handler to catch async runtime errors the device queue is used to enqueue
    // the kernels and encapsulates all the states needed for execution
    queue q(dselector, ehandler);

    return q;
  } catch (cl::sycl::exception const &e) {
    // catch the exception from devices that are not supported.
    std::cout << "An exception is caught when creating a device queue." << std::endl;
    std::cout << EXCEPTION_MSG;
    std::terminate();
  }
}

std::vector<glm::vec3> particlePositions;
std::atomic<bool> shouldReadUpdate(false);
std::atomic<bool> hasReadUpdate(true);

std::atomic<bool> shouldStopSimulating(false);

void SimulateParticles() {
    long particleCount = 16000;
    long reportInterval = 2;
    float dt = 0.0001f;

    std::vector<Particle> particles;

    Random randomGenerator = Random();
    for (long i = 0; i < particleCount; i++)
    {
        particles.push_back(Particle(randomGenerator));
    }

    queue q = create_device_queue();
    DeviceQuerier::OutputDeviceInfo(q.get_device());

    range<1> particleRange(particleCount);
    float* energy = malloc_shared<float>(1, q);

    // Pulled from NBody demo
    constexpr float kSofteningSquared = 1e-3;
    constexpr float kG = 6.67259e-11;

    long count = 0;
    while(!shouldStopSimulating.load()) {
        auto simStart = std::chrono::steady_clock::now();
        float kenergy = 0;

        {
            buffer<Particle, 1> particleBuffer(particles.data(), particleCount,
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
                        for (int j = 0; j < particleCount; j++) {
                            float dx, dy, dz;
                            float distance_sqr = 0.0;
                            float distance_inv = 0.0;

                            dx = p[j].position[0] - p[i].position[0];  // 1flop
                            dy = p[j].position[1] - p[i].position[1];  // 1flop
                            dz = p[j].position[2] - p[i].position[2];  // 1flop

                            distance_sqr =
                                dx * dx + dy * dy + dz * dz + kSofteningSquared;  // 6flops
                            distance_inv = 100.0f / sycl::sqrt(distance_sqr);       // 1div+1sqrt

                            acc0 += dx * kG * p[j].mass * distance_inv * distance_inv *
                                distance_inv;  // 6flops
                            acc1 += dy * kG * p[j].mass * distance_inv * distance_inv *
                                distance_inv;  // 6flops
                            acc2 += dz * kG * p[j].mass * distance_inv * distance_inv *
                                distance_inv;  // 6flops
                        }
                        p[i].acceleration[0] = acc0;
                        p[i].acceleration[1] = acc1;
                        p[i].acceleration[2] = acc2;
                        });
                    }).wait_and_throw();

                // This bit seems to not work well in debug mode.
                // Compute Kinetic Energy
                // Second kernel updates the velocity and position for all particles
                q.submit([&](handler& handler) {
                        auto p = particleBuffer.get_access<cl::sycl::access::mode::read_write>(handler);
                        handler.parallel_for(nd_range<1>{ particleRange, 1 },
                            ONEAPI::reduction(energy, 0.0f, std::plus<float>()), [=](nd_item<1> it, auto& energy) {

                                auto i = it.get_global_id();
                                p[i].velocity[0] += p[i].acceleration[0] * dt;  // 2flops
                                p[i].velocity[1] += p[i].acceleration[1] * dt;  // 2flops
                                p[i].velocity[2] += p[i].acceleration[2] * dt;  // 2flops

                                p[i].position[0] += p[i].velocity[0] * dt;  // 2flops
                                p[i].position[1] += p[i].velocity[1] * dt;  // 2flops
                                p[i].position[2] += p[i].velocity[2] * dt;  // 2flops
                              
                                p[i].acceleration[0] = 0.f;
                                p[i].acceleration[1] = 0.f;
                                p[i].acceleration[2] = 0.f;

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

                                energy += (p[i].mass *
                                    (p[i].velocity[0] * p[i].velocity[0] + p[i].velocity[1] * p[i].velocity[1] +
                                        p[i].velocity[2] * p[i].velocity[2]));  // 7flops
                            });
                        }).wait_and_throw();

                kenergy = 0.5 * (*energy);
                *energy = 0.f;
            }
            catch (sycl::exception const& e) {
                std::cout << "SYCL DPC++ Exception:" << std::endl
                    << e.what() << std::endl;
            }
        }

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

        // Log simulation status
        float elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(
            std::chrono::steady_clock::now() - simStart).count();
        if ((count % reportInterval) == 0) { // Output status every 2 steps
            float gflops = 1e-9 * ((11. + 18.) * particleCount * particleCount + particleCount * 19.);
            std::cout << " " << std::left << std::setw(8) << count << std::left
                << std::setprecision(5) << std::setw(8) << count * dt
                << std::left << std::setprecision(5) << std::setw(12)
                << kenergy << std::left << std::setprecision(5)
                << std::setw(12) << elapsed_seconds << std::left
                << std::setprecision(5) << std::setw(12)
                << gflops * reportInterval / elapsed_seconds << std::endl;
        } 

        count++;
    }

    std::cout << "Done with simulation!" << std::endl;
}

void RenderThread() {
    renderer = new Renderer(
        &shouldReadUpdate, &hasReadUpdate, &particlePositions);
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
    particlePositions = std::vector<glm::vec3>();

#if defined(ENABLE_GUI)
  std::thread rendererThread (&RenderThread);
#endif

  SimulateParticles();


#if defined(ENABLE_GUI)
  rendererThread.join();
#endif

  return 0;
}
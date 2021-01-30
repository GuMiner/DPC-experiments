#include <algorithm>
#include <memory>
#include <string>
#include <CL/sycl.hpp>
#include <CL/sycl/INTEL/fpga_extensions.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/transform.hpp>

#include "DeviceQuerier.h"
#include "Simulation.h"

Simulation::Simulation(std::atomic<bool>* shouldStopSimulating, FanMesh* const fanMesh)
    : shouldStopSimulating(shouldStopSimulating), randomGenerator(), fanMesh(fanMesh), fanZAngle(0.0f) {

    // Fan rotation (and rotated vertex positions) are pre-computed to avoid excessive device computation.
    UpdateFanRotation();
    UpdateMeshVertices();
}

#if ENABLE_GUI
void Simulation::EnableSimGuiSyncing(Synchronizer* const sync) {
    this->sync = sync;
}
#endif

void Simulation::InitParticleData() {
    for (long i = 0; i < PARTICLE_COUNT; i++) {
        // Don't add starting random particles in the fan, that's not great.
        particles.push_back(Particle(randomGenerator, fanMesh->min[2], fanMesh->max[2]));

        ExitingParticle exitingParticle;
        exitingParticle.valid = false;
        exitingParticles.push_back(exitingParticle);
    }
}

void Simulation::ResetRandomBuffer() {
    // It's very painful to generate random number generators (no API support)
    // So, do it on the host and pass that back to the device https://www.khronos.org/files/sycl/sycl-12-reference-card.pdf
    randomData.clear();
    for (long i = 0; i < PARTICLE_COUNT; i++) {
        randomData.push_back(RandData(randomGenerator));
    }
}

void Simulation::UpdateFanRotation() {
    fanRotationMatrix =
        glm::translate(glm::vec3(SIM_MAX / 2.0f, SIM_MAX / 2.0f, 0.0f)) *
        glm::rotate(fanZAngle, glm::vec3(0.0f, 0.0f, 1.0f)) *
        glm::translate(glm::vec3(-SIM_MAX / 2.0f, -SIM_MAX / 2.0f, 0.0f));
}

void Simulation::UpdateMeshVertices() {
    fanVertices.clear();
    for (size_t i = 0; i < fanMesh->Vertices.size(); i++) {
        fanVertices.push_back(glm::vec3(fanRotationMatrix * glm::vec4(fanMesh->Vertices[i], 1.0f)));
    }
}

// DPC++ logic is all not on the Simulation class to avoid having those imports flow everywhere.
cl::sycl::queue CreateDeviceQueue(std::string deviceType) {
    // "allocating an object of abstract clas type... yay unique ptrs
    cl::sycl::device_selector * deviceSelector = nullptr;

    // Select device from input 
    std::transform(deviceType.begin(), deviceType.end(), deviceType.begin(), std::toupper);
    if (deviceType.compare("CPU") == 0) {
        deviceSelector = new cl::sycl::cpu_selector();
    }
    else if (deviceType.compare("FPGA") == 0) {
        deviceSelector = new cl::sycl::INTEL::fpga_selector();
    }
    else if (deviceType.compare("FPGA_EMULATOR") == 0) {
        deviceSelector = new cl::sycl::INTEL::fpga_emulator_selector();
    }
    else if (deviceType.compare("GPU") == 0) {
        deviceSelector = new cl::sycl::gpu_selector();
    }
    else {
        deviceSelector = new cl::sycl::default_selector();
    }

    // Handle exceptions
    auto ehandler = [](cl::sycl::exception_list exceptionList) {
        for (std::exception_ptr const& e : exceptionList) {
            try {
                std::rethrow_exception(e);
            }
            catch (cl::sycl::exception const& e) {
                std::cout << "Caught an asynchronous DPC++ exception, terminating the program: " << e.what() << std::endl;
                std::terminate();
            }
        }
    };

    // Create device
    try {
        cl::sycl::queue deviceQueue(*deviceSelector, ehandler);
        DeviceQuerier::OutputDeviceInfo(deviceQueue.get_device());
        delete deviceSelector;
        return deviceQueue;
    }
    catch (cl::sycl::exception const& e) {
        std::cout << "An exception was caught when creating a device queue: " << e.what() << std::endl;
        delete deviceSelector;
        std::terminate();
    }
}

cl::sycl::event SimulateParticleToParticleCollisions(cl::sycl::queue& q, cl::sycl::range<1>& particleRange,
    cl::sycl::buffer<Particle, 1>& particleBuffer) {
    return q.submit([&](cl::sycl::handler& handler) {
        auto p = particleBuffer.get_access<cl::sycl::access::mode::read_write>(handler);
        handler.parallel_for(particleRange, [=](cl::sycl::nd_item<1> it) {
            auto i = it.get_global_id();

            glm::vec3 acc = glm::vec3(0.0f);
            for (int j = 0; j < PARTICLE_COUNT; j++) {
                glm::vec3 distance = p[j].position - p[i].position;

                // Only deal with repulsion if the particles are close together.
                // This (and other simulation speedups) won't work well on accelerators that work in lockstep and don't deal well with conditional actions.
                float distanceSqd = glm::length2(distance);
                if (distanceSqd < SQUARED_REPULSION_DISTANCE) {
                    float distance_inv = 1.0f / sycl::sqrt(distanceSqd + TOO_CLOSE_SOFTENING_CONST);

                    // F = ma and Coulomb's law, approximately.
                    acc += REPULSION_CONST * distance * distance_inv / p[i].mass;
                }
            }

            p[i].acceleration = acc;
        });
    });
}

cl::sycl::event SimulateParticleToFanMeshCollisions(cl::sycl::queue& q, cl::sycl::range<1>& particleRange,
    cl::sycl::buffer<Particle, 1>& particleBuffer, std::vector<glm::vec3>& fanVertices, FanMesh* fanMesh,
    cl::sycl::event& dependingEvent) {
    // Particle-to-FanMesh read-only buffers.
    cl::sycl::buffer<glm::vec3, 1> fanVerticesBuffer(fanVertices.data(), fanVertices.size(),
        { cl::sycl::property::buffer::use_host_ptr() });

    cl::sycl::buffer<glm::vec3, 1> fanBoundsMinBuffer(&fanMesh->min, 1,
        { cl::sycl::property::buffer::use_host_ptr() });

    cl::sycl::buffer<glm::vec3, 1> fanBoundsMaxBuffer(&fanMesh->max, 1,
        { cl::sycl::property::buffer::use_host_ptr() });

    // See if particles bounce off of the fan and update their velocities if they will.
    return q.submit([&](cl::sycl::handler& handler) {
        handler.depends_on(dependingEvent);
        auto p = particleBuffer.get_access<cl::sycl::access::mode::read_write>(handler);
        auto fanVertices = fanVerticesBuffer.get_access<cl::sycl::access::mode::read>(handler);
        auto fanMin = fanBoundsMinBuffer.get_access<cl::sycl::access::mode::read>(handler);
        auto fanMax = fanBoundsMaxBuffer.get_access<cl::sycl::access::mode::read>(handler);

        cl::sycl::stream os(1024, 128, handler); // For debugging
        handler.parallel_for(particleRange, [=](cl::sycl::nd_item<1> it) {
            auto i = it.get_global_id();

            // Only attempt to bounce if the particles are near the fan
            if (p[i].position.x > fanMin[0].x - FAN_COLLISION_DISTANCE &&
                p[i].position.x < fanMax[0].x + FAN_COLLISION_DISTANCE &&
                p[i].position.y > fanMin[0].y - FAN_COLLISION_DISTANCE &&
                p[i].position.y < fanMax[0].y + FAN_COLLISION_DISTANCE &&
                p[i].position.z > fanMin[0].z - FAN_COLLISION_DISTANCE &&
                p[i].position.z < fanMax[0].z + FAN_COLLISION_DISTANCE) {
                float closestIntersectionDistance = -1.0f; // None found
                glm::vec3 closestNormal;

                // Scan all triangles in the mesh to figure out if a collision happened.
                for (int j = 0; j < fanVertices.get_count() / 3; j++) {
                    glm::vec3 firstPos = fanVertices[j * 3];
                    glm::vec3 secondPos = fanVertices[j * 3 + 1];
                    glm::vec3 thirdPos = fanVertices[j * 3 + 2];

                    // Without a bit of wiggle, the particles can phase through the fan
                    //  because it moves and the particle does too, at the same time.
                    float wiggle_factor = 1.5f;

                    glm::vec2 unusedCollisionPos;
                    float intersectionDistance;
                    if (glm::intersectRayTriangle(p[i].position, glm::normalize(p[i].velocity),
                        firstPos, secondPos, thirdPos,
                        unusedCollisionPos, intersectionDistance) &&
                        intersectionDistance > 0.0f &&
                        intersectionDistance < (glm::length(p[i].velocity) * (TIME_STEP * wiggle_factor))) {
                        if (closestIntersectionDistance < 0.0f || intersectionDistance < closestIntersectionDistance) {
                            closestIntersectionDistance = intersectionDistance;

                            // Bounces are infrequent, so unlike mesh vertices, we don't re-compute normals all the time.
                            closestNormal = glm::normalize(glm::cross(secondPos - firstPos, thirdPos - firstPos));
                        }
                    }
                }

                // Use the closest collision, if any, for a bounce
                if (closestIntersectionDistance > 0.0f) {
                    glm::vec3 intersectionPoint = p[i].position + glm::normalize(p[i].velocity) * closestIntersectionDistance;

                    // Technically the particle are moving extra quickly to the surface of the mesh
                    // This may need fixing, TODO improve.
                    p[i].position = intersectionPoint;
                    p[i].velocity = glm::length(p[i].velocity) *
                        glm::normalize(glm::reflect(p[i].velocity, closestNormal));
                    // os << p[i].velocity.x << ", " << p[i].velocity.y << ", " << p[i].velocity.z << endl;
                    // TODO add (or subtract) energy in the bounce to account for the fan pushing air around.
                }
            }
        });
    });
}

cl::sycl::event SimulateParticleMotion(cl::sycl::queue& q, cl::sycl::range<1>& particleRange,
    cl::sycl::buffer<Particle, 1>& particleBuffer, std::vector<RandData>& randomData, std::vector<ExitingParticle>& exitingParticles,
    cl::sycl::event& dependingEvent) {

    // Setup particle re-creation / aggregation-specific buffers.
    cl::sycl::buffer<RandData, 1> randomBuffer(randomData.data(), randomData.size(),
        { cl::sycl::property::buffer::use_host_ptr() });

    cl::sycl::buffer<ExitingParticle, 1> exitingBuffer(exitingParticles.data(), exitingParticles.size(),
        { cl::sycl::property::buffer::use_host_ptr() });

    // Move particles and handle boundary conditions
    return q.submit([&](cl::sycl::handler& handler) {
        handler.depends_on(dependingEvent);

        auto p = particleBuffer.get_access<cl::sycl::access::mode::read_write>(handler);
        auto r = randomBuffer.get_access<cl::sycl::access::mode::read>(handler);
        auto e = exitingBuffer.get_access<cl::sycl::access::mode::write>(handler);
        handler.parallel_for(particleRange, [=](cl::sycl::nd_item<1> it) {
            auto i = it.get_global_id();

            // Move particles
            p[i].velocity += p[i].acceleration * TIME_STEP;
            p[i].position += p[i].velocity * TIME_STEP;
            p[i].acceleration = glm::vec3(0.0f, 0.0f, 0.0f);

            // Simulate infinite particles by randomly adding in a particle at the edge
            // (only if this particle has gone past the edge)
            if (p[i].position.x < 0 || p[i].position.y < 0 || p[i].position.z < 0 ||
                p[i].position.x > SIM_MAX || p[i].position.y > SIM_MAX || p[i].position.z > SIM_MAX) {
                // Add this particle as an exiting particle for simulation results.
                e[i].valid = true;
                e[i].mass = p[i].mass;
                e[i].position = p[i].position;
                e[i].velocity = p[i].velocity;

                // Make a new particle somewhere random
                p[i].position = r[i].position;

                // Give it a new random velocity
                p[i].velocity = r[i].velocity;

                // Flatten to one plane based on the randomly selected one, making sure the velocity is away from that plane!
                switch (r[i].plane)
                {
                case 0:
                    p[i].position.x = 0;
                    p[i].velocity.x = sycl::abs(p[i].velocity.x);
                    break;
                case 1:
                    p[i].position.x = SIM_MAX;
                    p[i].velocity.x = -sycl::abs(p[i].velocity.x);
                    break;
                case 2:
                    p[i].position.y = 0;
                    p[i].velocity.y = sycl::abs(p[i].velocity.y);
                    break;
                case 3:
                    p[i].position.y = SIM_MAX;
                    p[i].velocity.y = -sycl::abs(p[i].velocity.y);
                    break;
                case 4:
                    p[i].position.z = 0;
                    p[i].velocity.z = sycl::abs(p[i].velocity.z);
                    break;
                case 5:
                default:
                    p[i].position.z = SIM_MAX;
                    p[i].velocity.z = -sycl::abs(p[i].velocity.z);
                    break;
                }
            }
        });
    });
}

void Simulation::UpdateAggregateResults() {
    const float PARTICLE_DEFAULT_ENERGY = PARTICLE_MASS * PARTICLE_INIT_VELOCITY * PARTICLE_INIT_VELOCITY;
    // Extract out particles that exited and updated the average energy flow out.
    for (long i = 0; i < PARTICLE_COUNT; i++) {
        if (exitingParticles[i].valid) {
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
}

void Simulation::LogSimulationStatus(long count, std::chrono::steady_clock::time_point& simStart) {
    // Log simulation status
    float elapsed_seconds = std::chrono::duration_cast<std::chrono::duration<double>>(
        std::chrono::steady_clock::now() - simStart).count();
    if ((count % SIM_STATS_REPORT_INTERVAL) == 0) {
        std::cout << "  " <<
            count << ": " <<
            count * TIME_STEP << " sec (in " <<
            elapsed_seconds << " sec)." << std::endl;
    }
}

void Simulation::Simulate(std::string deviceToSelect) {
    cl::sycl::queue q = CreateDeviceQueue(deviceToSelect);
    cl::sycl::range<1> particleRange(PARTICLE_COUNT);

    InitParticleData();
    ResetRandomBuffer();

    long count = 0;
    while (!shouldStopSimulating->load()) {
#if ENABLE_GUI
        if (sync->reset.load()) {
            particles.clear();
            for (long i = 0; i < PARTICLE_COUNT; i++) {
                particles.push_back(Particle(randomGenerator, fanMesh->min[2], fanMesh->max[2]));
            }

            sync->reset = false;
        }
#endif

        auto simStart = std::chrono::steady_clock::now();

        // Enforce a scope so task-shared buffers (particles) closes and return to the host.
        {
            cl::sycl::buffer<Particle, 1> particleBuffer(particles.data(), particles.size(),
                { cl::sycl::property::buffer::use_host_ptr() });

            try {
                auto particleCollisionTask = SimulateParticleToParticleCollisions(q, particleRange, particleBuffer);
                auto meshCollisionTask = SimulateParticleToFanMeshCollisions(q, particleRange, particleBuffer,
                    fanVertices, fanMesh, particleCollisionTask);
                auto moveWithBoundaryConditionsTask = SimulateParticleMotion(q, particleRange, particleBuffer,
                    randomData, exitingParticles, meshCollisionTask);

                // Wait for all three tasks as per the sequence here!
                moveWithBoundaryConditionsTask.wait_and_throw();
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
        ResetRandomBuffer();

        fanZAngle += FAN_ROTATION_SPEED * TIME_STEP;
        UpdateFanRotation();
        UpdateMeshVertices();

        UpdateAggregateResults();
        LogSimulationStatus(count, simStart);

        // GUI runs forever for diagnostics, otherwise exit after MAX_SIMULATION_STEPS
#if !ENABLE_GUI || AUTO_EXIT
        if (count == MAX_SIMULATION_STEPS) {
            shouldStopSimulating = true;
        }
#endif

        count++;
    }

    LogFinalResults(count - 1);
}

void Simulation::LogFinalResults(long count) {
    // These results are going to have the default random flow of generated particles, in addition to any fan contributions.
    // That's fine for me, because I am interested in the comparison of these values with other fans, not the absolute values.
    avgExitPosition = avgExitPosition / (float)(exitedParticles + 0.001f); // Just avoid a crash if exitedParticles == 0.

    // Validation sanity checks (without a fan):
    // - The exit position should be 5, 5, 5 on average (without a fan).
    // - Both energy transfer scores should be near 0.
    std::cout << "Average exit position: ("
        << avgExitPosition[0] << ", "
        << avgExitPosition[1] << ", "
        << avgExitPosition[2] << "), with "
        << exitedParticles << " particles in "
        << count << " steps." << std::endl;
    std::cout << "Directional Energy Flow: ("
        << exitingEnergyFlow[0] << ", "
        << exitingEnergyFlow[1] << ", "
        << exitingEnergyFlow[2] << ")." << std::endl;
    std::cout << std::endl;
    std::cout << "Mesh Mass: " << fanMesh->mass << std::endl;
    std::cout << "Energy Transfer Performance Score: " << exitingTotalEnergy << std::endl;
    std::cout << "Directionality Performance Score: " << glm::length(exitingEnergyFlow) << std::endl;
    std::cout << "Done." << std::endl;
}
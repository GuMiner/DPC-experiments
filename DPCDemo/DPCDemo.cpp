#include <atomic>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <argparse/argparse.hpp>

#include "SimConstants.h"

#if ENABLE_GUI
#include "Synchronizer.h"
#include "Renderer.h"
Renderer* renderer;
#endif

#include "FanMesh.h"
#include "ModelLoader.h"
#include "Simulation.h"

std::unique_ptr<FanMesh> fanMesh;
std::atomic<bool> shouldStopSimulating(false);

#if ENABLE_GUI
std::unique_ptr<Synchronizer> sync;

void RenderThread() {
    std::unique_ptr<Renderer> renderer = std::unique_ptr<Renderer>(new Renderer(sync.get()));
    if (!renderer->Init(fanMesh.get())) {
        std::cout << "Failed to initialize the GUI" << std::endl;
        return;
    }

    renderer->Run();
    renderer->Teardown();

    shouldStopSimulating = true;
}
#endif

void LoadFanMesh(std::string path) {
    fanMesh = std::unique_ptr<FanMesh>(new FanMesh());
    std::unique_ptr<ModelLoader> modelLoader = std::unique_ptr<ModelLoader>(new ModelLoader());
    modelLoader->Load(path, fanMesh.get());
}

int main(int argc, char* argv[]) {
    // Parse arguments
    argparse::ArgumentParser program("DPCDemo");

    program.add_argument("-d", "--device")
        .default_value(std::string("default")) // TODO add default to help if not printed out.
        .help("The device DPC++ code will run on. Valid options are: cpu/fpga/fpga_emulator/gpu/default (the default)");

    program.add_argument("-i", "--input")
        .default_value(std::string("test-fans/plane.stl")) // TODO different default
        .help("The path to the .STL fan mesh that will be simulated. Defaults to \"test-fans/plane.stl\"");

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::runtime_error& err) {
        std::cout << err.what() << std::endl;
        std::cout << program;
        return 1;
    }

    // Setup simulation
    LoadFanMesh(program.get<std::string>("--input"));
    std::unique_ptr<Simulation> simulation = std::unique_ptr<Simulation>(
        new Simulation(&shouldStopSimulating, fanMesh.get()));

    // Run simulation
#if ENABLE_GUI
    sync = std::unique_ptr<Synchronizer>(new Synchronizer(&shouldStopSimulating));
    simulation->EnableSimGuiSyncing(sync.get());

    std::thread rendererThread (&RenderThread);
#endif

    simulation->Simulate(program.get<std::string>("--device"));

#if ENABLE_GUI
    rendererThread.join();
#endif

    return 0;
}
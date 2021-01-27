#pragma once
// ---------- Simulation (set 1) ----------
// TODO -- move to be configurable.
#define GPU 0
#define CPU 1
#define FPGA 0
#define FPGA_EMULATOR 0

#define VERBOSE_DEVICE_LOGS 0

// ---------- Display ----------
#define ENABLE_GUI 1
#define ENABLE_MODEL_LOADER_DEBUG_VIEWER 0
#define VERBOSE_RENDERER_LOGS 0

// ---------- Simulation ----------
// Overall simulation bounds
#define MAX_SIMULATION_STEPS 100
#define SIM_STATS_REPORT_INTERVAL 50
#define SIM_MAX 10.0f
#define RAND_GENERATOR_SEED 4321
#define TIME_STEP 0.01f

// Particle simulation details
#define PARTICLE_COUNT 2048
#define INIT_RAND_VELOCITY 1.0f
#define PARTICLE_MASS 10.0f

#define GRAVITY_CONST 6.67259e-11
#define NEARBY_PARTICLE_SOFTENING_CONST 1e-3

// FAN simulation details
#define FAN_CENTER_XY 5.0f
#define FAN_CENTER_Z 5.0f
#define FAN_SIZE_XY 5.0f
#define FAN_DENSITY 1.0f
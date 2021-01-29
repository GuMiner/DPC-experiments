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

#define MOUSE_ROTATION_SPEED 0.001f
#define KEYBOARD_MOVEMENT_SPEED 8.0f
#define KEYBOARD_ROLL_SPEED 0.5f

// ---------- Simulation ----------
// Overall simulation bounds
#define MAX_SIMULATION_STEPS 100
#define SIM_STATS_REPORT_INTERVAL 50
#define SIM_MAX 10.0f
#define RAND_GENERATOR_SEED 4321
#define TIME_STEP 0.0001f

// Particle simulation details
#define PARTICLE_COUNT 512

// https://opentextbc.ca/universityphysicsv2openstax/chapter/pressure-temperature-and-rms-speed/
#define INIT_RAND_VELOCITY 511.0f // m / s, apx speed of sound

// Real particles aren't this heavy, but there also are many more of them.
#define PARTICLE_MASS 1.0f // kg, just to make the math nice.

// http://hyperphysics.phy-astr.gsu.edu/hbase/electric/elefor.html
// TODO -- Make all of these constants physically accurate.
// 0.01f
#define SQUARED_REPULSION_DISTANCE 0.0001f
#define REPULSION_CONST -100.0f
#define TOO_CLOSE_SOFTENING_CONST 0.00000001f

// FAN simulation details
#define FAN_CENTER_XY 5.0f
#define FAN_CENTER_Z 5.0f
#define FAN_SIZE_XY 5.0f
#define FAN_DENSITY 1.0f

// Radians / sec
// 700 RPM by default, a reasonable computer fan speed
#define FAN_ROTATION_SPEED 73.30f
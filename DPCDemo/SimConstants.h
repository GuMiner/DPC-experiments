#pragma once
// ---------- Diagnostics ----------
#define VERBOSE_DEVICE_LOGS 0
#define ENABLE_MODEL_LOADER_DEBUG_VIEWER 0
#define VERBOSE_RENDERER_LOGS 0

// ---------- Display ----------
#define ENABLE_GUI 1

#define MOUSE_ROTATION_SPEED 0.001f
#define KEYBOARD_MOVEMENT_SPEED 8.0f
#define KEYBOARD_ROLL_SPEED 0.5f

// ---------- Simulation ----------
// Overall simulation bounds
#define AUTO_EXIT 0
#define MAX_SIMULATION_STEPS 3000
#define SIM_STATS_REPORT_INTERVAL 50
#define SIM_MAX 10.0f
#define RAND_GENERATOR_SEED 4321
#define TIME_STEP 0.000005f // 5 microseconds. Slow enough for visual inspection.

// Particle simulation details
#define PARTICLE_COUNT 2048

// https://opentextbc.ca/universityphysicsv2openstax/chapter/pressure-temperature-and-rms-speed/
#define PARTICLE_INIT_VELOCITY 511.0f // m / s, apx speed of 'air'. Good enough for a rough estimate

// Real particles aren't this heavy, but there also are many more of them.
// FUTURE_TODO -- Figuring out what to do in math needs more thought.
#define PARTICLE_MASS 1.0f // kg, just to make the math nice.
#define PARTICLE_RADIUS 0.01f // m

// FAN simulation details
#define FAN_CENTER_XY 5.0f
#define FAN_CENTER_Z 5.0f
#define FAN_SIZE_XY 5.0f
#define FAN_DENSITY 1.0f
#define FAN_COLLISION_DISTANCE 0.5f

// Radians / sec
// 700 RPM by default, a reasonable computer fan speed
#define FAN_ROTATION_SPEED 73.30f
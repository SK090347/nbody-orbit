#pragma once
#include "body.hpp"
#include <functional>
#include <vector>

namespace nbody {

// Velocity Verlet / leapfrog-equivalent symplectic integrator.
// One step advances positions and velocities by dt using accelerations
// computed via the provided force callback (typically O(n^2) direct).
void velocity_verlet_step(std::vector<Body>& bodies, double dt, double G,
                          void (*force_fn)(std::vector<Body>&, double));

struct SimConfig {
  double G = 1.0;
  double dt = 1e-3;
  int steps = 10000;
  int frame_every = 10;  // record every N steps
  double soft2 = 1e-6;
};

struct Trajectory {
  SimConfig config;
  std::vector<std::string> names;
  std::vector<double> masses;
  std::vector<Frame> frames;
  std::vector<double> energy;  // total energy at each recorded frame
};

Trajectory run_simulation(std::vector<Body> bodies, const SimConfig& cfg);

}  // namespace nbody

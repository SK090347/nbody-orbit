#pragma once
#include "integrator.hpp"
#include <string>

namespace nbody {

// Minimal JSON writer (no external deps) for trajectory export.
void write_trajectory_json(const Trajectory& traj, const std::string& path);

// Load a simple scenario file:
// { "G":1, "dt":0.001, "steps":10000, "frame_every":10, "soft2":1e-6,
//   "bodies":[{"name":"Sun","mass":1,"x":0,"y":0,"z":0,"vx":0,"vy":0,"vz":0}, ...] }
bool load_scenario(const std::string& path, std::vector<Body>& bodies, SimConfig& cfg,
                   std::string& err);

}  // namespace nbody

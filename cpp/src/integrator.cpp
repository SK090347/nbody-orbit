#include "integrator.hpp"
#include "force.hpp"

namespace nbody {

void velocity_verlet_step(std::vector<Body>& bodies, double dt, double G,
                          void (*force_fn)(std::vector<Body>&, double)) {
  // x <- x + v dt + 0.5 a dt^2
  for (auto& b : bodies) {
    b.pos += b.vel * dt + b.acc * (0.5 * dt * dt);
  }
  // Save old accelerations
  std::vector<Vec3> a_old;
  a_old.reserve(bodies.size());
  for (const auto& b : bodies) {
    a_old.push_back(b.acc);
  }
  force_fn(bodies, G);
  // v <- v + 0.5 (a_old + a_new) dt
  for (std::size_t i = 0; i < bodies.size(); ++i) {
    bodies[i].vel += (a_old[i] + bodies[i].acc) * (0.5 * dt);
  }
}

static Frame snapshot(const std::vector<Body>& bodies, double t) {
  Frame f;
  f.t = t;
  f.bodies.reserve(bodies.size());
  for (const auto& b : bodies) {
    f.bodies.push_back({b.name, b.mass, b.pos, b.vel});
  }
  return f;
}

Trajectory run_simulation(std::vector<Body> bodies, const SimConfig& cfg) {
  for (auto& b : bodies) {
    b.soft2 = cfg.soft2;
  }
  compute_accelerations(bodies, cfg.G);

  Trajectory traj;
  traj.config = cfg;
  for (const auto& b : bodies) {
    traj.names.push_back(b.name);
    traj.masses.push_back(b.mass);
  }

  traj.frames.push_back(snapshot(bodies, 0.0));
  traj.energy.push_back(total_energy(bodies, cfg.G));

  double t = 0.0;
  for (int step = 1; step <= cfg.steps; ++step) {
    velocity_verlet_step(bodies, cfg.dt, cfg.G, compute_accelerations);
    t += cfg.dt;
    if (step % cfg.frame_every == 0 || step == cfg.steps) {
      traj.frames.push_back(snapshot(bodies, t));
      traj.energy.push_back(total_energy(bodies, cfg.G));
    }
  }
  return traj;
}

}  // namespace nbody

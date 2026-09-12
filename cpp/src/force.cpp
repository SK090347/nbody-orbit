#include "force.hpp"
#include <cmath>

namespace nbody {

void compute_accelerations(std::vector<Body>& bodies, double G) {
  const std::size_t n = bodies.size();
  for (auto& b : bodies) {
    b.acc = {0, 0, 0};
  }
  for (std::size_t i = 0; i < n; ++i) {
    for (std::size_t j = i + 1; j < n; ++j) {
      Vec3 r = bodies[j].pos - bodies[i].pos;
      const double soft = 0.5 * (bodies[i].soft2 + bodies[j].soft2);
      const double r2 = r.norm2() + soft;
      const double inv_r = 1.0 / std::sqrt(r2);
      const double inv_r3 = inv_r * inv_r * inv_r;
      const double f = G * inv_r3;
      const Vec3 ai = r * (f * bodies[j].mass);
      const Vec3 aj = r * (-f * bodies[i].mass);
      bodies[i].acc += ai;
      bodies[j].acc += aj;
    }
  }
}

double total_energy(const std::vector<Body>& bodies, double G) {
  double ke = 0.0;
  double pe = 0.0;
  const std::size_t n = bodies.size();
  for (const auto& b : bodies) {
    ke += 0.5 * b.mass * b.vel.norm2();
  }
  for (std::size_t i = 0; i < n; ++i) {
    for (std::size_t j = i + 1; j < n; ++j) {
      Vec3 r = bodies[j].pos - bodies[i].pos;
      const double soft = 0.5 * (bodies[i].soft2 + bodies[j].soft2);
      const double dist = std::sqrt(r.norm2() + soft);
      pe -= G * bodies[i].mass * bodies[j].mass / dist;
    }
  }
  return ke + pe;
}

}  // namespace nbody

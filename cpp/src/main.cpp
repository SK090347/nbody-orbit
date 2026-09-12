#include <cmath>
#include "force.hpp"
#include "integrator.hpp"
#include "json_io.hpp"

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

static void usage(const char* argv0) {
  std::cerr
      << "nbody-orbit — high-performance N-body gravity lab (C++17)\n"
      << "Usage: " << argv0 << " [options]\n"
      << "  --scenario PATH   Initial conditions JSON (default: built-in figure-8)\n"
      << "  --out PATH        Trajectory JSON output (default: data/trajectory.json)\n"
      << "  --steps N         Override steps\n"
      << "  --dt DT           Override timestep\n"
      << "  --frame-every K   Record every K steps\n"
      << "  --G G             Gravitational constant\n"
      << "  --help            Show this help\n";
}

// Famous 3-body figure-8 choreography (Chenciner–Montgomery), G=1, m=1.
static std::vector<nbody::Body> figure8() {
  using nbody::Body;
  using nbody::Vec3;
  const double x1 = 0.97000436;
  const double y1 = -0.24308753;
  const double vx3 = -0.93240737;
  const double vy3 = -0.86473146;
  std::vector<Body> b(3);
  b[0].name = "A";
  b[0].mass = 1;
  b[0].pos = {x1, y1, 0};
  b[0].vel = {-vx3 / 2, -vy3 / 2, 0};
  b[1].name = "B";
  b[1].mass = 1;
  b[1].pos = {-x1, -y1, 0};
  b[1].vel = {-vx3 / 2, -vy3 / 2, 0};
  b[2].name = "C";
  b[2].mass = 1;
  b[2].pos = {0, 0, 0};
  b[2].vel = {vx3, vy3, 0};
  return b;
}

int main(int argc, char** argv) {
  std::string scenario;
  std::string out_path = "data/trajectory.json";
  nbody::SimConfig cfg;
  cfg.G = 1.0;
  cfg.dt = 1e-3;
  cfg.steps = 20000;
  cfg.frame_every = 20;
  cfg.soft2 = 1e-8;

  bool override_steps = false, override_dt = false, override_fe = false, override_G = false;

  for (int i = 1; i < argc; ++i) {
    std::string a = argv[i];
    auto need = [&](const char* flag) -> std::string {
      if (i + 1 >= argc) {
        std::cerr << "missing value for " << flag << "\n";
        std::exit(2);
      }
      return argv[++i];
    };
    if (a == "--help" || a == "-h") {
      usage(argv[0]);
      return 0;
    } else if (a == "--scenario") {
      scenario = need("--scenario");
    } else if (a == "--out") {
      out_path = need("--out");
    } else if (a == "--steps") {
      cfg.steps = std::stoi(need("--steps"));
      override_steps = true;
    } else if (a == "--dt") {
      cfg.dt = std::stod(need("--dt"));
      override_dt = true;
    } else if (a == "--frame-every") {
      cfg.frame_every = std::stoi(need("--frame-every"));
      override_fe = true;
    } else if (a == "--G") {
      cfg.G = std::stod(need("--G"));
      override_G = true;
    } else {
      std::cerr << "unknown arg: " << a << "\n";
      usage(argv[0]);
      return 2;
    }
  }

  std::vector<nbody::Body> bodies;
  if (!scenario.empty()) {
    nbody::SimConfig file_cfg;
    std::string err;
    if (!nbody::load_scenario(scenario, bodies, file_cfg, err)) {
      std::cerr << "scenario error: " << err << "\n";
      return 1;
    }
    if (!override_G) cfg.G = file_cfg.G;
    if (!override_dt) cfg.dt = file_cfg.dt;
    if (!override_steps) cfg.steps = file_cfg.steps;
    if (!override_fe) cfg.frame_every = file_cfg.frame_every;
    cfg.soft2 = file_cfg.soft2;
  } else {
    bodies = figure8();
  }

  std::cout << "nbody-orbit: " << bodies.size() << " bodies, steps=" << cfg.steps
            << " dt=" << cfg.dt << " G=" << cfg.G << "\n";

  auto traj = nbody::run_simulation(bodies, cfg);
  const double E0 = traj.energy.front();
  const double E1 = traj.energy.back();
  const double drift = (E0 != 0.0) ? std::abs((E1 - E0) / E0) : std::abs(E1 - E0);
  std::cout << "E0=" << E0 << " E_final=" << E1 << " |dE/E0|=" << drift << "\n";

  try {
    nbody::write_trajectory_json(traj, out_path);
  } catch (const std::exception& e) {
    std::cerr << e.what() << "\n";
    return 1;
  }
  std::cout << "wrote " << out_path << " (" << traj.frames.size() << " frames)\n";
  return 0;
}

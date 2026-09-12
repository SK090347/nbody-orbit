#include "json_io.hpp"
#include <cctype>
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace nbody {
namespace {

std::string esc(const std::string& s) {
  std::string o;
  o.reserve(s.size() + 8);
  for (char c : s) {
    if (c == '"' || c == '\\') o.push_back('\\');
    o.push_back(c);
  }
  return o;
}

void skip_ws(const std::string& s, std::size_t& i) {
  while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) ++i;
}

bool match(const std::string& s, std::size_t& i, char c) {
  skip_ws(s, i);
  if (i < s.size() && s[i] == c) {
    ++i;
    return true;
  }
  return false;
}

std::string parse_string(const std::string& s, std::size_t& i, std::string& err) {
  skip_ws(s, i);
  if (i >= s.size() || s[i] != '"') {
    err = "expected string";
    return {};
  }
  ++i;
  std::string out;
  while (i < s.size() && s[i] != '"') {
    if (s[i] == '\\' && i + 1 < s.size()) {
      out.push_back(s[i + 1]);
      i += 2;
    } else {
      out.push_back(s[i++]);
    }
  }
  if (i >= s.size()) {
    err = "unterminated string";
    return {};
  }
  ++i;
  return out;
}

double parse_number(const std::string& s, std::size_t& i, std::string& err) {
  skip_ws(s, i);
  std::size_t start = i;
  if (i < s.size() && (s[i] == '-' || s[i] == '+')) ++i;
  while (i < s.size() && (std::isdigit(static_cast<unsigned char>(s[i])) || s[i] == '.' ||
                          s[i] == 'e' || s[i] == 'E' || s[i] == '+' || s[i] == '-')) {
    // crude; stop on invalid continuation after first digit block via strtod
    if ((s[i] == '+' || s[i] == '-') && i > start && s[i - 1] != 'e' && s[i - 1] != 'E') break;
    ++i;
  }
  try {
    return std::stod(s.substr(start, i - start));
  } catch (...) {
    err = "bad number";
    return 0;
  }
}

}  // namespace

void write_trajectory_json(const Trajectory& traj, const std::string& path) {
  std::ofstream out(path);
  if (!out) throw std::runtime_error("cannot write " + path);

  out << std::scientific;
  out << "{\n";
  out << "  \"meta\": {\n";
  out << "    \"G\": " << traj.config.G << ",\n";
  out << "    \"dt\": " << traj.config.dt << ",\n";
  out << "    \"steps\": " << traj.config.steps << ",\n";
  out << "    \"frame_every\": " << traj.config.frame_every << ",\n";
  out << "    \"soft2\": " << traj.config.soft2 << ",\n";
  out << "    \"integrator\": \"velocity_verlet\",\n";
  out << "    \"force\": \"direct_o_n2\",\n";
  out << "    \"bodies\": [";
  for (std::size_t i = 0; i < traj.names.size(); ++i) {
    if (i) out << ", ";
    out << "{\"name\":\"" << esc(traj.names[i]) << "\",\"mass\":" << traj.masses[i] << "}";
  }
  out << "]\n";
  out << "  },\n";
  out << "  \"energy\": [";
  for (std::size_t i = 0; i < traj.energy.size(); ++i) {
    if (i) out << ", ";
    out << traj.energy[i];
  }
  out << "],\n";
  out << "  \"frames\": [\n";
  for (std::size_t fi = 0; fi < traj.frames.size(); ++fi) {
    const auto& f = traj.frames[fi];
    out << "    {\"t\":" << f.t << ",\"bodies\":[";
    for (std::size_t bi = 0; bi < f.bodies.size(); ++bi) {
      const auto& b = f.bodies[bi];
      if (bi) out << ",";
      out << "{\"name\":\"" << esc(b.name) << "\",\"mass\":" << b.mass
          << ",\"x\":" << b.pos.x << ",\"y\":" << b.pos.y << ",\"z\":" << b.pos.z
          << ",\"vx\":" << b.vel.x << ",\"vy\":" << b.vel.y << ",\"vz\":" << b.vel.z << "}";
    }
    out << "]}";
    if (fi + 1 < traj.frames.size()) out << ",";
    out << "\n";
  }
  out << "  ]\n";
  out << "}\n";
}

bool load_scenario(const std::string& path, std::vector<Body>& bodies, SimConfig& cfg,
                   std::string& err) {
  std::ifstream in(path);
  if (!in) {
    err = "cannot open " + path;
    return false;
  }
  std::ostringstream ss;
  ss << in.rdbuf();
  const std::string s = ss.str();
  std::size_t i = 0;
  err.clear();
  bodies.clear();
  cfg = SimConfig{};

  if (!match(s, i, '{')) {
    err = "expected object";
    return false;
  }

  auto read_field = [&](const std::string& key) -> bool {
    if (key == "G") {
      cfg.G = parse_number(s, i, err);
    } else if (key == "dt") {
      cfg.dt = parse_number(s, i, err);
    } else if (key == "steps") {
      cfg.steps = static_cast<int>(parse_number(s, i, err));
    } else if (key == "frame_every") {
      cfg.frame_every = static_cast<int>(parse_number(s, i, err));
    } else if (key == "soft2") {
      cfg.soft2 = parse_number(s, i, err);
    } else if (key == "bodies") {
      if (!match(s, i, '[')) {
        err = "bodies: expected array";
        return false;
      }
      skip_ws(s, i);
      while (i < s.size() && s[i] != ']') {
        if (!match(s, i, '{')) {
          err = "body: expected object";
          return false;
        }
        Body b;
        b.name = "body";
        while (true) {
          skip_ws(s, i);
          if (i < s.size() && s[i] == '}') {
            ++i;
            break;
          }
          if (bodies.size() || true) {
            // allow comma between fields
          }
          if (match(s, i, ',')) continue;
          std::string k = parse_string(s, i, err);
          if (!err.empty()) return false;
          if (!match(s, i, ':')) {
            err = "expected :";
            return false;
          }
          if (k == "name") {
            b.name = parse_string(s, i, err);
          } else if (k == "mass") {
            b.mass = parse_number(s, i, err);
          } else if (k == "x") {
            b.pos.x = parse_number(s, i, err);
          } else if (k == "y") {
            b.pos.y = parse_number(s, i, err);
          } else if (k == "z") {
            b.pos.z = parse_number(s, i, err);
          } else if (k == "vx") {
            b.vel.x = parse_number(s, i, err);
          } else if (k == "vy") {
            b.vel.y = parse_number(s, i, err);
          } else if (k == "vz") {
            b.vel.z = parse_number(s, i, err);
          } else {
            // skip unknown number or string
            skip_ws(s, i);
            if (i < s.size() && s[i] == '"')
              (void)parse_string(s, i, err);
            else
              (void)parse_number(s, i, err);
          }
          if (!err.empty()) return false;
          skip_ws(s, i);
          if (i < s.size() && s[i] == ',') ++i;
        }
        bodies.push_back(b);
        skip_ws(s, i);
        if (i < s.size() && s[i] == ',') ++i;
        skip_ws(s, i);
      }
      if (!match(s, i, ']')) {
        err = "bodies: expected ]";
        return false;
      }
    } else {
      // skip unknown value (number, string, bool, null) — not nested objects
      skip_ws(s, i);
      if (i < s.size() && s[i] == '"')
        (void)parse_string(s, i, err);
      else
        (void)parse_number(s, i, err);
    }
    return err.empty();
  };

  while (true) {
    skip_ws(s, i);
    if (i < s.size() && s[i] == '}') {
      ++i;
      break;
    }
    if (match(s, i, ',')) continue;
    std::string key = parse_string(s, i, err);
    if (!err.empty()) return false;
    if (!match(s, i, ':')) {
      err = "expected : after key";
      return false;
    }
    if (!read_field(key)) return false;
    skip_ws(s, i);
    if (i < s.size() && s[i] == ',') ++i;
  }

  if (bodies.empty()) {
    err = "no bodies in scenario";
    return false;
  }
  return true;
}

}  // namespace nbody

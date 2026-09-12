#pragma once
#include <string>
#include <vector>

namespace nbody {

struct Vec3 {
  double x = 0, y = 0, z = 0;

  Vec3() = default;
  Vec3(double x_, double y_, double z_) : x(x_), y(y_), z(z_) {}

  Vec3 operator+(const Vec3& o) const { return {x + o.x, y + o.y, z + o.z}; }
  Vec3 operator-(const Vec3& o) const { return {x - o.x, y - o.y, z - o.z}; }
  Vec3 operator*(double s) const { return {x * s, y * s, z * s}; }
  Vec3& operator+=(const Vec3& o) {
    x += o.x;
    y += o.y;
    z += o.z;
    return *this;
  }
  double norm2() const { return x * x + y * y + z * z; }
  double norm() const;
};

struct Body {
  std::string name;
  double mass = 1.0;
  Vec3 pos;
  Vec3 vel;
  Vec3 acc;
  // Softening length squared (eps^2); avoids 1/r singularities on close approaches.
  double soft2 = 1e-6;
};

struct FrameBody {
  std::string name;
  double mass;
  Vec3 pos;
  Vec3 vel;
};

struct Frame {
  double t = 0;
  std::vector<FrameBody> bodies;
};

}  // namespace nbody

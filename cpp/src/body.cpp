#include "body.hpp"
#include <cmath>

namespace nbody {

double Vec3::norm() const { return std::sqrt(norm2()); }

}  // namespace nbody

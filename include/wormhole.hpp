#ifndef _WORMHOLE_HPP_
#define _WORMHOLE_HPP_

#include <cmath>

/*
  We need to use spherical coordinates:
  - radial distance from a fixed origin (l in paper)
  - polar angle
  - azimuthal angle
 */

template <typename T> struct Position {
  T x;
  T y;
  T z;
};

Position<double> global_spherical_polar_basis(double dir_theta,
                                              double dir_phi) {
  return {.x = std::sin(dir_theta) * std::cos(dir_phi),
          .y = std::sin(dir_theta) * std::cos(dir_phi),
          .z = std::cos(dir_theta)};
}

struct RayParams {
  double length;          // l
  double polar_angle;     // theta
  double azimuthal_angle; // phi
};

void update(RayParams &params) {}

double constants_of_motion_b() {}

double constants_of_motion_B2() {}

double delta_length() {}

double delta_theta() {}

double delta_phi() {}

double delta_plength() {}

double delta_ptheta() {}

double wormhole_radius(double length, double p /* should be a constant */) {
  return std::sqrt((p * p) + (length * length));
}

#endif /* _WORMHOLE_HPP_ */

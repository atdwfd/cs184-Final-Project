#ifndef _WORMHOLE_HPP_
#define _WORMHOLE_HPP_

#include <cmath>

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

namespace parameters {
inline constexpr double p = 1.0;
inline constexpr double l = 1.0;
inline constexpr double radius = std::sqrt((p * p) + (l * l));
inline constexpr double M = 10000000;
} // namespace parameters

class Ray {
public:
  Ray(Position<double> camera_location, double camera_direction_theta,
      double camera_direction_phi) {
    const Position<double> unit_vector_N = global_spherical_polar_basis(
        camera_direction_theta, camera_direction_phi);
    p_l = -unit_vector_N.x;
    p_theta = parameters::radius * unit_vector_N.z;
    l = camera_location.x;
    theta = camera_location.y;
    phi = camera_location.z;
  }

  /* Camera position */
  double l;     /* length */
  double theta; /* polar angle */
  double phi;   /* azimuthal angle */

  /* Light ray's canoncial momenta */
  double p_l;
  double p_theta;
  double p_phi;
};

double constants_of_motion_b() {}

double constants_of_motion_B2() {}

double constants_drdl(Ray& ray){
  return (2/M_PI) * std::atan(2 * ray.l / (M_PI * parameters::M));
}

double delta_length(Ray& ray) {
  return ray.p_l;
}

double delta_theta(Ray& ray) {
  return ray.p_theta/(parameters::radius * parameters::radius);
}

double delta_phi(Ray& ray) {
  return constants_of_motion_b() /(parameters::radius * parameters::radius * std::sin(ray.theta) * std::sin(ray.theta));
}

double delta_plength(Ray& ray) {
  return constants_of_motion_B2() * constants_of_motion_B2() * constants_drdl(ray) / (parameters::radius * parameters::radius * parameters::radius);
}

double delta_ptheta(Ray& ray) {
  return constants_of_motion_b() * constants_of_motion_b() * std::cos(ray.theta) / (parameters::radius * parameters::radius * std::sin(ray.theta) * std::sin(ray.theta) * std::sin(ray.theta));
}

double wormhole_radius(double length, double p /* should be a constant */) {
  return std::sqrt((p * p) + (length * length));
}

#endif /* _WORMHOLE_HPP_ */

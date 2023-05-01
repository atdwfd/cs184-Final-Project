#ifndef WORMHOLE_SIMULATE_HPP__
#define WORMHOLE_SIMULATE_HPP__

#include <glm/glm.hpp>
#include <iostream>

/*
  This file is not to be used in the final project. Instead, it is an attempt to
  debug our fragment shader code. */

using namespace glm;

const float PI = 3.14159265359;
const float STEP_SIZE = 10000.0;
const float FOV = 60.0;
const float T_THRESHOLD =
    -10000000.0; // We start at 0 and descend to this value.
const float M = 2.0e30;
const float rho = 4.0e10;
const float a = 10.0; // rho / 10.0;

const vec3 i_camera_pos = vec3(0.0, PI / 2, 0.0);
const vec2 i_resolution = vec2(800.0, 600.0);

struct Ray {
  /* Camera position. */
  float l;
  float theta;
  float phi;

  /* Light ray's canonical momenta. */
  float p_l;
  float p_theta;
  float p_phi;
};

inline std::ostream &operator<<(std::ostream &os, const Ray &ray) {
  os << "{ l: " << ray.l << ", "
     << "theta: " << ray.theta << ", "
     << "phi: " << ray.phi << ", "
     << "p_l: " << ray.p_l << ", "
     << "p_theta: " << ray.p_l << ", "
     << "p_phi: " << ray.p_phi << '}';

  return os;
}

inline vec3
global_spherical_polar_basis(vec2 dir) { // this should be theta and phi
  return vec3(sin(dir.x) * cos(dir.y), sin(dir.x) * sin(dir.y), cos(dir.x));
}

/* ----- Constants of motion. ----- */
inline float constants_of_motion_b(Ray ray) { return ray.p_phi; }

inline float constants_of_motion_B2(Ray ray) {
  float sin_theta = sin(ray.theta);
  float sin_theta_squared = sin_theta * sin_theta;
  float p_theta_squared = ray.p_theta * ray.p_theta;
  float p_phi_squared = ray.p_phi * ray.p_phi;

  float B2 = p_theta_squared + (p_phi_squared / sin_theta_squared);

  // Debug information
  printf("sin_theta: %f, sin_theta_squared: %f, p_theta_squared: %f, "
         "p_phi_squared: %f, B2: %f\n",
         sin_theta, sin_theta_squared, p_theta_squared, p_phi_squared, B2);

  return B2;
  /*
  float sin_theta = sin(ray.theta);
  return ray.p_theta * ray.p_theta +
         (ray.p_phi * ray.p_phi) / (sin_theta * sin_theta);
  */
}

inline float constants_drdl(Ray ray) {
  return (2.0 / PI) * atan(2.0 * ray.l / (PI * M));
}

inline float constants_radius(Ray ray) {
  float abs_l = abs(ray.l);
  if (abs_l > a) {
    float x = 2.0 * (abs(ray.l) - a) / (PI * M);
    return rho + M * (x * atan(x) - 0.5 * log(1.0 + x * x));
  } else {
    return rho;
  }
}

/* ----- Derivatives. ----- */

inline float delta_l(Ray ray) {
  float result = ray.p_l;
  // Print the input and output values
  printf("delta_l: ray.p_l = %f, result = %f\n", ray.p_l, result);
  return result;
}

inline float delta_theta(Ray ray) {
  return ray.p_theta / (constants_radius(ray) * constants_radius(ray));
}

inline float delta_phi(Ray ray) {
  float b = constants_of_motion_b(ray);
  float r = constants_radius(ray);
  float sin_theta = sin(ray.theta);
  return b / (r * r * sin_theta * sin_theta);
}

inline float delta_p_l(Ray ray) {
  float B2 = constants_of_motion_B2(ray);
  float r = constants_radius(ray);
  float drdl = constants_drdl(ray);
  float result = B2 * B2 * drdl / (r * r * r);

  // Print the input and intermediate values, as well as the output value
  printf("delta_p_l: B2 = %f, r = %f, drdl = %f, result = %f\n", B2, r, drdl,
         result);
  return result;
  /*
  float B2 = constants_of_motion_B2(ray);
  float r = constants_radius(ray);
  return B2 * B2 * constants_drdl(ray) / (r * r * r);
  */
}

inline float delta_p_theta(Ray ray) {
  float b = constants_of_motion_b(ray);
  float sin_theta = sin(ray.theta);
  float r = constants_radius(ray);
  return b * b * cos(ray.theta) / (r * r * sin_theta * sin_theta * sin_theta);
}

inline Ray initial_ray(vec3 camera_dir) {
  Ray ray;
  vec3 cart_to_spherical =
      vec3(length(camera_dir), acos(camera_dir.z / length(camera_dir)),
           atan(camera_dir.y, camera_dir.x));
  vec3 unit_vector_N = global_spherical_polar_basis(
      vec2(cart_to_spherical.y, cart_to_spherical.z));
  //  vec3 unit_vector_N = global_spherical_polar_basis(camera_dir.yz);

  ray.l = i_camera_pos.x;
  ray.theta = i_camera_pos.y;
  ray.phi = i_camera_pos.z;

  unit_vector_N = normalize(unit_vector_N);

  ray.p_l = -unit_vector_N.x;
  ray.p_theta = constants_radius(ray) * unit_vector_N.z;
  ray.p_phi = constants_radius(ray) * sin(ray.theta) * -unit_vector_N.y;

  std::cout << "Ray initial values: " << ray << '\n';

  return ray;
}

inline float take_step(float curr_val, float delta) {
  return curr_val - delta * STEP_SIZE;
}

inline void ray_take_step(Ray &ray) {
  /* Update camera position. */
  Ray ray_cpy = ray;
  ray.l = take_step(ray.l, delta_l(ray_cpy));
  ray.theta = take_step(ray.theta, delta_theta(ray_cpy));
  ray.phi = take_step(ray.phi, delta_phi(ray_cpy));
  /* Update canonical momenta. */
  ray.p_l = take_step(ray.p_l, delta_p_l(ray_cpy));
  ray.p_theta = take_step(ray.p_theta, delta_p_theta(ray_cpy));
}

/*
  As a simplification, we take that the camera and image plane are sufficiently
  far from the wormhole so that the ray would not be bent before reaching the
  desired pixel (i.e., we assume the ray moves in a straight line from the
  camera to the pixel). */
inline vec3 camera_direction(vec2 gl_FragCoord) {
  vec2 normalized_coords = (gl_FragCoord - 0.5f * i_resolution) /
                           min(i_resolution.x, i_resolution.y);
  const float tan_half_fov = tan(radians(FOV) * 0.5);
  vec3 direction = vec3(normalized_coords * tan_half_fov, 1.0);
  return normalize(direction);
}

inline void integrate_geodesic(Ray &ray) {
  for (float t = 0.0; t >= T_THRESHOLD; t -= STEP_SIZE) {
    ray_take_step(ray);
    std::cout << "Ray: " << ray << '\n';
  }
}

// as a simplification for now, let's just do one color if it hits the lower
// celestial sphere and another color if it hits the other.

inline vec4 simulate(vec2 gl_FragCoord) {
  const vec4 red = vec4(1.0, 0.0, 0.0, 1.0);
  const vec4 blue = vec4(0.0, 0.0, 1.0, 1.0);
  Ray ray = initial_ray(camera_direction(gl_FragCoord));
  integrate_geodesic(ray);

  std::cout << ray << '\n';

  if (ray.l < 0.0) {
    return red;
    // saturn side
  } else {
    return blue;
  }
}

#endif /* WORMHOLE_SIMULATE_HPP__ */

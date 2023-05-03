#ifndef WORMHOLE_SIMULATE_HPP__
#define WORMHOLE_SIMULATE_HPP__

#include <glm/glm.hpp>
#include <iostream>

/*
  This file is not to be used in the final project. Instead, it is an attempt to
  debug our fragment shader code. */

using namespace glm;

const float PI = 3.14159265359;
const float STEP_SIZE = 1.0;
const float FOV = 60.0;
const float T_THRESHOLD = -1000000000.0; // We start at 0 and descend to this value.
const float M = 2.0e30;
const float rho = 4.0e10;
const float a = 10.0; //rho / 10.0;
const vec2 i_resolution{800.f, 600.f};
const vec3 i_camera_pos{0.f, 2.0f / PI, 0.f};


struct ConstantsOfMotion {
    float b;
    float B2;
};

struct Ray {
  /* Position. */
  float l; 
  float theta;
  float phi;

  /* Canonical momenta. */
  float p_l;
  float p_theta;
  float p_phi;

  ConstantsOfMotion constants;
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

inline vec3 globalSphericalPolarBasis(vec2 dir) { // this should be theta and phi
  return vec3(sin(dir.x) * cos(dir.y),
	      sin(dir.x) * sin(dir.y),
	      cos(dir.x));
}

/* ----- Constants of motion. ----- */

inline float constantsOfMotion_b(Ray ray) {
  return ray.p_phi;
}

inline float constantsOfMotion_B2(Ray ray) {
  float sin_theta = sin(ray.theta);
  const auto result = ray.p_theta * ray.p_theta + (ray.p_phi * ray.p_phi) / (sin_theta * sin_theta);
  std::cout << "B2 called with ray = " << ray << "(result = " << result << ")\n";
  return result;
}

inline float constants_drdl(Ray ray) {
  return (2.0 / PI) * atan(2.0 * ray.l / (PI * M));  
}  

inline float radius(Ray ray) {
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
  return ray.p_l;
}

inline float delta_theta(Ray ray) {
  float r = radius(ray);
  return ray.p_theta / (r * r);
}

inline float delta_phi(Ray ray) {
  float b = ray.constants.b;
  float r = radius(ray);
  float sin_theta = sin(ray.theta);
  return b / (r * r * sin_theta * sin_theta);
}

inline float delta_p_l(Ray ray) {
  float B2 = ray.constants.B2;
  float r = radius(ray);
  float drdl = constants_drdl(ray);
  return B2 * drdl / (r * r * r);
}

float delta_p_theta(Ray ray) {
  float b = ray.constants.b;
  float sin_theta = sin(ray.theta);
  float r = radius(ray);
  return b * b * cos(ray.theta) / (r * r  * sin_theta * sin_theta * sin_theta);
}

/*
  As a simplification, we take that the camera and image plane are sufficiently
  far from the wormhole so that the ray would not be bent before reaching the
  desired pixel (i.e., we assume the ray moves in a straight line from the camera
  to the pixel). */
inline vec3 cameraDirection(vec2 fragCoord) {
  vec2 normalized_coords = (fragCoord - i_resolution * 0.5f) / min(i_resolution.x, i_resolution.y);
  const float tan_half_fov = tan(radians(FOV) * 0.5);
  vec3 direction = vec3(normalized_coords * tan_half_fov, 1.0);
  return normalize(direction);
}

/* Initialize the ray, taking the direction in spherical coordinates. */
inline Ray constructRay(vec2 dir) {
  Ray ray;
  vec3 unitVectorN = globalSphericalPolarBasis(dir);  

  ray.l = i_camera_pos.x;
  ray.theta = i_camera_pos.y;
  ray.phi = i_camera_pos.z;

  unitVectorN = normalize(unitVectorN);
  
  ray.p_l = -unitVectorN.x;
  ray.p_theta = radius(ray) * unitVectorN.z;
  ray.p_phi = radius(ray) * sin(ray.theta) * -unitVectorN.y;

  ray.constants.b = constantsOfMotion_b(ray);
  ray.constants.B2 = constantsOfMotion_B2(ray);

  return ray;
}

inline float takeStep(float value, float delta, float stepSize) {
  /* We subtract as t moves downwards. */
  return value - delta * stepSize;
}

inline Ray rayTakeStep(Ray ray, float stepSize) {
  Ray next = ray;
  /* Update position. */
  std::cout << "radius = " << radius(ray) << '\n';
  next.l = takeStep(ray.l, delta_l(ray), stepSize);
  next.theta = takeStep(ray.theta, delta_theta(ray), stepSize);
  next.phi = takeStep(ray.phi, delta_phi(ray), stepSize);
  /* Update canonical momenta. */
  next.p_l = takeStep(ray.p_l, delta_p_l(ray), stepSize);
  next.p_theta = takeStep(ray.p_theta, delta_p_theta(ray), stepSize);
  return next;
}

/*
float evaluateFunction(float initX, float delta, float y, float initY) {
  return init_y + delta * (t - init_t);
}
*/


/*
Ray RKF45(in Ray ray, float tStart, float tEnd, float tolerance) {
  float t = tStart;
  float stepSize = 0.01;
  while (t > tEnd) {
    

    Ray k1 = stepSize * ray;
    // ray + 2/9 * k1
    Ray k2 = stepSize * rayTakeStep(rayAdd(ray, 2.0 / 9.0) * k1, (2.0 / 9.0) * stepSize);
    Ray k1 = rayTakeStep(ray, stepSize);
    Ray k2 = rayTakeStep(
    // continue here...
  }
}
*/

vec3 cartesianToSpherical(vec3 coord) {
  float r = length(coord);
  float theta = acos(coord.z / length(coord));
  float phi = sign(coord.y) * acos(coord.x / sqrt(coord.x * coord.x + coord.y + coord.y));
  return vec3(r, theta, phi);
}  

vec4 raytrace(vec2 fragCoord) {
  auto c = cartesianToSpherical(cameraDirection(fragCoord));
  Ray ray = constructRay(vec2(c.x, c.y));
  for (float t = 0.0; t >= T_THRESHOLD; t -= STEP_SIZE) {
    std::cout << "t = " << t << "\tray = " << ray << '\n';
    ray = rayTakeStep(ray, STEP_SIZE);
  }
  /* As a simplification for now, let's just do one color if it hits the lower
  celestial sphere and another color if it hits the other. */
  if (ray.l < 0.0) {
    return vec4(1.0, 0.0, 0.0, 1.0);
  } else {
    return vec4(0.0, 0.0, 1.0, 1.0);
  }    
}

inline vec4 simulate(vec2 fragCoord) {
  return raytrace(fragCoord);
}

#endif /* WORMHOLE_SIMULATE_HPP__ */

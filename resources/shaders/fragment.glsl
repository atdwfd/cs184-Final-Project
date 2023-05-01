#version 330

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform vec2 i_resolution;
uniform vec3 i_camera_pos;

in vec2 frag_tex_coord;

out vec4 color;

const float PI = 3.14159265359;
const float STEP_SIZE = 10000.0;
const float FOV = 60.0;
const float T_THRESHOLD = -1000000000.0; // We start at 0 and descend to this value.
const float M = 2.0e30;
const float rho = 4.0e10;
const float a = rho / 10.0;

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

vec3 global_spherical_polar_basis(vec2 dir) { // this should be theta and phi
  return vec3(sin(dir.x) * cos(dir.y),
	      sin(dir.x) * sin(dir.y),
	      cos(dir.x));
}

/* ----- Constants of motion. ----- */
float constants_of_motion_b(Ray ray) {
  return ray.p_phi;
}

float constants_of_motion_B2(Ray ray) {
  float sin_theta = sin(ray.theta);
  return ray.p_theta * ray.p_theta + (ray.p_phi * ray.p_phi) / (sin_theta * sin_theta);
}

float constants_drdl(Ray ray) {
  return (2.0 / PI) * atan(2.0 * ray.l / (PI * M));  
}  

float constants_radius(Ray ray){
  float abs_l = ray.l;
  if (abs_l > a) {
    float x = 2.0 * (abs(ray.l) - a) / (PI * M);
    return rho + M * (x * atan(x) - 0.5 * log(1.0 + x * x));
  } else {
    return rho;
  }
}

/* ----- Derivatives. ----- */

float delta_l(Ray ray) {
  return ray.p_l;
}

float delta_theta(Ray ray) {
  return ray.p_theta / (constants_radius(ray) * constants_radius(ray));
}

float delta_phi(Ray ray) {
  float b = constants_of_motion_b(ray);
  float r = constants_radius(ray);
  float sin_theta = sin(ray.theta);
  return b / (r * r * sin_theta * sin_theta);
}

float delta_p_l(Ray ray) {
  float B2 = constants_of_motion_B2(ray);
  float r = constants_radius(ray);
  return B2 * B2 * constants_drdl(ray) / (r * r * r);
}

float delta_p_theta(Ray ray) {
  float b = constants_of_motion_b(ray);
  float sin_theta = sin(ray.theta);
  float r = constants_radius(ray);
  return b * b * cos(ray.theta) / (r * r  * sin_theta * sin_theta * sin_theta);
}  

Ray initial_ray(vec3 camera_dir) {
  Ray ray;
  vec3 unit_vector_N = global_spherical_polar_basis(camera_dir.yz);

  ray.l = i_camera_pos.x;
  ray.theta = i_camera_pos.y;
  ray.phi = i_camera_pos.z;
  
  ray.p_l = -unit_vector_N.x;
  ray.p_theta = constants_radius(ray) * unit_vector_N.z;
  ray.p_phi = constants_radius(ray) * sin(ray.theta) * -unit_vector_N.y;
  
  return ray;
}

float take_step(float curr_val, float delta) {
  return curr_val + delta * STEP_SIZE;
}

void ray_take_step(inout Ray ray) {
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
  desired pixel (i.e., we assume the ray moves in a straight line from the camera
  to the pixel). */
vec3 camera_direction() {
  vec2 normalized_coords = (gl_FragCoord.xy - i_resolution * 0.5) / min(i_resolution.x, i_resolution.y);
  const float tan_half_fov = tan(radians(FOV) * 0.5);
  vec3 direction = vec3(normalized_coords * tan_half_fov, -1.0);
  return normalize(direction);
}

void integrate_geodesic(inout Ray ray) {
  for (float t = 0.0; t >= T_THRESHOLD; t -= STEP_SIZE) {
    ray_take_step(ray);
  }
}  

// as a simplification for now, let's just do one color if it hits the lower
// celestial sphere and another color if it hits the other. 

void main() {
  const vec4 red = vec4(1.0, 0.0, 0.0, 1.0);
  const vec4 blue = vec4(0.0, 0.0, 1.0, 1.0);
  Ray ray = initial_ray(camera_direction());
  integrate_geodesic(ray);
  // get the intersection point on the sphere.
  if (ray.l < 0.0) {
    color = red; 
    // saturn side 
  }
  else {
    color = blue;
  }
}

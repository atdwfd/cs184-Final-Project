#version 330

uniform sampler2D texture0;
uniform vec2 i_resolution;
uniform vec3 i_camera_pos;

/* Wormhole parameters. */
uniform float p;
uniform float l;
uniform float radius;
uniform float M;

in vec2 frag_tex_coord;

out vec4 color;

const float PI = 3.14159265359;
const float STEP_SIZE = 0.01;

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
  return vec2(sin(dir.x) * cos(dir.y),
	      sin(dir.x) * sin(dir.y),
	      cos(dir.x));
}

/* ----- Constants of motion. ----- */
float constants_of_motion(const Ray& ray) {
  return ray.p_phi;
}

float constants_of_motion_B2(const Ray& ray) {
  return ray.p_theta * ray.p_theta + (ray.p_phi * ray.p_phi) / (sin(ray.theta) * sin(ray.theta));
}

float constants_drdl(const Ray& ray) {
  return (2.0 / PI) * atan(2 * ray.l / (PI * M));  
}  

/* ----- Derivatives. ----- */

float delta_l(const Ray& ray) {
  return ray.l;
}

float delta_theta(const Ray& ray) {
  return ray.p_theta / (radius * radius);
}

float delta_phi(const Ray& ray) {
  return constants_of_motion_b(ray) / (radius * radius * radius * sin(ray.theta) * sin(ray.theta);
}

float delta_p_l(const Ray& ray) {
  const float B2 = constants_of_motion_B2(ray);
  return B2 * B2 * constants_drdl(ray) / (radius * radius * radius);
}

float delta_p_theta(const Ray& ray) {
  const float b = constants_of_motion_b(ray);
  const float sin_theta = sin(ray.theta);
  return b * b * cos(ray.theta) / (radius * radius * radius * sin_theta * sin_theta);
}  

Ray initial_ray(vec3 camera_dir) {
  Ray ray;
  vec2 unit_vector_N = global_spherical_polar_basis(camera_dir.xy);
  ray.p_l = -unit_vector_N.x;
  ray.p_theta = radius * unit_vector_N.z;
  ray.l = camera_location.x;
  ray.theta = camera_location.y;
  ray.phi = camera_location.z;
}

float take_step(float curr_val, float delta) {
  return curr_val + delta * STEP_SIZE;
}

void ray_take_step(Ray& ray) {
  /* Update camera position. */
  ray.l = take_step(ray.l, delta_l(ray));
  ray.theta = take_step(ray.theta, delta_theta(ray));
  ray.phi = take_step(ray.phi, delta_phi(ray));
  /* Update canonical momenta. */
  ray.p_l = take_step(ray.p_l, delta_p_l(ray));
  ray.p_theta = take_step(ray.p_theta, delta_p_theta(ray));
}

vec3 camera_direction(float fov) {
  vec2 normalized_coords = (gl_FragCoord - i_resolution * 0.5)
    / min(i_resolution.x, i_resolution.y);
  const float tan_half_fov = tan(radians(fov) * 0.5);
  vec3 direction = vec3(normalized_coords * tan_half_fov, -1.0);
  return normalize(direction);
}

void main() {
  color = texture(texture0, frag_tex_coord);

}

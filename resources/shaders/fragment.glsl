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
const float a = 10.0; //rho / 10.0;

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

vec3 globalSphericalPolarBasis(vec2 dir) { // this should be theta and phi
  return vec3(sin(dir.x) * cos(dir.y),
	      sin(dir.x) * sin(dir.y),
	      cos(dir.x));
}

/* ----- Constants of motion. ----- */

float constantsOfMotion_b(Ray ray) {
  return ray.p_phi;
}

float constantsOfMotion_B2(Ray ray) {
  float sin_theta = sin(ray.theta);
  return ray.p_theta * ray.p_theta + (ray.p_phi * ray.p_phi) / (sin_theta * sin_theta);
}

float constants_drdl(Ray ray) {
  return (2.0 / PI) * atan(2.0 * ray.l / (PI * M));  
}  

float radius(Ray ray) {
  float abs_l = abs(ray.l);
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
  float r = radius(ray);
  return ray.p_theta / (r * r);
}

float delta_phi(Ray ray) {
  float b = ray.constants.b;
  float r = radius(ray);
  float sin_theta = sin(ray.theta);
  return b / (r * r * sin_theta * sin_theta);
}

float delta_p_l(Ray ray) {
  float B2 = ray.constants.B2;
  float r = radius(ray);
  float drdl = constants_drdl(ray);
  return B2 * B2 * constants_drdl(ray) / (r * r * r);
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
vec3 cameraDirection() {
  vec2 normalized_coords = (gl_FragCoord.xy - i_resolution * 0.5) / min(i_resolution.x, i_resolution.y);
  const float tan_half_fov = tan(radians(FOV) * 0.5);
  vec3 direction = vec3(normalized_coords * tan_half_fov, 1.0);
  return normalize(direction);
}

/* Initialize the ray, taking the direction in spherical coordinates. */
Ray constructRay(vec2 dir) {
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

float takeStep(float value, float delta) {
  /* We subtract as t moves downwards. */
  return value - delta * STEP_SIZE;
}

Ray rayTakeStep(in Ray ray) {
  Ray next = ray;
  /* Update position. */
  next.l = takeStep(ray.l, delta_l(ray));
  next.theta = takeStep(ray.theta, delta_theta(ray));
  next.phi = takeStep(ray.phi, delta_phi(ray));
  /* Update canonical momenta. */
  next.p_l = takeStep(ray.p_l, delta_p_l(ray));
  next.p_theta = takeStep(ray.p_theta, delta_p_theta(ray));
  return next;
}

vec3 cartesianToSpherical(vec3 coord) {
  float r = length(coord);
  float theta = acos(coord.z / length(coord));
  float phi = sign(coord.y) * acos(coord.x / sqrt(coord.x * coord.x + coord.y + coord.y));
  return vec3(r, theta, phi);
}  

vec4 raytrace() {
  Ray ray = constructRay(cartesianToSpherical(cameraDirection()).yz);
  for (float t = 0.0; t >= T_THRESHOLD; t -= STEP_SIZE) {
    ray = rayTakeStep(ray);
  }
  /* As a simplification for now, let's just do one color if it hits the lower
  celestial sphere and another color if it hits the other. */
  if (ray.l < 0.0) {
    return vec4(1.0, 0.0, 0.0, 1.0);
  } else {
    return vec4(0.0, 0.0, 1.0, 1.0);
  }    
}


void main() {
  color = raytrace();
}

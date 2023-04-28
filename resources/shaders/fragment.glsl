#version 330

uniform sampler2D texture0;
in vec2 frag_tex_coord;

out vec4 color;

void main() {
  color = texture(texture0, frag_tex_coord);
}

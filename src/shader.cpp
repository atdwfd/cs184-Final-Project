#include "shader.hpp"

#include <glad/gl.h>

#include <array>
#include <iostream>

Shader::Shader(std::string src, ShaderStep step)
    : src_{std::move(src)}, step_{step} {}

auto Shader::compile() -> bool {
  const auto gl_step =
      step_ == ShaderStep::Vertex ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
  const auto gl_src = static_cast<const GLchar*>(src_.data());
  const auto gl_size = static_cast<GLint>(src_.size());
  
  internal_id_ = glCreateShader(gl_step);
  glShaderSource(internal_id_, 1, &gl_src, &gl_size);

  GLint success;
  glGetShaderiv(internal_id_, GL_COMPILE_STATUS, &success);
  if (!success) {
    std::array<char, 512> log{};
    glGetShaderInfoLog(internal_id_, sizeof(log), nullptr, log.data());
    std::cerr << "[ERROR]: Shader failed to compile: " << log.data() << '\n';
  }
  return success;
}

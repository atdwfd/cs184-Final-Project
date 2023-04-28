#include "shader.hpp"

#include <glad/gl.h>

#include <array>
#include <fstream>
#include <iostream>
#include <iterator>

Shader::Shader(std::string src, ShaderStep step)
    : src_{std::move(src)}, step_{step} {}

static constexpr auto translate_step(ShaderStep step) {
  return step == ShaderStep::Vertex ? GL_VERTEX_SHADER : GL_FRAGMENT_SHADER;
}

auto Shader::compile() -> bool {
  const auto gl_src = static_cast<const GLchar *>(src_.data());
  const auto gl_size = static_cast<GLint>(src_.size());

  GLid = glCreateShader(translate_step(step_));
  glShaderSource(GLid, 1, &gl_src, &gl_size);

  GLint success;
  glGetShaderiv(GLid, GL_COMPILE_STATUS, &success);
  if (!success) {
    std::array<char, 512> log{};
    glGetShaderInfoLog(GLid, sizeof(log), nullptr, log.data());
    std::cerr << "[ERROR]: Shader failed to compile: " << log.data() << '\n';
  }
  return success;
}

auto Shader::from_file(std::string_view file_path, ShaderStep step) -> Shader {
  std::ifstream fin{file_path.data()};
  if (fin.fail()) {
    std::cerr << "[ERROR]: Failed to open shader file " << file_path << '\n';
    std::exit(-1);
  }
  return Shader{std::string{std::istreambuf_iterator<char>{fin},
                            std::istreambuf_iterator<char>{}},
                step};
}

auto ShaderProgram::attach(const Shader &shader) -> ShaderProgram & {
  glAttachShader(GLid, translate_step(shader.step()));
  return *this;
}

auto ShaderProgram::link() -> bool {
  glLinkProgram(GLid);
  GLint success;
  glGetProgramiv(GLid, GL_LINK_STATUS, &success);
  return success;
}

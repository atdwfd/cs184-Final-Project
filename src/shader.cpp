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
  auto gl_src = static_cast<const GLchar*>(src_.data());

  GLid = glCreateShader(translate_step(step_));

  if (GLid == 0) {
    std::cerr << "[ERROR]: Failed to create shader\n";
    return false;
  }

  glShaderSource(GLid, 1, &gl_src, nullptr);
  glCompileShader(GLid);

  GLint success;
  glGetShaderiv(GLid, GL_COMPILE_STATUS, &success);
  if (!success) {
    std::array<char, 512> log{};
    glGetShaderInfoLog(GLid, log.size(), nullptr, log.data());
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

auto ShaderProgram::attach(const Shader& shader) -> ShaderProgram& {
  glAttachShader(GLid, shader.GLid);
  return *this;
}

auto ShaderProgram::use() -> void { glUseProgram(GLid); }

auto ShaderProgram::link() -> bool {
  glLinkProgram(GLid);
  GLint success;
  glGetProgramiv(GLid, GL_LINK_STATUS, &success);
  if (!success) {
    std::array<char, 512> log{};
    glGetProgramInfoLog(GLid, log.size(), nullptr, log.data());
    std::cerr << "[ERROR]: Shader failed to link: " << log.data() << '\n';
  }
  return success;
}

auto ShaderProgram::set_texture_uniform(const Texture& texture,
                                        std::string_view uniform_name) -> void {
  const auto loc = glGetUniformLocation(GLid, uniform_name.data());
  glUniform1i(loc, texture.texture_unit());
}

#ifndef WORMHOLE_SHADER_HPP__
#define WORMHOLE_SHADER_HPP__

#include <fstream>
#include <string>
#include <string_view>

#include "detail/globject.hpp"
#include "texture.hpp"

enum class ShaderStep { Vertex, Fragment };

class ShaderProgram;

class Shader : private detail::GLObject {
public:
  Shader(std::string src, ShaderStep step);

  Shader() = delete;
  Shader(const Shader&) = delete;
  Shader(Shader&&) = delete;

  /* Attempt to compile the shader. Returns whether it was successful.
    Compilation errors printed to STDERR. */
  auto compile() -> bool;

  inline auto step() const -> ShaderStep { return step_; }

  static auto from_file(std::string_view file_path, ShaderStep step) -> Shader;

  friend ShaderProgram;

private:
  std::string src_;
  ShaderStep step_;
};

class ShaderProgram : private detail::GLObject {
public:
  ShaderProgram() : detail::GLObject{glCreateProgram()} {}

  ShaderProgram(const ShaderProgram&) = delete;
  ShaderProgram(ShaderProgram&&) = delete;

  /* Attach the shader to the program. */
  auto attach(const Shader& shader) -> ShaderProgram&;

  /* Link the program. */
  auto link() -> bool;

  /* Use (activate) the program. */
  auto use() -> void;

  auto set_texture_uniform(const Texture& texture,
                           std::string_view uniform_name) -> void;
};

#endif /* WORMHOLE_SHADER_HPP__ */

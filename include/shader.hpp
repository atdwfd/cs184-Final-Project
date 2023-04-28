#ifndef WORMHOLE_SHADER_HPP__
#define WORMHOLE_SHADER_HPP__

#include <fstream>
#include <string>
#include <string_view>

#include <glad/gl.h>

namespace detail {
struct GLObject {
  GLuint GLid;
};
} // namespace detail

enum class ShaderStep { Vertex, Fragment };

class Shader : private detail::GLObject {
public:
  Shader(std::string src, ShaderStep step);

  Shader() = delete;
  Shader(const Shader &) = delete;
  Shader(Shader &&) = delete;

  /* Attempt to compile the shader. Returns whether it was successful.
    Compilation errors printed to STDERR. */
  auto compile() -> bool;

  inline auto step() const -> ShaderStep { return step_; }

  static auto from_file(std::string_view file_path, ShaderStep step) -> Shader;

private:
  std::string src_;
  ShaderStep step_;
};

class ShaderProgram : private detail::GLObject {
public:
  ShaderProgram() : detail::GLObject{glCreateProgram()} {}

  ShaderProgram(const ShaderProgram &) = delete;
  ShaderProgram(ShaderProgram &&) = delete;

  /* Attach the shader to the program. */
  auto attach(const Shader &shader) -> ShaderProgram &;

  /* Link the program. */
  auto link() -> bool;
};

#endif /* WORMHOLE_SHADER_HPP__ */

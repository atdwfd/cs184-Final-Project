#ifndef WORMHOLE_SHADER_HPP__
#define WORMHOLE_SHADER_HPP__

#include <fstream>
#include <string>
#include <string_view>

#include <glad/gl.h>

enum class ShaderStep { Vertex, Fragment };

class Shader {
public:
  Shader(std::string src, ShaderStep step);

  /* Attempt to compile the shader. Returns whether it was successful.
    Compilation errors printed to STDERR. */
  auto compile() -> bool;

  inline auto step() const -> ShaderStep { return step_; }

private:
  std::string src_;
  ShaderStep step_;
  GLuint internal_id_;
};

#endif /* WORMHOLE_SHADER_HPP__ */

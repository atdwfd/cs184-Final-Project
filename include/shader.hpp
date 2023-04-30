#ifndef WORMHOLE_SHADER_HPP__
#define WORMHOLE_SHADER_HPP__

#include <fstream>
#include <string>
#include <string_view>
#include <type_traits>

#include <glm/glm.hpp>

#include "detail/globject.hpp"
#include "texture.hpp"

template <typename T>
concept Uniform = std::is_same_v<T, int> || std::is_same_v<T, float> ||
    std::is_same_v<T, glm::vec2> || std::is_same_v<T, glm::vec3> ||
    std::is_same_v<T, glm::mat2> || std::is_same_v<T, glm::mat3> ||
    std::is_same_v<T, glm::mat4>;

enum class ShaderStep { Vertex, Fragment };

class ShaderProgram;

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

  friend ShaderProgram;

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

  /* Use (activate) the program. */
  auto use() -> void;


  template <Uniform U>
  auto set_uniform(int loc, const U& value) -> void {
    if constexpr (std::is_same_v<U, int>) {
      glUniform1i(loc, value);
    } else if constexpr (std::is_same_v<U, float>) {
      glUniform1f(loc, value);
    } else if constexpr (std::is_same_v<U, glm::vec2>) {
      glUniform2fv(loc, 1, &value[0]);
    } else if constexpr (std::is_same_v<U, glm::vec3>) {
      glUniform3fv(loc, 1, &value[0]);
    } else if constexpr (std::is_same_v<U, glm::vec4>) {
      glUniform4fv(loc, 1, &value[0]);
    } else if constexpr (std::is_same_v<U, glm::mat2>) {
      glUniformMatrix2fv(loc, 1, GL_FALSE, &value[0][0]);
    } else if constexpr (std::is_same_v<U, glm::mat3>) {
      glUniformMatrix3fv(loc, 1, GL_FALSE, &value[0][0]);
    } else if constexpr (std::is_same_v<U, glm::mat4>) {
      glUniformMatrix4fv(loc, 1, GL_FALSE, &value[0][0]);
    }
  }

  template <Uniform U>
  auto set_uniform(std::string_view uniform_name, const U &value) -> int {
    const auto loc = glGetUniformLocation(GLid, uniform_name.data());
    set_uniform(loc, value);
    return loc;
  }

  // TODO: overload set_uniform to include texture

  /* Set a texture uniform using TEXTURE with name UNIFORM_NAME. */
  auto set_texture_uniform(const Texture &texture,
                           std::string_view uniform_name) -> void;
};

#endif /* WORMHOLE_SHADER_HPP__ */

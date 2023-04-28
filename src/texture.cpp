#include "texture.hpp"

#include <stb_image.h>

#include <filesystem>
#include <functional>
#include <iostream>

#include "detail/globject.hpp"

Texture::Texture() { glGenTextures(1, &GLid); }

auto Texture::from_file(std::string_view file_path) -> Texture {
  if (!std::filesystem::exists(file_path)) {
    std::cerr << "[ERROR]: Texture image does not exist (" << file_path
              << ")\n";
    std::exit(-1);
  }
  Texture t{};
  unsigned char* data =
      stbi_load(file_path.data(), &t.width_, &t.height_, &t.nr_channels_, 0);

  if (!data) {
    std::cerr << "Failed to load texture image (" << file_path << ")\n";
    std::exit(-1);
  }

  const auto format = std::invoke([&]() {
    switch (t.nr_channels_) {
      case 1:
        return GL_RED;
      case 3:
        return GL_RGB;
      case 4:
        return GL_RGBA;
      default:
        std::cerr << "[ERROR]: Texture image format not supported ("
                  << file_path << ")\n";
        std::exit(-1);
    }
  });

  /* Bind and load texture. */
  glBindTexture(GL_TEXTURE_2D, t.GLid);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, format, t.width_, t.height_, 0, format,
               GL_UNSIGNED_BYTE, data);
  glGenerateMipmap(GL_TEXTURE_2D);

  stbi_image_free(data);

  return t;
}

static auto translate(int texture_unit) -> GLenum {
  switch (texture_unit) {
    case 0:
      return GL_TEXTURE0;
    case 1:
      return GL_TEXTURE1;
    case 2:
      return GL_TEXTURE2;
    case 3:
      return GL_TEXTURE3;
    case 4:
      return GL_TEXTURE4;
    case 5:
      return GL_TEXTURE5;
    case 6:
      return GL_TEXTURE6;
    case 7:
      return GL_TEXTURE7;
    case 8:
      return GL_TEXTURE8;
    case 9:
      return GL_TEXTURE9;
    default:
      throw std::out_of_range("Texture unit index out of range [0, 9]");
  }
}

auto Texture::bind(int texture_unit) -> void {
  glActiveTexture(translate(texture_unit));
  glBindTexture(GL_TEXTURE_2D, GLid);
  texture_unit_ = texture_unit;
}

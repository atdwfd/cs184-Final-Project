#ifndef WORMHOLE_TEXTURE_HPP__
#define WORMHOLE_TEXTURE_HPP__

#include <string_view>

#include "detail/globject.hpp"

class Texture : private detail::GLObject {
public:
  Texture();
  auto bind(int texture_unit) -> void;
  inline auto texture_unit() const {
    return texture_unit_;
  }
  
  static auto from_file(std::string_view file_path) -> Texture;

  
private:
  int width_;
  int height_;
  int nr_channels_;
  int texture_unit_{ -1 };
};

#endif /* WORMHOLE_TEXTURE_HPP__ */

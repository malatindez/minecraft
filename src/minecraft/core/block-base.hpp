#pragma once
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <array>
#include <parsers/yaml/yaml.hpp>
#include <shader.hpp>

namespace minecraft::core {
// storage for block data,
// holder for a TileEntity logic
class BlockBase final {
 public:
  BlockBase(std::array<uint32_t, 6> const &diffuse_textures,
            std::array<uint32_t, 6> const &specular_textures, uint8_t hardness)
      : diffuse_textures_(diffuse_textures),
        specular_textures_(specular_textures),
        hardness_(hardness) {}

  BlockBase(BlockBase const &) = default;
  BlockBase(BlockBase &&) = default;
  BlockBase &operator=(BlockBase const &) = default;
  BlockBase &operator=(BlockBase &&) = default;

  void BindTextures(uint32_t offset = 0) const noexcept {
    for (uint8_t i = 0; i < 6; i++) {
      glActiveTexture(GL_TEXTURE0 + i + offset);
      glBindTexture(GL_TEXTURE_2D, diffuse_textures_[i]);
    }
    for (uint8_t i = 0; i < 6; i++) {
      glActiveTexture(GL_TEXTURE6 + i + offset);
      glBindTexture(GL_TEXTURE_2D, specular_textures_[i]);
    }
  }

  std::array<uint32_t, 6> const &diffuse_textures() const noexcept {
    return diffuse_textures_;
  }
  std::array<uint32_t, 6> const &specular_textures() const noexcept {
    return specular_textures_;
  }
  uint32_t hardness() const noexcept { return hardness_; }

 private:
  std::array<uint32_t, 6> diffuse_textures_;
  std::array<uint32_t, 6> specular_textures_;
  uint32_t hardness_;
};

}  // namespace minecraft::core

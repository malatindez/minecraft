#pragma once
#include <stdint.h>

#include <string>

#include <parsers/yaml/yaml.hpp>

namespace minecraft::core {
class ItemBase final {
public:
  ItemBase(std::string_view const str_id, uint32_t id)
      : str_id_(str_id), id_(id) {}
  ItemBase(ItemBase const &) = default;
  ItemBase(ItemBase &&) = default;
  ItemBase &operator=(ItemBase const &) = default;
  ItemBase &operator=(ItemBase &&) = default;

  std::string str_id() const noexcept { return str_id_; }
  uint32_t id() const noexcept { return id_; }

  static ItemBase Load(yaml::Entry const &entry) {}

private:
  std::string str_id_; // minecraft:stone, minecraft:grass, etc.
  uint32_t id_;        // 1, 2, etc.
};

} // namespace minecraft::core

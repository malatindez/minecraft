#pragma once

#include <spdlog/spdlog.h>

#include <map>
#include <parsers/yaml/yaml.hpp>
#include <resources/resources.hpp>
#include <vector>

#include "core/block-base.hpp"
#include "core/interfaces/updatable.hpp"
#include "core/item-base.hpp"

namespace minecraft::core {
using BlockBaseMap = std::map<std::string, BlockBase>;
using ItemBaseMap = std::map<std::string, ItemBase>;
class Core final {
 public:
  Core(Core &&core) = default;
  Core(Core const &core) = default;
  Core &operator=(Core &&core) = default;
  Core &operator=(Core const &core) = default;

  static inline Core &GetInstance() { return instance(); }
  static inline Core &instance() {
    if (!instance_) {
      LoadInstance();
    }
    return *instance_;
  }

  inline BlockBaseMap const &blocks() const noexcept { return *blocks_; }
  inline ItemBaseMap const &items() const noexcept { return *items_; }

 private:
  Core();
  static void LoadInstance();
  std::shared_ptr<BlockBaseMap> blocks_;
  std::shared_ptr<ItemBaseMap> items_;
  std::shared_ptr<std::set<Updatable>> update_list_;

  resource::Entry const &resources_;
  static std::shared_ptr<Core> instance_;
};
}  // namespace minecraft::core

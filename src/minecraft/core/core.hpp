#pragma once

#include <spdlog/spdlog.h>

#include <map>
#include <vector>

#include <parsers/yaml/Yaml.hpp>
#include <resources/resources.hpp>

#include "core/block-base.hpp"
#include "core/interfaces/updatable.hpp"
#include "core/item-base.hpp"

namespace minecraft::core {
class Core final {
public:
  using BlockBaseMap = std::map<std::string, BlockBase>;
  using ItemBaseMap = std::map<std::string, ItemBase>;

  static inline Core &GetInstance() { return instance(); }
  static inline Core &instance() {
    if (!instance_) {
      LoadInstance();
    }
    return *instance_;
  }

  constexpr BlockBaseMap const &blocks() const noexcept { return *blocks_; }
  constexpr ItemBaseMap const &items() const noexcept { return *items_; }

private:
  static void RecursiveItemLoader(resource::Entry const &folder,
                                  BlockBaseMap &blocks, ItemBaseMap &items,
                                  std::string current_string = "") {
    for (resource::Entry const &entry : folder) {
      if (entry.is_folder()) {
        RecursiveItemLoader(entry, blocks, items,
                            current_string.empty()
                                ? current_string + entry.name()
                                : current_string + ":" + entry.name());
        return;
      }
      std::string data = entry.string();
      auto yaml = yaml::Parse(entry.string());
      if (yaml.contains("type")) {
        if (yaml["type"] == "block") {
          BlockBase::Load(yaml);
        } else if (yaml["type"] == "item") {
          ItemBase::Load(yaml);
        }
      } else {
        spdlog::info("Failed to load {}\n", entry.name());
      }
    }
  }

  Core() : resources_(resource::LoadResources("resources.pack")) {
    RecursiveItemLoader(resources_ / "data", *blocks_, *items_);
  }
  // create a singleton instance
  static void LoadInstance() {
    static std::mutex mutex;
    std::lock_guard lock(mutex);
    if (!instance_) {
      instance_ = std::make_shared<Core>(new Core());
    }
  }
  std::shared_ptr<BlockBaseMap> blocks_;
  std::shared_ptr<ItemBaseMap> items_;
  std::shared_ptr<std::set<Updatable>> update_list_;

  resource::Entry const &resources_;
  static std::shared_ptr<Core> instance_;
};

bool ends_with(std::string str, std::string suffix) {
  if (suffix.size() > str.size()) {
    return false;
  }
  for (size_t i = suffix.size() - 1; i != size_t(-1); i--) {
    if (str[str.size() - suffix.size() + i] != suffix[i]) {
      return false;
    }
  }
  return true;
}

} // namespace minecraft::core

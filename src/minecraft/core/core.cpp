#include "core.hpp"
namespace minecraft::core {
void RecursiveItemLoader(resource::Entry const &folder, BlockBaseMap &blocks,
                         ItemBaseMap &items, std::string current_string = "") {
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
        // BlockBase::Load(yaml);
      } else if (yaml["type"] == "item") {
        // ItemBase::Load(yaml);
      }
    } else {
      spdlog::info("Failed to load {}\n", entry.name());
    }
  }
}
Core::Core()
    : resources_(resource::LoadResources("resources.pack") / "resources") {
  RecursiveItemLoader(resources_ / "data", *blocks_, *items_);
}
void Core::LoadInstance() {
  static std::mutex mutex;
  std::lock_guard lock(mutex);
  if (!instance_) {
    instance_ = std::shared_ptr<Core>{new Core()};
  }
}

std::shared_ptr<Core> Core::instance_;
}  // namespace minecraft::core
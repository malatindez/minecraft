#include "Resources.hpp"
using namespace resource;

std::shared_ptr<Directory> Resources::LoadResources(
    std::filesystem::path path_to_file) {
  auto resource =
      AtomicIfstreamPointer(std::make_shared<std::ifstream>(
          path_to_file, std::ios::binary));
  if (!resource.Lock()->is_open()) {
    throw std::invalid_argument("Cannot open the provided file" +
                                path_to_file.string());
  }
  std::unique_lock lock(resource_mutex_);
  auto dir = std::make_shared<Directory>(0, resource);
  tree_.push_back(dir);
  resource_handles_.emplace_back(path_to_file, resource);
  return dir;
}
void Resources::UnloadResources(std::filesystem::path const& path_to_file) {
  std::unique_lock lock(resource_mutex_);
  {
    AtomicIfstreamPointer ref;
    for (auto const& [path, pointer] : resource_handles_) {
      if (path == path_to_file) {
        ref = pointer;
      }
    }
    auto it = tree_.begin();
    for (; it != tree_.end() && (*it)->resource_file_ptr() == ref; it++)
      ;
    if (it != tree_.end()) {
      tree_.erase(it);
    }
  }
  auto it = resource_handles_.begin();
  for (; it != resource_handles_.end() && it->first != path_to_file; it++)
    ;
  if (it != resource_handles_.end()) {
    it->second.Lock()->close();
    resource_handles_.erase(it);
  }
}

std::shared_mutex Resources::resource_mutex_;
std::vector<std::shared_ptr<Directory>> Resources::tree_;
std::vector<std::pair<std::filesystem::path, AtomicIfstreamPointer>>
    Resources::resource_handles_;
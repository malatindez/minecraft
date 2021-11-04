#include "Resources.hpp"
using namespace resource;

std::shared_ptr<Directory> Resources::LoadResources(
    std::string_view const& path_to_file, bool const& overwrite,
    bool const& merge) {
  auto resource =
      AtomicIfstreamPointer(std::make_shared<std::basic_ifstream<std::byte>>(
          path_to_file, std::ios::binary));
  if (!resource.Lock()->is_open()) {
    std::string buf(path_to_file.begin(), path_to_file.end());
    throw std::invalid_argument("Cannot open the provided file" + buf + ".");
  }
  std::unique_lock lock(resource_mutex_);
  auto dir = std::make_shared<Directory>(0, resource);
  tree_.push_back(dir);
  return dir;
}

std::shared_mutex Resources::resource_mutex_;
std::vector<std::shared_ptr<Directory>> Resources::tree_;
std::vector<AtomicIfstreamPointer> Resources::resource_handles_;
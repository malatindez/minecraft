#include "Directory.hpp"
using namespace resource;

using FilePtr = Directory::FilePtr;
using DirectoryPtr = Directory::DirectoryPtr;

static inline uint64_t BytesToUint64(const std::byte* buf) {
  return (uint64_t(buf[0]) << 0) | (uint64_t(buf[1]) << 8) |
         (uint64_t(buf[2]) << 16) | (uint64_t(buf[3]) << 24) |
         (uint64_t(buf[4]) << 32) | (uint64_t(buf[5]) << 40) |
         (uint64_t(buf[6]) << 48) | (uint64_t(buf[7]) << 56);
}

Directory::Directory(uint64_t const& begin,
                     AtomicIfstreamPointer const& resource_file_ptr)
    : BaseResource(begin, resource_file_ptr) {
  auto dir_header_ = FilePtr(new File(begin, resource_file_ptr));
  name_ = dir_header_->name();
  size_ = dir_size_;
  auto data = dir_header_->data()->data();
  std::vector<uint64_t> resources(dir_header_->size() / 4);
  // Process files
  bool flag = true;
  for (auto buf = data; buf < data + size_; buf += sizeof(uint64_t)) {
    uint64_t file_begin = BytesToUint64(buf);
    if (flag) {
      flag = file_begin == 0;
      if (flag) {
        files_.push_back(FilePtr(new File(file_begin, resource_file_ptr)));
      }
    } else {
      dirs_.push_back(DirectoryPtr(new Directory(file_begin, resource_file_ptr)));
    }
  }
  dir_size_ = CalculateDirSize();
}

uint64_t Directory::size() const noexcept { return dir_size_; }
uint64_t Directory::isFile() const noexcept { return false; }

std::vector<FilePtr> Directory::files() const noexcept { return files_; }
std::vector<DirectoryPtr> Directory::directories() const noexcept {
  return dirs_;
}
std::vector<DirectoryPtr> Directory::dirs() const noexcept { return dirs_; }

static inline std::pair<std::string, std::string> ltrim_path(
    std::string_view const& path) {
  auto it = std::find_if(path.begin(), path.end(),
                         [&](char const& c) { return c == '/' || c == '\\'; });
  auto it2 = it;
  if (it != path.end()) {
    it2++;
  }
  return std::pair<std::string, std::string>(std::string(path.begin(), it),
                                             std::string(it2, path.end()));
}

bool Directory::DirExists(std::string_view const& dirpath) const noexcept {
  return ResourceExists(dirpath) == std::byte{ 2 };
}
bool Directory::FolderExists(std::string_view const& dirpath) const noexcept {
  return ResourceExists(dirpath) == std::byte{ 2 };
}

bool Directory::DirectoryExists(
    std::string_view const& dirpath) const noexcept {
  return ResourceExists(dirpath) == std::byte{ 2 };
}

bool Directory::FileExists(std::string_view const& filepath) const noexcept {
  return ResourceExists(filepath) == std::byte{ 1 };
}

DirectoryPtr Directory::GetFolder(std::string_view const& dirpath) const {
  return GetDir(dirpath);
}

DirectoryPtr Directory::GetDirectory(std::string_view const& dirpath) const {
  return GetDir(dirpath);
}

DirectoryPtr Directory::GetDir(std::string_view const& dirpath) const {
  auto [left, right] = ltrim_path(dirpath);
  if (right.empty()) {
    auto itr = GetDirectoryIterator(dirpath);
    if (itr != dirs_.end()) {
      return *itr;
    }
  } else if (DirExists(left)) {
    return GetDirectory(left)->GetDir(right);
  }
  return nullptr;
}

FilePtr Directory::GetFile(std::string_view const& filepath) const {
    auto [left, right] = ltrim_path(filepath);
    if (right.empty()) {
    auto itr = GetFileIterator(filepath);
    if (itr != files_.end()) {
      return *itr;
    }
  } else if (DirExists(left)) {
    return GetDirectory(left)->GetFile(right);
  }
  return nullptr;
}

std::byte Directory::ResourceExists(std::string_view const& path) const noexcept {
    auto [left, right] = ltrim_path(path);
  if (right.empty()) {
      return (GetDirectoryIterator(path) == dirs_.end() ? std::byte{ 2 } : std::byte{ 0 }) |
           (GetFileIterator(path) == files_.end() ? std::byte{ 1 } : std::byte{ 0 });
  }
  if (DirExists(left)) {
      GetDirectory(left)->ResourceExists(right);
  }
  return std::byte{ 0 };
}

std::vector<DirectoryPtr>::const_iterator Directory::GetDirectoryIterator(
    std::string_view const& dirpath) const noexcept {
  std::shared_lock lock(*mutex_);
  return std::find_if(dirs_.begin(), dirs_.end(), [&](DirectoryPtr const& dir) {
    return dir->name() == dirpath;
  });
}
std::vector<FilePtr>::const_iterator Directory::GetFileIterator(
    std::string_view const& filepath) const noexcept {
  std::shared_lock lock(*mutex_);
  return std::find_if(files_.begin(), files_.end(), [&](FilePtr const& file) {
    return file->name() == filepath;
  });
}

uint64_t Directory::CalculateDirSize() const noexcept {
  auto dl = [](uint64_t a, DirectoryPtr const& b) { return a + b->size(); };
  auto fl = [](uint64_t a, FilePtr const& b) { return a + b->size(); };
  std::shared_lock lock(*mutex_);
  return std::accumulate(dirs_.begin(), dirs_.end(), (uint64_t)0, dl) +
         std::accumulate(files_.begin(), files_.end(), (uint64_t)0, fl) + size_;
}
void Directory::UpdateDirSize() noexcept { dir_size_ = CalculateDirSize(); }

void Directory::Sort() noexcept {
  std::unique_lock lock(*mutex_);
  std::sort(dirs_.begin(), dirs_.end(),
            [](DirectoryPtr const& first, DirectoryPtr const& second) {
              return first->name().compare(second->name());
            });
  std::sort(files_.begin(), files_.end(),
            [](FilePtr const& first, FilePtr const& second) {
              return first->name().compare(second->name());
            });
}
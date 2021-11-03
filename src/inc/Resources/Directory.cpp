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
static inline uint16_t BytesToUint16(const std::byte* buf) {
  return (uint16_t(buf[0]) << 0) | (uint16_t(buf[1]) << 8);
}
Directory::Directory(const uint64_t& begin,
                     const AtomicIfstreamPointer& resource_file_ptr)
    : BaseResource(begin, resource_file_ptr) {
  FilePtr dir_header_(
      std::shared_ptr<File>(new File(begin, resource_file_ptr)));
  name_ = dir_header_->name();
  auto data = dir_header_->data()->data();
  std::vector<uint64_t> resources(dir_header_->size() / 4);
  // Process files
  bool flag = true;
  for (auto buf = data; buf < data + size_; buf += sizeof(uint64_t)) {
    uint64_t file_begin = BytesToUint64(buf);
    if (flag) {
      flag = file_begin == 0;
      if (flag) {
        files_.push_back(
            std::shared_ptr<File>(new File(file_begin, resource_file_ptr)));
      }
    } else {
      dirs_.push_back(std::shared_ptr<Directory>(
          new Directory(file_begin, resource_file_ptr)));
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
    const std::string_view& path) {
  auto it = std::find_if(path.begin(), path.end(),
                         [&](const char& c) { return c == '/' || c == '\\'; });
  auto it2 = it;
  if (it != path.end()) {
    it2++;
  }
  return std::pair<std::string, std::string>(std::string(path.begin(), it),
                                             std::string(it2, path.end()));
}

bool Directory::DirExists(const std::string_view& dirpath) const noexcept {
  return ResourceExists(dirpath) & 2;
}
bool Directory::FolderExists(const std::string_view& dirpath) const noexcept {
  return ResourceExists(dirpath) & 2;
}

bool Directory::DirectoryExists(
    const std::string_view& dirpath) const noexcept {
  return ResourceExists(dirpath) & 2;
}

bool Directory::FileExists(const std::string_view& filepath) const noexcept {
  return ResourceExists(filepath) & 1;
}

DirectoryPtr Directory::GetFolder(const std::string_view& dirpath) const {
  return GetDir(dirpath);
}

DirectoryPtr Directory::GetDirectory(const std::string_view& dirpath) const {
  return GetDir(dirpath);
}

DirectoryPtr Directory::GetDir(const std::string_view& dirpath) const {
  auto t = ltrim_path(dirpath);
  if (t.second.empty()) {
    auto itr = GetDirectoryIterator(dirpath);
    if (itr != dirs_.end()) {
      return *itr;
    }
  } else if (DirExists(t.first)) {
    return GetDirectory(t.first)->GetDir(dirpath);
  }
  return nullptr;
}

FilePtr Directory::GetFile(const std::string_view& filepath) const {
  auto t = ltrim_path(filepath);
  if (t.second.empty()) {
    auto itr = GetFileIterator(filepath);
    if (itr != files_.end()) {
      return *itr;
    }
  } else if (DirExists(t.first)) {
    return GetDirectory(t.first)->GetFile(filepath);
  }
  return nullptr;
}

uint8_t Directory::ResourceExists(const std::string_view& path) const noexcept {
  auto t = ltrim_path(path);
  if (t.second.empty()) {
    return (GetDirectoryIterator(path) == dirs_.end() ? 2 : 0) +
           (GetFileIterator(path) == files_.end() ? 1 : 0);
  }
  return (DirExists(t.first) &&
          GetDirectory(t.first)->ResourceExists(t.second));
}

std::vector<DirectoryPtr>::const_iterator Directory::GetDirectoryIterator(
    const std::string_view& dirpath) const noexcept {
  std::shared_lock lock(*mutex_);
  return std::find_if(dirs_.begin(), dirs_.end(), [&](const DirectoryPtr& dir) {
    return dir->name() == dirpath;
  });
}
std::vector<FilePtr>::const_iterator Directory::GetFileIterator(
    const std::string_view& filepath) const noexcept {
  std::shared_lock lock(*mutex_);
  return std::find_if(files_.begin(), files_.end(), [&](const FilePtr& file) {
    return file->name() == filepath;
  });
}

uint64_t Directory::CalculateDirSize() const noexcept {
  auto dl = [](uint64_t a, const DirectoryPtr& b) { return a + b->size(); };
  auto fl = [](uint64_t a, const FilePtr& b) { return a + b->size(); };
  std::shared_lock lock(*mutex_);
  return std::accumulate(dirs_.begin(), dirs_.end(), (uint64_t)0, dl) +
         std::accumulate(files_.begin(), files_.end(), (uint64_t)0, fl) + size_;
}
void Directory::UpdateDirSize() noexcept { dir_size_ = CalculateDirSize(); }

void Directory::Sort() noexcept {
  std::unique_lock lock(*mutex_);
  std::sort(dirs_.begin(), dirs_.end(),
            [](const DirectoryPtr& first, const DirectoryPtr& second) {
              return first->name().compare(second->name());
            });
  std::sort(files_.begin(), files_.end(),
            [](const FilePtr& first, const FilePtr& second) {
              return first->name().compare(second->name());
            });
}
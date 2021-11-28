#include "Directory.hpp"
using namespace resource;

static inline uint64_t BytesToUint64(const char* sbuf) {
  auto buf = std::bit_cast<const unsigned char*>(sbuf);
  return (uint64_t(buf[0]) << 0) | (uint64_t(buf[1]) << 8) |
         (uint64_t(buf[2]) << 16) | (uint64_t(buf[3]) << 24) |
         (uint64_t(buf[4]) << 32) | (uint64_t(buf[5]) << 40) |
         (uint64_t(buf[6]) << 48) | (uint64_t(buf[7]) << 56);
}

Directory::Directory(uint64_t const& begin,
                     AtomicIfstreamPointer const& resource_file_ptr)
    : BaseResource(begin, resource_file_ptr) {
  File dir_header_(begin, resource_file_ptr);
  name_ = dir_header_.name();
  size_ = dir_header_.size();
  auto data_ptr = dir_header_.data();
  auto data = data_ptr->data();
  std::vector<uint64_t> resources((size_t)(dir_header_.size() / 4ull));
  // Process files
  bool flag = true;
  for (auto buf = data; buf < data + size_; buf += sizeof(uint64_t)) {
    uint64_t file_begin = BytesToUint64(buf);
    if (flag) {
      flag = file_begin != 0;
      if (flag) {
        files_.push_back(File(file_begin, resource_file_ptr));
      }
    } else {
      dirs_.emplace_back(file_begin, resource_file_ptr);
    }
  }
  dir_size_ = CalculateDirSize();
}

uint64_t Directory::size() const noexcept { return dir_size_; }
uint64_t Directory::isFile() const noexcept { return false; }

std::vector<File> const& Directory::files() const noexcept { return files_; }
std::vector<Directory> const& Directory::directories() const noexcept {
  return dirs_;
}
std::vector<Directory> const& Directory::dirs() const noexcept { return dirs_; }

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
  return DirectoryExists(dirpath);
}
bool Directory::FolderExists(std::string_view const& dirpath) const noexcept {
  return DirectoryExists(dirpath);
}

bool Directory::DirectoryExists(
    std::string_view const& dirpath) const noexcept {
  auto [left, right] = ltrim_path(dirpath);
  if (right.empty()) {
    return GetDirectoryIterator(dirpath) != dirs_.end();
  }
  if (DirExists(left)) {
    return GetDirectory(left).DirectoryExists(right);
  }
  return false;
}

bool Directory::FileExists(std::string_view const& filepath) const noexcept {
  auto [left, right] = ltrim_path(filepath);
  if (right.empty()) {
    return GetFileIterator(filepath) != files_.end();
  }
  if (DirExists(left)) {
    return GetDirectory(left).FileExists(right);
  }
  return false;
}

Directory const& Directory::GetFolder(std::string_view const& dirpath) const {
  return GetDir(dirpath);
}

Directory const& Directory::GetDirectory(
    std::string_view const& dirpath) const {
  return GetDir(dirpath);
}

Directory const& Directory::GetDir(std::string_view const& dirpath) const {
  auto [left, right] = ltrim_path(dirpath);
  if (right.empty()) {
    auto itr = GetDirectoryIterator(left);
    if (itr != dirs_.end()) {
      return *itr;
    }
  } else if (DirExists(left)) {
    return GetDir(left).GetDir(right);
  }
  throw std::invalid_argument(std::string(dirpath));
}

File const& Directory::GetFile(std::string_view const& filepath) const {
  auto [left, right] = ltrim_path(filepath);
  if (right.empty()) {
    auto itr = GetFileIterator(left);
    if (itr != files_.end()) {
      return *itr;
    }
  } else if (DirExists(left)) {
    return GetDirectory(left).GetFile(right);
  }
  throw std::invalid_argument(std::string(filepath));
}

std::vector<Directory>::const_iterator Directory::GetDirectoryIterator(
    std::string_view const& dirpath) const noexcept {
  return std::find_if(dirs_.begin(), dirs_.end(), [&](Directory const& dir) {
    return dir.name() == dirpath;
  });
}
std::vector<File>::const_iterator Directory::GetFileIterator(
    std::string_view const& filepath) const noexcept {
  return std::find_if(files_.begin(), files_.end(), [&](File const& file) {
    return file.name() == filepath;
  });
}

uint64_t Directory::CalculateDirSize() const noexcept {
  auto dl = [](uint64_t a, Directory const& b) { return a + b.size(); };
  auto fl = [](uint64_t a, File const& b) { return a + b.size(); };
  return std::accumulate(dirs_.begin(), dirs_.end(), (uint64_t)0, dl) +
         std::accumulate(files_.begin(), files_.end(), (uint64_t)0, fl) + size_;
}
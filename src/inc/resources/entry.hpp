#pragma once
#include <algorithm>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <string>

#include "__resource-file-ptr.hpp"

namespace resource {
class InvalidPathException : public std::runtime_error {
public:
  using std::runtime_error::runtime_error;
};
// abstract resource class
class Entry final {
public:
  [[nodiscard]] inline std::string name() const noexcept { return name_; }
  [[nodiscard]] constexpr uint64_t size() const noexcept { return size_; }
  [[nodiscard]] AtomicIfstreamPointer resource_file_ptr() const noexcept {
    return resource_file_ptr_;
  }

  [[nodiscard]] constexpr uint64_t is_file() const noexcept { return is_file_; }
  [[nodiscard]] constexpr uint64_t is_dir() const noexcept { return !is_file_; }
  [[nodiscard]] constexpr uint64_t is_folder() const noexcept {
    return !is_file_;
  }
  [[nodiscard]] constexpr uint64_t is_directory() const noexcept {
    return !is_file_;
  }

  [[nodiscard]] constexpr std::vector<std::reference_wrapper<const Entry>> const
      &
      files() const noexcept {
    return files_;
  }
  [[nodiscard]] constexpr std::vector<std::reference_wrapper<const Entry>> const
      &
      directories() const noexcept {
    return directories_;
  }
  [[nodiscard]] constexpr std::vector<std::reference_wrapper<const Entry>> const
      &
      folders() const noexcept {
    return directories_;
  }
  [[nodiscard]] constexpr std::vector<std::reference_wrapper<const Entry>> const
      &
      dirs() const noexcept {
    return directories_;
  }
  [[nodiscard]] bool FileExists(std::string_view const key) const noexcept;
  [[nodiscard]] bool DirectoryExists(std::string_view const key) const noexcept;

  [[nodiscard]] bool Exists(std::string_view const key) const noexcept;

  [[nodiscard]] Entry const &Get(std::string_view const key) const;
  [[nodiscard]] Entry const &GetFile(std::string_view const key) const;
  [[nodiscard]] Entry const &GetDirectory(std::string_view const key) const;

  [[nodiscard]] Entry const &operator/(std::string_view const key) const;

  [[nodiscard]] Entry const &operator[](std::string_view const key) const;

  [[nodiscard]] bool operator==(Entry const &other) const noexcept;

  [[nodiscard]] std::optional<std::reference_wrapper<const Entry>>
  GetIfExists(std::string_view const key) const noexcept;
  // returns nullptr if the entry is a folder
  [[nodiscard]] std::shared_ptr<std::vector<char>> data() const noexcept;
  // returns nullptr if the entry is a folder
  [[nodiscard]] std::shared_ptr<std::vector<char>> content() const noexcept;

  [[nodiscard]] std::string ToString() const noexcept;
  [[nodiscard]] std::string string() const noexcept;

  [[nodiscard]] auto begin() const noexcept { return entries_.begin(); }
  [[nodiscard]] auto end() const noexcept { return entries_.end(); }

  Entry(uint64_t begin, AtomicIfstreamPointer const &resource_file_ptr)
      : Entry(begin, resource_file_ptr, false) {}

private:
  [[nodiscard]] std::shared_ptr<std::vector<char>>
  data_internal() const noexcept;
  uint64_t CalculateDirSize() const noexcept;

  Entry(uint64_t begin, AtomicIfstreamPointer const &resource_file_ptr,
        bool is_file);

  std::string name_ = "";
  uint64_t size_ = 0;
  uint64_t data_begin_ = 0;
  AtomicIfstreamPointer resource_file_ptr_;
  bool is_file_ = true;

  std::vector<std::reference_wrapper<const Entry>> files_;
  std::vector<std::reference_wrapper<const Entry>> directories_;
  std::vector<std::reference_wrapper<const Entry>> entries_;
  std::map<std::string_view, std::reference_wrapper<const Entry>, std::less<>>
      entry_map_;
  std::shared_ptr<std::vector<std::unique_ptr<Entry>>> entries_holder_;
  std::shared_ptr<std::weak_ptr<std::vector<char>>> data_;
};
} // namespace resource
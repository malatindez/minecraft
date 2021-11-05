#pragma once
#include <algorithm>
#include <atomic>
#include <memory>
#include <numeric>
#include <shared_mutex>
#include <vector>
#include <bit>

#include "BaseResource.hpp"
#include "File.hpp"
namespace resource {
/*
 * file data:
 * 0x02 - 0x02 + name_size — name of the file
 * 0x02 + name_size - next_begin — data
 *
 * data only contains 8 byte integers following each other.
 * Each integer represent the location of the file within the resource file.
 * This way same file can be used by different folders.
 */
class Directory : public BaseResource {
 public:
  Directory(uint64_t const& begin,
            AtomicIfstreamPointer const& resource_file_ptr);

  [[nodiscard]] uint64_t size() const noexcept override;
  [[nodiscard]] uint64_t isFile() const noexcept override;

  [[nodiscard]] std::vector<File> files() const noexcept;
  [[nodiscard]] std::vector<Directory> directories() const noexcept;
  [[nodiscard]] std::vector<Directory> dirs() const noexcept;

  [[nodiscard]] bool FolderExists(std::string_view const&) const noexcept;
  [[nodiscard]] bool DirectoryExists(std::string_view const&) const noexcept;
  [[nodiscard]] bool DirExists(std::string_view const&) const noexcept;
  [[nodiscard]] bool FileExists(std::string_view const&) const noexcept;

  [[nodiscard]] Directory GetFolder(std::string_view const&) const;
  [[nodiscard]] Directory GetDirectory(std::string_view const&) const;
  [[nodiscard]] Directory GetDir(std::string_view const&) const;
  [[nodiscard]] File GetFile(std::string_view const&) const;

 private:
  [[nodiscard]] std::vector<Directory>::const_iterator GetDirectoryIterator(
      std::string_view const&) const noexcept;
  [[nodiscard]] std::vector<File>::const_iterator GetFileIterator(
      std::string_view const&) const noexcept;

  [[nodiscard]] uint64_t CalculateDirSize() const noexcept;
  void UpdateDirSize() noexcept;

  void Sort() noexcept;

  uint64_t dir_size_;
  std::vector<Directory> dirs_;
  std::vector<File> files_;

  std::shared_ptr<std::shared_mutex> mutex_;
};
}  // namespace resource
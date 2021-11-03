#pragma once
#include <algorithm>
#include <atomic>
#include <memory>
#include <numeric>
#include <shared_mutex>
#include <vector>

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
  using FilePtr = std::shared_ptr<File>;
  using DirectoryPtr = std::shared_ptr<Directory>;

  Directory(const uint64_t& begin,
            const AtomicIfstreamPointer& resource_file_ptr);

  Directory();

  [[nodiscard]] uint64_t size() const noexcept override;
  [[nodiscard]] uint64_t isFile() const noexcept override;

  [[nodiscard]] std::vector<FilePtr> files() const noexcept;
  [[nodiscard]] std::vector<DirectoryPtr> directories() const noexcept;
  [[nodiscard]] std::vector<DirectoryPtr> dirs() const noexcept;
  [[nodiscard]] uint64_t dir_size() const noexcept;

  [[nodiscard]] bool FolderExists(const std::string_view&) const noexcept;
  [[nodiscard]] bool DirectoryExists(const std::string_view&) const noexcept;
  [[nodiscard]] bool DirExists(const std::string_view&) const noexcept;
  [[nodiscard]] bool FileExists(const std::string_view&) const noexcept;

  [[nodiscard]] DirectoryPtr GetFolder(const std::string_view&) const;
  [[nodiscard]] DirectoryPtr GetDirectory(const std::string_view&) const;
  [[nodiscard]] DirectoryPtr GetDir(const std::string_view&) const;
  [[nodiscard]] FilePtr GetFile(const std::string_view&) const;

 private:
  uint8_t ResourceExists(const std::string_view& name) const noexcept;

  [[nodiscard]] std::vector<DirectoryPtr>::const_iterator GetDirectoryIterator(
      const std::string_view&) const noexcept;
  [[nodiscard]] std::vector<FilePtr>::const_iterator GetFileIterator(
      const std::string_view&) const noexcept;

  [[nodiscard]] uint64_t CalculateDirSize() const noexcept;
  void UpdateDirSize() noexcept;

  void Sort() noexcept;

  uint64_t dir_size_;
  std::vector<DirectoryPtr> dirs_;
  std::vector<FilePtr> files_;

  std::shared_ptr<std::shared_mutex> mutex_ =
      std::make_shared<std::shared_mutex>();
};
}  // namespace resource
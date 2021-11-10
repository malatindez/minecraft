#pragma once
#include "BaseResource.hpp"
namespace resource {
/*
 * file data:
 * begin:
 * 0x00 : 0x08 - next file location (k) (little endian)
 * 0x0A : 0x0C — size of the name of the file (n) (little endian)
 * 0x0C : 0x0C + n — name of the file
 * 0x0C + n : k — file data
 */
class File final : public BaseResource {
 public:
  friend class Directory;

  ~File() override;
  [[nodiscard]] std::shared_ptr<std::vector<char>> data() const noexcept;
  [[nodiscard]] std::shared_ptr<std::vector<char>> content() const noexcept;

  [[nodiscard]] std::string ToString() const noexcept;
  [[nodiscard]] std::string string() const noexcept;

  [[nodiscard]] uint64_t size() const noexcept override;

 private:
  File(uint64_t begin, AtomicIfstreamPointer const& resource_file_ptr);

  uint64_t data_begin_;
  std::shared_ptr<std::weak_ptr<std::vector<char>>> data_;
};
}  // namespace resource
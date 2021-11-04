#pragma once
#include "__ResourceFilePointer.hpp"
namespace resource {
// abstract resource class
class BaseResource {
 public:
  virtual ~BaseResource() = default;

  [[nodiscard]] virtual uint64_t size() const noexcept { return size_; }
  [[nodiscard]] virtual uint64_t isFile() const noexcept { return true; }
  [[nodiscard]] std::string name() const noexcept { return name_; }

 protected:
  BaseResource() {}
  // name_ should be loaded in the constructor of the inherited class
  BaseResource(uint64_t begin, AtomicIfstreamPointer const& resource_file_ptr)
      : begin_(begin), resource_file_ptr_(resource_file_ptr) {}
  // 0 for folder
  // 1 for file
  uint16_t type = 0;
  std::string name_ = "";
  uint64_t size_ = 0;
  uint64_t begin_;
  AtomicIfstreamPointer resource_file_ptr_;
};
}  // namespace resource
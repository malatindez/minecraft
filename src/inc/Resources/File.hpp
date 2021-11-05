#pragma once
#include "BaseResource.hpp"
namespace resource {

    static inline uint64_t BytesToUint64(const char* sbuf) {
        auto buf = std::bit_cast<const unsigned char*>(sbuf);
        return (uint64_t(buf[0]) << 0) | (uint64_t(buf[1]) << 8) |
            (uint64_t(buf[2]) << 16) | (uint64_t(buf[3]) << 24) |
            (uint64_t(buf[4]) << 32) | (uint64_t(buf[5]) << 40) |
            (uint64_t(buf[6]) << 48) | (uint64_t(buf[7]) << 56);
    }
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

  ~File() override {}
  [[nodiscard]] std::shared_ptr<std::vector<char>> data() noexcept {
    if (data_.expired()) {
      auto lock = resource_file_ptr_.Lock();
      lock->seekg(data_begin_);
      auto file_content =
          std::make_shared<std::vector<char>>(size_);
      lock->read(file_content->data(), size_);
      data_ = file_content;
      return file_content;
    }
    return data_.lock();
  }

  [[nodiscard]] std::shared_ptr<std::vector<char>> content() {
    return data();
  }

  [[nodiscard]] std::string ToString() noexcept {
    std::shared_ptr<std::vector<char>> data = this->data();
    std::string return_value;
    return_value.resize(data->size());
    std::transform(data->begin(), data->end(), return_value.begin(),
                   [](char b) { return char(b); });
    return std::string(return_value);
  }

  [[nodiscard]] uint64_t size() const noexcept override { return size_; }

 private:
  File(uint64_t begin, AtomicIfstreamPointer const &resource_file_ptr)
      : BaseResource(begin, resource_file_ptr) {
    auto lock = resource_file_ptr_.Lock();
    lock->seekg(begin_);
    // retrieve next byte location & the size of the filename in bytes
    std::vector<char> buf(sizeof(uint64_t) + sizeof(uint16_t),
                               char(0));

    lock->read(buf.data(),
               sizeof(uint64_t) + sizeof(uint16_t));
    uint64_t nextfile_location = BytesToUint64(buf.data());

    uint16_t name_size = (uint16_t(buf[8]) << 0) | (uint16_t(buf[9]) << 8);

    std::unique_ptr<char[]> name(new char[1U + name_size]);
    name.get()[name_size] = '\0';
    lock->read(name.get(),
               name_size * sizeof(char));

    name_ = std::string(name.get());
    data_begin_ = lock->tellg();
    size_ = nextfile_location - data_begin_;
  }

  uint64_t data_begin_;
  std::weak_ptr<std::vector<char>> data_;
};
}  // namespace resource
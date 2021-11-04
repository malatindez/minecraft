#pragma once
#include <fstream>
#include <memory>
#include <mutex>
namespace resource {
template <typename T>
class __AtomicSharedPtr {
 public:
  template <typename T>
  class __Locked : public std::shared_ptr<T> {
   public:
    ~__Locked() { mutex_->unlock(); }
    __Locked(__Locked&& other) noexcept = delete;
    __Locked(__Locked const& other) noexcept = delete;
    __Locked& operator=(__Locked&& other) noexcept = delete;
    __Locked& operator=(__Locked const& other) noexcept = delete;

   private:
    __Locked(std::shared_ptr<std::mutex> mutex, std::shared_ptr<T> obj)
        : mutex_(mutex), std::shared_ptr<T>(obj) {
      mutex_->lock();
    }
    friend __AtomicSharedPtr;
    std::shared_ptr<std::mutex> mutex_;
  };
  // Don't forget to delete retrieved pointers first, otherwise the program will
  // be terminated.
  __Locked<T> Lock() const noexcept { return __Locked<T>(mutex_, obj_); }
  explicit __AtomicSharedPtr() : mutex_(nullptr), obj_(nullptr) {}
  explicit __AtomicSharedPtr(std::shared_ptr<T> obj)
      : mutex_(std::make_shared<std::mutex>()), obj_(obj) {}
  bool operator==(__AtomicSharedPtr const& other) const noexcept {
    return other.obj_ == obj_;
  }

 private:
  std::shared_ptr<std::mutex> mutex_;
  std::shared_ptr<T> obj_;
};

// atomic wrapper around shared pointer of filebuf
using AtomicIfstreamPointer = __AtomicSharedPtr<std::basic_ifstream<std::byte>>;
}  // namespace resource
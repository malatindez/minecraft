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
    constexpr ~__Locked() {
      if (mutex_) {
        mutex_->unlock();
      }
    }
    __Locked(__Locked&& other) noexcept = delete;
    __Locked(__Locked const& other) noexcept = delete;
    __Locked& operator=(__Locked&& other) noexcept = delete;
    __Locked& operator=(__Locked const& other) noexcept = delete;

   private:
    constexpr __Locked(std::shared_ptr<std::mutex> mutex,
                       std::shared_ptr<T> obj, bool try_lock)
        : mutex_(mutex), std::shared_ptr<T>(obj) {
      if (try_lock) {
        if (!mutex_->try_lock()) {
          this->reset();   // if try_lock returns false,
          mutex_.reset();  // clear pointers to inaccessible objects
        }
      } else {
        mutex_->lock();
      }
    }
    // used to return an empty value in TryLock
    constexpr __Locked() = default;
    friend __AtomicSharedPtr;
    std::shared_ptr<std::mutex> mutex_;
  };
  // Don't forget to delete retrieved pointers first, otherwise the program will
  // be terminated.
  constexpr __Locked<T> Lock() const noexcept {
    return __Locked<T>(mutex_, obj_, false);
  }
  constexpr __Locked<T> TryLock() const noexcept {
    return __Locked<T>(mutex_, obj_, true);
  }
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
using AtomicIfstreamPointer = __AtomicSharedPtr<std::ifstream>;
}  // namespace resource
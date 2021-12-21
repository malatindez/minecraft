#pragma once
namespace minecraft::core {
//  Initializable interface
struct Updatable {
  virtual inline ~Updatable() = 0;

  virtual void Update() = 0;
};
} // namespace minecraft::core
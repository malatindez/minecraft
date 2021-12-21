#pragma once
namespace minecraft::core {
//  Initializable interface
struct Initializable {
  virtual inline ~Initializable() = 0;

  virtual void PreInit() = 0;
  virtual void Init() = 0;
  virtual void PostInit() = 0;
};
}  // namespace minecraft::core
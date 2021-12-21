#pragma once

#include <utils/nbt.hpp>

#include "core/block.hpp"
#include "core/core.hpp"
#include "core/interfaces/updatable.hpp"

namespace minecraft::core {
class TileEntity : public Updatable {
  TileEntity(nbt::NBT& data)
      : core(::minecraft::core::Core::instance()), nbt(data) {}

  virtual inline ~TileEntity() = 0;

  virtual void Update() = 0;

  ::minecraft::core::Core& core;
  nbt::NBT& nbt;
  Block block;
};
}  // namespace minecraft::core
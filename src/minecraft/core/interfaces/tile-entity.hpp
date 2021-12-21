#pragma once

#include <utils/nbt.hpp>

#include "core/block-base.hpp"
#include "core/core.hpp"
#include "core/interfaces/updatable.hpp"

class TileEntity : public Updatable {
  TileEntity(nbt::NBT &data) : core_(Core::instance()), nbt(nbt) {}

  virtual inline ~TileEntity() = 0;

  virtual void Update() = 0;

 protected:
  Core &core_;
  nbt::NBT &nbt;
  Block block_;
};
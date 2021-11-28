#include "Yaml.hpp"
namespace yaml {
class EntryPair : public Entry {
  friend class Entry;

 private:
  std::unique_ptr<Entry> key_ = nullptr;
  std::unique_ptr<Entry> value_ = nullptr;
};
class EntryInteger : public Entry {
  friend class Entry;

 private:
  int64_t integer_;
};
class EntryUnsignedInteger : public Entry {
  friend class Entry;

 private:
  uint64_t unsigned_integer_;
};
class EntryDouble : public Entry {
  friend class Entry;

 private:
  long double double_;
};
bool Entry::contains(std::string_view const& string) const {
  if (!is_sequence() && !is_map()) {
    throw std::invalid_argument("This entry is not a sequence nor a map");
  }
  for (Entry const& entry : entries_) {
    if (entry.is_string() && entry.to_string() == string) {
      return true;
    } else if (entry.is_pair() && entry.key().is_string() &&
               entry.key().to_string() == string) {
      return true;
    }
  }
  return false;
}

bool Entry::contains(int64_t integer) const {
  if (!is_sequence()) {
    throw std::invalid_argument("This entry is not a sequence");
  }
  for (Entry const& entry : entries_) {
    if (entry == integer) {
      return true;
    }
  }
  return false;
}
bool Entry::contains(int32_t integer) const {
  if (!is_sequence()) {
    throw std::invalid_argument("This entry is not a sequence");
  }
  for (Entry const& entry : entries_) {
    if (entry == integer) {
      return true;
    }
  }
  return false;
}
bool Entry::contains(int16_t integer) const {
  if (!is_sequence()) {
    throw std::invalid_argument("This entry is not a sequence");
  }
  for (Entry const& entry : entries_) {
    if (entry == integer) {
      return true;
    }
  }
  return false;
}
bool Entry::contains(uint64_t integer) const {
  if (!is_sequence()) {
    throw std::invalid_argument("This entry is not a sequence");
  }
  for (Entry const& entry : entries_) {
    if (entry == integer) {
      return true;
    }
  }
  return false;
}
bool Entry::contains(uint32_t integer) const {
  if (!is_sequence()) {
    throw std::invalid_argument("This entry is not a sequence");
  }
  for (Entry const& entry : entries_) {
    if (entry == integer) {
      return true;
    }
  }
  return false;
}
bool Entry::contains(uint16_t integer) const {
  if (!is_sequence()) {
    throw std::invalid_argument("This entry is not a sequence");
  }
  for (Entry const& entry : entries_) {
    if (entry == integer) {
      return true;
    }
  }
  return false;
}
bool Entry::contains(long double real) const {
  if (!is_sequence()) {
    throw std::invalid_argument("This entry is not a sequence");
  }
  for (Entry const& entry : entries_) {
    if (entry == real) {
      return true;
    }
  }
  return false;
}
bool Entry::contains(double real) const {
  if (!is_sequence()) {
    throw std::invalid_argument("This entry is not a sequence");
  }
  for (Entry const& entry : entries_) {
    if (entry == real) {
      return true;
    }
  }
  return false;
}
bool Entry::contains(float real) const {
  if (!is_sequence()) {
    throw std::invalid_argument("This entry is not a sequence");
  }
  for (Entry const& entry : entries_) {
    if (entry == real) {
      return true;
    }
  }
  return false;
}

bool Entry::operator==(Entry const& other) const noexcept {
  return other.type_ == type_ && str_ == other.str_;
}
bool Entry::operator==(std::string_view const& other) const noexcept {
  return is_string() && str_ == other;
}
bool Entry::operator==(int64_t const& other) const noexcept {
  return is_int() && to_int() == other;
}
bool Entry::operator==(int32_t const& other) const noexcept {
  return is_int() && to_int() == other;
}
bool Entry::operator==(int16_t const& other) const noexcept {
  return is_int() && to_int() == other;
}
bool Entry::operator==(uint64_t const& other) const noexcept {
  return is_int() && to_uint() == other;
}
bool Entry::operator==(uint32_t const& other) const noexcept {
  return is_int() && to_uint() == other;
}
bool Entry::operator==(uint16_t const& other) const noexcept {
  return is_int() && to_uint() == other;
}
bool Entry::operator==(long double const& other) const noexcept {
  return is_double() && to_double() == other;
}
bool Entry::operator==(double const& other) const noexcept {
  return is_double() && to_double() == other;
}
bool Entry::operator==(float const& other) const noexcept {
  return is_double() && to_double() == other;
}

Entry const& Entry::key() const {
  if (!is_pair()) {
    throw std::invalid_argument("This entry is not a pair");
  }
  return *reinterpret_cast<const EntryPair*>(this)->key_;
}

Entry const& Entry::value() const {
  if (!is_pair()) {
    throw std::invalid_argument("This entry is not a pair");
  }
  return *reinterpret_cast<const EntryPair*>(this)->value_;
}

inline long double Entry::to_double() const {
  if (!is_double()) {
    throw std::invalid_argument("This entry is not a double");
  }
  return reinterpret_cast<const EntryDouble*>(this)->double_;
}
inline long long int Entry::to_int() const {
  if (!is_int()) {
    throw std::invalid_argument("This entry is not an integer");
  }
  return reinterpret_cast<const EntryInteger*>(this)->integer_;
}
inline uint64_t Entry::to_uint() const {
  if (!is_uint()) {
    throw std::invalid_argument("This entry is not an integer");
  }
  return reinterpret_cast<const EntryUnsignedInteger*>(this)->unsigned_integer_;
}
inline std::string_view Entry::to_string() const noexcept { return str_; }

Entry Parse(std::string_view const& string) {
    return Entry();
}
}  // namespace yaml
#pragma once
#include <algorithm>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>
namespace yaml {

class Entry {
 public:
  enum class Type {
    kBool,
    kDate,
    kDir,
    kDouble,
    kInt,
    kMap,
    kNull,
    kOMap,
    kPair,
    kSequence,
    kSet,
    kString,
    kTime,
    kTimestamp,
    kUInt
  };
  auto begin() const noexcept { return entries_.begin(); }
  auto end() const noexcept { return entries_.end(); }
  size_t size() const noexcept { return entries_.size(); }
  inline bool is_bool() const noexcept { return type_ == Type::kBool; }
  inline bool is_date() const noexcept { return type_ == Type::kDate; }
  inline bool is_directive() const noexcept { return type_ == Type::kDir; }
  inline bool is_double() const noexcept { return type_ == Type::kDouble; }
  inline bool is_int() const noexcept { return type_ == Type::kInt; }
  inline bool is_map() const noexcept { return type_ == Type::kMap; }
  inline bool is_null() const noexcept { return type_ == Type::kNull; }
  inline bool is_omap() const noexcept { return type_ == Type::kOMap; }
  inline bool is_pair() const noexcept { return type_ == Type::kPair; }
  inline bool is_sequence() const noexcept { return type_ == Type::kSequence; }
  inline bool is_set() const noexcept { return type_ == Type::kSet; }
  inline bool is_string() const noexcept { return type_ == Type::kString; }
  inline bool is_time() const noexcept { return type_ == Type::kTime; }
  inline bool is_timestamp() const noexcept {
    return type_ == Type::kTimestamp;
  }
  inline bool is_uint() const noexcept { return type_ == Type::kUInt; }

  bool contains(std::string_view const& string) const;
  bool contains(int64_t integer) const;
  bool contains(int32_t integer) const;
  bool contains(int16_t integer) const;
  bool contains(uint64_t integer) const;
  bool contains(uint32_t integer) const;
  bool contains(uint16_t integer) const;
  bool contains(long double real) const;
  bool contains(double real) const;
  bool contains(float real) const;
  bool operator==(Entry const& other) const noexcept;
  bool operator==(std::string_view const& other) const noexcept;
  bool operator==(int64_t const& other) const noexcept;
  bool operator==(int32_t const& other) const noexcept;
  bool operator==(int16_t const& other) const noexcept;
  bool operator==(uint64_t const& other) const noexcept;
  bool operator==(uint32_t const& other) const noexcept;
  bool operator==(uint16_t const& other) const noexcept;
  bool operator==(long double const& other) const noexcept;
  bool operator==(double const& other) const noexcept;
  bool operator==(float const& other) const noexcept;
  Entry const& operator[](std::string_view const& key) const;
  Entry const& operator[](size_t i) const;
  Entry const& key() const;
  Entry const& value() const;

  long double to_double() const;
  int64_t to_int() const;
  uint64_t to_uint() const;
  std::string_view to_string() const noexcept;

  Type const& type() const { return type_; }
  std::string const& str() const { return str_; }
  std::string const& tag() const { return tag_; }

 private:
  std::vector<Entry> entries_;
  Type type_ = Entry::Type::kNull;
  std::string str_ = "";
  std::string tag_ = "";
};
Entry Parse(std::string_view const& string);
}  // namespace yaml
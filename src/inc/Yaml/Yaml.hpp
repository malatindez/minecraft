#pragma once
#include <algorithm>
#include <chrono>
#include <map>
#include <memory>
#include <set>
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
  Entry() noexcept;
  Entry(std::unique_ptr<Entry> key, std::unique_ptr<Entry> value);
  explicit Entry(std::string_view const& other) noexcept;
  explicit Entry(int64_t const& other) noexcept;
  explicit Entry(int32_t const& other) noexcept;
  explicit Entry(int16_t const& other) noexcept;
  explicit Entry(uint64_t const& other) noexcept;
  explicit Entry(uint32_t const& other) noexcept;
  explicit Entry(uint16_t const& other) noexcept;
  explicit Entry(long double const& other) noexcept;
  explicit Entry(double const& other) noexcept;
  explicit Entry(float const& other) noexcept;
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
  Entry& operator[](std::string_view const& key);
  Entry& operator[](size_t const& i);
  Entry& operator=(std::string_view const& other) noexcept;
  Entry& operator=(bool const& other) noexcept;
  Entry& operator=(int64_t const& other) noexcept;
  Entry& operator=(int32_t const& other) noexcept;
  Entry& operator=(int16_t const& other) noexcept;
  Entry& operator=(uint64_t const& other) noexcept;
  Entry& operator=(uint32_t const& other) noexcept;
  Entry& operator=(uint16_t const& other) noexcept;
  Entry& operator=(long double const& other) noexcept;
  Entry& operator=(double const& other) noexcept;
  Entry& operator=(float const& other) noexcept;
  // Accepts only values that are convertible by std::string
  template <typename T>
  Entry& operator=(std::vector<T> const& other) noexcept;
  // Accepts only values that are convertible by std::string
  template <typename T1, typename T2>
  Entry& operator=(std::map<T1, T2> const& other) noexcept;
  // Accepts only values that are convertible by std::string
  template <typename T>
  Entry& operator=(std::set<T> const& other) noexcept;
  Entry& key() const;
  Entry& value() const;

  std::chrono::hh_mm_ss<std::chrono::microseconds> to_time() const;
  std::chrono::year_month_day to_date() const;
  std::chrono::time_point<std::chrono::microseconds> to_datetime() const;
  std::chrono::time_point<std::chrono::microseconds> to_time_point() const;

  long double to_double() const;
  int64_t to_int() const;
  uint64_t to_uint() const;
  bool to_bool() const;
  bool to_boolean() const;
  std::string_view to_string() const noexcept;

  Type const& type() const { return type_; }
  std::string const& str() const { return str_; }
  std::string const& tag() const { return tag_; }

  std::string Serialize() const noexcept { return ""; }

 private:
  struct AbstractValue {};
  struct Pair : public AbstractValue {
    std::unique_ptr<Entry> key_ = nullptr;
    std::unique_ptr<Entry> value_ = nullptr;
    Pair(std::unique_ptr<Entry> key, std::unique_ptr<Entry> value)
        : key_(std::move(key)), value_(std::move(value)) {}
  };
  struct Integer : public AbstractValue {
    int64_t integer_;
    explicit Integer(int64_t integer_) : integer_(integer_) {}
  };
  struct UnsignedInteger : public AbstractValue {
    uint64_t unsigned_integer_;
    explicit UnsignedInteger(uint64_t unsigned_integer_)
        : unsigned_integer_(unsigned_integer_) {}
  };
  struct Double : public AbstractValue {
    long double double_;
    explicit Double(double double_) : double_(double_) {}
  };
  struct Boolean : public AbstractValue {
    bool boolean_;
    explicit Boolean(bool boolean) : boolean_(boolean) {}
  };
  struct TimePoint : public AbstractValue {
    std::chrono::time_point<std::chrono::microseconds> datetime_;
    std::chrono::year_month_day date_;
    std::chrono::hh_mm_ss<std::chrono::microseconds> time_;
  };

  std::vector<Entry> entries_;
  Type type_ = Entry::Type::kNull;
  std::string str_ = "";
  std::string tag_ = "";
  std::unique_ptr<AbstractValue> data_;
};
Entry Parse(std::string_view const& string);
}  // namespace yaml
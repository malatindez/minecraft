#pragma once
#include <errno.h>

#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

#include "Utils/Utils.hpp"
namespace yaml {
class Entry {
 public:
  struct Iterator : public utils::BaseIteratorWrapper<
                        std::vector<std::unique_ptr<Entry>>::iterator, Entry> {
    using utils::BaseIteratorWrapper<
        std::vector<std::unique_ptr<Entry>>::iterator,
        Entry>::BaseIteratorWrapper;
    [[nodiscard]] reference operator*() final { return **base_iterator(); }
    [[nodiscard]] pointer operator->() final { return base_iterator()->get(); }
  };

  struct ConstIterator
      : public utils::BaseIteratorWrapper<
            std::vector<std::unique_ptr<Entry>>::const_iterator, const Entry> {
    using utils::BaseIteratorWrapper<
        std::vector<std::unique_ptr<Entry>>::const_iterator,
        const Entry>::BaseIteratorWrapper;
    [[nodiscard]] reference operator*() final { return **base_iterator(); }
    [[nodiscard]] pointer operator->() final { return base_iterator()->get(); }
  };

  enum class Type : unsigned char {
    kBool,
    kDate,
    kDir,
    kDouble,
    kInt,
    kLink,
    kMap,
    kNull,
    kPair,
    kSequence,
    kSet,
    kString,
    kTime,
    kTimestamp,
    kUInt
  };
  ~Entry() = default;
  explicit Entry(Type type, Entry* parent = nullptr) noexcept;
  explicit Entry(Entry&& entry, Entry* parent = nullptr) noexcept;
  explicit Entry(Entry& entry, Entry* parent = nullptr);
  explicit Entry(std::unique_ptr<Entry> key, std::unique_ptr<Entry> value,
                 Entry* parent = nullptr) noexcept;
  explicit Entry(std::string_view const& other,
                 Entry* parent = nullptr) noexcept;
  template <std::integral T>
  explicit Entry(T const& other, Entry* parent = nullptr) noexcept;
  template <std::floating_point T>
  explicit Entry(T const& other, Entry* parent = nullptr) noexcept;
  explicit Entry(std::tm const& other, Entry* parent = nullptr) noexcept;
  explicit Entry(std::chrono::year_month_day const& other,
                 Entry* parent = nullptr) noexcept;
  explicit Entry(std::chrono::hh_mm_ss<std::chrono::microseconds> const& other,
                 Entry* parent = nullptr) noexcept;
  // Accepts only values that are convertible by std::string
  template <typename T>
  explicit Entry(std::vector<T>&& other, Entry* parent = nullptr)
      : parent_(parent) {
    operator=(other);
  }
  // Accepts only values that are convertible by std::string
  template <typename T1, typename T2>
  explicit Entry(std::map<T1, T2>&& other, Entry* parent = nullptr)
      : parent_(parent) {
    operator=(other);
  }
  // Accepts only values that are convertible by std::string
  template <typename T>
  explicit Entry(std::set<T>&& other, Entry* parent = nullptr)
      : parent_(parent) {
    operator=(other);
  }
  template <typename T>
  explicit Entry(std::vector<T> const& other, Entry* parent = nullptr)
      : parent_(parent) {
    operator=(other);
  }
  // Accepts only values that are convertible by std::string
  template <typename T1, typename T2>
  explicit Entry(std::map<T1, T2> const& other, Entry* parent = nullptr)
      : parent_(parent) {
    operator=(other);
  }
  // Accepts only values that are convertible by std::string
  template <typename T>
  explicit Entry(std::set<T> const& other, Entry* parent = nullptr)
      : parent_(parent) {
    operator=(other);
  }
  [[nodiscard]] auto begin() noexcept { return Iterator(entries_.begin()); }
  [[nodiscard]] auto end() noexcept { return Iterator(entries_.end()); }
  [[nodiscard]] auto begin() const noexcept {
    return ConstIterator(entries_.begin());
  }
  [[nodiscard]] auto end() const noexcept {
    return ConstIterator(entries_.end());
  }

  [[nodiscard]] size_t size() const noexcept { return entries_.size(); }
  [[nodiscard]] constexpr bool is_simple_type() const noexcept {
    return !is_pair() && !is_sequence() && !is_map() && !is_set();
  }
  [[nodiscard]] constexpr bool is_bool() const noexcept {
    return type_ == Type::kBool;
  }
  [[nodiscard]] constexpr bool is_date() const noexcept {
    return type_ == Type::kDate;
  }
  [[nodiscard]] constexpr bool is_directive() const noexcept {
    return type_ == Type::kDir;
  }
  [[nodiscard]] constexpr bool is_double() const noexcept {
    return type_ == Type::kDouble;
  }
  [[nodiscard]] constexpr bool is_int() const noexcept {
    return type_ == Type::kInt;
  }
  [[nodiscard]] constexpr bool is_link() const noexcept {
    return type_ == Type::kLink;
  }
  [[nodiscard]] constexpr bool is_map() const noexcept {
    return type_ == Type::kMap;
  }
  [[nodiscard]] constexpr bool is_null() const noexcept {
    return type_ == Type::kNull;
  }
  [[nodiscard]] constexpr bool is_pair() const noexcept {
    return type_ == Type::kPair;
  }
  [[nodiscard]] constexpr bool is_sequence() const noexcept {
    return type_ == Type::kSequence;
  }
  [[nodiscard]] constexpr bool is_set() const noexcept {
    return type_ == Type::kSet;
  }
  [[nodiscard]] constexpr bool is_string() const noexcept {
    return type_ == Type::kString;
  }
  [[nodiscard]] constexpr bool is_time() const noexcept {
    return type_ == Type::kTime;
  }
  [[nodiscard]] constexpr bool is_timestamp() const noexcept {
    return type_ == Type::kTimestamp;
  }
  [[nodiscard]] constexpr bool is_uint() const noexcept {
    return type_ == Type::kUInt;
  }

  [[nodiscard]] constexpr bool contains(std::string_view const& string) const {
    if (!is_sequence() && !is_map()) {
      throw std::invalid_argument("This entry is not a sequence nor a map");
    }
    return std::any_of(
        entries_.begin(), entries_.end(),
        [&string](std::unique_ptr<Entry> const& entry) {
          return (entry->is_string() && entry->to_string() == string) ||
                 (entry->is_pair() && entry->key().is_string() &&
                  entry->key().to_string() == string);
        });
  }
  [[nodiscard]] constexpr bool contains(int64_t integer) const {
    if (!is_sequence()) {
      throw std::invalid_argument("This entry is not a sequence");
    }
    return std::any_of(entries_.begin(), entries_.end(),
                       [&integer](std::unique_ptr<Entry> const& entry) {
                         return *entry == integer;
                       });
  }
  [[nodiscard]] constexpr bool contains(int32_t integer) const {
    if (!is_sequence()) {
      throw std::invalid_argument("This entry is not a sequence");
    }
    return std::any_of(entries_.begin(), entries_.end(),
                       [&integer](std::unique_ptr<Entry> const& entry) {
                         return *entry == integer;
                       });
  }
  [[nodiscard]] constexpr bool contains(int16_t integer) const {
    if (!is_sequence()) {
      throw std::invalid_argument("This entry is not a sequence");
    }
    return std::any_of(entries_.begin(), entries_.end(),
                       [&integer](std::unique_ptr<Entry> const& entry) {
                         return *entry == integer;
                       });
  }
  [[nodiscard]] constexpr bool contains(uint64_t integer) const {
    if (!is_sequence()) {
      throw std::invalid_argument("This entry is not a sequence");
    }
    return std::any_of(entries_.begin(), entries_.end(),
                       [&integer](std::unique_ptr<Entry> const& entry) {
                         return *entry == integer;
                       });
  }
  [[nodiscard]] constexpr bool contains(uint32_t integer) const {
    if (!is_sequence()) {
      throw std::invalid_argument("This entry is not a sequence");
    }
    return std::any_of(entries_.begin(), entries_.end(),
                       [&integer](std::unique_ptr<Entry> const& entry) {
                         return *entry == integer;
                       });
  }
  [[nodiscard]] constexpr bool contains(uint16_t integer) const {
    if (!is_sequence()) {
      throw std::invalid_argument("This entry is not a sequence");
    }
    return std::any_of(entries_.begin(), entries_.end(),
                       [&integer](std::unique_ptr<Entry> const& entry) {
                         return *entry == integer;
                       });
  }
  [[nodiscard]] constexpr bool contains(long double real) const {
    if (!is_sequence()) {
      throw std::invalid_argument("This entry is not a sequence");
    }
    return std::any_of(entries_.begin(), entries_.end(),
                       [&real](std::unique_ptr<Entry> const& entry) {
                         return *entry == real;
                       });
  }
  [[nodiscard]] constexpr bool contains(double real) const {
    if (!is_sequence()) {
      throw std::invalid_argument("This entry is not a sequence");
    }
    return std::any_of(entries_.begin(), entries_.end(),
                       [&real](std::unique_ptr<Entry> const& entry) {
                         return *entry == real;
                       });
  }
  [[nodiscard]] constexpr bool contains(float real) const {
    if (!is_sequence()) {
      throw std::invalid_argument("This entry is not a sequence");
    }
    return std::any_of(entries_.begin(), entries_.end(),
                       [&real](std::unique_ptr<Entry> const& entry) {
                         return *entry == real;
                       });
  }

  [[nodiscard]] bool operator==(Entry const& other) const noexcept;
  [[nodiscard]] bool operator==(std::string_view const& other) const noexcept;
  [[nodiscard]] bool operator==(int64_t const& other) const noexcept;
  [[nodiscard]] bool operator==(int32_t const& other) const noexcept;
  [[nodiscard]] bool operator==(int16_t const& other) const noexcept;
  [[nodiscard]] bool operator==(uint64_t const& other) const noexcept;
  [[nodiscard]] bool operator==(uint32_t const& other) const noexcept;
  [[nodiscard]] bool operator==(uint16_t const& other) const noexcept;
  [[nodiscard]] bool operator==(long double const& other) const noexcept;
  [[nodiscard]] bool operator==(double const& other) const noexcept;
  [[nodiscard]] bool operator==(float const& other) const noexcept;
  [[nodiscard]] bool operator==(std::tm const& other) const noexcept;
  [[nodiscard]] bool operator==(
      std::chrono::year_month_day const& other) const noexcept;
  [[nodiscard]] bool operator==(
      std::chrono::hh_mm_ss<std::chrono::microseconds> const& other)
      const noexcept;
  // if the entry was not found, this function will create the new one with
  // value None
  [[nodiscard]] Entry& operator[](std::string_view const& key);
  [[nodiscard]] Entry& operator[](Entry& key);
  [[nodiscard]] Entry& operator[](size_t const& i);
  [[nodiscard]] Entry& operator[](Entry&& key);
  [[nodiscard]] Entry& operator[](Entry const& key);
  Entry& operator=(Entry& entry);
  Entry& operator=(std::string_view const& other) noexcept;
  Entry& operator=(bool const& other) noexcept;
  template <std::integral T>
  Entry& operator=(T const& other) noexcept;
  template <std::floating_point T>
  Entry& operator=(T const& other) noexcept;
  Entry& operator=(std::tm const& other) noexcept;
  Entry& operator=(std::chrono::year_month_day const& other) noexcept;
  Entry& operator=(
      std::chrono::hh_mm_ss<std::chrono::microseconds> const& other) noexcept;
  // TODO
  // figure out how to move assignments below to the .cpp file
  template <typename T>
  Entry& operator=(std::vector<T>&& other) noexcept {
    entries_.clear();
    type_ = Entry::Type::kSequence;
    for (auto const& t : other) {
      entries_.emplace_back(std::move(t), this);
    }
    tag_ = "";
    str_ = "";
    return *this;
  }
  template <typename T1, typename T2>
  Entry& operator=(std::map<T1, T2>&& other) noexcept {
    entries_.clear();
    type_ = Entry::Type::kMap;
    for (auto const& [t1, t2] : other) {
      // Call an Entry() with two entries as parameters
      auto const& test = entries_.emplace_back(std::make_unique<Entry>(
          std::make_unique<Entry>(t1), std::make_unique<Entry>(t2), this));
      bool a = test->key().parent() == test.get();
      bool b = test->value().parent() == test.get();
    }
    tag_ = "";
    str_ = "";
    return *this;
  }
  template <typename T>
  Entry& operator=(std::set<T>&& other) noexcept {
    entries_.clear();
    type_ = Entry::Type::kSet;
    for (auto const& t : other) {
      // Call an Entry() with two entries as parameters, the value is set to be
      // Null
      entries_.emplace_back(Entry(t), Entry(Type::kNull, nullptr), this);
    }
    tag_ = "set";
    str_ = Serialize();
    return *this;
  }

  template <typename T>
  Entry& operator=(std::vector<T> const& other) noexcept {
    entries_.clear();
    type_ = Entry::Type::kSequence;
    for (auto const& t : other) {
      entries_.emplace_back(std::make_unique<Entry>(t, this));
    }
    tag_ = "";
    str_ = "";
    return *this;
  }
  template <typename T1, typename T2>
  Entry& operator=(std::map<T1, T2> const& other) noexcept {
    entries_.clear();
    type_ = Entry::Type::kMap;
    for (auto const& [t1, t2] : other) {
      // Call an Entry() with two entries as parameters
      entries_.emplace_back(std::make_unique<Entry>(std::move(t1)),
                            std::make_unique<Entry>(std::move(t2)), this);
    }
    tag_ = "";
    str_ = Serialize();
    return *this;
  }
  template <typename T>
  Entry& operator=(std::set<T> const& other) noexcept {
    entries_.clear();
    type_ = Entry::Type::kSet;
    for (auto const& t : other) {
      // Call an Entry() with two entries as parameters, value is null
      entries_.emplace_back(Entry(t), Entry(Type::kNull, nullptr), this);
    }
    tag_ = "set";
    str_ = Serialize();
    return *this;
  }

  [[nodiscard]] Entry& key() const;
  [[nodiscard]] Entry& value() const;

  [[nodiscard]] std::chrono::hh_mm_ss<std::chrono::microseconds> to_time()
      const;
  [[nodiscard]] std::chrono::year_month_day to_date() const;
  [[nodiscard]] std::tm to_datetime() const;
  [[nodiscard]] std::tm to_time_point() const;

  [[nodiscard]] long double to_double() const;
  [[nodiscard]] int64_t to_int() const;
  [[nodiscard]] uint64_t to_uint() const;
  [[nodiscard]] bool to_bool() const;
  [[nodiscard]] bool to_boolean() const;
  [[nodiscard]] std::string_view to_string() const noexcept;

  [[nodiscard]] Type const& type() const { return type_; }
  [[nodiscard]] std::string const& str() const { return str_; }
  [[nodiscard]] std::string const& tag() const { return tag_; }

  void append(Entry&& entry);
  void append(std::unique_ptr<Entry> entry);

  [[nodiscard]] std::vector<std::string> Serialize() const noexcept;

  [[nodiscard]] Entry* parent() const noexcept { return parent_; }

  [[nodiscard]] Entry const& link_value() const {
    if (!is_link()) {
      throw std::invalid_argument("This entry is not a boolean");
    }
    return *static_cast<Link*>(data_.get())->link_;
  }

 private:
  struct AbstractValue {
    virtual ~AbstractValue() = default;
  };
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
    explicit Double(long double double_) : double_(double_) {}
  };
  struct Boolean : public AbstractValue {
    bool boolean_;
    explicit Boolean(bool boolean) : boolean_(boolean) {}
  };
  struct TimePoint : public AbstractValue {
    std::tm datetime_;
    std::chrono::year_month_day date_;
    std::chrono::hh_mm_ss<std::chrono::microseconds> time_;
    explicit TimePoint(std::tm const& tm) : datetime_(tm) {
      using namespace std::chrono;
      date_ =
          year_month_day(year(tm.tm_year), month(tm.tm_mon), day(tm.tm_mday));
      time_ = hh_mm_ss<microseconds>(microseconds::duration(
          3600000000ULL * tm.tm_hour + 60000000ULL * tm.tm_min +
          1000000 * tm.tm_sec));
    }
    explicit TimePoint(std::chrono::year_month_day date) : date_(date) {
      datetime_.tm_year = int32_t(date.year());
      datetime_.tm_mon = uint32_t(date.month());
      datetime_.tm_mday = uint32_t(date.day());
      datetime_.tm_hour = 0;
      datetime_.tm_min = 0;
      datetime_.tm_sec = 0;
    }
    explicit TimePoint(
        std::chrono::hh_mm_ss<std::chrono::microseconds> const& time)
        : date_(std::chrono::local_days()), time_(time) {
      datetime_.tm_year = 0;
      datetime_.tm_mon = 0;
      datetime_.tm_mday = 0;
      datetime_.tm_hour = uint32_t(time.hours().count());
      datetime_.tm_min = uint32_t(time.minutes().count());
      datetime_.tm_sec = uint32_t(time.seconds().count());
    }
  };
  struct Link : public AbstractValue {
    Entry* link_;
    explicit Link(Entry* link) : link_(link) {}
  };

  std::vector<std::unique_ptr<Entry>> entries_;
  Type type_ = Entry::Type::kNull;
  std::string str_ = "";
  std::string tag_ = "";
  std::unique_ptr<AbstractValue> data_ = nullptr;
  Entry* parent_ = nullptr;
};
class InvalidSyntax : public std::invalid_argument {
 public:
  using std::invalid_argument::invalid_argument;
};
Entry Parse(std::string_view const& string);
std::optional<Entry> ParseNoexcept(std::string_view const& string) noexcept;
}  // namespace yaml
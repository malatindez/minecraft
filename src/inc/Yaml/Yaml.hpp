#pragma once
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>
namespace yaml {

class Entry {
 public:
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
  explicit Entry(Entry& entry, Entry* parent = nullptr) noexcept;
  explicit Entry(std::unique_ptr<Entry> key, std::unique_ptr<Entry> value,
                 Entry* parent = nullptr) noexcept;
  explicit Entry(std::string_view const& other,
                 Entry* parent = nullptr) noexcept;
  explicit Entry(int64_t const& other, Entry* parent = nullptr) noexcept;
  explicit Entry(int32_t const& other, Entry* parent = nullptr) noexcept;
  explicit Entry(int16_t const& other, Entry* parent = nullptr) noexcept;
  explicit Entry(uint64_t const& other, Entry* parent = nullptr) noexcept;
  explicit Entry(uint32_t const& other, Entry* parent = nullptr) noexcept;
  explicit Entry(uint16_t const& other, Entry* parent = nullptr) noexcept;
  explicit Entry(long double const& other, Entry* parent = nullptr) noexcept;
  explicit Entry(double const& other, Entry* parent = nullptr) noexcept;
  explicit Entry(float const& other, Entry* parent = nullptr) noexcept;
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
  auto begin() const noexcept { return entries_.begin(); }
  auto end() const noexcept { return entries_.end(); }
  size_t size() const noexcept { return entries_.size(); }
  inline bool is_simple_type() const noexcept {
    return !is_pair() && !is_sequence() && !is_map() && !is_set();
  }
  inline bool is_bool() const noexcept { return type_ == Type::kBool; }
  inline bool is_date() const noexcept { return type_ == Type::kDate; }
  inline bool is_directive() const noexcept { return type_ == Type::kDir; }
  inline bool is_double() const noexcept { return type_ == Type::kDouble; }
  inline bool is_int() const noexcept { return type_ == Type::kInt; }
  inline bool is_link() const noexcept { return type_ == Type::kLink; }
  inline bool is_map() const noexcept { return type_ == Type::kMap; }
  inline bool is_null() const noexcept { return type_ == Type::kNull; }
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
  bool operator==(std::tm const& other) const noexcept;
  bool operator==(std::chrono::year_month_day const& other) const noexcept;
  bool operator==(std::chrono::hh_mm_ss<std::chrono::microseconds> const& other)
      const noexcept;
  // if the entry was not found, this function will create the new one with
  // value None
  Entry& operator[](std::string_view const& key);
  Entry& operator[](Entry& key);
  Entry& operator[](size_t const& i);
  Entry& operator[](Entry&& key);
  Entry& operator[](Entry const& key);
  Entry& operator=(Entry& entry);
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
  Entry& operator=(std::tm const& other) noexcept;
  Entry& operator=(std::chrono::year_month_day const& other) noexcept;
  Entry& operator=(
      std::chrono::hh_mm_ss<std::chrono::microseconds> const& other) noexcept;
  // TODO
  // figure out how to move assignments below to the .cpp file
  // Accepts only values that are convertible by std::string
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
      // Call an Entry() with two entries as parameters, the value is set to be
      // Null
      entries_.emplace_back(Entry(t), Entry(Type::kNull, nullptr), this);
    }
    tag_ = "set";
    str_ = Serialize();
    return *this;
  }

  Entry& key() const;
  Entry& value() const;

  std::chrono::hh_mm_ss<std::chrono::microseconds> to_time() const;
  std::chrono::year_month_day to_date() const;
  std::tm to_datetime() const;
  std::tm to_time_point() const;

  long double to_double() const;
  int64_t to_int() const;
  uint64_t to_uint() const;
  bool to_bool() const;
  bool to_boolean() const;
  std::string_view to_string() const noexcept;

  Type const& type() const { return type_; }
  std::string const& str() const { return str_; }
  std::string const& tag() const { return tag_; }

  void append(Entry&& entry);

  std::vector<std::string> Serialize() const noexcept;

  Entry* parent() const noexcept { return parent_; }

  Entry const& link_value() const {
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
    explicit Double(double double_) : double_(double_) {}
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
      time_ = hh_mm_ss<std::chrono::microseconds>(microseconds::duration(
          3600000000ULL * tm.tm_hour + 60000000ULL * tm.tm_min +
          1000000 * tm.tm_sec));
    }
    explicit TimePoint(std::chrono::year_month_day date) : date_(date) {
      datetime_.tm_year = int32_t(date.year());
      datetime_.tm_mon = uint32_t(date.month());
      datetime_.tm_mday = uint32_t(date.day());
    }
    explicit TimePoint(
        std::chrono::hh_mm_ss<std::chrono::microseconds> const& time)
        : time_(time) {
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
Entry Parse(std::string_view const& string);
}  // namespace yaml
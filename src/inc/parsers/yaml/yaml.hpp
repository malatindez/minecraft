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
#include <variant>
#include <vector>

#include "utils/utils.hpp"
namespace yaml {
class Entry;

class InvalidSyntax : public std::invalid_argument {
 public:
  using std::invalid_argument::invalid_argument;
};

namespace impl {
struct Iterator final
    : public utils::BaseIteratorWrapper<
          std::vector<std::unique_ptr<Entry>>::iterator, Entry> {
  using utils::BaseIteratorWrapper<
      std::vector<std::unique_ptr<Entry>>::iterator,
      Entry>::BaseIteratorWrapper;
  [[nodiscard]] reference operator*() final { return **base_iterator(); }
  [[nodiscard]] pointer operator->() final { return base_iterator()->get(); }
};

struct ConstIterator final
    : public utils::BaseIteratorWrapper<
          std::vector<std::unique_ptr<Entry>>::const_iterator, const Entry> {
  using utils::BaseIteratorWrapper<
      std::vector<std::unique_ptr<Entry>>::const_iterator,
      const Entry>::BaseIteratorWrapper;
  [[nodiscard]] reference operator*() final { return **base_iterator(); }
  [[nodiscard]] pointer operator->() final { return base_iterator()->get(); }
};
}  // namespace impl
enum class Type : unsigned char {
  kBool,
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
  kUInt
};

class Entry {
 public:
  ~Entry() = default;
  explicit Entry(Type type, Entry *parent = nullptr) noexcept;
  explicit Entry(Entry &&entry, Entry *parent = nullptr) noexcept;
  explicit Entry(Entry &entry, Entry *parent = nullptr);
  explicit Entry(std::unique_ptr<Entry> key, std::unique_ptr<Entry> value,
                 Entry *parent = nullptr) noexcept;
  explicit Entry(std::string_view const other,
                 Entry *parent = nullptr) noexcept;
  template <std::integral T>
  explicit Entry(T const other, Entry *parent = nullptr) noexcept;
  template <std::floating_point T>
  explicit Entry(T const other, Entry *parent = nullptr) noexcept;
  template <typename T>
  explicit Entry(std::vector<T> &&other, Entry *parent = nullptr);
  template <typename T1, typename T2>
  explicit Entry(std::map<T1, T2> &&other, Entry *parent = nullptr);
  template <typename T>
  explicit Entry(std::set<T> &&other, Entry *parent = nullptr);
  template <typename T>
  explicit Entry(std::vector<T> const &other, Entry *parent = nullptr);
  template <typename T1, typename T2>
  explicit Entry(std::map<T1, T2> const &other, Entry *parent = nullptr);
  template <typename T>
  explicit Entry(std::set<T> const &other, Entry *parent = nullptr);
  [[nodiscard]] inline impl::Iterator begin() noexcept;
  [[nodiscard]] inline impl::Iterator end() noexcept;
  [[nodiscard]] inline impl::ConstIterator begin() const noexcept;
  [[nodiscard]] inline impl::ConstIterator end() const noexcept;
  [[nodiscard]] inline size_t size() const noexcept;
  [[nodiscard]] constexpr bool is_simple_type() const noexcept;
  [[nodiscard]] constexpr bool is_bool() const noexcept;
  [[nodiscard]] constexpr bool is_directive() const noexcept;
  [[nodiscard]] constexpr bool is_double() const noexcept;
  [[nodiscard]] constexpr bool is_int() const noexcept;
  [[nodiscard]] constexpr bool is_link() const noexcept;
  [[nodiscard]] constexpr bool is_map() const noexcept;
  [[nodiscard]] constexpr bool is_null() const noexcept;
  [[nodiscard]] constexpr bool is_pair() const noexcept;
  [[nodiscard]] constexpr bool is_sequence() const noexcept;
  [[nodiscard]] constexpr bool is_set() const noexcept;
  [[nodiscard]] constexpr bool is_string() const noexcept;
  [[nodiscard]] constexpr bool is_uint() const noexcept;
  [[nodiscard]] inline bool contains(std::string_view const string) const;
  template <std::integral T>
  [[nodiscard]] inline bool contains(T const integer) const;
  template <std::floating_point T>
  [[nodiscard]] inline bool contains(T const real) const;

  [[nodiscard]] bool operator==(Entry const &other) const noexcept;
  [[nodiscard]] bool operator==(std::string_view const other) const noexcept;

  template <std::signed_integral T>
  [[nodiscard]] constexpr bool operator==(T const other) const noexcept;
  template <std::floating_point T>
  [[nodiscard]] constexpr bool operator==(T const other) const noexcept;

  // if the entry was not found, this function will create the new one with
  // value None
  [[nodiscard]] Entry &operator[](std::string_view const key);
  [[nodiscard]] Entry &operator[](Entry &key);
  [[nodiscard]] Entry &operator[](size_t const &i);
  [[nodiscard]] Entry &operator[](Entry &&key);
  [[nodiscard]] Entry &operator[](Entry const &key);
  Entry &operator=(Entry &entry);
  Entry &operator=(std::string_view const other) noexcept;
  Entry &operator=(bool const &other) noexcept;
  template <std::integral T>
  Entry &operator=(T const other) noexcept;
  template <std::floating_point T>
  Entry &operator=(T const other) noexcept;
  template <typename T>
  Entry &operator=(std::vector<T> &&other) noexcept;
  template <typename T1, typename T2>
  Entry &operator=(std::map<T1, T2> &&other) noexcept;
  template <typename T>
  Entry &operator=(std::set<T> &&other) noexcept;

  template <typename T>
  Entry &operator=(std::vector<T> const &other) noexcept;
  template <typename T1, typename T2>
  Entry &operator=(std::map<T1, T2> const &other) noexcept;
  template <typename T>
  Entry &operator=(std::set<T> const &other) noexcept;

  [[nodiscard]] inline Entry &key() const;
  [[nodiscard]] inline Entry &value() const;

  [[nodiscard]] inline long double to_double() const;
  [[nodiscard]] inline int64_t to_int() const;
  [[nodiscard]] inline uint64_t to_uint() const;
  [[nodiscard]] inline bool to_bool() const;
  [[nodiscard]] inline bool to_boolean() const;
  [[nodiscard]] inline std::string_view to_string() const noexcept;

  [[nodiscard]] Type const &type() const;
  [[nodiscard]] std::string const &str() const;
  [[nodiscard]] std::string const &tag() const;

  void append(Entry &&entry);
  void append(std::unique_ptr<Entry> entry);

  [[nodiscard]] std::vector<std::string> Serialize() const noexcept;

  [[nodiscard]] Entry *parent() const noexcept { return parent_; }

  [[nodiscard]] Entry const &link_value() const {
    if (!is_link()) {
      throw std::invalid_argument("This entry is not a boolean");
    }
    auto const &t = std::get<Entry *>(data_);
    if (t) {
      return *t;
    }
    throw std::runtime_error("The link is invalid!");
  }

 private:
  typedef std::pair<std::unique_ptr<Entry>, std::unique_ptr<Entry>> Pair;
  std::vector<std::unique_ptr<Entry>> entries_;
  Type type_ = Type::kNull;
  std::string str_ = "";
  std::string tag_ = "";
  std::variant<std::monostate, Pair, int64_t, uint64_t, long double, bool,
               Entry *>
      data_;
  Entry *parent_ = nullptr;
};
template <std::integral T>
Entry::Entry(T const other, Entry *parent) noexcept : parent_(parent) {
  operator=(other);
}
template <std::floating_point T>
Entry::Entry(T const other, Entry *parent) noexcept : parent_(parent) {
  operator=(other);
}  // Accepts only values that are convertible by std::string
template <typename T>
Entry::Entry(std::vector<T> &&other, Entry *parent) : parent_(parent) {
  operator=(other);
}
// Accepts only values that are convertible by std::string
template <typename T1, typename T2>
Entry::Entry(std::map<T1, T2> &&other, Entry *parent) : parent_(parent) {
  operator=(other);
}
// Accepts only values that are convertible by std::string
template <typename T>
Entry::Entry(std::set<T> &&other, Entry *parent) : parent_(parent) {
  operator=(other);
}
template <typename T>
Entry::Entry(std::vector<T> const &other, Entry *parent) : parent_(parent) {
  operator=(other);
}
// Accepts only values that are convertible by std::string
template <typename T1, typename T2>
Entry::Entry(std::map<T1, T2> const &other, Entry *parent) : parent_(parent) {
  operator=(other);
}
// Accepts only values that are convertible by std::string
template <typename T>
Entry::Entry(std::set<T> const &other, Entry *parent) : parent_(parent) {
  operator=(other);
}

[[nodiscard]] inline impl::Iterator Entry::begin() noexcept {
  return impl::Iterator(entries_.begin());
}
[[nodiscard]] inline impl::Iterator Entry::end() noexcept {
  return impl::Iterator(entries_.end());
}
[[nodiscard]] inline impl::ConstIterator Entry::begin() const noexcept {
  return impl::ConstIterator(entries_.begin());
}
[[nodiscard]] inline impl::ConstIterator Entry::end() const noexcept {
  return impl::ConstIterator(entries_.end());
}

[[nodiscard]] inline size_t Entry::size() const noexcept {
  return entries_.size();
}
[[nodiscard]] constexpr bool Entry::is_simple_type() const noexcept {
  return !is_pair() && !is_sequence() && !is_map() && !is_set();
}
[[nodiscard]] constexpr bool Entry::is_bool() const noexcept {
  return type_ == Type::kBool;
}
[[nodiscard]] constexpr bool Entry::is_directive() const noexcept {
  return type_ == Type::kDir;
}
[[nodiscard]] constexpr bool Entry::is_double() const noexcept {
  return type_ == Type::kDouble;
}
[[nodiscard]] constexpr bool Entry::is_int() const noexcept {
  return type_ == Type::kInt;
}
[[nodiscard]] constexpr bool Entry::is_link() const noexcept {
  return type_ == Type::kLink;
}
[[nodiscard]] constexpr bool Entry::is_map() const noexcept {
  return type_ == Type::kMap;
}
[[nodiscard]] constexpr bool Entry::is_null() const noexcept {
  return type_ == Type::kNull;
}
[[nodiscard]] constexpr bool Entry::is_pair() const noexcept {
  return type_ == Type::kPair;
}
[[nodiscard]] constexpr bool Entry::is_sequence() const noexcept {
  return type_ == Type::kSequence;
}
[[nodiscard]] constexpr bool Entry::is_set() const noexcept {
  return type_ == Type::kSet;
}
[[nodiscard]] constexpr bool Entry::is_string() const noexcept {
  return type_ == Type::kString;
}
[[nodiscard]] constexpr bool Entry::is_uint() const noexcept {
  return type_ == Type::kUInt;
}
[[nodiscard]] inline bool Entry::contains(std::string_view const string) const {
  if (!is_sequence() && !is_map()) {
    throw std::invalid_argument("This entry is not a sequence nor a map");
  }
  return std::any_of(
      entries_.begin(), entries_.end(),
      [&string](std::unique_ptr<Entry> const &entry) {
        return (entry->is_string() && entry->to_string() == string) ||
               (entry->is_pair() && entry->key().is_string() &&
                entry->key().to_string() == string);
      });
}

template <std::integral T>
[[nodiscard]] inline bool Entry::contains(T const integer) const {
  if (!is_sequence()) {
    throw std::invalid_argument("This entry is not a sequence");
  }
  return std::any_of(entries_.begin(), entries_.end(),
                     [&integer](std::unique_ptr<Entry> const &entry) {
                       return *entry == integer;
                     });
}
template <std::floating_point T>
[[nodiscard]] inline bool Entry::contains(T const real) const {
  if (!is_sequence()) {
    throw std::invalid_argument("This entry is not a sequence");
  }
  return std::any_of(
      entries_.begin(), entries_.end(),
      [&real](std::unique_ptr<Entry> const &entry) { return *entry == real; });
}
template <std::signed_integral T>
[[nodiscard]] constexpr bool Entry::operator==(T const other) const noexcept {
  return (is_int() && std::get<int64_t>(data_) == (int64_t)other) ||
         (is_uint() && std::get<uint64_t>(data_) == (uint64_t)other);
}
template <std::floating_point T>
[[nodiscard]] constexpr bool Entry::operator==(T const other) const noexcept {
  return is_double() && (std::get<long double>(data_) - other) <
                            std::numeric_limits<long double>::epsilon();
}

template <std::integral T>
Entry &Entry::operator=(T const other) noexcept {
  entries_.clear();
  type_ = Type::kInt;
  str_ = std::to_string(other);
  data_ = (int64_t)other;
  tag_.clear();
  return *this;
}
template <std::floating_point T>
Entry &Entry::operator=(T const other) noexcept {
  entries_.clear();
  type_ = Type::kDouble;
  str_ = std::to_string(other);
  data_ = (long double)other;
  tag_.clear();
  return *this;
}
template <typename T>
Entry &Entry::operator=(std::vector<T> &&other) noexcept {
  entries_.clear();
  type_ = Type::kSequence;
  for (auto &t : other) {
    entries_.emplace_back(std::move(t), this);
  }
  tag_.clear();
  str_.clear();
  return *this;
}
template <typename T1, typename T2>
Entry &Entry::operator=(std::map<T1, T2> &&other) noexcept {
  entries_.clear();
  type_ = Type::kMap;
  for (auto const &[t1, t2] : other) {
    entries_.emplace_back(std::make_unique<Entry>(
        std::make_unique<Entry>(t1), std::make_unique<Entry>(t2), this));
  }
  tag_.clear();
  str_.clear();
  return *this;
}
template <typename T>
Entry &Entry::operator=(std::set<T> &&other) noexcept {
  entries_.clear();
  type_ = Type::kSet;
  for (auto const &t : other) {
    // Call an Entry() with two entries as parameters, the value is set to be
    // Null
    entries_.emplace_back(Entry(t), Entry(Type::kNull, nullptr), this);
  }
  tag_ = "set";
  str_ = Serialize();
  return *this;
}

template <typename T>
Entry &Entry::operator=(std::vector<T> const &other) noexcept {
  entries_.clear();
  type_ = Type::kSequence;
  for (auto const &t : other) {
    entries_.emplace_back(std::make_unique<Entry>(t, this));
  }
  tag_.clear();
  str_.clear();
  return *this;
}
template <typename T1, typename T2>
Entry &Entry::operator=(std::map<T1, T2> const &other) noexcept {
  entries_.clear();
  type_ = Type::kMap;
  for (auto const &[t1, t2] : other) {
    // Call an Entry() with two entries as parameters
    entries_.emplace_back(std::make_unique<Entry>(std::move(t1)),
                          std::make_unique<Entry>(std::move(t2)), this);
  }
  tag_.clear();
  str_ = Serialize();
  return *this;
}
template <typename T>
Entry &Entry::operator=(std::set<T> const &other) noexcept {
  entries_.clear();
  type_ = Type::kSet;
  for (auto const &t : other) {
    // Call an Entry() with two entries as parameters, value is null
    entries_.emplace_back(Entry(t), Entry(Type::kNull, nullptr), this);
  }
  tag_ = "set";
  str_ = Serialize();
  return *this;
}

inline Entry &Entry::key() const {
  if (!is_pair()) {
    throw std::invalid_argument("This entry is not a pair");
  }
  return *std::get<Pair>(data_).first;
}

inline Entry &Entry::value() const {
  if (!is_pair()) {
    throw std::invalid_argument("This entry is not a pair");
  }
  return *std::get<Pair>(data_).second;
}

inline long double Entry::to_double() const {
  if (!is_double()) {
    throw std::invalid_argument("This entry is not a double");
  }
  return std::get<long double>(data_);
}
inline int64_t Entry::to_int() const {
  if (!is_int()) {
    throw std::invalid_argument("This entry is not an integer");
  }
  return std::get<int64_t>(data_);
}
inline uint64_t Entry::to_uint() const {
  if (!is_uint()) {
    throw std::invalid_argument("This entry is not an unsigned integer");
  }
  return std::get<uint64_t>(data_);
}

inline bool Entry::to_bool() const {
  if (!is_bool()) {
    throw std::invalid_argument("This entry is not a boolean");
  }
  return std::get<bool>(data_);
}
inline bool Entry::to_boolean() const {
  if (!is_bool()) {
    throw std::invalid_argument("This entry is not a boolean");
  }
  return std::get<bool>(data_);
}
inline std::string_view Entry::to_string() const noexcept { return str_; }

[[nodiscard]] inline Type const &Entry::type() const { return type_; }
[[nodiscard]] inline std::string const &Entry::str() const { return str_; }
[[nodiscard]] inline std::string const &Entry::tag() const { return tag_; }

Entry Parse(std::string_view const string);
std::optional<Entry> ParseNoexcept(std::string_view const string) noexcept;
}  // namespace yaml

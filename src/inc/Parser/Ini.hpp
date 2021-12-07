#pragma once

#include <Utils/Utils.hpp>
#include <algorithm>
#include <cstdlib>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>

namespace ini {
class InvalidSyntaxException : public std::invalid_argument {
 public:
  using std::invalid_argument::invalid_argument;
};

class TypeConversionException : public std::runtime_error {
 public:
  using std::runtime_error::runtime_error;
};

class KeyErrorException : public std::runtime_error {
 public:
  using std::runtime_error::runtime_error;
};

class Entry {
 public:
  const enum class Type { kInt64, kLongDouble, kString, kNull };
  std::string const& to_string() const noexcept { return value_; }
  std::string const& str() const noexcept { return value_; }

  int64_t to_int() const {
    if (type_ != Type::kInt64) {
      throw TypeConversionException("This entry is not an integer!");
    }
    return static_cast<Int*>(data_.get())->value;
  }
  long double to_double() const {
    if (type_ != Type::kLongDouble) {
      throw TypeConversionException("This entry is not a long double!");
    }
    return static_cast<LongDouble*>(data_.get())->value;
  }

  constexpr explicit Entry(std::string_view const& value, Type type) noexcept
      : type_(type), value_(value) {}

  Entry() = default;
  virtual ~Entry() = default;

  template <typename T>
  constexpr std::enable_if_t<std::is_integral_v<T>, Entry&> operator=(
      T t) noexcept {
    value_ = std::to_string(t);
    type_ = Type::kInt64;
    data_ = std::make_unique<Int>(t);
    return *this;
  }
  template <typename T>
  constexpr std::enable_if_t<std::is_floating_point_v<T>, Entry&> operator=(
      T t) noexcept {
    value_ = std::to_string(t);
    type_ = Type::kLongDouble;
    data_ = std::make_unique<LongDouble>(t);
    return *this;
  }
  template <typename T>
  constexpr std::enable_if_t<std::is_constructible_v<std::string, T>, Entry&>
  operator=(T t) {
    if (utils::trim(t) != t) {
      throw std::invalid_argument("The input string should be trimmed!");
    }
    value_ = std::string(t);
    type_ = Type::kString;
    data_ = nullptr;
    return *this;
  }
  template <typename T>
  constexpr auto operator<=>(T t) const {
    if (type_ == Type::kString) {
      throw TypeConversionException("This entry is a string!");
    }
    if (type_ == Type::kInt64) {
      return t <=> static_cast<Int*>(data_.get())->value;
    } else if (type_ == Type::kLongDouble) {
      return t <=> static_cast<LongDouble*>(data_.get())->value;
    }
  }
  template <typename T>
  constexpr std::enable_if_t<
      std::is_floating_point_v<T> || std::is_integral_v<T>, bool>
  operator==(T t) const {
    if (type_ == Type::kInt64) {
      return static_cast<Int*>(data_.get())->value == t;
    } else if (type_ == Type::kLongDouble) {
      return static_cast<LongDouble*>(data_.get())->value == t;
    }
    return false;
  }
  template <typename T>
  constexpr std::enable_if_t<std::is_constructible_v<std::string, T>, bool>
  operator==(T t) const {
    return value_ == t;
  }
  bool operator==(Entry const& t) const noexcept {
    return type_ == t.type_ && value_ == t.value_;
  }

 private:
  struct AbstractValue {};
  struct Int : public AbstractValue {
    explicit Int(int64_t val) : value(val) {}
    int64_t value;
  };
  struct LongDouble : public AbstractValue {
    explicit LongDouble(long double val) : value(val) {}
    long double value;
  };
  Type type_ = Type::kNull;
  std::string value_ = "";
  std::unique_ptr<AbstractValue> data_ = nullptr;
};

class Section {
 public:
  friend class Ini;
  // Wrapper that converts map<string, Entry*> to map<string_view, Entry&>
  class EntryIterator {
   public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type =
        std::pair<std::string_view, std::reference_wrapper<Entry>>;
    using pointer =
        std::pair<std::string_view,
                  std::reference_wrapper<Entry>>*;  // or also value_type*
    using reference =
        std::pair<std::string_view,
                  std::reference_wrapper<Entry>>&;  // or also value_type&
    reference operator*() {
      if (buf == nullptr) {
        buf = std::make_unique<value_type>(it->first, *it->second);
      }
      return *buf;
    }
    pointer operator->() {
      if (buf == nullptr) {
        buf = std::make_unique<value_type>(it->first, *it->second);
      }
      return &*buf;
    }
    EntryIterator(
        std::map<std::string, std::unique_ptr<Entry>, std::less<>>::iterator it)
        : it(it) {}
    // Prefix increment
    EntryIterator& operator++() {
      it++;
      return *this;
    }

    // Postfix increment
    EntryIterator operator++(int) {
      EntryIterator tmp = *this;
      ++(*this);
      return tmp;
    }

    friend bool operator==(const EntryIterator& a, const EntryIterator& b) {
      return a.it == b.it;
    };

   private:
    std::map<std::string, std::unique_ptr<Entry>, std::less<>>::iterator it;
    std::shared_ptr<std::pair<std::string_view, std::reference_wrapper<Entry>>>
        buf = nullptr;
  };

  template <typename T>
  T const& GetValue(std::string const& key) const;

  Entry& operator[](std::string_view key);

  // Always returns the string value, even of the object of integer or double
  // type
  std::string_view GetString(std::string const& key);
  long double GetDouble(std::string const& key);
  int64_t GetInt(std::string const& key);

  template <typename T>
  void SetValue(std::string const& key, T value) noexcept {
    (*this)[key] = value;
  }

  std::string Serialize() const noexcept;

  EntryIterator begin() noexcept { return EntryIterator(dict_.begin()); }
  EntryIterator end() noexcept { return EntryIterator(dict_.end()); }

  bool EntryExists(std::string_view const& key) const noexcept {
    return Contains(key);
  }
  bool Contains(std::string_view const& key) const noexcept {
    return dict_.contains(std::string(key));
  }
  size_t size() const noexcept { return dict_.size(); }

 protected:
  Section() = default;

 private:
  std::map<std::string, std::unique_ptr<Entry>, std::less<>> dict_;
};

class Ini {
 public:
  // Wrapper that converts map<string, Section*> to map<string_view, Section&>
  class SectionIterator {
   public:
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type =
        std::pair<std::string_view, std::reference_wrapper<Section>>;
    using pointer =
        std::pair<std::string_view,
                  std::reference_wrapper<Section>>*;  // or also value_type*
    using reference =
        std::pair<std::string_view,
                  std::reference_wrapper<Section>>&;  // or also value_type&
    reference operator*() {
      if (buf == nullptr) {
        buf = std::make_unique<value_type>(it->first, *it->second);
      }
      return *buf;
    }
    pointer operator->() {
      if (buf == nullptr) {
        buf = std::make_unique<value_type>(it->first, *it->second);
      }
      return &*buf;
    }
    SectionIterator(std::map<std::string, std::unique_ptr<Section>,
                             std::less<>>::iterator it)
        : it(it) {}
    // Prefix increment
    SectionIterator& operator++() {
      it++;
      return *this;
    }

    // Postfix increment
    SectionIterator operator++(int) {
      SectionIterator tmp = *this;
      ++(*this);
      return tmp;
    }

    friend bool operator==(const SectionIterator& a, const SectionIterator& b) {
      return a.it == b.it;
    };

   private:
    std::map<std::string, std::unique_ptr<Section>, std::less<>>::iterator it;
    std::shared_ptr<
        std::pair<std::string_view, std::reference_wrapper<Section>>>
        buf = nullptr;
  };

  Ini() = default;

  Section& operator[](std::string_view key);

  Section& CreateSection(std::string const& key);
  std::string Serialize() const noexcept;
  static Ini Deserialize(std::string_view const& data);

  bool SectionExists(std::string_view const& key) const noexcept {
    return Contains(key);
  }
  bool Contains(std::string_view const& key) const noexcept {
    return dict_.contains(std::string(key));
  }
  SectionIterator begin() noexcept { return SectionIterator(dict_.begin()); }
  SectionIterator end() noexcept { return SectionIterator(dict_.end()); }

  size_t size() const noexcept { return dict_.size(); }

 private:
  friend inline void DeserializeLine(Ini& ini, std::string const& section,
                                     std::string& line);
  std::map<std::string, std::unique_ptr<Section>, std::less<>> dict_;
};

}  // namespace ini
#pragma once

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
  operator=(T t) noexcept {
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
  template <typename T>
  T const& GetValue(std::string const& key) const;

  Entry& operator[](std::string const& key);

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

 protected:
  Section() = default;

 private:
  std::map<std::string, std::unique_ptr<Entry>, std::less<>> dict_;
};

class Ini {
 public:
  Ini() = default;

  Section& operator[](std::string const& section_key);
  Section& CreateSection(std::string const& key);
  std::string Serialize() const noexcept;
  static Ini Deserialize(std::string_view const& data);

 private:
  friend inline void DeserializeLine(Ini& ini, std::string const& section,
                                     std::string& line);
  std::map<std::string, std::unique_ptr<Section>, std::less<>> dict_;
};

}  // namespace ini
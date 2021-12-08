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

  [[nodiscard]] int64_t to_int() const {
    if (type_ != Type::kInt64) {
      throw TypeConversionException("This entry is not an integer!");
    }
    return static_cast<Int*>(data_.get())->value;
  }
  [[nodiscard]] long double to_double() const {
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
  [[nodiscard]] constexpr auto operator<=>(T t) const {
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
  [[nodiscard]] constexpr std::enable_if_t<
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
  constexpr std::enable_if_t<std::is_constructible_v<std::string, T>,
                             bool> [[nodiscard]]
  operator==(T t) const {
    return value_ == t;
  }
  [[nodiscard]] bool operator==(Entry const& t) const noexcept {
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
  template <typename T>
  using wrap = std::reference_wrapper<T>;
  using iterator_t = utils::BaseIteratorWrapper<
      std::map<std::string, std::unique_ptr<Entry>, std::less<>>::iterator,
      std::pair<std::string_view, wrap<Entry>>>;
  using const_iterator_t = utils::BaseIteratorWrapper<
      std::map<std::string, std::unique_ptr<Entry>,
               std::less<>>::const_iterator,
      std::pair<std::string_view, wrap<const Entry>>>;

  class Iterator : public iterator_t {
   public:
    using iterator_t::BaseIteratorWrapper;
    [[nodiscard]] reference operator*() final {
      if (buf == nullptr) {
        buf = std::make_shared<value_type>(base_iterator()->first,
                                           *base_iterator()->second);
      }
      return *buf;
    }
    [[nodiscard]] pointer operator->() final {
      if (buf == nullptr) {
        buf = std::make_shared<value_type>(base_iterator()->first,
                                           *base_iterator()->second);
      }
      return &*buf;
    }

   private:
    std::shared_ptr<value_type> buf = nullptr;
  };
  class ConstIterator : public const_iterator_t {
    using const_iterator_t::BaseIteratorWrapper;
    [[nodiscard]] reference operator*() final {
      if (buf == nullptr) {
        buf = std::make_shared<value_type>(base_iterator()->first,
                                           *base_iterator()->second);
      }
      return *buf;
    }
    [[nodiscard]] pointer operator->() final {
      if (buf == nullptr) {
        buf = std::make_shared<value_type>(base_iterator()->first,
                                           *base_iterator()->second);
      }
      return &*buf;
    }

   private:
    std::shared_ptr<value_type> buf = nullptr;
  };

  template <typename T>
  [[nodiscard]] T const& GetValue(std::string const& key) const;

  [[nodiscard]] Entry& operator[](std::string_view key);

  // Always returns the string value, even of the object of integer or double
  // type
  [[nodiscard]] std::string_view GetString(std::string const& key);
  [[nodiscard]] long double GetDouble(std::string const& key);
  [[nodiscard]] int64_t GetInt(std::string const& key);

  template <typename T>
  void SetValue(std::string const& key, T value) noexcept {
    (*this)[key] = value;
  }

  [[nodiscard]] std::string Serialize() const noexcept;

  [[nodiscard]] Iterator begin() noexcept { return Iterator(dict_.begin()); }
  [[nodiscard]] Iterator end() noexcept { return Iterator(dict_.end()); }
  [[nodiscard]] ConstIterator begin() const noexcept {
    return ConstIterator(dict_.cbegin());
  }
  [[nodiscard]] ConstIterator end() const noexcept {
    return ConstIterator(dict_.cend());
  }
  [[nodiscard]] ConstIterator cbegin() const noexcept {
    return ConstIterator(dict_.cbegin());
  }
  [[nodiscard]] ConstIterator cend() const noexcept {
    return ConstIterator(dict_.cend());
  }

  [[nodiscard]] bool EntryExists(std::string_view const& key) const noexcept {
    return Contains(key);
  }
  [[nodiscard]] bool Contains(std::string_view const& key) const noexcept {
    return dict_.contains(std::string(key));
  }
  [[nodiscard]] size_t size() const noexcept { return dict_.size(); }

 protected:
  Section() = default;

 private:
  std::map<std::string, std::unique_ptr<Entry>, std::less<>> dict_;
};

class Ini {
 public:
  template <typename T>
  using wrap = std::reference_wrapper<T>;
  using iterator_t = utils::BaseIteratorWrapper<
      std::map<std::string, std::unique_ptr<Section>, std::less<>>::iterator,
      std::pair<std::string_view, wrap<Section>>>;
  using const_iterator_t = utils::BaseIteratorWrapper<
      std::map<std::string, std::unique_ptr<Section>,
               std::less<>>::const_iterator,
      std::pair<std::string_view, wrap<const Section>>>;
  class Iterator : public iterator_t {
   public:
    using iterator_t::BaseIteratorWrapper;
    [[nodiscard]] reference operator*() final {
      if (buf == nullptr) {
        buf = std::make_shared<value_type>(base_iterator()->first,
                                           *base_iterator()->second);
      }
      return *buf;
    }
    [[nodiscard]] pointer operator->() final {
      if (buf == nullptr) {
        buf = std::make_shared<value_type>(base_iterator()->first,
                                           *base_iterator()->second);
      }
      return &*buf;
    }

   private:
    std::shared_ptr<value_type> buf = nullptr;
  };

  class ConstIterator : public const_iterator_t {
   public:
    using const_iterator_t::BaseIteratorWrapper;
    [[nodiscard]] reference operator*() final {
      if (buf == nullptr) {
        buf = std::make_shared<value_type>(base_iterator()->first,
                                           *base_iterator()->second);
      }
      return *buf;
    }
    [[nodiscard]] pointer operator->() final {
      if (buf == nullptr) {
        buf = std::make_shared<value_type>(base_iterator()->first,
                                           *base_iterator()->second);
      }
      return &*buf;
    }

   private:
    std::shared_ptr<value_type> buf = nullptr;
  };
  Ini() = default;

  [[nodiscard]] Section& operator[](std::string_view key);

  Section& CreateSection(std::string const& key);
  [[nodiscard]] std::string Serialize() const noexcept;
  [[nodiscard]] static Ini Deserialize(std::string_view const& data);

  [[nodiscard]] bool SectionExists(std::string_view const& key) const noexcept {
    return Contains(key);
  }
  [[nodiscard]] bool Contains(std::string_view const& key) const noexcept {
    return dict_.contains(std::string(key));
  }

  [[nodiscard]] size_t size() const noexcept { return dict_.size(); }

  [[nodiscard]] Iterator begin() noexcept { return Iterator(dict_.begin()); }
  [[nodiscard]] Iterator end() noexcept { return Iterator(dict_.end()); }
  [[nodiscard]] ConstIterator begin() const noexcept {
    return ConstIterator(dict_.begin());
  }
  [[nodiscard]] ConstIterator end() const noexcept {
    return ConstIterator(dict_.end());
  }
  [[nodiscard]] ConstIterator cbegin() const noexcept {
    return ConstIterator(dict_.begin());
  }
  [[nodiscard]] ConstIterator cend() const noexcept {
    return ConstIterator(dict_.end());
  }

 private:
  friend inline void DeserializeLine(Ini& ini, std::string const& section,
                                     std::string& line);
  std::map<std::string, std::unique_ptr<Section>, std::less<>> dict_;
};

}  // namespace ini
#pragma once
#include <algorithm>
#include <cstdlib>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
class Config {
 public:
  class Section {
   public:
    friend class Config;

    class TypeConversionException : public std::runtime_error {
     public:
      explicit TypeConversionException(std::string const& t)
          : std::runtime_error(t) {}
    };

    template <typename T>
    T const& GetValue(std::string const& key) const;

    std::string_view operator[](std::string const& key);

    // Always returns the string value, even of the object of integer or double
    // type
    std::string_view GetString(std::string const& key);
    long double GetDouble(std::string const& key);
    uint64_t GetInt(std::string const& key);

    void SetValue(std::string const& key, uint64_t value) noexcept;
    void SetValue(std::string const& key, long double value) noexcept;
    void SetValue(std::string const& key, std::string const& value) noexcept;

    std::string Serialize() const noexcept;

   private:
    Section() = default;
    class AbstractValue {
     public:
      const enum class Type { kUint64, kDouble, kString };

      std::string_view value() const noexcept { return value_; }
      Type type() const noexcept { return type_; }
      explicit AbstractValue(std::string_view const& value, Type type)
          : type_(type), value_(value) {}
      AbstractValue() = default;
      virtual ~AbstractValue() = default;

     private:
      const Type type_;
      std::string value_;
    };
    template <typename T>
    class Value : public AbstractValue {
     public:
      T t;
      Value(std::string_view const& value, T t, Type type)
          : AbstractValue(value, type), t(t) {}
    };

    std::map<std::string, std::unique_ptr<AbstractValue>, std::less<>> dict_;
  };

  Config() = default;

  Section& operator[](std::string const& section_key);
  Section& CreateSection(std::string const& key);
  std::string Serialize() const noexcept;
  void Deserialize(std::string_view const& str);

 private:
  inline void DeserializeLine(std::string const& section, std::string& line);
  std::map<std::string, std::unique_ptr<Section>, std::less<>> dict_;
};
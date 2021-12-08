#pragma once
#include <stdexcept>
#include <string>

namespace utils {
// trim from start (in place)
constexpr std::string_view ltrimview(std::string_view const& s) {
  return std::string_view(
      std::find_if(s.begin(), s.end(),
                   [](unsigned char ch) { return !std::isspace(ch); }),
      s.end());
}

// trim from end (in place)
constexpr std::string_view rtrimview(std::string_view const& s) {
  return std::string_view(
      s.begin(), std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
                   return !std::isspace(ch);
                 }).base());
}

// trim from both ends (in place)
constexpr std::string_view trimview(std::string_view const& s) {
  return ltrimview(rtrimview(s));
}
// trim from start (in place)
constexpr std::string ltrim(std::string const& s) {
  return std::string(
      std::find_if(s.begin(), s.end(),
                   [](unsigned char ch) { return !std::isspace(ch); }),
      s.end());
}

// trim from end (in place)
constexpr std::string rtrim(std::string const& s) {
  return std::string(s.begin(),
                     std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
                       return !std::isspace(ch);
                     }).base());
}

// trim from both ends (in place)
constexpr std::string trim(std::string const& s) { return ltrim(rtrim(s)); }

template <typename T, typename U>
class BaseIteratorWrapper {
 public:
  using iterator_category = std::forward_iterator_tag;
  using iterator_type = T;
  using value_type = U;
  using difference_type = std::ptrdiff_t;
  using pointer = value_type*;    // or also value_type*
  using reference = value_type&;  // or also value_type&
  [[nodiscard]] virtual reference operator*() {
    throw std::runtime_error("operator* wasn't overloaded");
  }
  [[nodiscard]] virtual pointer operator->() {
    throw std::runtime_error("operator-> wasn't overloaded");
  }

  constexpr explicit BaseIteratorWrapper(iterator_type it) : it(it) {}
  // Prefix increment
  BaseIteratorWrapper& operator++() {
    it++;
    return *this;
  }
  virtual ~BaseIteratorWrapper() = default;

  // Postfix increment
  BaseIteratorWrapper operator++(int) {
    BaseIteratorWrapper tmp = *this;
    ++(*this);
    return tmp;
  }

  constexpr friend bool operator==(const BaseIteratorWrapper& a,
                                   const BaseIteratorWrapper& b) {
    return a.it == b.it;
  };

  constexpr iterator_type const& base_iterator() const noexcept { return it; }

 private:
  iterator_type it;
};

}  // namespace utils
#pragma once
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
    return std::string(
        s.begin(), std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
            }).base());
}

// trim from both ends (in place)
constexpr std::string trim(std::string const& s) {
    return ltrim(rtrim(s));
}
}
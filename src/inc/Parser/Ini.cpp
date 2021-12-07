#include "Ini.hpp"
namespace ini {

template <typename T>
T const& Section::GetValue(std::string const& key) const {
  return operator[]<T>(key);
}

Entry& Section::operator[](std::string const& key) {
  if (!dict_.contains(key)) {
    dict_[key] = std::make_unique<Entry>();
  }
  return *dict_[key];
}

// Always returns the string value, even of the object of integer or double
// type
std::string_view Section::GetString(std::string const& key) {
  if (!dict_.contains(key)) {
    throw KeyErrorException("Invalid key: " + key);
  }
  return dict_[key]->str();
}
long double Section::GetDouble(std::string const& key) {
  if (!dict_.contains(key)) {
    throw KeyErrorException("Invalid key: " + key);
  }
  return dict_[key]->to_double();
}
int64_t Section::GetInt(std::string const& key) {
  if (!dict_.contains(key)) {
    throw KeyErrorException("Invalid key: " + key);
  }
  return dict_[key]->to_int();
}

inline void Replace(std::string& str, std::string_view const& find,
                    std::string_view const& replace) noexcept {
  auto i = (size_t)-1;
  while (true) {
    i = str.find(find, i + 1);
    if (i == std::string::npos) {
      break;
    }
    str.replace(i, find.length(), replace);
  }
}

std::string Section::Serialize() const noexcept {
  static auto format = [](std::string_view const& s) {
    std::string str(s);
    std::erase(str, '\n');
    std::erase(str, '\r');
    Replace(str, "=", "\\=");
    return str;
  };
  std::string return_value;
  for (auto const& [key, value] : dict_) {
    return_value += format(key) + "=" + format(value->str()) + "\n";
  }
  return return_value;
}

Section& Ini::operator[](std::string const& key) {
  if (!dict_.contains(key)) {
    // just a temporary structure to call a protected construtor
    struct T : Section {};
    dict_[key] = std::make_unique<T>();
  }
  return *dict_[key];
}
Section& Ini::CreateSection(std::string const& key) {
  if (!dict_.contains(key)) {
    // just a temporary structure to call a protected construtor
    struct T : Section {};
    dict_[key] = std::make_unique<T>();
  }
  return *dict_[key];
}
std::string Ini::Serialize() const noexcept {
  std::string return_value;
  for (auto const& [key, value] : dict_) {
    return_value += "[" + key + "]\n" + value->Serialize();
  }
  return return_value;
}
Ini Ini::Deserialize(std::string_view const& str) {
  Ini return_value;
  uint64_t i = 0;
  bool skip = false;
  std::string line;
  std::string current_section = "";
  while (i < str.size()) {
    if (str[i] == '\n') {
      if (line.size() >= 3 && line[0] == '[' && line[line.size() - 1] == ']') {
        current_section = std::string(line.begin() + 1, line.end() - 1);
      } else {
        DeserializeLine(return_value, current_section, line);
      }
      line = "";
    } else if ((str[i] == ';' || str[i] == '#') &&
               (i == 0 || str[i - 1] != '\\')) {
      skip = true;
    } else if (!skip) {
      line += str[i];
    }
    i++;
  }
  if (line != "") {
      DeserializeLine(return_value, current_section, line);
  }

  return return_value;
}

// trim from start (in place)
constexpr std::string_view ltrim(std::string_view const& s) {
  return std::string_view(
      std::find_if(s.begin(), s.end(),
                   [](unsigned char ch) { return !std::isspace(ch); }),
      s.end());
}

// trim from end (in place)
constexpr std::string_view rtrim(std::string_view const& s) {
  return std::string_view(
      s.begin(), std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
                   return !std::isspace(ch);
                 }).base());
}

// trim from both ends (in place)
constexpr std::string_view trim(std::string_view const& s) {
  return ltrim(rtrim(s));
}

inline void DeserializeLine(Ini& ini, std::string const& section,
                            std::string& line) {
  if (!ini.dict_.contains(section)) {
    // just a temporary structure to call a protected construtor
    struct T : Section {};
    ini.dict_[section] = std::make_unique<T>();
  }
  size_t pos = line.find('=');
  while (pos != std::string::npos && pos != 0 && line[pos - 1] == '\\') {
    pos = line.find('=', pos + 1);
  }
  if (pos == std::string::npos) {
    return;
  }

  std::string key{trim(std::string_view{line.begin(), line.begin() + pos})};
  std::string value{trim(std::string_view{line.begin() + pos + 1, line.end()})};
  Replace(key, "\\=", "=");
  Replace(value, "\\=", "=");
  char* pEnd;
  int64_t ll = strtoll(value.c_str(), &pEnd, 10);
  if (value.c_str() != pEnd && *pEnd == '\0') {
    (*ini.dict_[section])[key] = ll;
    return;
  }
  long double ld = strtold(value.c_str(), &pEnd);
  if (value.c_str() != pEnd && *pEnd == '\0') {
    (*ini.dict_[section])[key] = ld;
    return;
  }
  (*ini.dict_[section])[key] = value;
}
}  // namespace ini
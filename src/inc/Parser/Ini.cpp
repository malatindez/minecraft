#include "Ini.hpp"
namespace ini {

template <typename T>
T const& Section::GetValue(std::string const& key) const {
  return operator[]<T>(key);
}

Entry& Section::operator[](std::string_view key) {
  if (utils::trimview(key) != key) {
    throw std::invalid_argument("The input string should be trimmed!");
  }
  std::string t{key};
  if (!dict_.contains(t)) {
    dict_[t] = std::make_unique<Entry>();
  }
  return *dict_[t];
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

std::string Section::Serialize() const noexcept {
  static constexpr auto format = [](std::string_view const& s) {
    std::string str(s);
    std::erase(str, '\n');
    std::erase(str, '\r');
    return str;
  };
  std::string return_value;
  for (auto const& [key, value] : dict_) {
    return_value += format(key) + "=" + format(value->str()) + "\n";
  }
  return return_value;
}

Section& Ini::operator[](std::string_view key) {
  if (utils::trimview(key) != key) {
    throw std::invalid_argument("The input string should be trimmed!");
  }
  std::string t{key};
  if (!dict_.contains(t)) {
    // just a temporary structure to call a protected construtor
    struct T : Section {};
    dict_[t] = std::make_unique<T>();
  }
  return *dict_[t];
}
Section& Ini::CreateSection(std::string const& key) {
  if (utils::trimview(key) != key) {
    throw std::invalid_argument("The input string should be trimmed!");
  }
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
      skip = false;
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

  std::string key{
      utils::trimview(std::string_view{line.begin(), line.begin() + pos})};
  std::string value{
      utils::trimview(std::string_view{line.begin() + pos + 1, line.end()})};

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
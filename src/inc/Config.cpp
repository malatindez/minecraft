#include "Config.hpp"

template <typename T>
T const& Config::Section::GetValue(std::string const& key) const {
  return operator[]<T>(key);
}

std::string_view Config::Section::operator[](std::string const& key) {
  if (!dict_.contains(key)) {
    throw std::invalid_argument("Invalid key");
  }
  return dict_[key]->value();
}

// Always returns the string value, even of the object of integer or double
// type
std::string_view Config::Section::GetString(std::string const& key) {
  if (!dict_.contains(key)) {
    throw std::invalid_argument("Invalid key");
  }
  return dict_[key]->value();
}
long double Config::Section::GetDouble(std::string const& key) {
  if (!dict_.contains(key)) {
    throw std::invalid_argument("Invalid key");
  }
  auto ptr = dynamic_cast<Value<long double>*>(dict_[key].get());
  if (ptr == nullptr) {
    throw "The value behind the key isn't a double.";
  }
  return ptr->t;
}
uint64_t Config::Section::GetInt(std::string const& key) {
  if (!dict_.contains(key)) {
    throw std::invalid_argument("Invalid key");
  }
  auto ptr = dynamic_cast<Value<uint64_t>*>(dict_[key].get());
  if (ptr == nullptr) {
    throw "The value behind the key isn't an integer.";
  }
  return ptr->t;
}

void Config::Section::SetValue(std::string const& key,
                               uint64_t value) noexcept {
  dict_[key] = std::unique_ptr<AbstractValue>(
      new Value(std::to_string(value), value, AbstractValue::Type::kUint64));
}
void Config::Section::SetValue(std::string const& key,
                               long double value) noexcept {
  dict_[key] = std::unique_ptr<AbstractValue>(
      new Value(std::to_string(value), value, AbstractValue::Type::kDouble));
}
void Config::Section::SetValue(std::string const& key,
                               std::string const& value) noexcept {
  dict_[key] = std::unique_ptr<AbstractValue>(
      new Value(value, value, AbstractValue::Type::kString));
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

std::string Config::Section::Serialize() const noexcept {
  static auto format = [](std::string_view const& s) {
    std::string str(s);
    std::erase(str, '\n');
    std::erase(str, '\r');
    Replace(str, "=", "\\=");
    return str;
  };
  std::string return_value;
  for (auto const& [key, value] : dict_) {
    return_value += format(key) + "=" + format(value->value()) + "\n";
  }
  return return_value;
}

Config::Section& Config::operator[](std::string const& key) {
  if (!dict_.contains(key)) {
    dict_[key] = std::unique_ptr<Section>(new Section());
  }
  return *dict_[key];
}
Config::Section& Config::CreateSection(std::string const& key) {
  if (!dict_.contains(key)) {
    dict_[key] = std::unique_ptr<Section>(new Section());
  }
  return *dict_[key];
}
std::string Config::Serialize() const noexcept {
  std::string return_value;
  for (auto const& [key, value] : dict_) {
    return_value += "[" + key + "]\n" + value->Serialize();
  }
  return return_value;
}
void Config::Deserialize(std::string_view const& str) {
  uint64_t i = 0;
  bool skip = false;
  std::string line;
  std::string current_section = "";
  while (i < str.size()) {
    if (str[i] == '\n') {
      if (line.size() > 3 && line[0] == '[' && line[line.size() - 1] == ']') {
        current_section = std::string(line.begin() + 1, line.end() - 1);
      } else {
        DeserializeLine(current_section, line);
      }
      line = "";
    } else if (str[i] == ';' && (i == 0 || str[i - 1] != '\\')) {
      skip = true;
    } else if (!skip) {
      line += str[i];
    }
    i++;
  }
}

inline void Config::DeserializeLine(std::string const& section,
                                    std::string& line) {
  if (!dict_.contains(section)) {
    dict_[section] = std::unique_ptr<Section>(new Section());
  }
  size_t pos = line.find('=');
  while (pos != std::string::npos && pos != 0 && line[pos - 1] == '\\') {
    pos = line.find('=', pos + 1);
  }
  if (pos == std::string::npos) {
    return;
  }

  auto key = std::string(line.begin(), line.begin() + pos);
  auto value = std::string(line.begin() + pos + 1, line.end());
  Replace(key, "\\=", "=");
  Replace(value, "\\=", "=");
  char* pEnd;
  uint64_t ull = strtoull(value.c_str(), &pEnd, 10);
  if (value.c_str() != pEnd && *pEnd == '\0') {
    dict_[section]->SetValue(key, ull);
    return;
  }
  long double ld = strtold(value.c_str(), &pEnd);
  if (value.c_str() != pEnd && *pEnd == '\0') {
    dict_[section]->SetValue(key, ld);
    return;
  }
  dict_[section]->SetValue(key, value);
}
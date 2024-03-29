#include "yaml.hpp"
namespace yaml {

Entry::Entry(Type type, Entry* parent) noexcept
    : type_(type), parent_(parent) {}

Entry::Entry(Entry&& entry, Entry* parent) noexcept
    : type_(entry.type_),
      str_(entry.str_),
      tag_(entry.tag_),
      parent_(entry.parent_) {
  entries_ = std::move(entry.entries_);
  data_ = std::move(entry.data_);
  if (parent != nullptr) {
    parent_ = parent;
  }
}
Entry::Entry(Entry& entry, Entry* parent) : parent_(entry.parent_) {
  operator=(entry);
  if (parent != nullptr) {
    parent_ = parent;
  }
}
Entry::Entry(std::unique_ptr<Entry> key, std::unique_ptr<Entry> value,
             Entry* parent) noexcept
    : parent_(parent) {
  type_ = Type::kPair;
  key->parent_ = this;
  value->parent_ = this;
  data_ = Pair{std::move(key), std::move(value)};
  str_.clear();
}
Entry::Entry(std::string_view const other, Entry* parent) noexcept
    : parent_(parent) {
  operator=(other);
}
bool Entry::operator==(Entry const& other) const noexcept {
  return other.type_ == type_ && str_ == other.str_;
}
bool Entry::operator==(std::string_view const other) const noexcept {
  return is_string() && str_ == other;
}
Entry& Entry::operator[](std::string_view const key) {
  if (!is_null() && !is_map()) {
    throw std::invalid_argument("This entry is not a map");
  }
  if (is_null()) {
    operator=(std::map<int, int>());
  }
  auto it = std::find_if(entries_.begin(), entries_.end(),
                         [&key](std::unique_ptr<Entry> const& entry) {
                           return entry->is_pair() &&
                                  entry->key().is_string() &&
                                  entry->key().to_string() == key;
                         });
  if (it == entries_.end()) {
    return entries_
        .emplace_back(
            std::make_unique<Entry>(std::make_unique<Entry>(key),
                                    std::make_unique<Entry>(Type::kNull), this))
        ->value();
  }
  return (*it)->value();
}
Entry& Entry::operator[](Entry&& key) {
  if (!is_null() && !is_map()) {
    throw std::invalid_argument("This entry is not a map");
  }
  if (is_null()) {
    operator=(std::map<int, int>());
  }
  auto it = std::find_if(entries_.begin(), entries_.end(),
                         [&key](std::unique_ptr<Entry> const& entry) {
                           return entry->is_pair() && entry->key() == key;
                         });
  if (it == entries_.end()) {
    return entries_
        .emplace_back(std::make_unique<Entry>(
            std::make_unique<Entry>(std::move(key), nullptr),
            std::make_unique<Entry>(Type::kNull), this))
        ->value();
  }
  return (*it)->value();
}

Entry& Entry::operator[](Entry const& key) {
  if (!is_null() && !is_map()) {
    throw std::invalid_argument("This entry is not a map");
  }
  if (is_null()) {
    operator=(std::map<int, int>());
  }
  auto it = std::find_if(entries_.begin(), entries_.end(),
                         [&key](std::unique_ptr<Entry> const& entry) {
                           return entry->is_pair() && entry->key() == key;
                         });
  if (it == entries_.end()) {
    throw std::invalid_argument("Invalid key");
  }
  return (*it)->value();
}
Entry& Entry::operator[](size_t const& i) {
  if (!is_map() && !is_sequence()) {
    throw std::invalid_argument("This entry is not a map nor a sequence");
  }
  if (entries_.size() < i) {
    throw std::invalid_argument("The index is not valid");
  }
  return *entries_[i];
}
bool recusive_validity_check(Entry const& entry, Entry const& key) {
  if (auto parent = entry.parent(); parent != nullptr) {
    if (*parent == key) {
      return false;
    }
    return recusive_validity_check(*parent, key);
  }
  return true;
}
Entry& Entry::operator=(Entry& entry) {
  if (!(*this != entry && recusive_validity_check(*this, entry) &&
        recusive_validity_check(entry, *this))) {
    throw std::invalid_argument("The entry cannot contain itself");
  }
  entries_.clear();
  type_ = Type::kLink;
  data_ = &entry;
  str_.clear();
  tag_.clear();  // TODO link name generation
  return *this;
}

Entry& Entry::operator=(std::string_view const other) noexcept {
  entries_.clear();
  type_ = Type::kString;
  str_ = other;
  tag_.clear();
  return *this;
}
Entry& Entry::operator=(bool const& other) noexcept {
  entries_.clear();
  type_ = Type::kBool;
  str_ = other ? "true" : "false";
  data_ = other;
  tag_.clear();
  return *this;
}

void Entry::append(Entry&& entry) {
  append(std::make_unique<Entry>(std::move(entry), this));
}
void Entry::append(std::unique_ptr<Entry> entry) {
  entry->parent_ = this;
  if (is_sequence()) {
    entries_.emplace_back(std::move(entry));
    return;
  }
  if (is_map()) {
    if (entry->is_pair()) {
      entries_.emplace_back(std::move(entry));
      return;
    }
    throw std::invalid_argument(
        "This entry is a map, but the argument is not a pair");
  }
  throw std::invalid_argument("This entry is not a sequence nor a map");
}
inline std::vector<std::string> SerializeMap(
    std::vector<std::unique_ptr<Entry>> const& entries) {
  std::vector<std::string> return_value;
  for (std::unique_ptr<Entry> const& entry : entries) {
    if (!entry->is_pair()) {
      continue;
    }
    for (std::string& str : entry->Serialize()) {
      return_value.emplace_back(std::move(str));
    }
  }
  return return_value;
}
inline std::vector<std::string> SerializeSet(
    std::vector<std::unique_ptr<Entry>> const& entries) {
  std::vector<std::string> return_value;
  for (std::unique_ptr<Entry> const& entry : entries) {
    if (!entry->is_pair()) {
      continue;
    }
    auto t = entry->Serialize();

    for (std::string& str : t) {
      str.insert(0, "  ");
    }
    t[0][0] = '?';
    for (std::string& str : t) {
      return_value.emplace_back(std::move(str));
    }
  }
  return return_value;
}
inline std::vector<std::string> SerializeSequence(
    std::vector<std::unique_ptr<Entry>> const& entries) {
  std::vector<std::string> return_value;
  for (std::unique_ptr<Entry> const& entry : entries) {
    auto t = entry->Serialize();

    for (std::string& str : t) {
      str.insert(0, "  ");
    }
    t[0][0] = '-';
    for (std::string& str : t) {
      return_value.emplace_back(std::move(str));
    }
  }
  return return_value;
}
inline std::vector<std::string> SerializePair(Entry const& entry) {
  std::vector<std::string> return_value;
  auto const& key = entry.key();
  if (key.is_simple_type() && entry.value().is_simple_type()) {
    return_value.emplace_back(key.Serialize()[0] + ": " +
                              entry.value().Serialize()[0]);
  } else if (key.is_simple_type()) {
    auto t = key.Serialize()[0] + ": ";
    return_value.emplace_back(std::move(t));
    auto y = entry.value().Serialize();
    for (std::string& str : y) {
      str.insert(0, "  ");
    }
    for (std::string& str : y) {
      return_value.emplace_back(std::move(str));
    }
  } else {
    auto t = key.Serialize();
    for (std::string& str : t) {
      str.insert(0, "  ");
    }
    t[0][0] = '?';
    for (std::string& str : t) {
      return_value.emplace_back(std::move(str));
    }
    t = entry.value().Serialize();

    for (std::string& str : t) {
      str.insert(0, "  ");
    }
    t[0][0] = ':';
    for (std::string& str : t) {
      return_value.emplace_back(std::move(str));
    }
  }
  return return_value;
}
std::vector<std::string> Entry::Serialize() const noexcept {
  std::vector<std::string> return_value;
  if (is_map()) {
    return_value = SerializeMap(entries_);
  } else if (is_set()) {
    return_value = SerializeSet(entries_);
  } else if (is_sequence()) {
    return_value = SerializeSequence(entries_);
  } else if (is_pair()) {
    return_value = SerializePair(*this);
  } else if (is_uint()) {
    return_value = std::vector<std::string>{std::to_string(to_uint())};
  } else if (is_int()) {
    return_value = std::vector<std::string>{std::to_string(to_int())};
  } else if (is_double()) {
    return_value = std::vector<std::string>{std::to_string(to_double())};
  } else if (is_null()) {
    return_value = std::vector<std::string>{""};
  } else if (is_string()) {
    return_value = std::vector<std::string>{std::string(to_string())};
  } else if (is_link()) {
    return_value = link_value().Serialize();
  }
  if (return_value.empty()) {
    return_value.emplace_back("");
  }
  return return_value;
}

// trim from start (in place)
constexpr std::string_view ltrim(std::string_view const s) {
  return std::string_view(
      std::find_if(s.begin(), s.end(),
                   [](unsigned char ch) { return !std::isspace(ch); }),
      s.end());
}

// trim from end (in place)
constexpr std::string_view rtrim(std::string_view const s) {
  return std::string_view(
      s.begin(), std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
                   return !std::isspace(ch);
                 }).base());
}

// trim from both ends (in place)
constexpr std::string_view trim(std::string_view const s) {
  return ltrim(rtrim(s));
}

constexpr bool beginswith(std::string_view const target,
                          std::string_view const begin) {
  return target.rfind(begin, 0) != std::string::npos;
}
constexpr bool endswith(std::string_view const target,
                        std::string_view const end) {
  if (target.size() > end.size()) {
    return false;
  }
  return target.find(end, target.size() - end.size()) != std::string::npos;
}
constexpr bool contains(std::string_view const target,
                        std::string_view const contains) {
  return target.find(contains) != std::string::npos;
}
// remove all characters before first c
constexpr std::string_view lskip(std::string_view const s, char c) {
  size_t t = s.find(c);
  if (t == std::string::npos) {
    return s.substr(0, 0);
  }
  return s.substr(t + 1);
}
// get all characters before first c
constexpr std::string_view lget(std::string_view const s, char c) {
  size_t t = s.find(c);
  if (t == std::string::npos) {
    return s.substr(0, 0);
  }
  return s.substr(0, t);
}
std::unique_ptr<Entry> ParseLine(std::string_view input_line) {
  bool flag = false;
  auto end_it = input_line.end();
  for (auto it = input_line.begin(); it != input_line.end(); ++it) {
    if (*it == '\"' && (it != input_line.begin() || *(it - 1) != '\\')) {
      flag = !flag;
      continue;
    }
    if (!flag && *it == '#') {
      end_it = it;
    }
  }
  auto line = std::string(input_line.begin(), end_it);
  line = trim(line);

  char* end = nullptr;
  long long ll = std::strtoll(line.data(), &end, 10);

  if (errno == ERANGE) {
    errno = 0;
  } else if (end == line.data() + line.size()) {
    return std::make_unique<Entry>((uint64_t)ll);
  }
  unsigned long long ull = std::strtoull(line.data(), &end, 10);

  if (errno == ERANGE) {
    errno = 0;
  } else if (end == line.data() + line.size()) {
    return std::make_unique<Entry>((uint64_t)ull);
  }
  long double ld = std::strtold(line.data(), &end);

  if (errno == ERANGE) {
    errno = 0;
  } else if (end == line.data() + line.size()) {
    return std::make_unique<Entry>((long double)ld);
  }

  return std::make_unique<Entry>(line);
}
std::unique_ptr<Entry> Parse(std::span<std::string_view> const& span);

void remove_empty_strings(std::vector<std::string_view>& vec) {
  std::erase_if(vec, [](std::string_view const s) { return s.empty(); });
}

std::unique_ptr<Entry> ParsePair(std::span<std::string_view> const& span) {
  std::unique_ptr<Entry> key = ParseLine(lget(*span.begin(), ':'));
  std::vector<std::string_view> vec{span.begin(), span.end()};
  vec[0] = lskip(span[0], ':');
  remove_empty_strings(vec);
  std::unique_ptr<Entry> value = Parse(vec);

  return std::make_unique<Entry>(std::move(key), std::move(value), nullptr);
}

std::unique_ptr<Entry> ParseMap(std::span<std::string_view> const& span) {
  auto return_value = std::make_unique<Entry>(Type::kMap);
  auto map_vec = std::vector<std::string_view>(span.begin(), span.end());
  remove_empty_strings(map_vec);

  if (beginswith(ltrim(map_vec[0]), "{")) {
    map_vec[0] = map_vec[0].substr(map_vec[0].find('{') + 1);

    auto rit = map_vec.rbegin();
    bool flag = false;
    while (rit != map_vec.rend()) {
      if (auto k = rit->rfind("}"); k != std::string::npos) {
        flag = true;
        *rit = std::string_view(rit->begin(), rit->begin() + k);
        break;
      }
      ++rit;
    }
    if (!flag) {
      throw yaml::InvalidSyntax("invalid syntax exception");
    }
    remove_empty_strings(map_vec);
    auto it = map_vec.begin();
    size_t prev_comma = 0;
    auto prev_comma_it = map_vec.begin();
    while (it != map_vec.end()) {
      size_t buf = std::string::npos;
      do {
        size_t comma = it->find(',', buf + 1);

        if (comma == std::string::npos) {
          break;
        }
        buf = buf + 1;

        std::vector<std::string_view> temp{};

        while (prev_comma_it != it) {
          temp.push_back((prev_comma_it++)->substr(prev_comma));
          prev_comma = 0;
        }
        temp.push_back(it->substr(buf, comma - buf));

        return_value->append(ParsePair(temp));
        buf = comma;

      } while (true);

      if (buf != std::string::npos) {
        prev_comma = buf;
        prev_comma_it = it;
      }
      ++it;
    }

    std::vector<std::string_view> temp{};

    do {
      if (auto t = prev_comma_it->substr(prev_comma + 1); !t.empty()) {
        temp.push_back(t);
      }
      prev_comma = 0;
      ++prev_comma_it;
    } while (prev_comma_it != map_vec.end());
    if (!temp.empty()) {
      return_value->append(ParsePair(temp));
    }
    return return_value;
  }

  auto begin = map_vec.begin();

  do {
    auto it = begin;
    if (beginswith(*it, "?")) {
      ++it;
      while (!beginswith(*it, ":")) ++it;
      std::span<std::string_view> vec{begin, it};
      for (std::string_view& line : vec) {
        line = line.substr(2);
      }
      std::unique_ptr<Entry> key = Parse(vec);
      begin = it;
      ++it;
      while (it != map_vec.end() && beginswith(*it, "  ")) ++it;
      vec = std::span<std::string_view>{begin, it};
      for (std::string_view& line : vec) {
        line = line.substr(2);
      }
      std::unique_ptr<Entry> value = Parse(vec);

      return_value->append(std::make_unique<Entry>(
          std::move(key), std::move(value), return_value.get()));
    } else if (it->size() >= 2 && (*it)[1] != ' ' && contains(*it, ":")) {
      ++it;
      while (it != map_vec.end() && (!contains(*it, ":") || (*it)[1] == ' '))
        ++it;
      std::span<std::string_view> vec{begin, it};
      return_value->append(ParsePair(vec));
    } else {
      ++it;
    }
    begin = it;
  } while (begin != map_vec.end());
  return return_value;
}
std::unique_ptr<Entry> ParseSet(std::span<std::string_view> const& span) {
  auto return_value = std::make_unique<Entry>(Type::kMap);

  auto begin = span.begin();
  do {
    auto it = begin + 1;
    while (!beginswith(*it, "?")) ++it;
    std::vector<std::string_view> vec{begin, it};
    for (std::string_view& line : vec) {
      if (line.size() > 1) {
        line = line.substr(2);
      } else {
        line = "";
      }
    }
    std::unique_ptr<Entry> key = Parse(vec);
    return_value->append(std::make_unique<Entry>(
        std::move(key), std::make_unique<Entry>(Type::kNull),
        return_value.get()));
    begin = it;
  } while (begin != span.end());
  return return_value;
}
std::unique_ptr<Entry> ParseSequence(std::span<std::string_view> const& span) {
  auto return_value = std::make_unique<Entry>(Type::kSequence);
  auto seq_vec = std::vector<std::string_view>(span.begin(), span.end());
  remove_empty_strings(seq_vec);

  if (beginswith(seq_vec[0], "[")) {
    seq_vec[0] = seq_vec[0].substr(1);

    auto rit = seq_vec.rbegin();
    // while true because if no closing square bracket is found then
    // the ++rit will throw an exception
    while (true) {
      if (auto k = rit->rfind("]"); k != std::string::npos) {
        *rit = std::string_view(rit->begin(), rit->begin() + k);
        break;
      }
      ++rit;
    }
    remove_empty_strings(seq_vec);
    auto it = seq_vec.begin();
    size_t prev_comma = std::string::npos;
    auto prev_comma_it = seq_vec.begin();
    while (it != seq_vec.end()) {
      size_t buf = 0;
      do {
        size_t comma = it->find(',', buf);

        if (comma == std::string::npos) {
          break;
        }

        std::vector<std::string_view> temp{};

        while (prev_comma_it != it) {
          temp.push_back((prev_comma_it++)->substr(prev_comma));
          prev_comma = 0;
        }
        temp.push_back(it->substr(buf, comma - buf));

        return_value->append(Parse(temp));
        buf = comma + 1;

      } while (true);

      if (buf != 0) {
        prev_comma = buf == std::string::npos ? 0 : buf;
        prev_comma_it = it;
      }
      ++it;
    }

    std::vector<std::string_view> temp{};

    do {
      if (auto t = prev_comma_it->substr(prev_comma); !t.empty()) {
        temp.push_back(t);
      }
      prev_comma = 0;
      ++prev_comma_it;
    } while (prev_comma_it != seq_vec.end());
    if (!temp.empty()) {
      return_value->append(Parse(temp));
    }
    return return_value;
  }

  auto begin = seq_vec.begin();
  do {
    auto it = begin + 1;
    while (it != seq_vec.end() && !beginswith(*it, "-")) ++it;
    std::span<std::string_view> vec{begin, it};
    for (std::string_view& line : vec) {
      if (line.size() > 1) {
        line = line.substr(2);
      } else {
        line = "";
      }
    }
    std::unique_ptr<Entry> value = Parse(vec);
    return_value->append(std::move(value));
    begin = it;
  } while (begin != seq_vec.end());
  return return_value;
}
std::unique_ptr<Entry> Parse(std::span<std::string_view> const& span) {
  auto it = span.begin();
  auto type = Type::kNull;
  while (it != span.end() && type == Type::kNull) {
    if (it->size() < 1) {
      ++it;
      continue;
    }
    if (beginswith(*it, "?")) {
      auto itr = std::find_if(
          span.begin(), span.end(),
          [](std::string_view const str) { return beginswith(str, ":"); });
      type = (itr == span.end()) ? Type::kSet : Type::kMap;
    } else if (it->size() >= 2 && (*it)[1] != ' ' && contains(*it, ":")) {
      type = Type::kMap;
    } else if (beginswith(*it, "-")) {
      type = Type::kSequence;
    } else if (beginswith(ltrim(*it), "[")) {
      type = Type::kSequence;
    } else if (beginswith(ltrim(*it), "{")) {
      type = Type::kMap;
    }
    ++it;
  }
  std::unique_ptr<Entry> return_value;
  if (span.empty()) {
    throw yaml::InvalidSyntax("The syntax of the provided string is invalid!");
  }

  if (type == Type::kMap) {
    return_value = ParseMap(span);
  } else if (type == Type::kSet) {
    return_value = ParseSet(span);
  } else if (type == Type::kSequence) {
    return_value = ParseSequence(span);
  } else if (span[0].empty()) {
    throw yaml::InvalidSyntax("The syntax of the provided string is invalid!");
  } else if (span.size() == 1) {
    return_value = ParseLine(span[0]);
  } else if (ltrim(span[0])[0] == '|') {
    std::string t{};
    for (auto i = span.begin() + 1; i != span.end(); i++) {
      t += *i;
      t += '\n';
    }
    return_value = std::make_unique<Entry>(t);
  } else if (ltrim(span[0])[0] == '>') {
    std::string t{};
    for (auto i = span.begin() + 1; i != span.end(); i++) {
      t += *i;
      t += ' ';
    }
    return_value = std::make_unique<Entry>(t);
  } else {
    std::string t{};
    for (auto i = span.begin() + 1; i != span.end(); i++) {
      t += *i;
      t += ' ';
    }
    return_value = std::make_unique<Entry>(t);
  }
  return return_value;
}
Entry Parse(std::string_view const string) {
  std::vector<std::string_view> lines;

  auto i = (size_t)(-1);
  do {
    size_t j = string.find("\n", i + 1);
    size_t len = std::min(j - i - 1, string.size() - i - 1);
    lines.emplace_back(string.substr(i + 1, len));
    i = j;
  } while (i != std::string::npos);

  std::unique_ptr<Entry> t = Parse(lines);
  return Entry(std::move(*t.get()));
}
std::optional<Entry> ParseNoexcept(std::string_view const string) noexcept {
  try {
    return std::optional<Entry>{Parse(string)};
  } catch (...) {
  }
  return std::nullopt;
}
}  // namespace yaml
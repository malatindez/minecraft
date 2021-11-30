#include "Yaml.hpp"
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
Entry::Entry(Entry& entry, Entry* parent) noexcept : parent_(entry.parent_) {
  operator=(entry);
  if (parent != nullptr) {
    parent_ = parent;
  }
}
Entry::Entry(std::unique_ptr<Entry> key, std::unique_ptr<Entry> value,
             Entry* parent) noexcept
    : parent_(parent) {
  type_ = Entry::Type::kPair;
  key->parent_ = this;
  value->parent_ = this;
  data_ = std::make_unique<Pair>(std::move(key), std::move(value));
  str_ = "";
}
Entry::Entry(std::string_view const& other, Entry* parent) noexcept
    : parent_(parent) {
  operator=(other);
}
Entry::Entry(int64_t const& other, Entry* parent) noexcept : parent_(parent) {
  operator=(other);
}
Entry::Entry(int32_t const& other, Entry* parent) noexcept : parent_(parent) {
  operator=(other);
}
Entry::Entry(int16_t const& other, Entry* parent) noexcept : parent_(parent) {
  operator=(other);
}
Entry::Entry(uint64_t const& other, Entry* parent) noexcept : parent_(parent) {
  operator=(other);
}
Entry::Entry(uint32_t const& other, Entry* parent) noexcept : parent_(parent) {
  operator=(other);
}
Entry::Entry(uint16_t const& other, Entry* parent) noexcept : parent_(parent) {
  operator=(other);
}
Entry::Entry(long double const& other, Entry* parent) noexcept
    : parent_(parent) {
  operator=(other);
}
Entry::Entry(double const& other, Entry* parent) noexcept : parent_(parent) {
  operator=(other);
}
Entry::Entry(float const& other, Entry* parent) noexcept : parent_(parent) {
  operator=(other);
}
Entry::Entry(std::tm const& other, Entry* parent) noexcept : parent_(parent) {
  operator=(other);
}
Entry::Entry(std::chrono::year_month_day const& other, Entry* parent) noexcept
    : parent_(parent) {
  operator=(other);
}
Entry::Entry(std::chrono::hh_mm_ss<std::chrono::microseconds> const& other,
             Entry* parent) noexcept
    : parent_(parent) {
  operator=(other);
}
bool Entry::operator==(Entry const& other) const noexcept {
  return other.type_ == type_ && str_ == other.str_;
}
bool Entry::operator==(std::string_view const& other) const noexcept {
  return is_string() && str_ == other;
}
bool Entry::operator==(int64_t const& other) const noexcept {
  return is_int() && to_int() == other;
}
bool Entry::operator==(int32_t const& other) const noexcept {
  return is_int() && to_int() == other;
}
bool Entry::operator==(int16_t const& other) const noexcept {
  return is_int() && to_int() == other;
}
bool Entry::operator==(uint64_t const& other) const noexcept {
  return is_int() && to_uint() == other;
}
bool Entry::operator==(uint32_t const& other) const noexcept {
  return is_int() && to_uint() == other;
}
bool Entry::operator==(uint16_t const& other) const noexcept {
  return is_int() && to_uint() == other;
}
bool Entry::operator==(long double const& other) const noexcept {
  return is_double() && to_double() == other;
}
bool Entry::operator==(double const& other) const noexcept {
  return is_double() && to_double() == other;
}
bool Entry::operator==(float const& other) const noexcept {
  return is_double() && to_double() == other;
}
bool Entry::operator==(std::tm const& other) const noexcept {
  auto t = to_datetime();
  return is_timestamp() && t.tm_year == other.tm_year &&
         t.tm_wday == other.tm_wday && t.tm_yday == other.tm_yday &&
         t.tm_mon == other.tm_mon && t.tm_mday == other.tm_mday &&
         t.tm_hour == other.tm_hour && t.tm_min == other.tm_min &&
         t.tm_sec == other.tm_sec;
}
bool Entry::operator==(
    std::chrono::year_month_day const& other) const noexcept {
  return is_date() && to_date() == other;
}
bool Entry::operator==(std::chrono::hh_mm_ss<std::chrono::microseconds> const&
                           other) const noexcept {
  return is_time() && to_time().to_duration() == other.to_duration();
}
Entry& Entry::operator[](std::string_view const& key) {
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
  data_ = std::make_unique<Link>(&entry);
  str_ = "";
  tag_ = "";  // TODO link name generation
  return *this;
}

Entry& Entry::operator=(std::string_view const& other) noexcept {
  entries_.clear();
  type_ = Entry::Type::kString;
  str_ = other;
  tag_ = "";
  return *this;
}
Entry& Entry::operator=(bool const& other) noexcept {
  entries_.clear();
  type_ = Entry::Type::kBool;
  str_ = other ? "true" : "false";
  data_ = std::make_unique<Boolean>(other);
  tag_ = "";
  return *this;
}
Entry& Entry::operator=(int64_t const& other) noexcept {
  entries_.clear();
  type_ = Entry::Type::kInt;
  str_ = std::to_string(other);
  data_ = std::make_unique<Integer>(other);
  tag_ = "";
  return *this;
}
Entry& Entry::operator=(int32_t const& other) noexcept {
  entries_.clear();
  type_ = Entry::Type::kInt;
  str_ = std::to_string(other);
  data_ = std::make_unique<Integer>(other);
  tag_ = "";
  return *this;
}
Entry& Entry::operator=(int16_t const& other) noexcept {
  entries_.clear();
  type_ = Entry::Type::kInt;
  str_ = std::to_string(other);
  data_ = std::make_unique<Integer>(other);
  tag_ = "";
  return *this;
}
Entry& Entry::operator=(uint64_t const& other) noexcept {
  entries_.clear();
  type_ = Entry::Type::kUInt;
  str_ = std::to_string(other);
  data_ = std::make_unique<UnsignedInteger>(other);
  tag_ = "";
  return *this;
}
Entry& Entry::operator=(uint32_t const& other) noexcept {
  entries_.clear();
  type_ = Entry::Type::kUInt;
  str_ = std::to_string(other);
  data_ = std::make_unique<UnsignedInteger>(other);
  tag_ = "";
  return *this;
}
Entry& Entry::operator=(uint16_t const& other) noexcept {
  entries_.clear();
  type_ = Entry::Type::kUInt;
  str_ = std::to_string(other);
  data_ = std::make_unique<UnsignedInteger>(other);
  tag_ = "";
  return *this;
}
Entry& Entry::operator=(long double const& other) noexcept {
  entries_.clear();
  type_ = Entry::Type::kDouble;
  str_ = std::to_string(other);
  data_ = std::make_unique<Double>(other);
  tag_ = "";
  return *this;
}
Entry& Entry::operator=(double const& other) noexcept {
  entries_.clear();
  type_ = Entry::Type::kDouble;
  str_ = std::to_string(other);
  data_ = std::make_unique<Double>(other);
  tag_ = "";
  return *this;
}
Entry& Entry::operator=(float const& other) noexcept {
  entries_.clear();
  type_ = Entry::Type::kDouble;
  str_ = std::to_string(other);
  data_ = std::make_unique<Double>(other);
  tag_ = "";
  return *this;
}
std::string format_time(std::tm const& other) {
  std::ostringstream buf;
  buf << other.tm_year << "-" << std::setfill('0') << std::setw(2)
      << other.tm_mon << "-" << std::setw(2) << other.tm_mday << " "
      << std::setw(2) << other.tm_hour << ":" << std::setw(2) << other.tm_min
      << ":" << std::setw(2) << other.tm_sec;
  return buf.str();
}
std::string format_time(std::chrono::year_month_day const& other) {
  std::ostringstream buf;
  buf << (int32_t)other.year() << "-" << std::setfill('0') << std::setw(2)
      << (uint32_t)other.month() << "-" << std::setw(2)
      << (uint32_t)other.day();
  return buf.str();
}
std::string format_time(
    std::chrono::hh_mm_ss<std::chrono::microseconds> const& other) {
  std::ostringstream buf;
  buf << std::setfill('0') << std::setw(2) << (uint32_t)other.hours().count()
      << ":" << std::setw(2) << (uint32_t)other.minutes().count() << ":"
      << std::setw(2) << (uint32_t)other.seconds().count();
  return buf.str();
}
Entry& Entry::operator=(std::tm const& other) noexcept {
  entries_.clear();
  type_ = Entry::Type::kTimestamp;

  str_ = format_time(other);
  data_ = std::make_unique<TimePoint>(other);
  tag_ = "";
  return *this;
}
Entry& Entry::operator=(std::chrono::year_month_day const& other) noexcept {
  if (is_pair()) {
    value() = other;
    return *this;
  }
  entries_.clear();
  type_ = Entry::Type::kDate;
  str_ = format_time(other);
  data_ = std::make_unique<TimePoint>(other);
  tag_ = "";
  return *this;
}
Entry& Entry::operator=(
    std::chrono::hh_mm_ss<std::chrono::microseconds> const& other) noexcept {
  if (is_pair()) {
    value() = other;
    return *this;
  }
  entries_.clear();
  type_ = Entry::Type::kDate;
  str_ = format_time(other);
  data_ = std::make_unique<TimePoint>(other);
  tag_ = "";
  return *this;
}

Entry& Entry::key() const {
  if (!is_pair()) {
    throw std::invalid_argument("This entry is not a pair");
  }
  return *static_cast<Pair*>(data_.get())->key_;
}

Entry& Entry::value() const {
  if (!is_pair()) {
    throw std::invalid_argument("This entry is not a pair");
  }
  return *static_cast<Pair*>(data_.get())->value_;
}

inline std::chrono::hh_mm_ss<std::chrono::microseconds> Entry::to_time() const {
  if (!is_time() && !is_timestamp()) {
    throw std::invalid_argument("This entry is not a time");
  }
  return static_cast<TimePoint*>(data_.get())->time_;
}
inline std::chrono::year_month_day Entry::to_date() const {
  if (!is_date() && !is_timestamp()) {
    throw std::invalid_argument("This entry is not date");
  }
  return static_cast<TimePoint*>(data_.get())->date_;
}
inline std::tm Entry::to_datetime() const {
  if (!is_timestamp()) {
    throw std::invalid_argument("This entry is not a timestamp");
  }
  return static_cast<TimePoint*>(data_.get())->datetime_;
}
inline std::tm Entry::to_time_point() const {
  if (!is_timestamp()) {
    throw std::invalid_argument("This entry is not a timestamp");
  }
  return static_cast<TimePoint*>(data_.get())->datetime_;
}

inline long double Entry::to_double() const {
  if (!is_double()) {
    throw std::invalid_argument("This entry is not a double");
  }
  return static_cast<Double*>(data_.get())->double_;
}
inline long long int Entry::to_int() const {
  if (!is_int()) {
    throw std::invalid_argument("This entry is not an integer");
  }
  return static_cast<Integer*>(data_.get())->integer_;
}
inline uint64_t Entry::to_uint() const {
  if (!is_uint()) {
    throw std::invalid_argument("This entry is not an unsigned integer");
  }
  return static_cast<UnsignedInteger*>(data_.get())->unsigned_integer_;
}

inline bool Entry::to_bool() const {
  if (!is_bool()) {
    throw std::invalid_argument("This entry is not a boolean");
  }
  return static_cast<Boolean*>(data_.get())->boolean_;
}
inline bool Entry::to_boolean() const {
  if (!is_bool()) {
    throw std::invalid_argument("This entry is not a boolean");
  }
  return static_cast<Boolean*>(data_.get())->boolean_;
}
inline std::string_view Entry::to_string() const noexcept { return str_; }

void Entry::append(Entry&& entry) {
  if (is_sequence()) {
    entries_.emplace_back(std::make_unique<Entry>(std::move(entry), this));
    return;
  }
  if (is_map()) {
    if (entry.is_pair()) {
      entries_.emplace_back(std::make_unique<Entry>(std::move(entry), this));
    }
    throw std::invalid_argument(
        "This entry is a map, but the argument is not a pair");
    return;
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
  if (entry.key().is_simple_type() && entry.value().is_simple_type()) {
    return_value.emplace_back(entry.key().Serialize()[0] + ": " +
                              entry.value().Serialize()[0]);
  } else if (entry.key().is_simple_type()) {
    auto t = entry.key().Serialize()[0] + ": ";
    return_value.emplace_back(std::move(t));
    auto y = entry.value().Serialize();
    for (std::string& str : y) {
      str.insert(0, "  ");
    }
    for (std::string& str : y) {
      return_value.emplace_back(std::move(str));
    }
  } else {
    auto t = entry.key().Serialize();
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
  } else if (is_time()) {
    return_value = std::vector<std::string>{std::string(to_string())};
  } else if (is_timestamp()) {
    return_value = std::vector<std::string>{std::string(to_string())};
  } else if (is_date()) {
    return_value = std::vector<std::string>{std::string(to_string())};
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
constexpr bool beginswith(std::string_view const& target, std::string_view const& begin) {
    return target.compare(begin) == 0;
}
constexpr bool contains(std::string_view const& target, std::string_view const& contains) {
    return target.find(contains) != std::string::npos;
}
Entry Parse(std::string_view const& string) {
    return Entry(Entry::Type::kNull);
}
}  // namespace yaml
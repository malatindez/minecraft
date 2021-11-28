#include "Yaml.hpp"
namespace yaml {

Entry::Entry() noexcept { type_ = Entry::Type::kNull; }
Entry::Entry(std::unique_ptr<Entry> key, std::unique_ptr<Entry> value) {
  type_ = Entry::Type::kPair;
  data_ = std::make_unique<Pair>(std::move(key), std::move(value));
  tag_ = "";
  str_ = Serialize();
}
Entry::Entry(std::string_view const& other) noexcept { operator=(other); }
Entry::Entry(int64_t const& other) noexcept { operator=(other); }
Entry::Entry(int32_t const& other) noexcept { operator=(other); }
Entry::Entry(int16_t const& other) noexcept { operator=(other); }
Entry::Entry(uint64_t const& other) noexcept { operator=(other); }
Entry::Entry(uint32_t const& other) noexcept { operator=(other); }
Entry::Entry(uint16_t const& other) noexcept { operator=(other); }
Entry::Entry(long double const& other) noexcept { operator=(other); }
Entry::Entry(double const& other) noexcept { operator=(other); }
Entry::Entry(float const& other) noexcept { operator=(other); }

bool Entry::contains(std::string_view const& string) const {
  if (!is_sequence() && !is_map()) {
    throw std::invalid_argument("This entry is not a sequence nor a map");
  }
  return std::any_of(
      entries_.begin(), entries_.end(), [&string](Entry const& entry) {
        return (entry.is_string() && entry.to_string() == string) ||
               (entry.is_pair() && entry.key().is_string() &&
                entry.key().to_string() == string);
      });
}
bool Entry::contains(int64_t integer) const {
  if (!is_sequence()) {
    throw std::invalid_argument("This entry is not a sequence");
  }
  for (Entry const& entry : entries_) {
    if (entry == integer) {
      return true;
    }
  }
  return false;
}
bool Entry::contains(int32_t integer) const {
  if (!is_sequence()) {
    throw std::invalid_argument("This entry is not a sequence");
  }
  return std::any_of(
      entries_.begin(), entries_.end(),
      [&integer](Entry const& entry) { return entry == integer; });
}
bool Entry::contains(int16_t integer) const {
  if (!is_sequence()) {
    throw std::invalid_argument("This entry is not a sequence");
  }
  return std::any_of(
      entries_.begin(), entries_.end(),
      [&integer](Entry const& entry) { return entry == integer; });
}
bool Entry::contains(uint64_t integer) const {
  if (!is_sequence()) {
    throw std::invalid_argument("This entry is not a sequence");
  }
  return std::any_of(
      entries_.begin(), entries_.end(),
      [&integer](Entry const& entry) { return entry == integer; });
}
bool Entry::contains(uint32_t integer) const {
  if (!is_sequence()) {
    throw std::invalid_argument("This entry is not a sequence");
  }
  return std::any_of(
      entries_.begin(), entries_.end(),
      [&integer](Entry const& entry) { return entry == integer; });
}
bool Entry::contains(uint16_t integer) const {
  if (!is_sequence()) {
    throw std::invalid_argument("This entry is not a sequence");
  }
  return std::any_of(
      entries_.begin(), entries_.end(),
      [&integer](Entry const& entry) { return entry == integer; });
}
bool Entry::contains(long double real) const {
  if (!is_sequence()) {
    throw std::invalid_argument("This entry is not a sequence");
  }
  return std::any_of(entries_.begin(), entries_.end(),
                     [&real](Entry const& entry) { return entry == real; });
}
bool Entry::contains(double real) const {
  if (!is_sequence()) {
    throw std::invalid_argument("This entry is not a sequence");
  }
  return std::any_of(entries_.begin(), entries_.end(),
                     [&real](Entry const& entry) { return entry == real; });
}
bool Entry::contains(float real) const {
  if (!is_sequence()) {
    throw std::invalid_argument("This entry is not a sequence");
  }
  return std::any_of(entries_.begin(), entries_.end(),
                     [&real](Entry const& entry) { return entry == real; });
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
Entry& Entry::operator[](std::string_view const& key) {
  if (!is_map()) {
    throw std::invalid_argument("This entry is not a map");
  }
  auto it = std::find_if(entries_.begin(), entries_.end(),
                         [&key](Entry const& entry) {
                           return entry.is_pair() && entry.key().is_string() &&
                                  entry.key().to_string() == key;
                         });
  if (it == entries_.end()) {
    throw std::invalid_argument("The key is not valid");
  }
  return it->value();
}
Entry& Entry::operator[](size_t const& i) {
  if (!is_map() && !is_sequence()) {
    throw std::invalid_argument("This entry is not a map nor a sequence");
  }
  if (entries_.size() < i) {
    throw std::invalid_argument("The key is not valid");
  }
  return entries_[i];
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
// Accepts only values that are convertible by std::string
template <typename T>
Entry& Entry::operator=(std::vector<T> const& other) noexcept {
  entries_.clear();
  type_ = Entry::Type::kSequence;
  for (T const& t : other) {
    entries_.emplace_back(Entry(t));
  }
  tag_ = "";
  str_ = Serialize();
  return *this;
}
// Accepts only values that are convertible by std::string
template <typename T1, typename T2>
Entry& Entry::operator=(std::map<T1, T2> const& other) noexcept {
  entries_.clear();
  type_ = Entry::Type::kMap;
  for (auto const& [t1, t2] : other) {
    // Call an Entry() with two entries as parameters
    entries_.emplace_back(Entry(t1), Entry(t2));
  }
  tag_ = "";
  str_ = Serialize();
  return *this;
}
// Accepts only values that are convertible by std::string
template <typename T>
Entry& Entry::operator=(std::set<T> const& other) noexcept {
  entries_.clear();
  type_ = Entry::Type::kSet;
  for (T const& t : other) {
    // Call an Entry() with two entries as parameters, the value is set to be
    // Null
    entries_.emplace_back(Entry(t), Entry());
  }
  tag_ = "set";
  str_ = Serialize();
  return *this;
}

Entry& Entry::key() const {
  if (!is_pair()) {
    throw std::invalid_argument("This entry is not a pair");
  }
  return *reinterpret_cast<const Pair*>(data_.get())->key_;
}

Entry& Entry::value() const {
  if (!is_pair()) {
    throw std::invalid_argument("This entry is not a pair");
  }
  return *reinterpret_cast<const Pair*>(data_.get())->value_;
}

inline std::chrono::hh_mm_ss<std::chrono::microseconds> Entry::to_time() const {
  if (!is_time() && !is_timestamp()) {
    throw std::invalid_argument("This entry is not a time");
  }
  return reinterpret_cast<const TimePoint*>(data_.get())->time_;
}
inline std::chrono::year_month_day Entry::to_date() const {
  if (!is_date() && !is_timestamp()) {
    throw std::invalid_argument("This entry is not date");
  }
  return reinterpret_cast<const TimePoint*>(data_.get())->date_;
}
inline std::chrono::time_point<std::chrono::microseconds> Entry::to_datetime()
    const {
  if (!is_timestamp()) {
    throw std::invalid_argument("This entry is not a timestamp");
  }
  return reinterpret_cast<const TimePoint*>(data_.get())->datetime_;
}
inline std::chrono::time_point<std::chrono::microseconds> Entry::to_time_point()
    const {
  if (!is_timestamp()) {
    throw std::invalid_argument("This entry is not a timestamp");
  }
  return reinterpret_cast<const TimePoint*>(data_.get())->datetime_;
}

inline long double Entry::to_double() const {
  if (!is_double()) {
    throw std::invalid_argument("This entry is not a double");
  }
  return reinterpret_cast<const Double*>(data_.get())->double_;
}
inline long long int Entry::to_int() const {
  if (!is_int()) {
    throw std::invalid_argument("This entry is not an integer");
  }
  return reinterpret_cast<const Integer*>(data_.get())->integer_;
}
inline uint64_t Entry::to_uint() const {
  if (!is_uint()) {
    throw std::invalid_argument("This entry is not an unsigned integer");
  }
  return reinterpret_cast<const UnsignedInteger*>(data_.get())
      ->unsigned_integer_;
}

inline bool Entry::to_bool() const {
  if (!is_bool()) {
    throw std::invalid_argument("This entry is not a boolean");
  }
  return reinterpret_cast<const Boolean*>(data_.get())->boolean_;
}
inline bool Entry::to_boolean() const {
  if (!is_bool()) {
    throw std::invalid_argument("This entry is not a boolean");
  }
  return reinterpret_cast<const Boolean*>(data_.get())->boolean_;
}
inline std::string_view Entry::to_string() const noexcept { return str_; }

Entry Parse(std::string_view const& string) { return Entry(); }
}  // namespace yaml
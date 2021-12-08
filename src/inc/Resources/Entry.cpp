#include "Entry.hpp"

namespace resource {
[[nodiscard]] bool Entry::FileExists(
    std::string_view const key) const noexcept {
  auto opt = GetIfExists(key);
  return opt.has_value() && opt.value().get().is_file();
}
[[nodiscard]] bool Entry::DirectoryExists(
    std::string_view const key) const noexcept {
  auto opt = GetIfExists(key);
  return opt.has_value() && opt.value().get().is_dir();
}

[[nodiscard]] bool Entry::Exists(std::string_view const key) const noexcept {
  return GetIfExists(key).has_value();
}

[[nodiscard]] Entry const& Entry::Get(std::string_view const key) const {
  return operator[](key);
}
[[nodiscard]] Entry const& Entry::GetFile(std::string_view const key) const {
  Entry const& t = operator[](key);
  if (t.is_directory()) {
    throw std::invalid_argument("The value behind the key is a directory!");
  }
  return t;
}
[[nodiscard]] Entry const& Entry::GetDirectory(
    std::string_view const key) const {
  Entry const& t = operator[](key);
  if (t.is_file()) {
    throw std::invalid_argument("The value behind the key is a file!");
  }
  return t;
}

[[nodiscard]] Entry const& Entry::operator/(std::string_view const key) const {
  return operator[](key);
}

[[nodiscard]] Entry const& Entry::operator[](std::string_view const key) const {
  auto opt = GetIfExists(key);
  if (!opt.has_value()) {
    throw resource::InvalidPathException("Invalid path specified: " +
                                         std::string(key));
  }
  return opt.value();
}
[[nodiscard]] static constexpr std::pair<std::string_view, std::string_view>
lcrop_path(std::string_view const& path) noexcept {
  auto it = std::find_if(path.begin(), path.end(),
                         [&](char const& c) { return c == '/' || c == '\\'; });
  auto it2 = it;
  if (it != path.end()) {
    it2++;
  }
  return std::make_pair<std::string_view, std::string_view>(
      std::string_view(path.begin(), it), std::string_view(it2, path.end()));
}

[[nodiscard]] std::optional<std::reference_wrapper<const Entry>>
Entry::GetIfExists(std::string_view const key) const noexcept {
  using return_value_t = std::optional<std::reference_wrapper<const Entry>>;
  auto [left, right] = lcrop_path(key);
  if (entry_map_.contains(left)) {
    if (right.empty()) {
      return return_value_t{entry_map_.at(left)};
    }
    return entry_map_.at(left).get().GetIfExists(right);
  }

  return return_value_t{};
}

// returns nullptr if the entry is a folder
[[nodiscard]] std::shared_ptr<std::vector<char>> Entry::data() const noexcept {
  if (is_dir()) {
    return nullptr;
  }
  return data_internal();
}

// returns nullptr if the entry is a folder
[[nodiscard]] std::shared_ptr<std::vector<char>> Entry::content()
    const noexcept {
  return data();
}

[[nodiscard]] std::string Entry::ToString() const noexcept {
  std::shared_ptr<std::vector<char>> data = this->data();
  std::string return_value;
  return_value.resize(data->size());
  std::transform(data->begin(), data->end(), return_value.begin(),
                 [](char b) { return char(b); });
  return std::string(return_value);
}
[[nodiscard]] std::string Entry::string() const noexcept { return ToString(); }

[[nodiscard]] std::shared_ptr<std::vector<char>> Entry::data_internal()
    const noexcept {
  if (data_->expired()) {
    if (auto lock = resource_file_ptr_.TryLock()) {
      lock->seekg(data_begin_);
      auto file_content = std::make_shared<std::vector<char>>(size_);
      lock->read(file_content->data(), size_);
      (*data_) = file_content;
      return file_content;
    }
    // wait until data_ is initialized in another thread
    { auto lock = resource_file_ptr_.Lock(); }
    return data();
  }
  return data_->lock();
}

static constexpr uint64_t BytesToUint64(const char* sbuf) {
  auto buf = std::bit_cast<const unsigned char*>(sbuf);
  return (uint64_t(buf[0]) << 0) | (uint64_t(buf[1]) << 8) |
         (uint64_t(buf[2]) << 16) | (uint64_t(buf[3]) << 24) |
         (uint64_t(buf[4]) << 32) | (uint64_t(buf[5]) << 40) |
         (uint64_t(buf[6]) << 48) | (uint64_t(buf[7]) << 56);
}

uint64_t Entry::CalculateDirSize() const noexcept {
  auto dl = [](uint64_t a, std::unique_ptr<Entry> const& b) {
    return a + b->size();
  };
  return std::accumulate(entries_->begin(), entries_->end(), (uint64_t)0, dl);
  return 0;
}

Entry::Entry(uint64_t begin, AtomicIfstreamPointer const& resource_file_ptr,
             bool is_file)
    : data_begin_(begin),
      resource_file_ptr_(resource_file_ptr),
      is_file_(is_file) {
  entries_ = std::make_shared<std::vector<std::unique_ptr<Entry>>>();
  data_ = std::make_shared<std::weak_ptr<std::vector<char>>>();
  // lock the resource file
  {
    auto lock = resource_file_ptr_.Lock();
    lock->seekg(data_begin_);
    // retrieve the next entry location & the size of the filename in bytes
    std::vector<char> buf(sizeof(uint64_t) + sizeof(uint16_t), char(0));
    lock->read(buf.data(), sizeof(uint64_t) + sizeof(uint16_t));
    uint64_t nextfile_location = BytesToUint64(buf.data());

    uint16_t name_size = (uint16_t(buf[8]) << 0) | (uint16_t(buf[9]) << 8);

    name_.resize((size_t)name_size);

    name_.data()[name_size] = '\0';
    lock->read(name_.data(), name_size * sizeof(char));

    data_begin_ = lock->tellg();
    size_ = nextfile_location - data_begin_;
  }
  if (is_file) {
    return;
  }
  // if the entry is a folder, continue parsing

  auto data_ptr = data_internal();
  auto data = data_ptr->data();
  std::vector<uint64_t> resources(size() / 4U);

  // Process files
  bool flag = true;
  for (auto buf = data; buf < data + size_; buf += sizeof(uint64_t)) {
    uint64_t file_begin = BytesToUint64(buf);
    if (flag) {
      flag = file_begin != 0;
      if (!flag) {
        continue;
      }
    }
    auto value =
        std::unique_ptr<Entry>(new Entry(file_begin, resource_file_ptr, flag));
    std::string_view key{value->name_};
    if (flag) {
      files_.push_back(std::reference_wrapper<const Entry>(*value));
    } else {
      directories_.push_back(std::reference_wrapper<const Entry>(*value));
    }
    entry_map_.try_emplace(key, std::reference_wrapper<const Entry>(*value));
    entries_->emplace_back(std::move(value));
  }
  size_ = CalculateDirSize() + size_;
}
}  // namespace resource
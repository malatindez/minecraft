#include "pack.hpp"

[[nodiscard]] static inline std::vector<char> Uint64ToBytes(
    uint64_t const &integer) {
  auto return_value = std::vector<char>(8);
  return_value[0] = (unsigned char)((integer >> 0x00) & 0xff);
  return_value[1] = (unsigned char)((integer >> 0x08) & 0xff);
  return_value[2] = (unsigned char)((integer >> 0x10) & 0xff);
  return_value[3] = (unsigned char)((integer >> 0x18) & 0xff);
  return_value[4] = (unsigned char)((integer >> 0x20) & 0xff);
  return_value[5] = (unsigned char)((integer >> 0x28) & 0xff);
  return_value[6] = (unsigned char)((integer >> 0x30) & 0xff);
  return_value[7] = (unsigned char)((integer >> 0x38) & 0xff);
  return return_value;
}
[[nodiscard]] static inline std::vector<char> Uint16ToBytes(
    uint16_t const &integer) {
  auto return_value = std::vector<char>(2);
  return_value[0] = (unsigned char)((integer >> 0x00) & 0xff);
  return_value[1] = (unsigned char)((integer >> 0x08) & 0xff);
  return return_value;
}

[[nodiscard]] std::vector<char> PrepareHeader(std::string_view const &name,
                                              uint64_t const &file_begin,
                                              uint64_t const &file_size) {
  if (name.size() > UINT16_MAX) {
    throw std::invalid_argument("name cannot be larger than uint16_t");
  }
  uint32_t header_size = 10 + (uint16_t)name.size();
  std::vector<char> header;
  auto nextfile_location_b =
      Uint64ToBytes(file_begin + header_size + file_size);
  // Write the location of the next file

  header.insert(header.end(), nextfile_location_b.begin(),
                nextfile_location_b.end());
  auto name_size_b = Uint16ToBytes((uint16_t)name.size());
  header.insert(header.end(), name_size_b.begin(), name_size_b.end());
  header.insert(header.end(), name.begin(), name.end());
  return header;
}

static inline uint64_t min(uint64_t const &a, uint64_t const &b) {
  return (a < b) ? a : b;
}

static inline void ReserveBytes(std::ofstream &file, uint64_t amount) {
  static const char nullbyte = 0;
  for (uint64_t i = 0; i < amount; i++) {
    file.write(&nullbyte, 1);
  }
}

static inline void ProcessFile(std::ofstream &output_file,
                               std::filesystem::path const &filepath) {
  auto length = std::filesystem::file_size(filepath);
  auto data =
      PrepareHeader(filepath.filename().string(), output_file.tellp(), length);
  output_file.write(data.data(), data.size());

  const size_t kBlockSize = (uint32_t)min(2UL * 1024 * 1024 * 1024, length);
  std::vector<char> buf(kBlockSize);

  std::basic_ifstream<char> file(filepath, std::ios::binary | std::ios::in);
  for (uint64_t i = 0; i < length; i += kBlockSize) {
    file.read(buf.data(), min(kBlockSize, length - i));
    output_file.write(buf.data(), buf.size());
  }
  file.close();
}

static inline uint64_t ProcessFolder(std::ofstream &output_file,
                                     std::filesystem::path const &dir) {
  uint64_t folder_amount = 0;
  uint64_t file_amount = 0;
  for (auto const &dir_entry : std::filesystem::directory_iterator{dir}) {
    if (dir_entry.is_regular_file()) {
      file_amount++;
    } else if (dir_entry.is_directory()) {
      folder_amount++;
    }
  }

  uint64_t folder_begin = output_file.tellp();
  uint64_t folder_size = (folder_amount + file_amount + 1) * sizeof(uint64_t) +
                         10 + (uint16_t)dir.filename().string().size();
  ReserveBytes(output_file, folder_size);

  std::vector<uint64_t> files;
  std::vector<uint64_t> folders;

  for (auto const &dir_entry : std::filesystem::directory_iterator{dir}) {
    if (dir_entry.is_regular_file()) {
      files.push_back(output_file.tellp());
      ProcessFile(output_file, dir_entry.path());
    } else if (dir_entry.is_directory()) {
      folders.push_back(ProcessFolder(output_file, dir_entry.path()));
    }
  }
  uint64_t folder_end = output_file.tellp();

  files.push_back(0);
  files.insert(files.end(), folders.begin(), folders.end());
  auto data =
      PrepareHeader(dir.filename().string(), folder_begin,
                    (folder_amount + file_amount + 1) * sizeof(uint64_t));
  for (uint64_t const &entry : files) {
    auto buf = Uint64ToBytes(entry);
    data.insert(data.end(), buf.begin(), buf.end());
  }
  if (data.size() != folder_size) {
    throw std::runtime_error("data.size() != folder_size");
  }

  output_file.seekp(folder_begin);
  output_file.write(data.data(), data.size());
  output_file.seekp(folder_end);
  return folder_begin;
}

void resource::packer::Pack(std::vector<std::filesystem::path> &folder_paths,
                            std::filesystem::path const &output_path) {
  namespace fs = std::filesystem;
  std::sort(folder_paths.begin(), folder_paths.end());
  if (std::adjacent_find(folder_paths.begin(), folder_paths.end(),
                         [](fs::path const &first, fs::path const &second) {
                           return first.filename() == second.filename();
                         }) != folder_paths.end()) {
    throw std::invalid_argument(
        "You cannot add folders with the same folder names!");
  }
  std::vector<uint64_t> folders;
  folders.push_back(0);
  auto data =
      PrepareHeader("", 0, (folder_paths.size() + 1) * sizeof(uint64_t));

  std::ofstream output_file(output_path, std::ios::binary | std::ios::out);
  ReserveBytes(output_file,
               data.size() + (folder_paths.size() + 1) * sizeof(uint64_t));
  for (auto const &folder : folder_paths) {
    folders.push_back(ProcessFolder(output_file, fs::path(folder)));
  }
  for (uint64_t const &entry : folders) {
    auto buf = Uint64ToBytes(entry);
    data.insert(data.end(), buf.begin(), buf.end());
  }
  output_file.seekp(0);
  output_file.write(data.data(), data.size());
  output_file.close();
}

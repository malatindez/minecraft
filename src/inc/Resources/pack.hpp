#pragma once
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

/*
 * Define minecraft_RESOURCE_PACKING to enable resource packing to disk
 */

#define minecraft_RESOURCE_PACKING
#ifdef minecraft_RESOURCE_PACKING
namespace resource {
namespace Packer {
[[nodiscard]] static inline std::string rtrim_path(
    std::string_view const& path) {
  auto it = std::find_if(path.begin(), path.end(),
                         [&](char const& c) { return c == '/' || c == '\\'; });
  if (it != path.end()) {
    return std::string(it + 1, path.end());
  }
  return std::string(path);
}

[[nodiscard]] static inline std::vector<std::byte> Uint64ToBytes(
    uint64_t const& integer) {
  auto return_value = std::vector<std::byte>(8);
  return_value[0] = std::byte((integer >> 0x00) & 0xff);
  return_value[1] = std::byte((integer >> 0x08) & 0xff);
  return_value[2] = std::byte((integer >> 0x10) & 0xff);
  return_value[3] = std::byte((integer >> 0x18) & 0xff);
  return_value[4] = std::byte((integer >> 0x20) & 0xff);
  return_value[5] = std::byte((integer >> 0x28) & 0xff);
  return_value[6] = std::byte((integer >> 0x30) & 0xff);
  return_value[7] = std::byte((integer >> 0x38) & 0xff);
  return return_value;
}
[[nodiscard]] static inline std::vector<std::byte> Uint16ToBytes(
    uint16_t const& integer) {
  auto return_value = std::vector<std::byte>(2);
  return_value[0] = std::byte((integer >> 0x00) & 0xff);
  return_value[1] = std::byte((integer >> 0x08) & 0xff);
  return return_value;
}

[[nodiscard]] std::vector<std::byte> PrepareHeader(std::string const& name,
                                                   uint64_t const& file_begin,
                                                   uint64_t const& file_size) {
  if (name.size() > UINT16_MAX) {
    throw std::invalid_argument("name cannot be larger than uint16_t");
  }
  uint32_t header_size = 10 + (uint16_t)name.size();
  std::vector<std::byte> header;
  auto nextfile_location_b =
      Uint64ToBytes(file_begin + header_size + file_size);
  // Write the location of the next file

  header.insert(header.end(), nextfile_location_b.begin(),
                nextfile_location_b.end());
  auto name_size_b = Uint16ToBytes((uint16_t)name.size());
  header.insert(header.end(), name_size_b.begin(), name_size_b.end());
  for (auto it = name.begin(); it != name.end(); it++) {
    header.push_back(std::byte(*it));
  }
  return header;
}

static inline uint64_t min(uint64_t const& a, uint64_t const& b) {
  return (a < b) ? a : b;
}

void ProcessFile(std::basic_ofstream<std::byte>& output_file,
                 std::filesystem::path const& filepath) {
  std::basic_ifstream<std::byte> file(filepath,
                                      std::ios::binary | std::ios::in);
  file.ignore(std::numeric_limits<std::streamsize>::max());
  auto length = (uint64_t)file.gcount();
  auto data =
      PrepareHeader(filepath.filename().string(), output_file.tellp(), length);
  output_file.write(data.data(), data.size());
  file.clear();
  file.seekg(0, std::ios_base::beg);
  static std::vector<std::byte> buf(8192);
  for (uint64_t i = 0; i < length; i += 8192) {
    file.read(reinterpret_cast<std::byte*>(buf.data()), min(8192, length - i));
    output_file.write(buf.data(), min(8192, length - i));
  }
  file.close();
}

uint64_t ProcessFolder(std::basic_ofstream<std::byte>& output_file,
                       std::filesystem::path const& dir) {
  std::cout << "Processing folder " << dir << std::endl;
  int folder_amount = 0, file_amount = 0;
  for (auto const& dir_entry : std::filesystem::directory_iterator{dir}) {
    if (dir_entry.is_regular_file()) {
      file_amount++;
    } else if (dir_entry.is_directory()) {
      folder_amount++;
    }
  }

  std::byte nullbyte{0};

  uint64_t folder_begin = output_file.tellp();
  uint64_t folder_size = 10 + (uint16_t)dir.filename().string().size() +
                         (folder_amount + file_amount + 1) * sizeof(uint64_t);
  for (int i = 0; i < folder_size; i++) {
    output_file.write(&nullbyte, sizeof(std::byte));
  }

  std::vector<uint64_t> files;
  std::vector<uint64_t> folders;

  for (auto const& dir_entry : std::filesystem::directory_iterator{dir}) {
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
  for (uint64_t const& entry : files) {
    auto buf = Uint64ToBytes(entry);
    data.insert(data.end(), buf.begin(), buf.end());
  }

  output_file.seekp(folder_begin);
  output_file.write(data.data(), data.size());
  output_file.seekp(folder_end);
  return folder_begin;
}
void Pack(std::vector<std::string>& folder_paths, std::string output_path) {
  std::sort(folder_paths.begin(), folder_paths.end());
  if (std::adjacent_find(
          folder_paths.begin(), folder_paths.end(),
          [](std::string const& first, std::string const& second) {
            return rtrim_path(first) == rtrim_path(second);
          }) != folder_paths.end()) {
    throw std::invalid_argument(
        "You cannot add folders with the same folder names!");
  }
  std::basic_ofstream<std::byte> output_file(output_path,
                                             std::ios::binary | std::ios::out);
  for (auto const& folder : folder_paths) {
    ProcessFolder(output_file, std::filesystem::path(folder));
  }
  output_file.close();
}
}  // namespace Packer
}  // namespace resource

#endif
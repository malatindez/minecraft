#pragma once
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#define minecraft_RESOURCE_PACKING
#ifdef minecraft_RESOURCE_PACKING
namespace resource {
namespace Packer {
[[nodiscard]] static inline std::string rtrim_path(
    const std::string_view& path) {
  auto it = std::find_if(path.begin(), path.end(),
                         [&](const char& c) { return c == '/' || c == '\\'; });
  if (it != path.end()) {
    return std::string(it + 1, path.end());
  }
  return std::string(path);
}

[[nodiscard]] static inline std::vector<std::byte> Uint64ToBytes(
    uint64_t integer) {
  auto return_value = std::vector<std::byte>(8);
  return_value[0] = std::byte((integer << 0x00) & 0xff);
  return_value[1] = std::byte((integer << 0x08) & 0xff);
  return_value[2] = std::byte((integer << 0x10) & 0xff);
  return_value[3] = std::byte((integer << 0x18) & 0xff);
  return_value[4] = std::byte((integer << 0x20) & 0xff);
  return_value[5] = std::byte((integer << 0x28) & 0xff);
  return_value[6] = std::byte((integer << 0x30) & 0xff);
  return_value[7] = std::byte((integer << 0x38) & 0xff);
  return return_value;
}
[[nodiscard]] static inline std::vector<std::byte> Uint16ToBytes(
    uint16_t integer) {
  auto return_value = std::vector<std::byte>(2);
  return_value[0] = std::byte((integer << 0x00) & 0xff);
  return_value[1] = std::byte((integer << 0x08) & 0xff);
  return return_value;
}

[[nodiscard]] std::vector<std::byte> PrepareHeader(const std::string& name,
                                                   const uint64_t& file_begin,
                                                   const uint64_t& file_size) {
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
  header.reserve(header.size() + name.size());
  std::generate(header.begin() + 10, header.end(),
                [&, i = 0]() mutable { return std::byte(name[i++]); });
  return header;
}

void ProcessFile(std::basic_ofstream<std::byte>& output_file,
                 std::filesystem::path name) {}

uint64_t ProcessFolder(std::basic_ofstream<std::byte>& output_file,
                       std::filesystem::path dir) {
  int folder_amount = 0, file_amount = 0;
  for (auto const& dir_entry : std::filesystem::directory_iterator{dir}) {
    if (dir_entry.is_regular_file()) {
      file_amount++;
    } else if (dir_entry.is_directory()) {
      folder_amount++;
    }
  }

  auto null_uint64bytes = Uint64ToBytes(0);
  uint64_t folder_begin = output_file.tellp();

  for (int i = 0; i < folder_amount + file_amount + 1; i++) {
    output_file.write(null_uint64bytes.data(), sizeof(uint64_t));
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

  files.push_back(0);
  auto data = PrepareHeader(dir.filename().string(), output_file.tellp(),
                            files.size() * sizeof(uint64_t));
  files.insert(files.end(), folders.begin(), folders.end());
  for (uint64_t const& entry : files) {
    auto buf = Uint64ToBytes(entry);
    data.insert(data.end(), buf.begin(), buf.end());
  }

  uint64_t folder_end = output_file.tellp();

  output_file.seekp(folder_begin);
  output_file.write(data.data(), data.size());
  output_file.seekp(folder_end);
  return folder_begin;
}

void Pack(std::vector<std::string>& folder_paths, std::string output_path) {
  std::sort(folder_paths.begin(), folder_paths.end());
  if (std::adjacent_find(
          folder_paths.begin(), folder_paths.end(),
          [](const std::string& first, const std::string& second) {
            return rtrim_path(first) == rtrim_path(second);
          }) != folder_paths.end()) {
    throw std::invalid_argument(
        "You cannot add folders with the same folder names!");
  }
  std::basic_ofstream<std::byte> output_file(output_path,
                                             std::ios::binary | std::ios::in);
  for (const auto& folder : folder_paths) {
    ProcessFolder(output_file, std::filesystem::path(folder));
  }
}
}  // namespace Packer
}  // namespace resource

#endif
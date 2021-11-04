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
namespace resource::packer {

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

[[nodiscard]] std::vector<std::byte> PrepareHeader(std::string_view const& name,
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
  std::for_each(name.begin(), name.end(),
                [&](char const& c) { header.emplace_back(std::byte(c)); });
  return header;
}

static inline uint64_t min(uint64_t const& a, uint64_t const& b) {
  return (a < b) ? a : b;
}

void ReserveBytes(std::basic_ofstream<std::byte>& file, uint64_t amount) {
  static const std::byte nullbyte{0};
  for (int i = 0; i < amount; i++) {
    file.write(&nullbyte, sizeof(std::byte));
  }
}

void ProcessFile(std::basic_ofstream<std::byte>& output_file,
                 std::filesystem::path const& filepath) {
  auto length = std::filesystem::file_size(filepath);
  auto data =
      PrepareHeader(filepath.filename().string(), output_file.tellp(), length);
  output_file.write(data.data(), data.size());

  static const uint32_t kBlockSize = 2 * 1024 * 1024;
  static std::vector<std::byte> buf(kBlockSize);

  std::basic_ifstream<std::byte> file(filepath,
                                      std::ios::binary | std::ios::in);
  for (uint64_t i = 0; i < length; i += kBlockSize) {
    file.read(buf.data(), min(kBlockSize, length - i));
    output_file.write(buf.data(), min(kBlockSize, length - i));
  }
  file.close();
}

uint64_t ProcessFolder(std::basic_ofstream<std::byte>& output_file,
                       std::filesystem::path const& dir) {
  uint64_t folder_amount = 0;
  uint64_t file_amount = 0;
  for (auto const& dir_entry : std::filesystem::directory_iterator{dir}) {
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
  _ASSERT(data.size() == folder_size);

  output_file.seekp(folder_begin);
  output_file.write(data.data(), data.size());
  output_file.seekp(folder_end);
  return folder_begin;
}
/**
 * @brief Function to pack provided folders to a single file which can be used
 * later on by Resources class
 *
 * @param folder_paths vector, which contains paths to the target folders
 * @param output_path path to the output file.
 */
void Pack(std::vector<std::filesystem::path>& folder_paths,
          std::filesystem::path const& output_path) {
  namespace fs = std::filesystem;
  std::sort(folder_paths.begin(), folder_paths.end());
  if (std::adjacent_find(folder_paths.begin(), folder_paths.end(),
                         [](fs::path const& first, fs::path const& second) {
                           return first.filename() == second.filename();
                         }) != folder_paths.end()) {
    throw std::invalid_argument(
        "You cannot add folders with the same folder names!");
  }
  std::vector<uint64_t> folders;
  folders.push_back(0);
  auto data =
      PrepareHeader("", 0, (folder_paths.size() + 1) * sizeof(uint64_t));

  std::basic_ofstream<std::byte> output_file(output_path,
                                             std::ios::binary | std::ios::out);
  ReserveBytes(output_file,
               data.size() + (folder_paths.size() + 1) * sizeof(uint64_t));
  for (auto const& folder : folder_paths) {
    folders.push_back(ProcessFolder(output_file, fs::path(folder)));
  }
  for (uint64_t const& entry : folders) {
    auto buf = Uint64ToBytes(entry);
    data.insert(data.end(), buf.begin(), buf.end());
  }
  output_file.seekp(0);
  output_file.write(data.data(), data.size());
  output_file.close();
}
}  // namespace resource::packer

#endif
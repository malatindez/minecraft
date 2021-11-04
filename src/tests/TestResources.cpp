#define minecraft_RESOURCE_PACKING
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <random>

#include "Resources/Resources.hpp"
#include "Resources/pack.hpp"
#include "gtest/gtest.h"
namespace fs = std::filesystem;

static std::random_device rd;
static std::mt19937 gen(rd());

[[nodiscard]] static inline std::string Uint32ToUTF8(uint32_t const& value) {
  if (value > 0x10FFFF) {
    throw std::invalid_argument("Invalid value provided.");
  }
  std::string return_value = "";
  if (value < 0x80) {
    return_value += (char)value;
  } else if (value < 0x800) {
    return_value += (char)((value >> 0x06) & 0b00011111) | 0b11000000;
    return_value += (char)((value >> 0x00) & 0b00111111) | 0b10000000;
  } else if (value < 0x10000) {
    return_value += (char)((value >> 0x0C) & 0b00001111) | 0b11100000;
    return_value += (char)((value >> 0x06) & 0b00111111) | 0b10000000;
    return_value += (char)((value >> 0x00) & 0b00111111) | 0b10000000;
  } else {
    return_value += (char)((value >> 0x12) & 0b00000111) | 0b11110000;
    return_value += (char)((value >> 0x0C) & 0b00111111) | 0b10000000;
    return_value += (char)((value >> 0x06) & 0b00111111) | 0b10000000;
    return_value += (char)((value >> 0x00) & 0b00111111) | 0b10000000;
  }
  return return_value;
}

[[nodiscard]] static inline std::string RandomUTF8Char(
    uint32_t const& from = 0, uint32_t const& to = 0xA000) {
  static const std::uniform_int_distribution<uint32_t> dis(from, to);
  return Uint32ToUTF8(dis(gen));
}

[[nodiscard]] static std::string RandomUTF8String(size_t const& size = 32) {
  std::string return_value;
  for (int i = 0; i < size; i++) {
    return_value += RandomUTF8Char();
  }
  return return_value;
}

[[nodiscard]] static std::string RandomUTF8Filename(size_t const& size = 32) {
  static const std::string kProhibitedCharacters = "<>:\"/\\|?*";
  std::string return_value;
  for (int i = 0; i < size - 1;) {
    std::string temp = RandomUTF8Char(32);
    if (temp.size() == 1 && (kProhibitedCharacters.end() !=
                             std::find(kProhibitedCharacters.begin(),
                                       kProhibitedCharacters.end(), temp[0]))) {
      continue;
    }
    return_value += temp;
    i++;
  }
  std::string
      final_char;  // files in windows cannot end with space nor with dot
  do {
    final_char = RandomUTF8Char(33);
  } while (final_char.size() == 1 &&
           (kProhibitedCharacters.end() !=
                std::find(kProhibitedCharacters.begin(),
                          kProhibitedCharacters.end(), final_char[0]) ||
            final_char[0] == '.'));
  return return_value + final_char;
}

static void CreateFile(fs::path const& path, std::string_view const& data) {
  fs::create_directories({path.parent_path()});
  std::ofstream ofs(path);
  ofs << data;
  ofs.close();
}

static inline double time_diff(
    std::chrono::high_resolution_clock::time_point begin,
    std::chrono::high_resolution_clock::time_point end) {
  return std::chrono::duration<double, std::milli>(end - begin).count();
}

TEST(TestResources, TestPacking) {
  auto dir = fs::temp_directory_path() / "minecraft_test/TestPacking/";
  static const std::vector<std::string> kFixedTestfiles = {
      "a/b.txt", "a/b/c.txt", "a/b/c/d.txt"};
  for (std::string const& path : kFixedTestfiles) {
    CreateFile(dir / path, path);
  }
  std::vector<std::filesystem::path> files;
  static const std::uniform_int_distribution<uint32_t> amount_of_files(0, 1000);
  static const std::uniform_int_distribution<uint32_t> string_size(16, 32);
  static const std::uniform_int_distribution<uint32_t> file_size(0, 10000);

  for (uint32_t i = 0; i < amount_of_files(gen); i++) {
    std::string name = RandomUTF8Filename(string_size(gen));
    auto path = dir / "unicode_test/" / name;
    CreateFile(path, RandomUTF8String());
    files.push_back(std::filesystem::path("unicode_test") / name);
  }
  ASSERT_NO_THROW(resource::packer::Pack(
      std::vector<fs::path>({dir / "a", dir / "unicode_test"}),
      dir / "test.pack"))
      << "Pack function should throw any exceptions";
  auto t = Resources::LoadResources(dir / "test.pack");
  for (std::string const& path : kFixedTestfiles) {
    ASSERT_TRUE(t->FileExists(path))
        << "file " << path << " doesn't exist within the resource file";
  }
  for (std::filesystem::path const& file : files) {
    ASSERT_TRUE(t->FileExists(file.string()))
        << "file " << file << " doesn't exist within the resource file";
  }
  for (std::string const& path : kFixedTestfiles) {
    ASSERT_TRUE(t->GetFile(path).ToString() == path)
        << "File content is broken";
  }

  for (std::filesystem::path const& file : files) {
    std::basic_ifstream<std::byte> fileStream{dir / file, std::ios::in};
    uint64_t size = std::filesystem::file_size(dir / file);
    auto data_ptr = t->GetFile(file.string()).data();

    std::vector<std::byte> file_data(size);
    fileStream.read(file_data.data(), size);

    ASSERT_TRUE(
        std::equal(file_data.begin(), file_data.end(), data_ptr->begin()))
        << "File content is broken";
    fileStream.close();
  }

  ASSERT_NO_THROW(Resources::UnloadResources(dir / "test.pack"))
      << "Failed to unload resources";
  fs::remove_all(fs::temp_directory_path() / "minecraft_test/TestPacking");
}

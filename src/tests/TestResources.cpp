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

  static const std::vector<uint32_t> kUnicodeThresholds = {0x80, 0x800, 0x10000,
                                                           UINT32_MAX};

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
  for (size_t i = 0; i < size; i++) {
    return_value += RandomUTF8Char();
  }
  return return_value;
}
[[nodiscard]] static std::string RandomUTF8Filename(size_t const& size = 32) {
  static const std::string kProhibitedCharacters = "<>:\"/\\|?*";
  std::string return_value;
  for (size_t i = 0; i < size - 1;) {
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

static void CreateFile(fs::path const& path, char const* data,
                       const size_t size) {
  fs::create_directories({path.parent_path()});
  std::ofstream ofs(path);
  ofs.write(data, size);
  ofs.close();
}

static inline double time_diff(
    std::chrono::high_resolution_clock::time_point begin,
    std::chrono::high_resolution_clock::time_point end) {
  return std::chrono::duration<double, std::milli>(end - begin).count();
}

static const std::vector<std::string> kFixedTestfiles = {"a/b.txt", "a/b/c.txt",
                                                         "a/b/c/d.txt"};

class TestResources : public ::testing::Test {
 protected:
  static void SetUpTestSuite() {
    static const std::uniform_int_distribution<uint32_t> kAmountOfFiles{28, 34};
    static const std::uniform_int_distribution<uint32_t> kStringSize{16, 24};
    static const std::uniform_int_distribution<uint32_t> kFileSize{10000,
                                                                   100000};
    packing_succeded = false;
    dir_ = fs::temp_directory_path() / "minecraft_test/TestResources/";
    for (std::string const& path : kFixedTestfiles) {
      CreateFile(dir_ / path, path.c_str(), path.size());
    }

    for (uint32_t i = 0; i < kAmountOfFiles(gen); i++) {
      std::filesystem::path name = RandomUTF8Filename(kStringSize(gen));
      auto path = dir_ / "unicode_test/" / name;
      std::vector<char> t(kFileSize(gen));
      CreateFile(path, t.data(), t.size());
      unicode_files_.push_back(std::filesystem::path("unicode_test") / name);
    }
    packing_succeded = true;
  }

  static void TearDownTestSuite() {
    fs::remove_all(fs::temp_directory_path() / "minecraft_test/TestResources");
  }

  static std::filesystem::path dir_;
  static std::vector<std::filesystem::path> unicode_files_;
  static bool packing_succeded;
};

std::filesystem::path TestResources::dir_;
std::vector<std::filesystem::path> TestResources::unicode_files_;
bool TestResources::packing_succeded;

TEST_F(TestResources, TestPacking) {
  auto t = std::vector<fs::path>({dir_ / "a", dir_ / "unicode_test"});
  ASSERT_NO_THROW(resource::packer::Pack(t, dir_ / "test.pack"))
      << "Pack function should throw any exceptions";
}
TEST_F(TestResources, TestLoading) {
  ASSERT_NO_THROW(Resources::LoadResources(dir_ / "test.pack"));
  ASSERT_NO_THROW(Resources::UnloadResources(dir_ / "test.pack"));
}

TEST_F(TestResources, FixedFileLoading) {
  auto resources_ = Resources::LoadResources(dir_ / "test.pack");
  for (std::string const& path : kFixedTestfiles) {
    ASSERT_TRUE(resources_->FileExists(path))
        << "file " << path << " doesn't exist within the resource file";
  }
  for (std::string const& path : kFixedTestfiles) {
    ASSERT_TRUE(resources_->GetFile(path).ToString() == path)
        << "File content is broken";
  }
  ASSERT_NO_THROW(Resources::UnloadResources(dir_ / "test.pack"));
}
TEST_F(TestResources, RandomFileLoading) {
  auto resources_ = Resources::LoadResources(dir_ / "test.pack");
  for (std::filesystem::path const& file : TestResources::unicode_files_) {
    ASSERT_TRUE(resources_->FileExists(file.string()))
        << "file " << file << " doesn't exist within the resource file";
  }
  for (std::filesystem::path const& file : TestResources::unicode_files_) {
    std::ifstream fileStream{TestResources::dir_ / file, std::ios::in};
    uint64_t size = std::filesystem::file_size(TestResources::dir_ / file);
    auto data_ptr = resources_->GetFile(file.string()).data();
    std::vector<char> file_data((size_t)size);
    fileStream.read(file_data.data(), size);

    ASSERT_TRUE(
        std::equal(file_data.begin(), file_data.end(), data_ptr->begin()))
        << "Content of the file" << file << "is broken";
    fileStream.close();
  }
  ASSERT_NO_THROW(Resources::UnloadResources(dir_ / "test.pack"));
}
const uint16_t kThreadAmount = 32;
TEST_F(TestResources, TestMultithreadedRandomFileLoading) {
  auto resources_ = Resources::LoadResources(dir_ / "test.pack");
  std::condition_variable cv;
  std::mutex m;
  std::mutex counter_mutex;
  int counter = 0;
  using ptr_vec = std::vector<std::shared_ptr<std::vector<char>>>;
  ptr_vec pointers(kThreadAmount);

  auto thread_function = [&](ptr_vec::iterator itr) {
    for (std::filesystem::path const& file : TestResources::unicode_files_) {
      std::unique_lock<std::mutex> lk(m);
      counter++;
      cv.wait(lk);
      ASSERT_TRUE(*itr = resources_->GetFile(file.string()).data())
          << "file " << file << " doesn't exist within the resource file";
      std::unique_lock<std::mutex> t(counter_mutex);
    }
  };
  std::vector<std::thread> threads;
  for (auto itr = pointers.begin(); itr != pointers.end(); itr++) {
    threads.push_back(std::thread(thread_function, itr));
  }

  // thread synchronization
  for (std::filesystem::path const& file : TestResources::unicode_files_) {
    using namespace std::chrono_literals;
    while (counter != kThreadAmount) {
      std::this_thread::sleep_for(1ms);
    }
    counter = 0;
    bool eq = true;
    auto itr = pointers.begin();
    while (eq && itr != pointers.end()) {
      eq = (itr == ++itr);
    }
    ASSERT_TRUE(eq);
    cv.notify_all();
  }
  for (auto& thread : threads) {
    thread.join();
  }
  ASSERT_NO_THROW(Resources::UnloadResources(dir_ / "test.pack"));
}
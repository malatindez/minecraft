
#define minecraft_RESOURCE_PACKING
#include <thread>

#include <resources/pack.hpp>
#include <resources/resources.hpp>

#include "pch.h"
#include "utils.hpp"

namespace fs = std::filesystem;
using namespace resource;
static const std::vector<std::string> kFixedTestfiles = {"a/b.txt", "a/b/c.txt",
                                                         "a/b/c/d.txt"};

class TestResources : public ::testing::Test {
protected:
  static void SetUpTestSuite() {
    static const std::pair<uint32_t, uint32_t> kAmountOfFiles{28, 34};
    static const std::pair<uint32_t, uint32_t> kStringSize{16, 24};
    static const std::pair<uint32_t, uint32_t> kFileSize{10000, 100000};
    packing_succeded = false;
    dir_ = fs::temp_directory_path() / "minecraft_test/TestResources/";
    for (std::string const &path : kFixedTestfiles) {
      CreateFile(dir_ / path, path.c_str(), path.size());
    }

    for (uint32_t i = 0;
         i < RandomUint32(kAmountOfFiles.first, kAmountOfFiles.second); i++) {
      std::filesystem::path name = RandomUTF8Filename(
          RandomUint32(kStringSize.first, kStringSize.second));
      auto path = dir_ / "unicode_test/" / name;
      std::vector<char> t(RandomUint32(kFileSize.first, kFileSize.second));
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
      << "Pack function shouldn't throw any exceptions";
}
TEST_F(TestResources, TestLoading) {
  ASSERT_NO_THROW(resource::LoadResources(dir_ / "test.pack"));
  ASSERT_NO_THROW(resource::UnloadResources(dir_ / "test.pack"));
}

TEST_F(TestResources, FixedFileLoading) {
  auto resources_ = resource::LoadResources(dir_ / "test.pack");
  for (std::string const &path : kFixedTestfiles) {
    ASSERT_TRUE(resources_.FileExists(path))
        << "file " << path << " doesn't exist within the resource file";
  }
  for (std::string const &path : kFixedTestfiles) {
    ASSERT_TRUE(resources_.GetFile(path).ToString() == path)
        << "File content is broken";
  }
  ASSERT_NO_THROW(resource::UnloadResources(dir_ / "test.pack"));
}
TEST_F(TestResources, RandomFileLoading) {
  auto resources_ = resource::LoadResources(dir_ / "test.pack");
  for (std::filesystem::path const &file : TestResources::unicode_files_) {
    ASSERT_TRUE(resources_.FileExists(file.string()))
        << "file " << file << " doesn't exist within the resource file";
  }
  for (std::filesystem::path const &file : TestResources::unicode_files_) {
    std::ifstream fileStream{TestResources::dir_ / file, std::ios::in};
    uint64_t size = std::filesystem::file_size(TestResources::dir_ / file);
    auto data_ptr = resources_.GetFile(file.string()).data();
    std::vector<char> file_data((size_t)size);
    fileStream.read(file_data.data(), size);

    ASSERT_TRUE(
        std::equal(file_data.begin(), file_data.end(), data_ptr->begin()))
        << "Content of the file" << file << "is broken";
    fileStream.close();
  }
  for (Entry const &file : resources_ / "unicode_test") {
    ASSERT_TRUE(file.is_file());
  }
  ASSERT_NO_THROW(resource::UnloadResources(dir_ / "test.pack"));
}
const uint16_t kThreadAmount = 32;
TEST_F(TestResources, TestMultithreadedRandomFileLoading) {
  auto resources_ = resource::LoadResources(dir_ / "test.pack");
  std::condition_variable cv;
  std::mutex m;
  std::mutex counter_mutex;
  int counter = 0;
  using ptr_vec = std::vector<std::shared_ptr<std::vector<char>>>;
  ptr_vec pointers(kThreadAmount);

  auto thread_function = [&m, &counter, &cv, &counter_mutex,
                          &resources_](ptr_vec::iterator itr) {
    for (std::filesystem::path const &file : TestResources::unicode_files_) {
      std::unique_lock lk(m);
      counter++;
      cv.wait(lk);
      *itr = resources_.GetFile(file.string()).data();
      ASSERT_TRUE(*itr) << "file " << file
                        << " doesn't exist within the resource file";
      std::unique_lock t(counter_mutex);
    }
  };
  std::vector<std::jthread> threads;
  for (auto itr = pointers.begin(); itr != pointers.end(); itr++) {
    threads.emplace_back(thread_function, itr);
  }

  // thread synchronization
  for (size_t i = 0; i < unicode_files_.size(); i++) {
    using namespace std::chrono_literals;
    while (counter != kThreadAmount) {
      std::this_thread::sleep_for(1ms);
    }
    counter = 0;
    bool eq = true;
    auto itr = pointers.begin();
    while (eq && std::next(++itr) != pointers.end()) {
      eq = *itr == *std::next(itr);
    }
    ASSERT_TRUE(eq);
    cv.notify_all();
  }
  for (auto &thread : threads) {
    thread.join();
  }
  ASSERT_NO_THROW(resource::UnloadResources(dir_ / "test.pack"));
}
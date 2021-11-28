
#include <filesystem>
#include <fstream>

#include "Config.hpp"
#include "gtest/gtest.h"
namespace fs = std::filesystem;
TEST(TEST_CONFIG, TestSaving) {
  Config conf;
  conf.CreateSection("Graphics");
  Config::Section& graphics = conf["Graphics"];

  graphics.SetValue("ResolutionX", (uint64_t)3200);
  graphics.SetValue("ResolutionY", (uint64_t)3200);

  fs::create_directories(
      {fs::temp_directory_path() / "minecraft_test/TestConfig"});
  std::ofstream ofs(fs::temp_directory_path() /
                    "minecraft_test/TestConfig/TestFile.ini");
  auto t = conf.Serialize();
  ofs.write(t.c_str(), t.size());
  ofs.close();
}
TEST(TEST_CONFIG, TestLoading) {
  std::ifstream ifs(
      fs::temp_directory_path() / "minecraft_test/TestConfig/TestFile.ini",
      std::ios::in);
  std::string value;
  auto* const buffer = new char[1];
  while (ifs.good()) {
    ifs.read(buffer, 1);
    value += buffer[0];
  }
  delete[] buffer;
  ifs.close();

  Config conf;
  conf.Deserialize(value);
  ASSERT_TRUE(conf["Graphics"].GetInt("ResolutionX") == 3200);
  ASSERT_TRUE(conf["Graphics"].GetInt("ResolutionX") == 3200);
}
TEST(TEST_CONFIG, TestExceptions) {}
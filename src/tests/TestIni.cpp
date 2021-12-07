
#include <filesystem>
#include <fstream>
#include <queue>
#include <set>

#include "Parser/Ini.hpp"
#include "gtest/gtest.h"
namespace fs = std::filesystem;
TEST(TEST_INI, TestSaving) {
  ini::Ini conf;
  conf.CreateSection("Graphics");
  ini::Section& graphics = conf["Graphics"];

  graphics.SetValue("ResolutionX", 3200);
  graphics.SetValue("ResolutionY", 3200);
  conf["Graphics"]["Gamma"] = 35.5;

  conf["Controls"]["MovementForward"] = "W";
  conf["Controls"]["MovementBackwards"] = "S";
  fs::create_directories(
      {fs::temp_directory_path() / "minecraft_test/test_ini"});
  std::ofstream ofs(fs::temp_directory_path() /
                    "minecraft_test/test_ini/TestFile.ini");
  auto t = conf.Serialize();
  ofs.write(t.c_str(), t.size());
  ofs.close();
}
TEST(TEST_INI, TestLoading) {
  std::ifstream ifs(
      fs::temp_directory_path() / "minecraft_test/test_ini/TestFile.ini",
      std::ios::in);
  std::string value;
  auto* const buffer = new char[1];
  while (ifs.good()) {
    ifs.read(buffer, 1);
    value += buffer[0];
  }
  delete[] buffer;
  ifs.close();

  auto conf = ini::Ini::Deserialize(value);
  ASSERT_EQ(conf["Graphics"].GetInt("ResolutionX"), 3200);
  ASSERT_EQ(conf["Graphics"].GetInt("ResolutionY"), 3200);
  ASSERT_EQ(conf["Controls"]["MovementForward"], "W");
  ASSERT_EQ(conf["Controls"].GetString("MovementBackwards"), "S");
  ASSERT_EQ(conf["Graphics"]["Gamma"], 35.5);
}
TEST(TEST_INI, TestTrimming) {
  auto conf = ini::Ini::Deserialize(R"(
[A]
B = 0
C    =      1     
D =           5)");
  ASSERT_EQ(conf["A"]["B"], 0);
  ASSERT_EQ(conf["A"]["C"], 1);
  ASSERT_EQ(conf["A"]["D"], 5);
  ASSERT_EQ(conf["A"]["B"], "0");
  ASSERT_EQ(conf["A"]["C"], "1");
  ASSERT_EQ(conf["A"]["D"], "5");
}
TEST(TEST_INI, TestExceptions) {}

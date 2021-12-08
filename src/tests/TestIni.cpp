
#include <filesystem>
#include <fstream>
#include <queue>
#include <set>

#include "Parser/Ini.hpp"
#include "Utils.hpp"
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
  auto t = conf.Serialize();
  CreateFile(fs::temp_directory_path() / "minecraft_test/test_ini/TestFile.ini",
             t.c_str(), t.size());
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
  B = 0  #comments test  23 
   C    =      1  ; test   
D   =           5)");
  ASSERT_EQ(conf["A"]["B"], 0);
  ASSERT_EQ(conf["A"]["C"], 1);
  ASSERT_EQ(conf["A"]["D"], 5);
  ASSERT_EQ(conf["A"]["B"], "0");
  ASSERT_EQ(conf["A"]["C"], "1");
  ASSERT_EQ(conf["A"]["D"], "5");
}
static const size_t kRandomSectionsSize = 128;
static const size_t kRandomIntegerKeysSize = 128;
static const size_t kRandomDoubleKeysSize = 128;
static const size_t kRandomStringKeysSize = 128;
static const size_t kSectionKeySize = 64;
static const size_t kKeySize = 128;
static const size_t kStringValueSize = 128;
static const std::string kSectionKeyCharacters =
    ExcludeString(kAsciiCharacters, "[]#;\\");
static const std::string kKeyCharacters =
    ExcludeString(kAsciiCharacters, "[]=#;\\");
TEST(TEST_INI, RandomTest) {
  auto conf = ini::Ini();
  std::vector<std::string> random_sections;
  for (size_t i = 0; i < kRandomSectionsSize; i++) {
    auto t = utils::trim(RandomString(kSectionKeySize, kSectionKeyCharacters));
    ini::Section& section = conf[t];
    for (size_t i = 0; i < kRandomIntegerKeysSize; i++) {
      section[utils::trim(RandomString(kKeySize, kKeyCharacters))] =
          RandomInteger();
    }
    for (size_t i = 0; i < kRandomDoubleKeysSize; i++) {
      section[utils::trim(RandomString(kKeySize, kKeyCharacters))] =
          RandomLongDouble();
    }
    for (size_t i = 0; i < kRandomStringKeysSize; i++) {
      section[utils::trim(RandomString(kKeySize, kKeyCharacters))] =
          utils::trim(RandomString(kStringValueSize, kSectionKeyCharacters));
    }
  }
  auto conf2 = ini::Ini::Deserialize(conf.Serialize());
  ASSERT_EQ(conf2.size(), conf.size());
  for (auto const& [section_key, section] : conf) {
    ASSERT_TRUE(conf2.SectionExists(section_key)) << section_key;
    ini::Section& section2 = conf2[section_key];

    ASSERT_EQ(section.get().size(), section2.size());

    for (auto const& [key, entry] : section.get()) {
      ASSERT_TRUE(section2.Contains(key))
          << section_key << "    " << key << "    " << entry.get().str();
      ASSERT_EQ(section2[key], entry);
    }
  }
}

TEST(TEST_INI, TestExceptions) {}

#pragma once
#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <limits>
#include <random>

#include <utils/utils.hpp>

namespace fs = std::filesystem;

[[nodiscard]] inline std::string ExcludeString(std::string const &a,
                                               std::string const &b) {
  std::string return_value{a};
  std::erase_if(return_value, [&b](char const &c) {
    return std::find(b.begin(), b.end(), c) != b.end();
  });
  return return_value;
}

[[nodiscard]] std::string Uint32ToUTF8(uint32_t const &value);

[[nodiscard]] inline std::string RandomUTF8Char(uint32_t const &from = 0,
                                                uint32_t const &to = 0xA000);

[[nodiscard]] std::string RandomUTF8String(size_t const &size = 32);
[[nodiscard]] std::string RandomUTF8Filename(size_t const &size = 32);

const std::string kAsciiCharacters =
    "' "
    "!\"#$%&'()*+,-./"
    "0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"
    "abcdefghijklmnopqrstuvwxyz{|}~";

inline std::string AllBinaryCharacters() {
  std::string return_value;
  for (char i = -128; i < 127; i++) {
    return_value.push_back(i);
  }
  return return_value;
}
const std::string kAllBinaryCharacters = AllBinaryCharacters();

[[nodiscard]] std::string RandomBinaryString(size_t const &size = 32);

[[nodiscard]] std::string
RandomString(size_t const &size = 32,
             std::string const &including = kAsciiCharacters);

[[nodiscard]] uint64_t
RandomUint64(uint64_t min = std::numeric_limits<uint64_t>::min(),
             uint64_t max = std::numeric_limits<uint64_t>::max());
[[nodiscard]] uint32_t
RandomUint32(uint32_t min = std::numeric_limits<uint32_t>::min(),
             uint32_t max = std::numeric_limits<uint32_t>::max());
[[nodiscard]] uint32_t
RandomUint16(uint16_t min = std::numeric_limits<uint16_t>::min(),
             uint16_t max = std::numeric_limits<uint16_t>::max());
[[nodiscard]] int64_t
RandomInt64(int64_t min = std::numeric_limits<int64_t>::min(),
            int64_t max = std::numeric_limits<int64_t>::max());
[[nodiscard]] int32_t
RandomInt32(int32_t min = std::numeric_limits<int32_t>::min(),
            int32_t max = std::numeric_limits<int32_t>::max());
[[nodiscard]] int16_t
RandomInt16(int16_t min = std::numeric_limits<int16_t>::min(),
            int16_t max = std::numeric_limits<int16_t>::max());

[[nodiscard]] int64_t
RandomInteger(int64_t min = std::numeric_limits<int64_t>::min(),
              int64_t max = std::numeric_limits<int64_t>::max());

[[nodiscard]] uint64_t
RandomUnsignedInteger(uint64_t min = std::numeric_limits<uint64_t>::min(),
                      uint64_t max = std::numeric_limits<uint64_t>::max());

[[nodiscard]] size_t
RandomSizeT(size_t min = std::numeric_limits<size_t>::min(),
            size_t max = std::numeric_limits<size_t>::max());

[[nodiscard]] char RandomChar(char min = std::numeric_limits<char>::min(),
                              char max = std::numeric_limits<char>::max());

[[nodiscard]] float RandomFloat(float min = std::numeric_limits<float>::min(),
                                float max = std::numeric_limits<float>::max());

[[nodiscard]] double
RandomDouble(double min = std::numeric_limits<double>::min(),
             double max = std::numeric_limits<double>::max());

[[nodiscard]] long double
RandomLongDouble(long double min = std::numeric_limits<long double>::min(),
                 long double max = std::numeric_limits<long double>::max());

[[nodiscard]] std::string RandomFilename(size_t const &size = 32);
void CreateFile(fs::path const &path, char const *data, const size_t size);

[[nodiscard]] constexpr double
time_diff(std::chrono::high_resolution_clock::time_point begin,
          std::chrono::high_resolution_clock::time_point end) {
  return std::chrono::duration<double, std::milli>(end - begin).count();
}
#include "Utils.hpp"
namespace fs = std::filesystem;

static std::random_device rd;
static std::mt19937 gen(rd());

[[nodiscard]] uint64_t RandomUint64(uint64_t min, uint64_t max) {
  std::uniform_int_distribution dis{min, max};
  return dis(gen);
}

[[nodiscard]] uint32_t RandomUint32(uint32_t min, uint32_t max) {
  std::uniform_int_distribution dis{min, max};
  return dis(gen);
}
[[nodiscard]] uint32_t RandomUint16(uint16_t min, uint16_t max) {
  std::uniform_int_distribution dis{min, max};
  return dis(gen);
}
[[nodiscard]] int64_t RandomInt64(int64_t min, int64_t max) {
  std::uniform_int_distribution dis{min, max};
  return dis(gen);
}
[[nodiscard]] int32_t RandomInt32(int32_t min, int32_t max) {
  std::uniform_int_distribution dis{min, max};
  return dis(gen);
}
[[nodiscard]] int16_t RandomInt16(int16_t min, int16_t max) {
  std::uniform_int_distribution dis{min, max};
  return dis(gen);
}
[[nodiscard]] int64_t RandomInteger(int64_t min, int64_t max) {
  return RandomInt64(min, max);
}

[[nodiscard]] uint64_t RandomUnsignedInteger(uint64_t min, uint64_t max) {
  return RandomUint64(min, max);
}
[[nodiscard]] size_t RandomSizeT(size_t min, size_t max) {
  std::uniform_int_distribution dis{min, max};
  return dis(gen);
}

[[nodiscard]] char RandomChar(char min, char max) {
  std::uniform_int_distribution<short> dis{min, max};
  return char(dis(gen));
}
[[nodiscard]] float RandomFloat(float min, float max) {
  std::uniform_real_distribution dis{min, max};
  return dis(gen);
}

[[nodiscard]] double RandomDouble(double min, double max) {
  std::uniform_real_distribution dis{min, max};
  return dis(gen);
}

[[nodiscard]] long double RandomLongDouble(long double min, long double max) {
  std::uniform_real_distribution dis{min, max};
  return dis(gen);
}

constexpr std::string ProhibitedCharactersInFilename() {
  std::string return_value = "<>:\"/\\|?*";
  for (char i = 0; i < 32; i++) {
    return_value += i;
  }
  return return_value;
}
const std::string kProhibitedCharacters = ProhibitedCharactersInFilename();

[[nodiscard]] std::string Uint32ToUTF8(uint32_t const &value) {
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

[[nodiscard]] inline std::string RandomUTF8Char(uint32_t const &from,
                                                uint32_t const &to) {
  return Uint32ToUTF8(RandomUint32(from, to));
}

[[nodiscard]] std::string RandomUTF8String(size_t const &size) {
  std::string return_value;
  for (size_t i = 0; i < size; i++) {
    return_value += RandomUTF8Char();
  }
  return return_value;
}
[[nodiscard]] std::string RandomUTF8Filename(size_t const &size) {
  std::string return_value;
  for (size_t i = 0; i < size - 1; i++) {
    std::string temp = RandomUTF8Char(32);

    while (temp.size() == 1 &&
           std::find_if(kProhibitedCharacters.begin(),
                        kProhibitedCharacters.end(), [&temp](char const &c) {
                          return c == temp[0];
                        }) != kProhibitedCharacters.end()) {
      temp = RandomUTF8Char(32);
    }
    return_value += temp;
  }
  std::string final_char; // files in windows cannot end with space nor with dot
  while (final_char.size() == 1 &&
         std::find_if(kProhibitedCharacters.begin(),
                      kProhibitedCharacters.end(),
                      [&final_char](char const &c) {
                        return c == final_char[0] || final_char[0] == ' ' ||
                               final_char[0] == '.';
                      }) != kProhibitedCharacters.end()) {
    final_char = RandomUTF8Char(32);
  }
  return return_value + final_char;
}

[[nodiscard]] std::string RandomBinaryString(size_t const &size) {
  return RandomString(size, kAllBinaryCharacters);
}

[[nodiscard]] std::string RandomString(size_t const &size,
                                       std::string const &including) {
  std::string return_value;
  return_value.reserve(size);
  const std::uniform_int_distribution<size_t> dis{0, including.size() - 1};
  for (size_t i = 0; i < size; i++) {
    return_value += including[dis(gen)];
  }

  return return_value;
}

[[nodiscard]] std::string RandomFilename(size_t const &size) {
  if (size == 0) {
    return "";
  }
  std::string return_value = RandomString(
      size, ExcludeString(kAsciiCharacters, kProhibitedCharacters));
  if (return_value[return_value.size() - 1] == ' ' ||
      return_value[return_value.size() - 1] == '.') {
    return_value[return_value.size() - 1] = RandomChar('a', 'z');
  }
  return return_value;
}

void CreateFile(fs::path const &path, char const *data, const size_t size) {
  fs::create_directories({path.parent_path()});
  std::ofstream ofs(path);
  ofs.write(data, size);
  ofs.close();
}

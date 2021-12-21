#pragma once
#include <fstream>
#include <mutex>
#include <stdexcept>

#include "parsers/ini.hpp"
class Config final : public ini::Ini {
 public:
  static Config const &GetInstance() {
    static std::mutex mutex;
    if (!config_) {
      std::lock_guard lock(mutex);
      if (config_) {
        return *config_;
      }

      std::ifstream config("config.ini");
      std::string data;
      config >> data;
      config.close();
      config_ = std::unique_ptr<Config>(new Config(data));
    }
    return *config_;
  }
  Config(Config &&) = delete;
  Config &operator=(Config &&) = delete;
  Config(Config const &) = delete;
  Config &operator=(Config const &) = delete;

 private:
  using ini::Ini::Ini;
  static std::unique_ptr<Config> config_;
};
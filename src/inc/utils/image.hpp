#include <stb/stb_image.h>

#include <filesystem>
namespace utils {

// wrapper around stbi image pointer
class Image final {
 public:
  static Image LoadFromMemory(const uint8_t *data, const size_t size) {
    int x, y, channels;
    uint8_t *img = stbi_load_from_memory(data, size, &x, &y, &channels, 0);
    return Image(img, x, y, channels);
  }
  explicit Image(uint8_t *img, int x, int y, int channels) {
    if (!img || x <= 0 || y <= 0 || channels <= 0) {
      throw std::invalid_argument("Error in loading the image");
    }
    data_ =
        std::shared_ptr<uint8_t>{img, [](uint8_t *im) { stbi_image_free(im); }};
    width_ = x;
    height_ = y;
    channels_ = channels;
  }
  [[nodiscard]] constexpr size_t width() const noexcept { return width_; }
  [[nodiscard]] constexpr size_t sizex() const noexcept { return width_; }
  [[nodiscard]] constexpr size_t height() const noexcept { return height_; }
  [[nodiscard]] constexpr size_t sizey() const noexcept { return height_; }
  [[nodiscard]] constexpr size_t channels() const noexcept { return channels_; }
  [[nodiscard]] constexpr size_t ch() const noexcept { return channels_; }
  [[nodiscard]] constexpr uint8_t const *data() const noexcept {
    return data_.get();
  }
  [[nodiscard]] constexpr size_t size() const noexcept {
    return width_ * height_ * channels_;
  }

 private:
  std::shared_ptr<uint8_t> data_;
  size_t width_, height_, channels_;
};
}  // namespace utils
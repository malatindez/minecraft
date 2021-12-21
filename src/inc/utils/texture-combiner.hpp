#include <stb/stb_image.h>

#include "resources/entry.hpp"
#include "utils/image.hpp"

namespace utils::texture_combiner {
// Bakes the provided textures to the one file
// Removes used textures from the provided vector.

struct __Coord {
  size_t x = 0;
  size_t y = 0;
  Image const &im;
};

std::vector<__Coord> PackRectangles(std::vector<Image> &images,
                                    size_t maxX = 2048, size_t maxY = 2048) {}
inline std::vector<Image> LoadImages(
    std::vector<std::reference_wrapper<const resource::Entry>> image_files) {
  std::vector<Image> return_value;
  // retrieve images
  for (auto const &image : image_files) {
    auto ptr = image.get().data();
    return_value.push_back(Image::LoadFromMemory(
        reinterpret_cast<uint8_t *>(ptr->data()), ptr->size()));
  }
  return return_value;
}
inline std::vector<char> Bake(std::vector<Image> &images, size_t max_size_x,
                              size_t max_size_y) {
  std::vector<__Coord> vec = PackRectangles(images, max_size_x, max_size_y);
  // combine images and return the result
  std::vector<uint8_t> data;
  data.resize(max_size_x * max_size_y * 4, 0);
  uint8_t *ptr = data.data();
  for (auto const &image : vec) {
    for (int i = 0; i < image.im.sizex(); i++) {
      memcpy(ptr + image.x + image.y * max_size_x,
             image.im.data() + image.im.sizex() * i, image.im.sizex());
    }
  }

  std::vector<char> return_value;
}

} // namespace utils::texture_combiner
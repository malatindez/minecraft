#define minecraft_RESOURCE_PACKING
#include <iostream>

#include <resources/pack.hpp>
/*
 * packer.exe can process only valid paths, otherwise it will throw an error.
 * first path is a path to file, other ones are paths to folders
 */
int main(int argc, char **argv) {
  std::vector<std::filesystem::path> arguments;
  for (int i = 0; i < argc; i++) {
    arguments.emplace_back(argv[i]);
  }
  std::filesystem::path output = arguments[1];
  arguments.erase(arguments.begin(), arguments.begin() + 2);
  resource::packer::Pack(arguments, output);
  std::cout << "Successfully packed all data to " << output << std::endl;
}
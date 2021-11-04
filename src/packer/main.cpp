#define minecraft_RESOURCE_PACKING
#include <iostream>

#include "Resources/pack.hpp"
/*
 * packer.exe can process only valid paths, otherwise it will throw an error.
 * first path is a path to file, other ones are paths to folders
 */
int main(int argc, char **argv) {
  std::vector<std::string> arguments;
  for (int i = 0; i < argc; i++) {
    arguments.push_back(std::string(argv[i]));
  }
  std::string output = arguments[1];
  arguments.erase(arguments.begin(), arguments.begin() + 2);
  resource::Packer::Pack(arguments, output);
}
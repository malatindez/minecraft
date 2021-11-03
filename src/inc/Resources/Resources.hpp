#include <codecvt>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <locale>
#include <mutex>

#include "Directory.hpp"
#include "File.hpp"

using namespace resource;
/*
 *  A simple singleton class that loads and unpacks different resources from
 *    .res files. You can separate resources into multiple files, pass them to
 *    the loader and it will automatically parse them to folders and files.
 *
 * Define minecraft_RESOURCE_PACKING to enable resource packing to disk
 *
 *
 * Each file and folder are defined as following:
 * File:
 * 0x00 : 0x08 - next file location (k) (little endian)
 * 0x0A : 0x0C — size of the name of the file (n) (little endian)
 * 0x0C : 0x0C + n — name of the file
 * 0x0C + n : k — file data
 *
 * Folder depends on File, and stores data inside of the file.
 * 0x00 : 0x08            }
 * 0x08 : 0x10              }
 * 0x10 : 0x18                } — file locations within the resource file
 * ...                      }
 * ...  : n               }
 *
 *  n   : n + 0x08 — 8 zero bytes that are used to define the end of file
 *                       section and the beginning of the folder section
 *
 * n + 0x08 : n + 0x10    }
 * n + 0x10 : n + 0x18      }
 * n + 0x18 : n + 0x20        } — folder locations within the resource file
 * ...                      }
 * ...      : k           }
 */
class Resources {
 public:
  /*
   * Function to load resources from .res file
   * local_path is used to modify folder, within the loaded resources will be
   * located. the name of .res file should be unique.
   */
  static std::shared_ptr<Directory> LoadResources(
      const std::string_view& path_to_file, const bool& overwrite = true,
      const bool& merge = true);
  static std::shared_ptr<Directory> GetResourceFolder() {}

 private:
  static std::shared_mutex resource_mutex_;
  static std::vector<std::shared_ptr<Directory>> tree_;
  static std::vector<AtomicIfstreamPointer> resource_handles_;
};

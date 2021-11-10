#pragma once
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

/*
 * Define minecraft_RESOURCE_PACKING to enable resource packing to disk
 */

#define minecraft_RESOURCE_PACKING
#ifdef minecraft_RESOURCE_PACKING
namespace resource::packer {

/**
 * @brief Function to pack provided folders to a single file which can be used
 * later on by Resources class
 *
 * @param folder_paths vector, which contains paths to the target folders
 * @param output_path path to the output file.
 */
void Pack(std::vector<std::filesystem::path>& folder_paths,
          std::filesystem::path const& output_path);
}  // namespace resource::packer

#endif
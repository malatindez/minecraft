#ifndef BLOCK_H
#define BLOCK_H
#include "Shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>
#include <fstream>
#include "TextureLoader.h"
class UBlock { // universal block
public:
    uint32_t *diffuseTextures = nullptr;
    uint32_t *specularTextures = nullptr;
    std::string str_id; // minecraft:stone, minecraft:grass, etc.
    uint32_t integer_id; // 1, 2, etc.
    uint8_t hardness;
    UBlock(const uint32_t* diffuseTextures, const uint32_t* specularTextures, const std::string str_id, uint8_t hardness) {
        this->diffuseTextures = new uint32_t[6];
        this->specularTextures = new uint32_t[6];
        memcpy(this->diffuseTextures, diffuseTextures, sizeof(uint32_t) * 6);
        memcpy(this->specularTextures, specularTextures, sizeof(uint32_t) * 6);
        this->str_id = str_id;
        this->hardness = hardness;
    }
    UBlock(const UBlock& b) {
        this->diffuseTextures = new uint32_t[6];
        this->specularTextures = new uint32_t[6];
        memcpy(this->diffuseTextures, b.diffuseTextures, sizeof(uint32_t) * 6);
        memcpy(this->specularTextures, b.specularTextures, sizeof(uint32_t) * 6);
        this->str_id = b.str_id;
        this->integer_id = b.integer_id;
        this->hardness = b.hardness;
    }
    UBlock(const UBlock* b) {
        this->diffuseTextures = new uint32_t[6];
        this->specularTextures = new uint32_t[6];
        memcpy(this->diffuseTextures, b->diffuseTextures, sizeof(uint32_t) * 6);
        memcpy(this->specularTextures, b->specularTextures, sizeof(uint32_t) * 6);
        this->str_id = b->str_id;
        this->integer_id = b->integer_id;
        this->hardness = b->hardness;
    }
    ~UBlock() {
        if (this->diffuseTextures != nullptr){
            delete[] this->diffuseTextures;
        }
        if (this->specularTextures != nullptr) {
            delete[] this->specularTextures;
        }
    }
    void BindTextures() const {
        for (uint8_t i = 0; i < 6; i++) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, diffuseTextures[i]);
        }
        for (uint8_t i = 0; i < 6; i++) {
            glActiveTexture(GL_TEXTURE6 + i);
            glBindTexture(GL_TEXTURE_2D, specularTextures[i]);
        }
    }
};
bool ends_with(std::string str, std::string suffix) {
    if (suffix.size() > str.size()) {
        return false;
    }
    for (size_t i = suffix.size() - 1; i != size_t(-1); i--) {
        if (str[str.size() - suffix.size() + i] != suffix[i]) {
            return false;
        }
    }
    return true;
}
#include <vector>
#include <algorithm>
::std::vector<UBlock> loadBlocks(TextureLoader* loader) {
    ::std::vector<UBlock> returnValue;
    ::std::string path = "resources\\minecraft\\";
    uint32_t* diffuseTextures = new uint32_t[6];
    uint32_t* specularTextures = new uint32_t[6];
    for (const auto& entry : ::std::filesystem::directory_iterator(path)) {
        std::string blockPath = entry.path().string();
        if (ends_with(blockPath, (".block"))) {
            ::std::ifstream block(blockPath.c_str());
            std::string line;
            for (uint8_t i = 0; i < 12; i++) {
                getline(block, line);
                if (i < 6) {
                    diffuseTextures[i] = loader->LoadTexture(("resources\\minecraft\\blocks\\" + line).c_str());
                }
                else {
                    specularTextures[i - 6] = loader->LoadTexture(("resources\\minecraft\\blocks\\" + line).c_str());
                }
            }
            getline(block, line);
            uint8_t hardness = std::atoi(line.c_str());
            std::string name = "";
            for (size_t i = blockPath.size() - 7; i != size_t(-1) && blockPath[i] != '\\'; i--) {
                name += blockPath[i];
            }
            std::reverse(name.begin(), name.end());
            returnValue.push_back(UBlock(diffuseTextures, specularTextures, "minecraft:" + name, hardness));

        }
    }
    ::std::ifstream blocks_manifest("resources\\BLOCKS_MANIFEST");

    ::std::string line;
    ::std::vector<std::pair<int32_t, ::std::string>> lines;
    while (blocks_manifest.good()) {
        getline(blocks_manifest, line);
        size_t i = 0;
        std::string integer = "";
        for (; i < line.size() && line[i] != ' '; integer += line[i++]); // getting ' ' location and writing id to integer
        for (size_t j = 0; j <= i; j++) {
            line.erase(line.begin());
        }
        lines.push_back(std::pair<int32_t, ::std::string>(std::atoi(integer.c_str()), line));
    }
    std::vector<UBlock> buf;
    for (std::vector<std::pair<int32_t, ::std::string>>::iterator itr = lines.begin(); itr != lines.end(); itr++) {
        for (std::vector<UBlock>::iterator i = returnValue.begin(); i != returnValue.end(); i++) {
            if ((*itr).second == (*i).str_id) {
                (*i).integer_id = (*itr).first;
                buf.push_back((*i));
                break;
            }
        }
    }
    blocks_manifest.close();
    delete[] diffuseTextures;
    delete[] specularTextures;
    return buf;
}
#endif
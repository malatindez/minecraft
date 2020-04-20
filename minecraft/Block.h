#ifndef BLOCK_H
#define BLOCK_H
#include "Shader.h"
#include "Model.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <filesystem>
#include <fstream>
class Block {
public:
    uint32_t *diffuseTextures = nullptr;
    uint32_t *specularTextures = nullptr;
    std::string id; // minecraft:grass, minecraft:stone, etc.
    uint8_t hardness;
    Block(const uint32_t* diffuseTextures, const uint32_t* specularTextures, const std::string id, uint8_t hardness) {
        this->diffuseTextures = new uint32_t[6];
        this->specularTextures = new uint32_t[6];
        memcpy(this->diffuseTextures, diffuseTextures, sizeof(uint32_t) * 6);
        memcpy(this->specularTextures, specularTextures, sizeof(uint32_t) * 6);
        this->id = id;
        this->hardness = hardness;
    }
    Block(const Block& b) {
        this->diffuseTextures = new uint32_t[6];
        this->specularTextures = new uint32_t[6];
        memcpy(this->diffuseTextures, b.diffuseTextures, sizeof(uint32_t) * 6);
        memcpy(this->specularTextures, b.specularTextures, sizeof(uint32_t) * 6);
        this->id = b.id;
        this->hardness = b.hardness;
    }
    Block(const Block* b) {
        this->diffuseTextures = new uint32_t[6];
        this->specularTextures = new uint32_t[6];
        memcpy(this->diffuseTextures, b->diffuseTextures, sizeof(uint32_t) * 6);
        memcpy(this->specularTextures, b->specularTextures, sizeof(uint32_t) * 6);
        this->id = b->id;
        this->hardness = b->hardness;
    }
    ~Block() {
        if (diffuseTextures != nullptr)
        delete[] diffuseTextures;
        if (specularTextures != nullptr)
        delete[] specularTextures;
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
struct BlockTexture {
    unsigned int id;
    std::string path;
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
::std::vector<Block> loadBlocks() {
    ::std::vector<Block> returnValue;
    ::std::vector<BlockTexture> textures;
    ::std::string path = "blocks\\minecraft\\";
    uint32_t* diffuseTextures = new uint32_t[6];
    uint32_t* specularTextures = new uint32_t[6];
    for (const auto& entry : ::std::filesystem::directory_iterator(path)) {
        std::string blockPath = entry.path().string();
        if (ends_with(blockPath,(".block"))) {
            ::std::ifstream block(blockPath.c_str());
            std::string line;
            for (uint8_t i = 0; i < 12; i++) {
                getline(block, line);
                bool flag = false;
                for (size_t j = 0; j < textures.size(); j++) {
                    if (textures[j].path == "blocks\\minecraft\\textures\\" + line) {
                        if (i < 6) {
                            diffuseTextures[i] = textures[j].id;
                        } else {
                            specularTextures[i - 6] = textures[j].id;
                        }
                        flag = true;
                        break;
                    }
                }
                if (not flag) {
                    BlockTexture t;
                    t.id = loadTexture(("blocks\\minecraft\\textures\\" + line).c_str());
                    t.path = "blocks\\minecraft\\textures\\" + line;
                    textures.push_back(t);
                    if (i < 6) {
                        diffuseTextures[i] = t.id;
                    }
                    else {
                        specularTextures[i - 6] = t.id;
                    }
                }
            }
            getline(block, line);
            uint8_t hardness = std::atoi(line.c_str());
            std::string name = "";
            for (size_t i = blockPath.size() - 7; i != size_t(-1) and blockPath[i] != '\\'; i--) {
                name += blockPath[i];
            }
            std::reverse(name.begin(), name.end());
            returnValue.push_back(Block(diffuseTextures, specularTextures, "minecraft:" + name, hardness));
            
        }
    }
    delete[] diffuseTextures;
    delete[] specularTextures;
    return returnValue;
}
#endif
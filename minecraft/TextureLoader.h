#ifndef TEXTURE_LOADER_H
#define TEXTURE_LOADER_H
#include <vector>
#include "Model.h"
class TextureLoader {
public:
	struct Texture {
		uint32_t id;
		std::string path;
        Texture(uint32_t id, std::string path) {
            this->id = id; this->path = path;
        }
	};
	std::vector<Texture> textures;
	TextureLoader() {}
	~TextureLoader() {
		for (size_t i = 0; i < textures.size(); i++) {
			glDeleteTextures(1, &textures[i].id);
		}
	}
	uint32_t LoadTexture(std::string path) {
        for (size_t i = 0; i < textures.size(); i++) {
            if (textures[i].path == path) {
                return textures[i].id;
            }
        }
        uint32_t textureID;
        glGenTextures(1, &textureID);

        int32_t width, height, nrComponents;
        uint8_t* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);
        if (data) {
            GLenum format = GL_RED;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

            stbi_image_free(data);
        }
        else {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }

        textures.push_back(Texture(textureID, path));
        return textureID;
	}
};

#endif
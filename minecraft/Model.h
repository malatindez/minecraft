#ifndef MODEL_H
#define MODEL_H
#include <vector>
#include "Shader.h"
#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "stb_image.h"
unsigned int loadTexture(char const* path);
class Model {
public:
    Model(std::string path) {
        loadModel(path);
    }
    void Draw(Shader shader) {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }
private:
    /*  Model data */
    std::vector<Mesh> meshes;
    std::string directory;
    std::vector<Texture> loadedTextures;
    /*  Methods   */
    void loadModel(std::string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

#endif
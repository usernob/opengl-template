#ifndef SRC_MODEL_H
#define SRC_MODEL_H

#include "assimp/Importer.hpp"
#include "assimp/material.h"
#include "assimp/mesh.h"
#include "assimp/scene.h"
#include "mesh.h"
#include "shader.h"
#include "stb_image.h"
#include <string>
#include <unordered_map>
#include <vector>

int loadTexture(const char *path);

class Model
{
private:
    std::vector<Mesh> m_meshes;
    std::unordered_map<std::string, Texture> m_textures_loaded;
    std::string m_directory;
    Shader &m_shader;

    void mLoadModel(const char *path);

    void mProcessNode(const aiNode *node, const aiScene *scene);

    void mProcessMesh(const aiMesh *mesh, const aiScene *scene);

    void mLoadMaterialTextures(
        const aiMaterial *mat,
        const aiTextureType &type,
        const std::string &typeName,
        std::vector<Texture *> &out_textures
    );

public:
    Model(const char *path, Shader &shader);

    void draw();
};

#endif // SRC_MODEL_H

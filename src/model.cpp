#include "model.h"
#include "assimp/material.h"
#include "assimp/postprocess.h"
#include "assimp/types.h"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "mesh.h"
#include <iostream>

int loadTexture(const char *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrChannels;
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format;
        if (nrChannels == 1) format = GL_RED;
        else if (nrChannels == 3) format = GL_RGB;
        else if (nrChannels == 4) format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
        return textureID;
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
        return -1;
    }
    stbi_image_free(data);
}

void Model::mLoadModel(const char *path)
{
    Assimp::Importer importer;
    // add optimizations flags is important because if we don't, it will generate to much meshes
    // that will increase draw call count
    const aiScene *scene = importer.ReadFile(
        path,
        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_OptimizeGraph |
            aiProcess_OptimizeMeshes | aiProcess_JoinIdenticalVertices | aiProcess_SortByPType
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }

    std::string path_str = std::string(path);
    m_directory = path_str.substr(0, path_str.find_last_of('/'));

    mProcessNode(scene->mRootNode, scene);
}

void Model::mProcessNode(const aiNode *node, const aiScene *scene)
{
    // process current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        const aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        mProcessMesh(mesh, scene);
    }

    // process children node recursively
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        mProcessNode(node->mChildren[i], scene);
    }
}

void Model::mProcessMesh(const aiMesh *mesh, const aiScene *scene)
{
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture *> textures;

    // process vertices
    vertices.reserve(mesh->mNumVertices);
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex &vertex = vertices.emplace_back();

        // mesh always contains position, don't need to check it
        vertex.position.x = mesh->mVertices[i].x;
        vertex.position.y = mesh->mVertices[i].y;
        vertex.position.z = mesh->mVertices[i].z;

        if (mesh->HasNormals())
        {
            vertex.normal.x = mesh->mNormals[i].x;
            vertex.normal.y = mesh->mNormals[i].y;
            vertex.normal.z = mesh->mNormals[i].z;
        }

        if (mesh->HasTextureCoords(0) && mesh->mTextureCoords[0])
        {
            vertex.texCoords.x = mesh->mTextureCoords[0][i].x;
            vertex.texCoords.y = mesh->mTextureCoords[0][i].y;
        }
    }

    // process indices
    indices.reserve(mesh->mNumFaces * 3);
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        const aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    // process material
    textures.reserve(2);
    if (mesh->mMaterialIndex >= 0)
    {
        const aiMaterial *const material = scene->mMaterials[mesh->mMaterialIndex];

        mLoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", textures);

        mLoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", textures);
    }

    // mesh must be created after all vertices and indices are loaded
    // because mesh will setup buffers on constructor to make sure buffers not created multiple
    // times
    const Mesh &mesh_ = m_meshes.emplace_back(vertices, indices, textures);
    mesh_.bindTexture(m_shader);
}

void Model::mLoadMaterialTextures(
    const aiMaterial *mat,
    const aiTextureType &type,
    const std::string &typeName,
    std::vector<Texture *> &out_textures
)
{
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        std::string path = m_directory + '/' + str.C_Str();

        // if texture not loaded, load it
        if (m_textures_loaded.find(path) == m_textures_loaded.end())
        {
            Texture texture;
            texture.id = loadTexture(path.c_str());
            texture.type = type;
            texture.path = path;
            m_textures_loaded.insert(std::make_pair(texture.path, texture));
        }

        out_textures.push_back(&m_textures_loaded[path]);
    }
}

Model::Model(const char *path, Shader &shader) : m_shader(shader)
{
    mLoadModel(path);
}

void Model::draw()
{
    for (const auto &mesh : m_meshes)
    {
        mesh.draw();
    }
}

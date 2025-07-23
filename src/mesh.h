#ifndef SRC_MESH_H
#define SRC_MESH_H

#include "assimp/material.h"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "shader.h"
#include <vector>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoords;
};

struct Texture
{
    GLuint id;
    aiTextureType type;
    std::string path;
};

class Mesh
{
public:
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture *> textures;

    Mesh(
        const std::vector<Vertex> &vertex,
        const std::vector<GLuint> &index,
        const std::vector<Texture *> &texture
    );

    void bindTexture(Shader &shader) const;
    void draw() const;

private:
    GLuint VAO, VBO, EBO;
    void mSetupMesh();
};

#endif // SRC_MESH_H

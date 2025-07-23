#include "mesh.h"
#include "assimp/material.h"
#include "shader.h"
#include <cstddef>
#include <string>

Mesh::Mesh(
    const std::vector<Vertex> &vertex,
    const std::vector<GLuint> &index,
    const std::vector<Texture *> &texture
)
    : vertices(vertex), indices(index), textures(texture)
{
    mSetupMesh();
};

void Mesh::mSetupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(Vertex),
        vertices.data(),
        GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        indices.size() * sizeof(GLuint),
        indices.data(),
        GL_STATIC_DRAW
    );

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void *)offsetof(Vertex, normal)
    );

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        (void *)offsetof(Vertex, texCoords)
    );

    glBindVertexArray(0);
}

void Mesh::bindTexture(Shader &shader) const
{
    unsigned int diffuse_nr = 0, specular_nr = 0;
    shader.use();

    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);

        std::string name;
        // currently only support one diffuse and specular
        if (textures[i]->type == aiTextureType_DIFFUSE && diffuse_nr < 1)
        {
            name = "material.diffuse";
            diffuse_nr++;
        }
        else if (textures[i]->type == aiTextureType_SPECULAR && specular_nr < 1)
        {
            name = "material.specular";
            specular_nr++;
        }

        shader.setInt(name, i);
        glBindTexture(GL_TEXTURE_2D, textures[i]->id);
    }
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::draw() const
{
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);

        glBindTexture(GL_TEXTURE_2D, textures[i]->id);
    }
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE0);
}

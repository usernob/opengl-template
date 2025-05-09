#ifndef SRC_SHADER_H
#define SRC_SHADER_H

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float3.hpp"
#include <glad/glad.h>
#include <string>

class Shader
{
public:
    unsigned int ID;

    Shader(const char *vertex_path, const char *fragment_path);

    void use();

    GLuint getID() const
    {
        return ID;
    }

    GLuint getUniformLocation(const std::string &name) const
    {
        return glGetUniformLocation(ID, name.c_str());
    }

    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setMat4(const std::string &name, glm::mat4 value) const;
    void setMat3(const std::string &name, glm::mat3 value) const;
    void setVec3(const std::string &name, glm::vec3 value) const;
    void setVec3(const std::string &name, float x, float y, float z) const;
};

#endif // SRC_SHADER_H

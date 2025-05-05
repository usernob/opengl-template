#ifndef SRC_SHADER_H
#define SRC_SHADER_H

#include "glm/ext/matrix_float4x4.hpp"
#include <string>
class Shader
{
public:
    unsigned int ID;

    Shader(const char *vertex_path, const char *fragment_path);

    void use();

    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;
    void setMat4(const std::string &name, glm::mat4 value) const;
};

#endif // SRC_SHADER_H

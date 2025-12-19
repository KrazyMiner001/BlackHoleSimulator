#include "shader_program.h"

#include "glbinding/gl/functions.h"
#include "glm/gtc/type_ptr.hpp"

using namespace gl;

void shader_program::use() const {
    glUseProgram(ID);
}

void shader_program::setMat3(const std::string &name, const glm::mat3 &value) const {
    setMat3(glGetUniformLocation(ID, name.c_str()), value);
}

void shader_program::setMat3(const int location, const glm::mat3 &value) {
    glUniformMatrix3fv(location, 1, false, value_ptr(value));
}

void shader_program::setVec3(const std::string &name, const glm::vec3 &value) const {
    setVec3(glGetUniformLocation(ID, name.c_str()), value);
}

void shader_program::setVec3(const int location, const glm::vec3 &value) {
    glUniform3fv(location, 1, value_ptr(value));
}

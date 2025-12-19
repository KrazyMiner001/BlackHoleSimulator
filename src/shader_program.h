#ifndef BLACKHOLESIMULATOR_SHADER_PROGRAM_H
#define BLACKHOLESIMULATOR_SHADER_PROGRAM_H
#include <string>

#include "glm/mat3x3.hpp"


class shader_program {
public:
    void use() const;

    void setMat3(const std::string &name, const glm::mat3 &value) const;
    static void setMat3(int location, const glm::mat3 &value);

    void setVec3(const std::string &name, const glm::vec3 &value) const;
    static void setVec3(int location, const glm::vec3 &value);

protected:
    const unsigned int ID;
    shader_program(const unsigned int ID) : ID(ID) {}
};


#endif //BLACKHOLESIMULATOR_SHADER_PROGRAM_H
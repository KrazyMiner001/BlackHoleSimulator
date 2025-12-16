#ifndef BLACKHOLESIMULATOR_COMPUTE_SHADER_H
#define BLACKHOLESIMULATOR_COMPUTE_SHADER_H
#include <string>

#include "shader_program.h"


class compute_shader : public shader_program {
public:
    explicit compute_shader(const std::string &path);
};


#endif //BLACKHOLESIMULATOR_COMPUTE_SHADER_H
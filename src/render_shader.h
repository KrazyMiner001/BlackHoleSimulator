#ifndef BLACKHOLESIMULATOR_RENDER_SHADER_H
#define BLACKHOLESIMULATOR_RENDER_SHADER_H
#include <string>

#include "shader_program.h"


class render_shader : public shader_program {
public:
    render_shader(const std::string &vertex_shader_path, const std::string &fragment_shader_path);
};


#endif //BLACKHOLESIMULATOR_RENDER_SHADER_H